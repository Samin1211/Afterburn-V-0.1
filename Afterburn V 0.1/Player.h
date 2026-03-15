#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>
#include <vector> /* For projectiles */

/* ══════════════════════════════════════════════════════════
 *  PLAYER MODULE
 *  Management of the player's car, movement, and combat.
 *
 *  HOW IT WORKS:
 *  This module controls the player's car: position, health,
 *  score, rotation/aiming, continuous firing, and projectile
 *  management. The car uses a 6x6 sprite sheet for 360°
 *  rotation. Movement is WASD-based, aiming follows the mouse.
 *
 *  DEPENDENCIES: iGraphics.h (for rendering and input),
 *                math.h (for trigonometry), vector (for projectiles).
 * ══════════════════════════════════════════════════════════ */

/* ── Constants ──────────────────────────────────────────── */

/* PLAYER_SPEED: How many pixels the player car moves per frame when
 * pressing WASD. At 60 FPS this means 300 pixels/second.
 * HOW TO CHANGE: Increase for a faster car, decrease for slower.
 * EFFECT: Higher values make the car harder to control precisely;
 *         lower values make dodging enemies more difficult. */
#define PLAYER_SPEED 5.0f

/* PLAYER_MAX_HEALTH: The starting and maximum health of the player.
 * Health is a float displayed as a percentage bar in the HUD.
 * HOW TO CHANGE: Increase to make the game easier (player survives longer),
 *                decrease to make it harder.
 * EFFECT: This value is used by playerReset() and the health orb pickup. */
#define PLAYER_MAX_HEALTH 100.0f

/* PI: Mathematical constant used for angle conversions (radians <-> degrees).
 * Used in rotation/aiming calculations. Do not change. */
#define PI 3.14159265f

/* SHOOT_COOLDOWN: Number of frames between automatic shots when holding
 * fire (left mouse or space). At 60 FPS, 5 frames = ~12 shots/second.
 * HOW TO CHANGE: Lower = faster fire rate, higher = slower fire rate.
 * EFFECT: Lower values dramatically increase DPS; consider balancing
 *         enemy health if you change this. */
#define SHOOT_COOLDOWN 5 /* Frames between shots (~12/sec at 60FPS) */

/* ROAD_LEFT_LIMIT / ROAD_RIGHT_LIMIT: The horizontal pixel boundaries
 * of the drivable road area on the 1920px-wide canvas.
 * The player, enemies, and NPCs are clamped within these limits.
 * HOW TO CHANGE: Adjust to match a different road texture width.
 *   Move ROAD_LEFT_LIMIT left (smaller) or ROAD_RIGHT_LIMIT right (larger)
 *   to widen the playable area. Must match the visual road edges.
 * EFFECT: Wider road = more dodge room; narrower = more cramped. */
#define ROAD_LEFT_LIMIT 440
#define ROAD_RIGHT_LIMIT 1480

/* Car Sprite Sheet Config: The player car uses a single sprite sheet
 * (Car.png) containing a 6x6 grid of rotation frames (36 total).
 * Each frame shows the car rotated ~10° from the previous one,
 * giving smooth 360° visual rotation.
 * CAR_FRAME_W/H: The pixel size of each frame IN the source image.
 * HOW TO CHANGE: Only change if you replace Car.png with a different
 *   sprite sheet that has different grid dimensions.
 * EFFECT: Wrong values will display garbled/offset car sprites. */
#define CAR_SHEET_ROWS 6
#define CAR_SHEET_COLS 6
#define CAR_TOTAL_FRAMES 36
#define CAR_FRAME_W 300
#define CAR_FRAME_H 300

/* CAR_DRAW_W / CAR_DRAW_H: The on-screen pixel size of the player car.
 * The 300x300 source frame is scaled down to 100x100 for gameplay.
 * HOW TO CHANGE: Increase for a bigger car, decrease for smaller.
 * EFFECT: Larger = easier to get hit by enemies and projectiles
 *         (bigger collision box); smaller = harder to hit but
 *         also harder for the player to see. Also affects enemy
 *         collision checks using CAR_DRAW_W/H. */
#define CAR_DRAW_W 100
#define CAR_DRAW_H 100

/* Hitbox insets: shrink collision box to match visible sprite bounds */
#define PLAYER_HB_X 30    /* Left/right inset from draw origin */
#define PLAYER_HB_Y 10    /* Bottom/top inset from draw origin */
#define PLAYER_HB_W 40    /* Player collision width  */
#define PLAYER_HB_H 80    /* Player collision height */

/* ── Projectile Struct ──────────────────────────────────── */
/* Projectile: Represents a single bullet fired by the player's cannon.
 * x, y   — Current position on screen (pixels).
 * dx, dy — Velocity vector (pixels/frame). Calculated from the
 *           direction toward the mouse cursor at time of firing.
 * active — Whether this projectile is still alive. Set to false
 *           when it goes off-screen or hits an enemy/NPC.
 * HOW TO CHANGE: The speed of projectiles is set in playerShoot()
 *   (currently 10.0f). To make bullets faster, increase that value.
 * EFFECT: Faster bullets are easier to land; slower ones require
 *         more lead aiming. */
struct Projectile {
  float x, y;
  float dx, dy;
  bool active;
};

/* ── Player Globals ─────────────────────────────────────── */
/* PlayerState: The complete state of the player's car.
 *
 * x, y            — Bottom-left position of the car on screen (pixels).
 * health          — Current HP (0-100). When <= 0, game over triggers.
 * score           — Current score. Increases by 10 per road scroll loop
 *                   and by killing enemies (10 normal, 20 special2, 50
 * special1). Score is capped at 1000 before the first boss fight. angle —
 * Visual rotation in degrees, derived from mouse position. shootAngle      —
 * Firing direction in radians (atan2 toward mouse cursor). spriteIndex     —
 * Which frame (0-35) of the car sprite sheet to draw. active          — Whether
 * the player is alive and controllable. isFiring        — True while left mouse
 * or space is held down (continuous fire). shootTimer      — Cooldown counter;
 * decrements each frame, fires when 0. lossControlTimer— Status effect: when >
 * 0, player cannot move or shoot. Set to 120 frames (2s) by oil puddle
 * collision. HOW TO CHANGE: Modify the value in Enemy.h puddle collision.
 *                   EFFECT: Longer = more punishing; shorter = minor annoyance.
 * missileCount    — Number of heavy missiles (right-click). Granted by
 *                   power orbs from supply trucks during boss fights.
 */
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
  /* Status Effect */
  int lossControlTimer;
  /* Ammo */
  int missileCount;
};

/* player: The single global player instance. All modules read/write this. */
static PlayerState player;

/* bossFightOver: Flag set to true after the first boss is defeated and
 * the cloud transition completes. Used to gate Stage 2 content (NPCs,
 * Special Enemy 2, Boss2, different score thresholds).
 * HOW TO CHANGE: Set to true earlier to skip to Stage 2 for testing.
 * EFFECT: Controls which enemies spawn, score thresholds, and road texture. */
static bool bossFightOver = false;
static bool boss2FightOver = false;

/* projectiles: Dynamic list of all active player cannon bullets on screen. */
static std::vector<Projectile> projectiles;

/* texCar: OpenGL texture ID for the car sprite sheet (Car.png). */
static unsigned int texCar = 0;

/* texProjectile: OpenGL texture ID for the cannon bullet image. */
static unsigned int texProjectile = 0;

/* ── Helper: Map Angle to Frame Index ───────────────────── */
/* getFrameFromAngle: Converts a rotation angle (degrees) to a sprite
 * sheet frame index (0-35). The 36 frames divide 360° into 10° slices.
 *
 * HOW IT WORKS:
 * 1. Normalizes the angle to 0-360 range.
 * 2. Divides by (360/36 = 10°) to get the frame index.
 * 3. Clamps to valid range.
 *
 * HOW TO CHANGE: Only change if you use a sprite sheet with a different
 *   number of rotation frames (update CAR_TOTAL_FRAMES).
 * EFFECT: Wrong mapping causes the car to face the wrong direction. */
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
/* iShowImageGrid: Draws a single frame from a sprite sheet (grid layout).
 *
 * HOW IT WORKS:
 * 1. Computes the row/column of the desired frame in the grid.
 * 2. Calculates UV texture coordinates for that cell.
 * 3. Renders a textured quad at (x, y) with size (w, h) using OpenGL.
 *
 * PARAMETERS:
 *   x, y       — Screen position (bottom-left corner).
 *   w, h       — Draw size in pixels.
 *   texture    — OpenGL texture ID of the sprite sheet.
 *   frameIndex — Which frame to draw (0-based, row-major order).
 *   rows, cols — Grid dimensions of the sprite sheet.
 *
 * USED BY: playerDraw() for drawing the car, Boss2 turret rendering.
 * HOW TO CHANGE: Only modify if you change the OpenGL rendering pipeline.
 * EFFECT: Incorrect UV math will show the wrong frame or garbled graphics. */
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

/* ── init (load textures — call once) ────────────────── */
/* playerInit: Loads the player's texture assets from disk into GPU memory.
 * Called once at startup in gameInit(). Do NOT call during gameplay.
 * HOW TO CHANGE: Replace the file paths to use different car/projectile art.
 * EFFECT: Changes the visual appearance of the player car and bullets. */
void playerInit(void) {
  texCar = iLoadImage("Asset/Car.png");
  texProjectile = iLoadImage("Asset/projectile.png");
}

/* ── reset (state only — call on restart) ───────────── */
/* playerReset: Resets the player to starting state for a new game.
 * Called by gameReset() when starting a new game or restarting.
 * Does NOT reload textures (textures persist in GPU memory).
 *
 * HOW IT WORKS:
 * - Places the car at the horizontal center of the screen, near the bottom.
 * - Restores full health and zeroes the score.
 * - Clears all active projectiles from the screen.
 * - Resets bossFightOver flag so the game starts from Stage 1.
 *
 * HOW TO CHANGE: Modify starting position (player.x/y), starting health,
 *   or starting score. Set bossFightOver = true to start in Stage 2.
 * EFFECT: Changed starting values alter initial difficulty and game state. */
void playerReset(void) {
  player.x = 960 - CAR_DRAW_W / 2;
  player.y = 100;
  player.health = PLAYER_MAX_HEALTH;
  player.score = 0;
  player.active = true;
  player.isFiring = false;
  player.shootTimer = 0;
  player.lossControlTimer = 0;
  player.missileCount = 0;
  bossFightOver = false;
  boss2FightOver = false;
  projectiles.clear();
}

/* ── shooting ───────────────────────────────────────────── */
/* playerShoot: Creates a new projectile aimed at the mouse cursor.
 *
 * HOW IT WORKS:
 * 1. Checks if the player is under the "loss of control" effect
 *    (from oil puddles). If so, shooting is blocked.
 * 2. Calculates the center of the car as the bullet spawn point.
 * 3. Computes a normalized direction vector from car center to mouse.
 * 4. Scales by bullet speed (10.0f pixels/frame = 600 px/sec at 60FPS).
 * 5. Adds the new projectile to the global projectiles list.
 *
 * HOW TO CHANGE:
 * - Bullet speed: Change `speed` (currently 10.0f). Higher = faster bullets.
 * - Spawn point: Modify cx/cy offsets to fire from a different car position.
 * EFFECT: Faster bullets make hitting enemies easier. Changing spawn point
 *         shifts where bullets appear (e.g., from the car's gun barrel). */
void playerShoot(void) {
  /* Prevent shooting if lost control (oil puddle status effect) */
  if (player.lossControlTimer > 0)
    return;

  Projectile p;
  float speed = 10.0f; /* Bullet speed in pixels/frame */

  /* Calculate direct vector from car center to mouse for perfect accuracy */
  float cx = player.x + CAR_DRAW_W / 2; /* Car center X */
  float cy = player.y + CAR_DRAW_H / 2; /* Car center Y */

  float dx = (float)iMouseX - cx; /* Direction X toward mouse */
  float dy = (float)iMouseY - cy; /* Direction Y toward mouse */

  /* Normalize the direction vector and scale to desired speed */
  float length = sqrt(dx * dx + dy * dy);
  if (length == 0)
    length = 1; /* Prevent division by zero if mouse is exactly on car center */

  p.x = cx; /* Spawn at car center */
  p.y = cy;
  p.dx = (dx / length) * speed; /* Velocity X component */
  p.dy = (dy / length) * speed; /* Velocity Y component */
  p.active = true;

  projectiles.push_back(p); /* Add to global projectile list */
}

/* ── update ─────────────────────────────────────────────── */
/* playerUpdate: Called every frame (~60 FPS) to process player logic.
 *
 * HOW IT WORKS:
 * 1. If player is not active (dead), skip everything.
 * 2. If under "loss of control" effect, just count down the timer
 *    and block all movement/shooting input.
 * 3. Otherwise, process WASD movement with boundary clamping,
 *    calculate mouse-relative aiming angle, select the correct
 *    sprite frame, and handle continuous fire logic.
 * 4. Update all active projectile positions and deactivate any
 *    that fly off-screen.
 *
 * HOW TO CHANGE:
 * - Movement keys: Change 'w','s','a','d' to different keys.
 * - Boundary limits: Modify the ROAD_LEFT/RIGHT_LIMIT and screen values.
 * - Aiming: The formula (90.0f - deg) converts math angles to sprite
 * orientation. Change the offset if your sprite sheet's "up" frame is
 * different.
 * - Fire rate: Controlled by SHOOT_COOLDOWN constant.
 *
 * EFFECT: This function ties together all per-frame player behavior.
 *         Changing movement speed, fire rate, or boundary values here
 *         directly impacts gameplay feel and difficulty. */
void playerUpdate(void) {
  if (!player.active)
    return; /* Dead players don't update */

  /* Handle Loss of Control (oil puddle debuff) */
  if (player.lossControlTimer > 0) {
    player.lossControlTimer--; /* Count down each frame */
    /* All inputs disabled while timer > 0 */
  } else {
    /* Movement (WASD) — each key adds PLAYER_SPEED pixels per frame */
    if (isKeyPressed('w') || isKeyPressed('W'))
      player.y += PLAYER_SPEED; /* Move up */
    if (isKeyPressed('s') || isKeyPressed('S'))
      player.y -= PLAYER_SPEED; /* Move down */
    if (isKeyPressed('a') || isKeyPressed('A'))
      player.x -= PLAYER_SPEED; /* Move left */
    if (isKeyPressed('d') || isKeyPressed('D'))
      player.x += PLAYER_SPEED; /* Move right */

    /* Boundary Clamping — prevent the car from leaving the road */
    if (player.x < ROAD_LEFT_LIMIT)
      player.x = ROAD_LEFT_LIMIT; /* Left wall */
    if (player.x > ROAD_RIGHT_LIMIT - CAR_DRAW_W)
      player.x = ROAD_RIGHT_LIMIT - CAR_DRAW_W; /* Right wall */
    if (player.y < 0)
      player.y = 0; /* Bottom wall */
    if (player.y > 1080 - CAR_DRAW_H)
      player.y = 1080 - CAR_DRAW_H; /* Top wall */

    /* Rotation & Aiming: compute angle from car center to mouse cursor */
    float dx = (float)iMouseX - (player.x + CAR_DRAW_W / 2);
    float dy = (float)iMouseY - (player.y + CAR_DRAW_H / 2);

    player.shootAngle = atan2(dy, dx); /* Radians for bullet direction */
    float deg = player.shootAngle * 180.0f / PI; /* Convert to degrees */
    player.angle = deg;                          /* Store visual angle */
    /* Convert math angle (0°=right, CCW) to sprite frame index.
     * The sprite sheet has frame 0 = facing UP, and increments clockwise.
     * Formula: 90° - math_angle maps correctly. */
    player.spriteIndex = getFrameFromAngle(90.0f - deg);

    /* Continuous Fire Logic — auto-fires while isFiring is true */
    if (player.shootTimer > 0)
      player.shootTimer--; /* Count down cooldown */

    if (player.isFiring && player.shootTimer == 0) {
      playerShoot();                      /* Fire a bullet */
      player.shootTimer = SHOOT_COOLDOWN; /* Reset cooldown */
    }
  }

  /* Update all active Projectiles — move them and remove off-screen ones */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      projectiles[i].x += projectiles[i].dx; /* Apply X velocity */
      projectiles[i].y += projectiles[i].dy; /* Apply Y velocity */

      /* Deactivate if bullet goes off the 1920x1080 screen */
      if (projectiles[i].x < 0 || projectiles[i].x > 1920 ||
          projectiles[i].y < 0 || projectiles[i].y > 1080) {
        projectiles[i].active = false;
      }
    }
  }
}

/* ── draw ───────────────────────────────────────────────── */
/* playerDraw: Renders the player car and all active projectiles.
 *
 * HOW IT WORKS:
 * 1. Draws all active projectiles as 20x20 sprites centered on their position.
 * 2. If the player is active, draws the car using the sprite sheet grid.
 *    The correct rotation frame is selected by player.spriteIndex.
 *
 * HOW TO CHANGE:
 * - Projectile size: Change the 20, 20 values. Currently 20x20 pixels.
 * - Projectile centering: The -10 offset centers the 20px sprite on the
 * position.
 * - Car draw size: Uses CAR_DRAW_W/H (100x100). Change those constants.
 * EFFECT: Visual only — does not affect collision detection (collisions use
 *         separate width/height checks in Enemy.h). */
void playerDraw(void) {
  /* Draw all active Projectiles (20x20 sprites, centered on position) */
  for (unsigned int i = 0; i < projectiles.size(); i++) {
    if (projectiles[i].active) {
      iShowImage((int)projectiles[i].x - 10, (int)projectiles[i].y - 10, 20, 20,
                 texProjectile);
    }
  }

  if (!player.active)
    return; /* Don't draw the car if player is dead */

  int frame = player.spriteIndex; /* Get the rotation frame to display */

  /* Draw the car using the sprite sheet grid renderer */
  iShowImageGrid((int)player.x, (int)player.y, CAR_DRAW_W, CAR_DRAW_H, texCar,
                 frame, CAR_SHEET_ROWS, CAR_SHEET_COLS);
}

#endif /* PLAYER_H */
