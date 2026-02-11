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
#define ENEMY_SPEED_BASE 4.0f
#define ENEMY_FAST_SPEED 8.0f
#define ENEMY_MAX_HP 30
#define SPECIAL_MAX_HP 50
#define ENEMY_RAM_DMG 5
#define ENEMY_SELF_RAM 2
#define PROJECTILE_DMG 5

/*  Explosion Config (Grid 4x4)                                  */
#define EXPLOSION_ROWS 4
#define EXPLOSION_COLS 4
#define EXPLOSION_FRAMES 16
#define EXPLOSION_SIZE 128

/*  Special Effect Config (Grid 4x4 assumed)                     */
#define EFFECT_ROWS 4
#define EFFECT_COLS 4
#define EFFECT_FRAMES 16
#define EFFECT_SIZE 200

#define PUDDLE_W 80
#define PUDDLE_H 70
/* Road moves at 10.0f, puddles should match to look stationary on road */
#define ROAD_SCROLL_SPEED 10.0f

/* ── Enums ──────────────────────────────────────────────── */
enum EnemyType { TYPE_NORMAL, TYPE_SPECIAL1 };
enum EnemyState { S_CHASE, S_PRE_ATTACK, S_ATTACK, S_COOLDOWN };

/* ── Structs ────────────────────────────────────────────── */
struct Enemy {
  float x, y;
  float speed;
  int health;
  int frame;      /* For animation (0-2) */
  int frameTimer; /* To cycle frames */
  bool active;

  /* Special Enemy Fields */
  EnemyType type;
  EnemyState state;
  int stateTimer; /* General timer for behaviors */
  int animFrame;  /* For special effect animation */
};

enum StagePhase {
  PHASE_NORMAL,
  PHASE_WARNING,
  PHASE_TRUCK,
  PHASE_BOSS,
  PHASE_WIN
};

struct Boss {
  float x, y;
  int health;
  int maxHealth;
  int state; /* 0: Ram, 1: Retreat, 2: Wait, 3: Fire, 4: Wait */
  int timer;
  int missileTimer;
  bool active;
};

struct SupplyTruck {
  float x, y;
  int state; /* 0: Enter, 1: Animate, 2: Drop, 3: Leave */
  int timer;
  int animFrame;
  bool active;
};

struct PowerOrb {
  float x, y;
  bool active;
};

struct Missile { /* Player's Missile */
  float x, y;
  bool active;
};

struct BossMissile { /* Boss's Rocket */
  float x, y;
  float dx, dy;
  bool active;
};

struct Explosion {
  float x, y;
  int frameIndex;
  bool active;
};

struct Puddle {
  float x, y;
  bool active;
};

/* ── Globals ────────────────────────────────────────────── */
static StagePhase currentPhase = PHASE_NORMAL;
static Boss boss;
static SupplyTruck truck;
static PowerOrb orb;
static std::vector<Missile> playerMissiles;
static std::vector<BossMissile> bossMissiles;

static std::vector<Enemy> enemies;
static std::vector<Explosion> explosions;
static std::vector<Puddle> puddles;

static unsigned int texPolice[3];
static unsigned int texExplosion = 0;
static unsigned int texSpecialEnemy1 = 0;
static unsigned int texSpecialEffect = 0;
static unsigned int texOilPuddle = 0;
static unsigned int texBoss = 0;
static unsigned int texTruck = 0;
static unsigned int texPowerAnim = 0;
static unsigned int texPowerOrb = 0;
static unsigned int texTorpedo = 0;

/* Tracking for Special Spawn */
static int lastSpecialScore = 400; /* Start offset so 500 triggers first */

/* ── Helpers ────────────────────────────────────────────── */
static bool checkAABB(float x1, float y1, float w1, float h1, float x2,
                      float y2, float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

/* Helper to draw special effect grid */
static void iShowEffectGrid(int x, int y, int w, int h, unsigned int texture,
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

/* ── Init (load textures — call once) ────────────────────── */
void enemyInit(void) {
  texPolice[0] = iLoadImage("Asset/Police_Animation/Police 1.png");
  texPolice[1] = iLoadImage("Asset/Police_Animation/Police 2.png");
  texPolice[2] = iLoadImage("Asset/Police_Animation/Police 3.png");
  texExplosion = iLoadImage("Asset/Explosion.png");

  texSpecialEnemy1 = iLoadImage("Asset/Special Enemy 1.png");
  texSpecialEffect = iLoadImage("Asset/Special Effect.png");
  texOilPuddle = iLoadImage("Asset/Oil Puddle.png");

  /* Boss Stage Assets */
  texBoss = iLoadImage("Asset/Boss 1.png");
  texTruck = iLoadImage("Asset/truck.png");
  texPowerAnim = iLoadImage("Asset/Power Deployment.png");
  texPowerOrb = iLoadImage("Asset/Power Orb.png");
  texTorpedo = iLoadImage("Asset/torpedo.png");
}

/* ── Reset (state only — call on restart) ──────────────── */
void enemyReset(void) {
  enemies.clear();
  explosions.clear();
  puddles.clear();
  playerMissiles.clear();
  bossMissiles.clear();
  currentPhase = PHASE_NORMAL;
  lastSpecialScore = 400;

  /* Reset Boss state */
  boss.active = false;
  boss.health = 0;
  boss.maxHealth = 200;
  boss.state = 0;
  boss.timer = 0;
  boss.x = 0;
  boss.y = 0;

  /* Reset Truck state */
  truck.active = false;
  truck.state = 0;
  truck.timer = 0;
  truck.animFrame = 0;

  /* Reset Orb */
  orb.active = false;
}

/* ── Spawn Logic ────────────────────────────────────────── */
void enemySpawn(void) {
  /* 1. Normal Wave Logic */
  int maxEnemies = 3;
  if (player.score >= 250)
    maxEnemies = 5;

  int activeCount = 0;
  std::vector<int> freeSlots;

  for (unsigned int i = 0; i < enemies.size(); ++i) {
    if (enemies[i].active)
      activeCount++;
    else
      freeSlots.push_back((int)i);
  }

  if (activeCount < maxEnemies) {
    Enemy e;
    int range = ROAD_RIGHT_LIMIT - ENEMY_WIDTH - ROAD_LEFT_LIMIT;
    if (range < 1)
      range = 1;
    e.x = ROAD_LEFT_LIMIT + (float)(rand() % range);
    e.y = (float)(-ENEMY_HEIGHT - (rand() % 400));
    e.speed = ENEMY_SPEED_BASE + (rand() % 3);
    e.health = ENEMY_MAX_HP;
    e.frame = 0;
    e.frameTimer = 0;
    e.active = true;
    e.type = TYPE_NORMAL;
    e.state = S_CHASE; /* Not used for normal really */

    if (freeSlots.size() > 0) {
      enemies[freeSlots.back()] = e;
      freeSlots.pop_back();
    } else {
      enemies.push_back(e);
    }
  }

  /* 2. Special Enemy Spawn Logic */
  /* "After 500 points for every 100 points spawn 2 special enemy types" */
  if (player.score >= 500) {
    if (player.score - lastSpecialScore >= 100) {
      /* Trigger Spawn */
      lastSpecialScore += 100; /* Advance threshold */

      /* Spawn 2 */
      for (int k = 0; k < 2; k++) {
        Enemy e;
        int range = ROAD_RIGHT_LIMIT - ENEMY_WIDTH - ROAD_LEFT_LIMIT;
        if (range < 1)
          range = 1;
        e.x = ROAD_LEFT_LIMIT + (float)(rand() % range);
        e.y = (float)(-ENEMY_HEIGHT - 200 - (k * 150)); /* Staggered */
        e.speed = ENEMY_SPEED_BASE;                     /* Start simple */
        e.health = SPECIAL_MAX_HP;
        e.frame = 0;
        e.frameTimer = 0;
        e.active = true;
        e.type = TYPE_SPECIAL1;
        e.state = S_CHASE;
        e.stateTimer = 0;
        e.animFrame = 0;

        enemies.push_back(e);
      }
    }
  }
}

/* ── Update ─────────────────────────────────────────────── */
void enemyUpdate(void) {
  /* ── Explosions (always tick, regardless of phase) ──── */
  for (unsigned int i = 0; i < explosions.size(); i++) {
    if (explosions[i].active) {
      explosions[i].frameIndex++;
      if (explosions[i].frameIndex >= EXPLOSION_FRAMES)
        explosions[i].active = false;
    }
  }

  /* ── Boss Stage Logic ────────────────────────────── */
  if (currentPhase == PHASE_NORMAL && player.score >= 1000) {
    currentPhase = PHASE_WARNING;
  }

  if (currentPhase == PHASE_WARNING) {
    /* Clear Enemies */
    bool allClear = true;
    for (unsigned int i = 0; i < enemies.size(); i++) {
      if (enemies[i].active) {
        enemies[i].y += 15.0f; /* Accelerate out FAST */
        enemies[i].x += (enemies[i].x < 960) ? -5.0f : 5.0f;
        if (enemies[i].y > 1200)
          enemies[i].active = false; /* Fix: Deactivate off-screen */
        allClear = false;
      }
    }
    /* Clear Puddles too */
    for (unsigned int i = 0; i < puddles.size(); i++)
      puddles[i].active = false;

    if (allClear) {
      currentPhase = PHASE_TRUCK;
      truck.active = true;
      truck.x = 960 - 128; /* Center (256w) */
      truck.y = -300;      /* Start below screen */
      truck.state = 0;     /* Enter */
      truck.timer = 0;
      truck.animFrame = 0;
    }
    return; /* Skip normal spawn */
  }

  if (currentPhase == PHASE_TRUCK) {
    truck.timer++;
    switch (truck.state) {
    case 0:                            /* Enter */
      truck.y += 12.0f;                /* Move UP fast */
      if (truck.y >= player.y + 300) { /* Stop ahead */
        truck.state = 1;               /* Animate */
        truck.timer = 0;
      }
      break;
    case 1: /* Animate Power Deployment */
      if (truck.timer % 5 == 0)
        truck.animFrame++;
      if (truck.animFrame >= 16) { /* 16 frames (4x4 grid) */
        truck.state = 2;           /* Drop */
        truck.timer = 0;
      }
      break;
    case 2: /* Drop Orb */
      orb.active = true;
      orb.x = truck.x + 128 - 50; /* Center Orb (100w) on truck */
      orb.y = truck.y + 128;      /* Drop at truck center height */

      truck.state = 3; /* Leave */
      break;
    case 3:             /* Leave */
      truck.y += 20.0f; /* Zoom out */
      if (truck.y > 1200) {
        truck.active = false;
      }
      break;
    }

    /* Orb Logic */
    if (orb.active) {
      orb.y -= ROAD_SCROLL_SPEED; /* Move with road */
      if (checkAABB(orb.x, orb.y, 100, 100, player.x, player.y, CAR_DRAW_W,
                    CAR_DRAW_H)) {
        /* Picked up */
        orb.active = false;
        player.missileCount = 5;
      } else if (orb.y < -150) {
        /* Missed */
        orb.active = false;
        /* Respawn Truck */
        truck.active = true;
        truck.y = -300;
        truck.state = 0;
        truck.timer = 0;
        truck.animFrame = 0;
      }
    }

    /* If truck gone and orb gone (picked up), start Boss */
    if (!truck.active && !orb.active) {
      currentPhase = PHASE_BOSS;
      boss.active = true;
      boss.x = 960 - 125; /* Center (250w) */
      boss.y = 1200;      /* Top */
      if (boss.health <= 0)
        boss.health = 200;
      boss.maxHealth = 200;
      boss.state = 0; /* Ram */
      boss.timer = 0;
    }
    return;
  }

  if (currentPhase == PHASE_BOSS) {
    /* ── Resupply Truck (sub-state during boss fight) ──── */
    if (player.missileCount == 0 && !truck.active && !orb.active) {
      bool activeMissiles = false;
      for (unsigned int m = 0; m < playerMissiles.size(); m++)
        if (playerMissiles[m].active)
          activeMissiles = true;

      if (!activeMissiles) {
        /* Spawn Truck — boss stays active */
        truck.active = true;
        truck.x = 960 - 128;
        truck.y = -300;
        truck.state = 0;
        truck.timer = 0;
        truck.animFrame = 0;
      }
    }

    /* Truck Update (during boss fight) */
    if (truck.active) {
      truck.timer++;
      switch (truck.state) {
      case 0: /* Enter — go to a safe spot (away from boss) */
        truck.y += 12.0f;
        if (truck.y >= 200) { /* Lower area, away from boss */
          truck.state = 1;
          truck.timer = 0;
        }
        break;
      case 1: /* Animate */
        if (truck.timer % 5 == 0)
          truck.animFrame++;
        if (truck.animFrame >= 16) {
          truck.state = 2;
          truck.timer = 0;
        }
        break;
      case 2: /* Drop Orb */
        orb.active = true;
        orb.x = truck.x + 128 - 50;
        orb.y = truck.y + 128;
        truck.state = 3;
        break;
      case 3: /* Leave */
        truck.y += 20.0f;
        if (truck.y > 1200)
          truck.active = false;
        break;
      }
    }

    /* Orb Logic (during boss fight) */
    if (orb.active) {
      orb.y -= ROAD_SCROLL_SPEED;
      if (checkAABB(orb.x, orb.y, 100, 100, player.x, player.y, CAR_DRAW_W,
                    CAR_DRAW_H)) {
        orb.active = false;
        player.missileCount = 5;
      } else if (orb.y < -150) {
        orb.active = false;
        /* Respawn Truck */
        truck.active = true;
        truck.x = 960 - 128;
        truck.y = -300;
        truck.state = 0;
        truck.timer = 0;
        truck.animFrame = 0;
      }
    }
    /* Player Missiles */
    for (unsigned int m = 0; m < playerMissiles.size(); m++) {
      if (playerMissiles[m].active) {
        playerMissiles[m].y += 15.0f;
        if (playerMissiles[m].y > 1080)
          playerMissiles[m].active = false;

        /* Boss Collision */
        if (boss.active && checkAABB(playerMissiles[m].x, playerMissiles[m].y,
                                     40, 40, boss.x, boss.y, 250, 250)) {
          boss.health -= 20;
          playerMissiles[m].active = false;
        }
      }
    }

    /* Boss Missiles */
    for (unsigned int bm = 0; bm < bossMissiles.size(); bm++) {
      if (bossMissiles[bm].active) {
        bossMissiles[bm].x += bossMissiles[bm].dx;
        bossMissiles[bm].y += bossMissiles[bm].dy;

        if (bossMissiles[bm].y < -50 || bossMissiles[bm].x < -50 ||
            bossMissiles[bm].x > 1920)
          bossMissiles[bm].active = false;

        /* Player Hit */
        if (player.active &&
            checkAABB(bossMissiles[bm].x, bossMissiles[bm].y, 40, 40, player.x,
                      player.y, CAR_DRAW_W, CAR_DRAW_H)) {
          player.health -= 10;
          bossMissiles[bm].active = false;
        }
      }
    }

    /* Boss Logic */
    boss.timer++;
    switch (boss.state) {
    case 0: /* Ram (5s = 300f) */
      /* Move towards player (slow enough to be evadable) */
      if (boss.y > player.y)
        boss.y -= 3.0f;
      if (boss.x < player.x)
        boss.x += 2.0f;
      if (boss.x > player.x)
        boss.x -= 2.0f;

      if (boss.timer >= 300) {
        boss.state = 1; /* Retreat */
        boss.timer = 0;
      }
      break;
    case 1: /* Retreat/Wait (2s = 120f) */
      /* Move to top */
      if (boss.y < 800)
        boss.y += 5.0f;

      if (boss.timer >= 120) {
        boss.state = 2; /* Fire */
        boss.timer = 0;
        boss.missileTimer = 0;
      }
      break;
    case 2:                   /* Fire (Spawn 3) */
      if (boss.timer == 10) { /* Fire once */
        for (int k = -1; k <= 1; k++) {
          BossMissile bm;
          bm.x = boss.x + 125; /* Center of 250w boss */
          bm.y = boss.y;
          bm.active = true;

          /* Calc vector to player */
          float dx = player.x - bm.x;
          float dy = player.y - bm.y;
          float len = sqrt(dx * dx + dy * dy);
          if (len < 1)
            len = 1;

          /* Spread */
          float angle = atan2(dy, dx);
          angle += k * 0.3f; /* ~17 degrees spread */

          bm.dx = cos(angle) * 10.0f;
          bm.dy = sin(angle) * 10.0f;
          bossMissiles.push_back(bm);
        }
      }
      if (boss.timer >= 60) { /* Wait a bit */
        boss.state = 3;       /* Wait */
        boss.timer = 0;
      }
      break;
    case 3: /* Wait (2s) */
      if (boss.timer >= 120) {
        boss.state = 0; /* Loop to Ram */
        boss.timer = 0;
      }
      break;
    }

    /* Boss Collision (Ramming Logic) */
    if (player.active && checkAABB(boss.x, boss.y, 250, 250, player.x, player.y,
                                   CAR_DRAW_W, CAR_DRAW_H)) {
      /* Boss Rams Player */
      if (boss.state == 0) { /* Only damage if ramming? Or always collision? "If
                                the boss successfully rams the player..." */
        player.health -= 10;
        /* Push player? */
        player.y -= 50;
      }
    }

    /* Cannon Hits Boss */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, boss.x, boss.y,
                    250, 250)) {
        projectiles[p].active = false;
        boss.health -= 1;
      }
    }

    /* Boss Death */
    if (boss.health <= 0) {
      player.score += 1000;
      boss.active = false;
      boss.health = 0;
      currentPhase = PHASE_WIN; /* Trigger Win */
    }

    return;
  }

  /* Explosions are now updated at the top of enemyUpdate() */

  /* Puddles */
  for (unsigned int i = 0; i < puddles.size(); i++) {
    if (puddles[i].active) {
      puddles[i].y -= ROAD_SCROLL_SPEED; /* Move with road */
      if (puddles[i].y < -PUDDLE_H)
        puddles[i].active = false;

      /* Collision with Player */
      if (checkAABB(puddles[i].x, puddles[i].y, PUDDLE_W, PUDDLE_H, player.x,
                    player.y, CAR_DRAW_W, CAR_DRAW_H)) {
        /* Trigger Loss of Control */
        player.lossControlTimer = 120; /* 2 Seconds */
        puddles[i].active = false;     /* Splash/Consume */
      }
    }
  }

  enemySpawn();

  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (!enemies[i].active)
      continue;

    if (enemies[i].type == TYPE_NORMAL) {
      /* Normal Logic: Move Down + Mild Ramming */
      enemies[i].y += enemies[i].speed;

      /* Try to Ram (Mild tracking) */
      /* Only if roughly near player Y or approaching? */
      /* Just track X gently. 1.0f speed vs Player 5.0f -> Evadable. */
      if (enemies[i].x < player.x)
        enemies[i].x += 1.0f;
      if (enemies[i].x > player.x)
        enemies[i].x -= 1.0f;

      if (enemies[i].y > 1080)
        enemies[i].active = false;
    } else if (enemies[i].type == TYPE_SPECIAL1) {
      /* Special Logic */
      enemies[i].stateTimer++;

      switch (enemies[i].state) {
      case S_CHASE:
      case S_COOLDOWN:
        /* Hover Logic: Stay "just outside hitbox" (approx 110px).
         * Hitbox is 100x100.
         * Target Y: player.y - 110.
         */
        if (enemies[i].y < player.y - 110)
          enemies[i].y += (ENEMY_SPEED_BASE + 2.0f);
        else if (enemies[i].y > player.y - 100)
          enemies[i].y -= 2.0f; /* Too close, back off */
        else
          enemies[i].y += 0; /* Perfect hover */

        /* Follow Behavior (X-Axis) */
        /* Just follow the player, don't jitter or ram aggressively */
        if (enemies[i].x < player.x - 10)
          enemies[i].x += 1.5f;
        else if (enemies[i].x > player.x + 10)
          enemies[i].x -= 1.5f;

        /* Timer: 5s (300 frames) -> PRE_ATTACK */
        if (enemies[i].stateTimer >= 300) {
          enemies[i].state = S_PRE_ATTACK;
          enemies[i].stateTimer = 0;
        }
        break;

      case S_PRE_ATTACK:
        /* Move to front of player */
        /* Target: x = player.x, y = player.y + 300 */
        if (enemies[i].y < player.y + 300)
          enemies[i].y += ENEMY_FAST_SPEED;

        /* X Tracking (slowly) */
        if (enemies[i].x < player.x)
          enemies[i].x += 3.0f;
        if (enemies[i].x > player.x)
          enemies[i].x -= 3.0f;

        if (enemies[i].y >= player.y + 300) {
          enemies[i].state = S_ATTACK;
          enemies[i].stateTimer = 0;
          enemies[i].animFrame = 0;
        }
        break;

      case S_ATTACK:
        /* Play Animation */
        /* Used stateTimer for frame pacing? */
        if (enemies[i].stateTimer % 5 == 0)
          enemies[i].animFrame++;

        if (enemies[i].animFrame >= EFFECT_FRAMES) {
          /* Done. Deploy. */
          Puddle p;
          p.x = enemies[i].x + ENEMY_WIDTH / 2 - PUDDLE_W / 2;
          p.y = enemies[i].y;
          p.active = true;
          puddles.push_back(p);

          enemies[i].state = S_COOLDOWN;
          enemies[i].stateTimer = 0;
        }
        break;
      }

      /* Special Enemy stays on screen unless destroyed */
      /* Boundaries? */
      if (enemies[i].x < ROAD_LEFT_LIMIT)
        enemies[i].x = ROAD_LEFT_LIMIT;
      if (enemies[i].x > ROAD_RIGHT_LIMIT - ENEMY_WIDTH)
        enemies[i].x = ROAD_RIGHT_LIMIT - ENEMY_WIDTH;
    }

    /* Animation (Frame Cycle) */
    enemies[i].frameTimer++;
    if (enemies[i].frameTimer >= 10) {
      enemies[i].frame = (enemies[i].frame + 1) % 3;
      enemies[i].frameTimer = 0;
    }

    /* Collisions */
    /* Vs Player */
    if (player.active &&
        checkAABB(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_WIDTH,
                  player.x, player.y, CAR_DRAW_W, CAR_DRAW_H)) {
      player.health -= ENEMY_RAM_DMG;
      enemies[i].health -= 2;
      enemies[i].y -= 20; /* Bounce */
    }

    /* Vs Projectiles */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, enemies[i].x,
                    enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {

        projectiles[p].active = false;
        enemies[i].health -= PROJECTILE_DMG;

        if (enemies[i].health <= 0) {
          Explosion ex;
          ex.x = enemies[i].x + ENEMY_WIDTH / 2 - EXPLOSION_SIZE / 2;
          ex.y = enemies[i].y + ENEMY_HEIGHT / 2 - EXPLOSION_SIZE / 2;
          ex.frameIndex = 0;
          ex.active = true;
          explosions.push_back(ex);

          enemies[i].active = false;
          player.score += (enemies[i].type == TYPE_SPECIAL1) ? 50 : 10;
          break;
        }
      }
    }
  }
}

/* ── Draw ───────────────────────────────────────────────── */
void enemyDraw(void) {
  /* Puddles */
  for (unsigned int i = 0; i < puddles.size(); i++) {
    if (puddles[i].active) {
      iShowImage((int)puddles[i].x, (int)puddles[i].y, PUDDLE_W, PUDDLE_H,
                 texOilPuddle);
    }
  }

  /* Enemies */
  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (enemies[i].active) {
      if (enemies[i].type == TYPE_NORMAL) {
        iShowImage((int)enemies[i].x, (int)enemies[i].y, ENEMY_WIDTH,
                   ENEMY_HEIGHT, texPolice[enemies[i].frame]);
      } else {
        iShowImage((int)enemies[i].x, (int)enemies[i].y, ENEMY_WIDTH,
                   ENEMY_HEIGHT, texSpecialEnemy1);

        /* Draw Effect if Attacking */
        if (enemies[i].state == S_ATTACK) {
          iShowEffectGrid((int)enemies[i].x - 50, (int)enemies[i].y - 50,
                          EFFECT_SIZE, EFFECT_SIZE, texSpecialEffect,
                          enemies[i].animFrame, EFFECT_ROWS, EFFECT_COLS);
        }
      }
    }
  }

  /* Truck (draws in PHASE_TRUCK or during boss resupply) */
  if (truck.active) {
    iShowImage((int)truck.x, (int)truck.y, 256, 256, texTruck);

    if (truck.state == 1) { /* Animate */
      /* Center 256x256 animation on the 256x256 truck */
      int animX = (int)truck.x; /* Same position as truck */
      int animY = (int)truck.y;
      iShowEffectGrid(animX, animY, 256, 256, texPowerAnim, truck.animFrame, 4,
                      4);
    }
  }

  if (orb.active) {
    iShowImage((int)orb.x, (int)orb.y, 100, 100, texPowerOrb);
  }

  if (currentPhase == PHASE_BOSS && boss.active) {
    iShowImage((int)boss.x, (int)boss.y, 250, 250, texBoss);

    /* Boss Missiles */
    for (unsigned int bm = 0; bm < bossMissiles.size(); bm++) {
      if (bossMissiles[bm].active) {
        iShowImage((int)bossMissiles[bm].x, (int)bossMissiles[bm].y, 40, 40,
                   texTorpedo);
      }
    }
  }

  /* Player Missiles (always draw, even during truck resupply) */
  if (currentPhase == PHASE_BOSS) {
    for (unsigned int m = 0; m < playerMissiles.size(); m++) {
      if (playerMissiles[m].active) {
        iShowImage((int)playerMissiles[m].x, (int)playerMissiles[m].y, 40, 40,
                   texTorpedo);
      }
    }
  }

  /* Explosions */
  for (unsigned int i = 0; i < explosions.size(); i++) {
    if (explosions[i].active) {
      iShowEffectGrid((int)explosions[i].x, (int)explosions[i].y,
                      EXPLOSION_SIZE, EXPLOSION_SIZE, texExplosion,
                      explosions[i].frameIndex, EXPLOSION_ROWS, EXPLOSION_COLS);
    }
  }
}

#endif /* ENEMY_H */

/* ── Helpers ────────────────────────────────────────────── */
StagePhase getPhase() { return currentPhase; }

int getBossHealth() { return boss.health; }

void firePlayerMissile() {
  if (player.missileCount > 0) {
    Missile m;
    m.x = player.x; // + width/2?
    m.y = player.y; // + height/2?
    /* Center missile */
    m.x += (CAR_DRAW_W / 2) - 20;
    m.y += (CAR_DRAW_H / 2);
    m.active = true;
    playerMissiles.push_back(m);
    player.missileCount--;
  }
}
