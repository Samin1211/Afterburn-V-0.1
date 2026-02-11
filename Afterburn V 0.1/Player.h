#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>
#include <vector> /* For projectiles */

/* ══════════════════════════════════════════════════════════
 *  PLAYER MODULE
 *  Management of the player's car, movement, and combat.
 * ══════════════════════════════════════════════════════════ */

/* ── Constants ──────────────────────────────────────────── */
#define PLAYER_SPEED 5.0f
#define PLAYER_MAX_HEALTH 100.0f
#define PI 3.14159265f
#define SHOOT_COOLDOWN 10 /* Frames between shots (~6/sec at 60FPS) */

/*  Road Boundaries (Defined here for global access)             */
#define ROAD_LEFT_LIMIT 440
#define ROAD_RIGHT_LIMIT 1480

/*  Car Sprite Sheet Config                                      */
#define CAR_SHEET_ROWS 6
#define CAR_SHEET_COLS 6
#define CAR_TOTAL_FRAMES 36
#define CAR_FRAME_W 300
#define CAR_FRAME_H 300

/*  Draw size (scaled down from source frame)                    */
#define CAR_DRAW_W 100
#define CAR_DRAW_H 100

/* ── Projectile Struct ──────────────────────────────────── */
struct Projectile {
  float x, y;
  float dx, dy;
  bool active;
};

/* ── Player Globals ─────────────────────────────────────── */
struct PlayerState {
  float x, y;
  float health;
  int score;
  float angle;      /* Visual rotation angle in degrees */
  float shootAngle; /* Actual shooting angle in radians */
  int spriteIndex;  /* Current frame index to draw */
  bool active;
  /* Firing State */
  bool isFiring;
  int shootTimer;
};

static PlayerState player;

static std::vector<Projectile> projectiles;
static unsigned int texCar = 0;
static unsigned int texProjectile = 0;

/* ── Helper: Map Angle to Frame Index ───────────────────── */
static int getFrameFromAngle(float angle) {
  /* Normalize angle to 0-360 */
  while (angle < 0)
    angle += 360;
  while (angle >= 360)
    angle -= 360;

  int frame = (int)(angle / (360.0f / CAR_TOTAL_FRAMES));
  if (frame >= CAR_TOTAL_FRAMES)
    frame = CAR_TOTAL_FRAMES - 1;
  return frame;
}

/* ── Helper: Draw Grid Sprite ───────────────────────────── */
static void iShowImageGrid(int x, int y, int w, int h, unsigned int texture,
                           int frameIndex, int rows, int cols) {
  int r = frameIndex / cols;
  int c = frameIndex % cols;

  float cellW = 1.0f / cols;
  float cellH = 1.0f / rows;

  float uLeft = c * cellW;
  float uRight = uLeft + cellW;
  float vTop = -1.0f + r * cellH;
  float vBottom = vTop + cellH;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBegin(GL_QUADS);
  glTexCoord2f(uLeft, vBottom);
  glVertex2f((float)x, (float)y);
  glTexCoord2f(uRight, vBottom);
  glVertex2f((float)(x + w), (float)y);
  glTexCoord2f(uRight, vTop);
  glVertex2f((float)(x + w), (float)(y + h));
  glTexCoord2f(uLeft, vTop);
  glVertex2f((float)x, (float)(y + h));
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

/* ── init ───────────────────────────────────────────────── */
void playerInit(void) {
  texCar = iLoadImage("Asset/Car.png");
  texProjectile = iLoadImage("Asset/projectile.png");

  player.x = 960 - CAR_DRAW_W / 2;
  player.y = 100;
  player.health = PLAYER_MAX_HEALTH;
  player.score = 0;
  player.active = true;
  player.isFiring = false;
  player.shootTimer = 0;
  projectiles.clear();
}

/* ── shooting ───────────────────────────────────────────── */
void playerShoot(void) {
  Projectile p;
  float speed = 10.0f;

  /* Calculate direct vector to mouse for perfect accuracy */
  /* Center of player car */
  float cx = player.x + CAR_DRAW_W / 2;
  float cy = player.y + CAR_DRAW_H / 2;

  float dx = (float)iMouseX - cx;
  float dy = (float)iMouseY - cy;

  /* Normalize and scale */
  float length = sqrt(dx * dx + dy * dy);
  if (length == 0)
    length = 1;

  p.x = cx;
  p.y = cy;
  p.dx = (dx / length) * speed;
  p.dy = (dy / length) * speed;
  p.active = true;

  projectiles.push_back(p);
}

/* ── update ─────────────────────────────────────────────── */
void playerUpdate(void) {
  if (!player.active)
    return;

  /* Movement (WASD) */
  if (isKeyPressed('w') || isKeyPressed('W'))
    player.y += PLAYER_SPEED;
  if (isKeyPressed('s') || isKeyPressed('S'))
    player.y -= PLAYER_SPEED;
  if (isKeyPressed('a') || isKeyPressed('A'))
    player.x -= PLAYER_SPEED;
  if (isKeyPressed('d') || isKeyPressed('D'))
    player.x += PLAYER_SPEED;

  /* Boundaries */
  if (player.x < ROAD_LEFT_LIMIT)
    player.x = ROAD_LEFT_LIMIT;
  if (player.x > ROAD_RIGHT_LIMIT - CAR_DRAW_W)
    player.x = ROAD_RIGHT_LIMIT - CAR_DRAW_W;
  if (player.y < 0)
    player.y = 0;
  if (player.y > 1080 - CAR_DRAW_H)
    player.y = 1080 - CAR_DRAW_H;

  /* Rotation & Aiming Logic */
  float dx = (float)iMouseX - (player.x + CAR_DRAW_W / 2);
  float dy = (float)iMouseY - (player.y + CAR_DRAW_H / 2);

  /* Actual Angle (Radians, CCW) for math if needed */
  player.shootAngle = atan2(dy, dx);

  /* Visual Angle (Degrees) */
  /* User feedback: "if I move anti clockwise cannon should move anti clockwise"
   */
  /* Standard atan2 is CCW. So mapping angle directly should work if sprites are
   * CCW. If sprites are CW, we negate. Let's use standard CCW angle first. +90
   * deg offset? or 0? Usually 0 deg is Right. `player.angle` logic:
   */
  float deg = player.shootAngle * 180.0f / PI;
  /* Just use the degree directly. If it feels inverted, user will say.
   * Previous attempt inverted it (-atan2).
   * User said "messed up". So revert to standard atan2.
   */
  player.angle = deg;

  /* Continuous Fire Logic */
  if (player.shootTimer > 0)
    player.shootTimer--;

  if (player.isFiring && player.shootTimer == 0) {
    playerShoot();
    player.shootTimer = SHOOT_COOLDOWN;
  }

  /* Update Projectiles */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      projectiles[i].x += projectiles[i].dx;
      projectiles[i].y += projectiles[i].dy;

      if (projectiles[i].x < 0 || projectiles[i].x > 1920 ||
          projectiles[i].y < 0 || projectiles[i].y > 1080) {
        projectiles[i].active = false;
      }
    }
  }
}

/* ── draw ───────────────────────────────────────────────── */
void playerDraw(void) {
  /* Draw Projectiles */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      iShowImage((int)projectiles[i].x - 10, (int)projectiles[i].y - 10, 20, 20,
                 texProjectile);
    }
  }

  if (!player.active)
    return;

  int frame = getFrameFromAngle(player.angle);

  iShowImageGrid((int)player.x, (int)player.y, CAR_DRAW_W, CAR_DRAW_H, texCar,
                 frame, CAR_SHEET_ROWS, CAR_SHEET_COLS);
}

#endif /* PLAYER_H */
