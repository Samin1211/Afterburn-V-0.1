#ifndef ENEMY_H
#define ENEMY_H

#include "Player.h"
#include <cstdlib>
#include <vector>


/* ══════════════════════════════════════════════════════════
 *  ENEMY & EXPLOSION MODULE
 *  Management of enemies, interactions, and FX.
 *  Dependencies: Player.h (for player/projectile globals)
 * ══════════════════════════════════════════════════════════ */

/* ── Config ─────────────────────────────────────────────── */
#define ENEMY_WIDTH 100
#define ENEMY_HEIGHT 100
#define ENEMY_SPEED_BASE 4.0f /* Slightly faster to catch up */
#define ENEMY_MAX_HP 30
#define ENEMY_RAM_DMG 5
#define ENEMY_SELF_RAM 2
#define PROJECTILE_DMG 5

/*  Explosion Config (Grid Assumption: 4x4)                      */
#define EXPLOSION_ROWS 4
#define EXPLOSION_COLS 4
#define EXPLOSION_FRAMES 16
#define EXPLOSION_SIZE 128

/* ── Structs ────────────────────────────────────────────── */
struct Enemy {
  float x, y;
  float speed;
  int health;
  int frame;      /* For animation (0-2) */
  int frameTimer; /* To cycle frames */
  bool active;
};

struct Explosion {
  float x, y;
  int frameIndex;
  bool active;
};

/* ── Globals ────────────────────────────────────────────── */
static std::vector<Enemy> enemies;
static std::vector<Explosion> explosions;
static unsigned int texPolice[3];
static unsigned int texExplosion = 0;

/* ── Helpers ────────────────────────────────────────────── */
static bool checkAABB(float x1, float y1, float w1, float h1, float x2,
                      float y2, float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

/* ── Init ───────────────────────────────────────────────── */
void enemyInit(void) {
  texPolice[0] = iLoadImage("Asset/Police_Animation/Police 1.png");
  texPolice[1] = iLoadImage("Asset/Police_Animation/Police 2.png");
  texPolice[2] = iLoadImage("Asset/Police_Animation/Police 3.png");
  texExplosion = iLoadImage("Asset/Explosion.png");

  enemies.clear();
  explosions.clear();
}

/* ── Spawn Logic ────────────────────────────────────────── */
void enemySpawn(void) {
  /* Determine Spawn Cap based on Score */
  int maxEnemies = 3;
  if (player.score >= 250) {
    maxEnemies = 5;
  }

  /* Count active enemies to decide if we need to spawn */
  int activeCount = 0;
  /* Also track indices of inactive slots to reuse memory */
  std::vector<int> freeSlots;

  for (unsigned int i = 0; i < enemies.size(); ++i) {
    if (enemies[i].active) {
      activeCount++;
    } else {
      freeSlots.push_back((int)i);
    }
  }

  /* Spawn until we reach the cap */
  /* User requirement: "if destroy one, another appears to complete count"
   * This implies we should fill up to maxEnemies immediately.
   * To prevent instant overlap, we can randomize X and add slight speed
   * variance. We don't want them all on top of each other. But if we must
   * maintin count, we spawn. Collision logic handles overlap? Or we simply
   * spawn them at y = -height - random_offset.
   */
  if (activeCount < maxEnemies) {
    int needed = maxEnemies - activeCount;

    /* Initialize new enemy state */
    Enemy e; /* Temp template */
    e.x = (float)(rand() % (1920 - ENEMY_WIDTH));
    /* Spawn from BEHIND (Bottom) */
    /* Add random offset to avoid stacking if multiple spawn at once */
    e.y = (float)(-ENEMY_HEIGHT - (rand() % 300));
    e.speed = ENEMY_SPEED_BASE + (rand() % 4); /* Speed 4-7 */
    e.health = ENEMY_MAX_HP;
    e.frame = 0;
    e.frameTimer = 0;
    e.active = true;

    if (freeSlots.size() > 0) {
      /* Reuse slot */
      enemies[freeSlots.back()] = e;
      freeSlots.pop_back();
    } else {
      /* Add new */
      enemies.push_back(e);
    }
  }
}

/* ── Update ─────────────────────────────────────────────── */
void enemyUpdate(void) {
  /* 1. Update Explosions */
  for (unsigned int i = 0; i < explosions.size(); i++) {
    if (explosions[i].active) {
      explosions[i].frameIndex++;
      if (explosions[i].frameIndex >= EXPLOSION_FRAMES) {
        explosions[i].active = false;
      }
    }
  }

  /* 2. Spawn Logic (Check and fill quota) */
  enemySpawn();

  /* 3. Update Enemies */
  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (!enemies[i].active)
      continue;

    /* Move UP (from behind) */
    enemies[i].y += enemies[i].speed;

    /* Remove if off screen via TOP */
    if (enemies[i].y > 1080) {
      enemies[i].active = false;
      continue;
    }

    /* Animation */
    enemies[i].frameTimer++;
    if (enemies[i].frameTimer >= 10) {
      enemies[i].frame = (enemies[i].frame + 1) % 3;
      enemies[i].frameTimer = 0;
    }

    /* ── Collision: Player ───────────────────────────── */
    if (player.active &&
        checkAABB(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT,
                  player.x, player.y, CAR_DRAW_W, CAR_DRAW_H)) {
      /* Collision! */
      player.health -= ENEMY_RAM_DMG;
      enemies[i].health -= 2;

      /* Push back? If Enemy moving UP and hits Player, enemy is behind.
       * Should we push enemy DOWN (y -= bump)? */
      enemies[i].y -= 20;
    }

    /* ── Collision: Projectiles ──────────────────────── */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, enemies[i].x,
                    enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {

        projectiles[p].active = false;
        enemies[i].health -= PROJECTILE_DMG;

        if (enemies[i].health <= 0) {
          /* Boom */
          Explosion ex;
          ex.x = enemies[i].x + ENEMY_WIDTH / 2 - EXPLOSION_SIZE / 2;
          ex.y = enemies[i].y + ENEMY_HEIGHT / 2 - EXPLOSION_SIZE / 2;
          ex.frameIndex = 0;
          ex.active = true;
          explosions.push_back(ex);

          enemies[i].active = false;
          player.score += 10;
          break;
        }
      }
    }
  }
}

/* ── Draw ───────────────────────────────────────────────── */
void enemyDraw(void) {
  /* Draw Enemies */
  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (enemies[i].active) {
      iShowImage((int)enemies[i].x, (int)enemies[i].y, ENEMY_WIDTH,
                 ENEMY_HEIGHT, texPolice[enemies[i].frame]);
    }
  }

  /* Draw Explosions */
  for (unsigned int i = 0; i < explosions.size(); i++) {
    if (explosions[i].active) {
      int r = explosions[i].frameIndex / EXPLOSION_COLS;
      int c = explosions[i].frameIndex % EXPLOSION_COLS;

      float cellW = 1.0f / EXPLOSION_COLS;
      float cellH = 1.0f / EXPLOSION_ROWS;
      float uLeft = c * cellW;
      float uRight = uLeft + cellW;
      float vTop = -1.0f + r * cellH;
      float vBottom = vTop + cellH;

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texExplosion);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glBegin(GL_QUADS);
      glTexCoord2f(uLeft, vBottom);
      glVertex2f(explosions[i].x, explosions[i].y);
      glTexCoord2f(uRight, vBottom);
      glVertex2f(explosions[i].x + EXPLOSION_SIZE, explosions[i].y);
      glTexCoord2f(uRight, vTop);
      glVertex2f(explosions[i].x + EXPLOSION_SIZE,
                 explosions[i].y + EXPLOSION_SIZE);
      glTexCoord2f(uLeft, vTop);
      glVertex2f(explosions[i].x, explosions[i].y + EXPLOSION_SIZE);
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
  }
}

#endif /* ENEMY_H */
