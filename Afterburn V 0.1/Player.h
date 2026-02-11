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

/*  Car Sprite Sheet Config
 *  Assumption: Grid layout for rotation frames.
 *  Image: Asset/Car.png (1800x1800)
 *  Grid:  6x6 (36 frames) -> 300x300 px per frame?
 *  Or     10x10 (100 frames) -> 180x180 px per frame?
 *  Let's start with 6x6 (10 degrees per frame if 36 frames).
 *  If incorrect, update these values.                           */
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
  float angle;     /* Rotation angle in degrees (0-360) */
  int spriteIndex; /* Current frame index to draw       */
  bool active;
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

  /* 360 degrees / 36 frames = 10 degrees per frame.
   * Frame 0 = 0 degrees (Right? Up?).
   * Adjust offset if needed.                                  */
  int frame = (int)(angle / (360.0f / CAR_TOTAL_FRAMES));
  if (frame >= CAR_TOTAL_FRAMES)
    frame = CAR_TOTAL_FRAMES - 1;
  return frame;
}

/* ── Helper: Draw Grid Sprite ───────────────────────────── */
static void iShowImageGrid(int x, int y, int w, int h, unsigned int texture,
                           int frameIndex, int rows, int cols) {
  /* Calculate row and col 0-based */
  int r = frameIndex / cols;
  int c = frameIndex % cols;

  /* OpenGL texture coords (0,0 is bottom-left)
   * Rows go from Top(0) to Bottom(rows-1) in sprite sheet convention Usually.
   * Cols go from Left(0) to Right(cols-1).
   *
   * iGraphics V axis: -1 (top) to 0 (bottom]? No usually 0 to 1.
   * Wait, iShowImageSub used -1 to 0 in V?
   * Let's re-verify iShowImageSub logic in UI.h or iMain.cpp.
   * "V goes from 0 at bottom to -1 at top" according to comment in iMain.cpp.
   *
   * So Row 0 (Top) corresponds to V approx -1.
   * Row rows-1 (Bottom) corresponds to V approx 0.
   */

  float cellW = 1.0f / cols;
  float cellH = 1.0f / rows;

  /* Top-Left of cell */
  float uLeft = c * cellW;
  float uRight = uLeft + cellW;

  /* Vertical: Top is -1. Bottom is 0.
   * Row 0 is at Top (-1).
   * Row r starts at -1 + r*cellH
   */
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
  /* Load textures */
  texCar = iLoadImage("Asset/Car.png");
  texProjectile = iLoadImage("Asset/projectile.png");

  /* Init State */
  player.x = 960 - CAR_DRAW_W / 2;
  player.y = 100;
  player.health = PLAYER_MAX_HEALTH;
  player.score = 0;
  player.active = true;
  projectiles.clear();
}

/* ── update ─────────────────────────────────────────────── */
void playerUpdate(void) {
  if (!player.active)
    return;

  /* Input is handled via callbacks, providing direct position updates
   * here requires polling keyboard state array if available.
   * iGraphics exposes `isKeyPressed(key)`. We can use that.
   */

  /* W / w */
  if (isKeyPressed('w') || isKeyPressed('W')) {
    player.y += PLAYER_SPEED;
  }
  /* S / s */
  if (isKeyPressed('s') || isKeyPressed('S')) {
    player.y -= PLAYER_SPEED;
  }
  /* A / a */
  if (isKeyPressed('a') || isKeyPressed('A')) {
    player.x -= PLAYER_SPEED;
  }
  /* D / d */
  if (isKeyPressed('d') || isKeyPressed('D')) {
    player.x += PLAYER_SPEED;
  }

  /* Boundaries (1920x1080) */
  if (player.x < 0)
    player.x = 0;
  if (player.x > 1920 - CAR_DRAW_W)
    player.x = 1920 - CAR_DRAW_W;
  if (player.y < 0)
    player.y = 0;
  if (player.y > 1080 - CAR_DRAW_H)
    player.y = 1080 - CAR_DRAW_H;

  /* Calculate Angle from Mouse */
  /* Note: iMouseX/Y are global in iGraphics.h?
   * Usually they are passed to iMouseMove.
   * iGraphics often exposes iMouseX, iMouseY as variables.
   * Let's assume we need to track mouse separately if not exposed.
   * But usually simple implementations track it in iPassiveMouseMove.
   * Let's verify iGraphics.h has global iMouseX/Y.
   * YES: line 23: int iMouseX, iMouseY;
   */
  float dx = (float)iMouseX - (player.x + CAR_DRAW_W / 2);
  float dy = (float)iMouseY - (player.y + CAR_DRAW_H / 2);
  player.angle = atan2(dy, dx) * 180.0f / PI;

  /* Update Projectiles */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      projectiles[i].x += projectiles[i].dx;
      projectiles[i].y += projectiles[i].dy;

      /* Out of bounds check */
      if (projectiles[i].x < 0 || projectiles[i].x > 1920 ||
          projectiles[i].y < 0 || projectiles[i].y > 1080) {
        projectiles[i].active = false;
      }
    }
  }

  /* Remove inactive projectiles?
   * Simple swap-verify or just ignore rendering.
   */
}

/* ── draw ───────────────────────────────────────────────── */
void playerDraw(void) {
  /* Draw Projectiles */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      /* Draw small projectile (20x20) centered */
      iShowImage((int)projectiles[i].x - 10, (int)projectiles[i].y - 10, 20, 20,
                 texProjectile);
    }
  }

  if (!player.active)
    return;

  /* Determine Frame based on angle */
  /* 0 degrees = Right usually in Math.
   * Check Car.png orientation. If 0 deg is UP, offset logic.
   * Assume Standard Math (0 = Right, CCW).
   */
  int frame = getFrameFromAngle(player.angle);

  /* Debug: Draw simple rect if texture fails? No, trust texture. */
  iShowImageGrid((int)player.x, (int)player.y, CAR_DRAW_W, CAR_DRAW_H, texCar,
                 frame, CAR_SHEET_ROWS, CAR_SHEET_COLS);
}

/* ── shooting ───────────────────────────────────────────── */
void playerShoot(void) {
  if (!player.active)
    return;

  Projectile p;
  float rad = player.angle * PI / 180.0f;
  float speed = 10.0f;

  /* Spawn at center of car */
  p.x = player.x + CAR_DRAW_W / 2;
  p.y = player.y + CAR_DRAW_H / 2;
  p.dx = cos(rad) * speed;
  p.dy = sin(rad) * speed;
  p.active = true;

  projectiles.push_back(p);
}

#endif /* PLAYER_H */
