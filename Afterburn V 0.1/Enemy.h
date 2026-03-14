#ifndef ENEMY_H
#define ENEMY_H

#include "NPC.h"

/* Forward declaration (defined in Road.h, included after Enemy.h) */
void roadChangeStage(char path[]);

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
/* PROJECTILE_DMG is in NPC.h */

/* Explosion Config is in NPC.h */

/*  Special Effect Config (Grid 4x4 assumed)                     */
#define EFFECT_ROWS 4
#define EFFECT_COLS 4
#define EFFECT_FRAMES 16
#define EFFECT_SIZE 200

#define PUDDLE_W 80
#define PUDDLE_H 70
/* ROAD_SCROLL_SPEED is in NPC.h */

/*  Boss2 Config                                                 */
#define BOSS2_W 250
#define BOSS2_H 250
#define BOSS2_FIRE_COOLDOWN 10
#define BOSS2_MOVE_SPEED 3.5f
#define BOSS2_PROJ_SPEED 12.0f
#define BOSS2_MISSILE_SPEED 18.0f

/* ── Enums ──────────────────────────────────────────────── */
enum EnemyType { TYPE_NORMAL, TYPE_SPECIAL1, TYPE_SPECIAL2 };
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
  PHASE_HEALTH_TRUCK,
  PHASE_CLOUD_IN,
  PHASE_CLOUD_OUT,
  PHASE_WARNING2,
  PHASE_TRUCK2,
  PHASE_BOSS2,
  PHASE_WIN
};

/* Boss struct: Stage 1 boss (police boss car, 200 HP).
 * state: 0=Ram (chase player 5s), 1=Retreat (move to top 2s),
 *        2=Fire (3 spread missiles), 3=Wait (2s), 4=Dying (3s explosions).
 * HOW TO CHANGE: Modify maxHealth to make boss harder/easier. */
struct Boss {
  float x, y;
  int health;
  int maxHealth;
  int state; /* 0: Ram, 1: Retreat, 2: Wait, 3: Fire, 4: Dying */
  int timer;
  int missileTimer;
  bool active;
};

/* Boss2 struct: Stage 2 tank boss (300 HP, rotatable turret).
 * state: 0=Move randomly + fire projectiles (5s), 1=Retreat to top (2s),
 *        2=Fire 3 sequential homing missiles, 3=Wait (2s), 4=Dying.
 * turretAngle/Frame: Turret always aims at the player using sprite sheet.
 * HOW TO CHANGE: Modify maxHealth (300), missile count, or state durations. */
struct Boss2 {
  float x, y;
  int health;
  int maxHealth;
  int state; /* 0:Move+Fire, 1:Retreat, 2:SeqMissiles, 3:Wait, 4:Dying */
  int timer;
  bool active;
  /* Turret */
  float turretAngle;
  int turretFrame;
  /* Firing */
  int fireTimer;
  /* Random Movement */
  float targetX, targetY;
  /* Sequential Missiles */
  int missilesFired;
  int missileDelay;
};

/* SupplyTruck: Delivers power/health orbs during boss fights.
 * state: 0=Enter (scroll in), 1=Animate (deployment animation),
 *        2=Drop (spawn orb), 3=Leave (scroll out).
 * HOW TO CHANGE: Adjust movement speeds in the state machine code. */
struct SupplyTruck {
  float x, y;
  int state; /* 0: Enter, 1: Animate, 2: Drop, 3: Leave */
  int timer;
  int animFrame;
  bool active;
};

/* PowerOrb: Collectible orb dropped by supply trucks.
 * Gives missiles when picked up (5 for Boss1, 10 for Boss2). */
struct PowerOrb {
  float x, y;
  bool active;
};

/* Missile: Player's heavy missile (right-click to fire, moves straight up).
 * Deals 20 damage to Boss1, 15 to Boss2. Limited ammo from orb pickups. */
struct Missile { /* Player's Missile */
  float x, y;
  bool active;
};

/* BossMissile: Projectile fired by bosses (directional, has velocity vector).
 * Used for Boss1's spread shots, Boss2's auto-fire, and Boss2's homing
 * missiles. */
struct BossMissile { /* Boss's Rocket */
  float x, y;
  float dx, dy;
  bool active;
};

/* Explosion struct is in NPC.h */

/* Puddle: Oil puddle dropped by Special Enemy 1 after its attack animation.
 * Scrolls with the road. Player touching it triggers 2-second loss of control.
 * HOW TO CHANGE: Modify lossControlTimer value (120 frames) in the collision
 * code. */
struct Puddle {
  float x, y;
  bool active;
};

/* NPC struct is in NPC.h */

/* ── Globals ────────────────────────────────────────────── */
static StagePhase currentPhase = PHASE_NORMAL;
static Boss boss;
static SupplyTruck truck;
static PowerOrb orb;
static std::vector<Missile> playerMissiles;
static std::vector<BossMissile> bossMissiles;

static std::vector<Enemy> enemies;
static std::vector<Puddle> puddles;

/* NPC Config is in NPC.h */

static unsigned int texPolice[3];
/* texExplosion is in NPC.h */
static unsigned int texSpecialEnemy1 = 0;
static unsigned int texSpecialEnemy2 = 0;
static unsigned int texSpecialEffect = 0;
static unsigned int texOilPuddle = 0;
static unsigned int texBoss = 0;
static unsigned int texTruck = 0;
static unsigned int texPowerAnim = 0;
static unsigned int texPowerOrb = 0;
static unsigned int texTorpedo = 0;
static unsigned int texHealthAnim = 0;
static unsigned int texHealthOrb = 0;
/* texNPC is in NPC.h */
static unsigned int texCloudLeft = 0;
static unsigned int texCloudRight = 0;
static unsigned int texTank = 0;
static float cloudLeftX = -1920.0f;
static float cloudRightX = 1920.0f;
#define CLOUD_SPEED 30.0f

/* Tracking for Special Enemy Spawning */
/* lastSpecialScore: Score checkpoint for spawning special enemies.
 * Starts at 400 so first special spawn triggers at score 500 (400+100).
 * Every time score increases by 100 past this, new specials spawn.
 * HOW TO CHANGE: Change initial value or the += 100 increment in enemySpawn().
 */
static int lastSpecialScore = 400;
static int truckDelayTimer = 0; /* Delay counter before resupply truck spawns */
static int bossDeathTimer = 0;  /* Frames since Boss1 entered dying state */
static SupplyTruck
    healthTruck; /* Health truck (post-Boss1, drops health orb) */
static int special2SpawnTimer = 0; /* Cooldown for Special Enemy 2 spawning */
static PowerOrb healthOrb;         /* Health orb pickup (restores full HP) */

/* Boss2 Globals */
static Boss2 boss2;                               /* Stage 2 boss instance */
static std::vector<BossMissile> boss2Projectiles; /* Boss2 auto-fire bullets */
static std::vector<BossMissile> boss2Missiles;    /* Boss2 homing missiles */
static int boss2DeathTimer = 0; /* Frames since Boss2 entered dying state */
static int boss2TruckDelayTimer = 0; /* Delay before Boss2 resupply truck */

/* Helper to check if boss is in dying animation (for Game Over prevention).
 * During dying animation, the player should not get a Game Over even if HP<=0.
 */
bool isBossDying() { return boss.active && boss.state == 4; }
bool isBoss2Dying() { return boss2.active && boss2.state == 4; }

/* checkAABB is in NPC.h */

/* iShowEffectGrid: Draws a single frame from a sprite sheet grid (same as
 * iShowImageGrid but used for special effects like oil attack and explosions).
 * HOW IT WORKS: Computes UV coordinates for the given frame in the grid
 * and renders a textured quad at (x,y) with size (w,h).
 * USED BY: Special Enemy 1 attack animation, explosion rendering, truck
 * deployment animation. */
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
/* enemyInit: Loads ALL enemy, boss, truck, orb, cloud, and effect textures.
 * Called once at startup from gameInit(). Police cars use 3 animation frames.
 * HOW TO CHANGE: Replace file paths to use different artwork.
 * EFFECT: Changes visuals of all enemies and boss entities. */
void enemyInit(void) {
  texPolice[0] = iLoadImage("Asset/Police_Animation/Police 1.png");
  texPolice[1] = iLoadImage("Asset/Police_Animation/Police 2.png");
  texPolice[2] = iLoadImage("Asset/Police_Animation/Police 3.png");
  /* texExplosion loaded in npcInit() */

  texSpecialEnemy1 = iLoadImage("Asset/Special Enemy 1.png");
  texSpecialEffect = iLoadImage("Asset/Special Effect.png");
  texOilPuddle = iLoadImage("Asset/Oil Puddle.png");

  /* Boss Stage Assets */
  texBoss = iLoadImage("Asset/Boss 1.png");
  texTruck = iLoadImage("Asset/truck.png");
  texPowerAnim = iLoadImage("Asset/Power Deployment.png");
  texPowerOrb = iLoadImage("Asset/Power Orb.png");
  texTorpedo = iLoadImage("Asset/torpedo.png");
  texHealthAnim = iLoadImage("Asset/Health Deployment.png");
  texHealthOrb = iLoadImage("Asset/Health Orb.png");
  /* NPC textures loaded in npcInit() */
  texSpecialEnemy2 = iLoadImage("Asset/NPC 5.png");
  texCloudLeft = iLoadImage("Asset/Level transition/cloud_left.png");
  texCloudRight = iLoadImage("Asset/Level transition/cloud_right.png");

  /* Boss2 Assets */
  texTank = iLoadImage("Asset/Tank.png");
}

/* ── Reset (state only — call on restart) ──────────────── */
/* enemyReset: Clears ALL enemies, bosses, trucks, orbs, puddles, missiles,
 * and NPCs for a fresh game. Resets phase to PHASE_NORMAL and all timers.
 * Called by gameReset() on restart. Does NOT reload textures. */
void enemyReset(void) {
  enemies.clear();
  npcReset(); /* clears npcs and explosions */
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

  /* Reset Timers */
  bossDeathTimer = 0;
  truckDelayTimer = 0;

  /* Reset Truck state */
  truck.active = false;
  truck.state = 0;
  truck.timer = 0;
  truck.animFrame = 0;

  /* Reset Orb */
  orb.active = false;

  /* Reset Health Truck/Orb */
  healthTruck.active = false;
  healthTruck.state = 0;
  healthTruck.timer = 0;
  healthTruck.animFrame = 0;
  healthOrb.active = false;
  npcs.clear(); /* NPC vector (defined in NPC.h) */
  special2SpawnTimer = 0;
  cloudLeftX = -1920.0f;
  cloudRightX = 1920.0f;

  /* Reset Boss2 */
  boss2.active = false;
  boss2.health = 0;
  boss2.maxHealth = 300;
  boss2.state = 0;
  boss2.timer = 0;
  boss2.x = 0;
  boss2.y = 0;
  boss2.turretAngle = 0;
  boss2.turretFrame = 0;
  boss2.fireTimer = 0;
  boss2.targetX = 960;
  boss2.targetY = 600;
  boss2.missilesFired = 0;
  boss2.missileDelay = 0;
  boss2Projectiles.clear();
  boss2Missiles.clear();
  boss2DeathTimer = 0;
  boss2TruckDelayTimer = 0;
}

/* ── Spawn Logic ────────────────────────────────────────── */
/* enemySpawn: Handles spawning for ALL enemy types each frame.
 * Normal: caps 3 (early) or 5 (late), random X on road.
 * Special1: 2 per 100pts after score 500 (1 in Stage 2).
 * NPCs: Stage 2 only via npcSpawn(). Special2: Stage 2, score>=2000.
 * HOW TO CHANGE: Modify maxEnemies, thresholds, rates for difficulty. */
void enemySpawn(void) {
  /* 1. Normal Wave Logic */
  int maxEnemies = 3;
  if ((!bossFightOver && player.score >= 250) ||
      (bossFightOver && player.score >= 1750))
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
  if ((!bossFightOver && player.score >= 500) ||
      (bossFightOver && player.score >= 2000)) {
    if (player.score - lastSpecialScore >= 100) {
      /* Trigger Spawn */
      lastSpecialScore += 100; /* Advance threshold */

      /* Spawn 2 in stage 1, 1 in stage 2 */
      int spawnCount = bossFightOver ? 1 : 2;
      for (int k = 0; k < spawnCount; k++) {
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

  /* 3. NPC Spawn Logic (Stage 2 only) */
  if (bossFightOver) {
    npcSpawn();
  }

  /* 4. Special Enemy 2 Spawn Logic (Stage 2, score >= 2000) */
  if (bossFightOver && player.score >= 2000) {
    /* Count active Special Enemy 2s */
    int se2Count = 0;
    for (unsigned int i = 0; i < enemies.size(); i++)
      if (enemies[i].active && enemies[i].type == TYPE_SPECIAL2)
        se2Count++;

    if (se2Count < 3) {
      special2SpawnTimer++;
      if (special2SpawnTimer >= 60) { /* 1 second delay */
        special2SpawnTimer = 0;

        Enemy e;
        int range = ROAD_RIGHT_LIMIT - ENEMY_WIDTH - ROAD_LEFT_LIMIT;
        if (range < 1)
          range = 1;
        /* Spawn at random X, but at least 200px away from player */
        e.x = ROAD_LEFT_LIMIT + (float)(rand() % range);
        for (int att = 0; att < 10; att++) {
          float dx = e.x - player.x;
          if (dx < 0)
            dx = -dx;
          if (dx >= 200)
            break;
          e.x = ROAD_LEFT_LIMIT + (float)(rand() % range);
        }
        e.y =
            1080.0f + (float)(rand() % 300); /* Start above screen (like NPC) */
        e.speed = ENEMY_SPEED_BASE;
        e.health = ENEMY_MAX_HP; /* Same HP as police */
        e.frame = 0;
        e.frameTimer = 0;
        e.active = true;
        e.type = TYPE_SPECIAL2;
        e.state = S_COOLDOWN; /* Phase 1: NPC-like scrolling */
        e.stateTimer = 0;
        e.animFrame = 0;
        enemies.push_back(e);
      }
    } else {
      special2SpawnTimer = 0;
    }
  }
}

/* ── Update ─────────────────────────────────────────────── */
/* enemyUpdate: THE MAIN GAME LOGIC. Called every frame by gameUpdate().
 * Controls ALL phase transitions, boss fights, truck sequences, enemy AI,
 * projectile collisions, puddle effects, and NPC updates.
 * See StagePhase enum for all phases this function manages. */
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
  if (currentPhase == PHASE_NORMAL && player.score >= 1000 && !bossFightOver) {
    currentPhase = PHASE_WARNING;
  }

  /* Stage 2 Boss trigger */
  if (currentPhase == PHASE_NORMAL && bossFightOver && player.score >= 2500) {
    currentPhase = PHASE_WARNING2;
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
      boss.health = 200;
      boss.maxHealth = 200;
      boss.state = 0; /* Ram */
      boss.timer = 0;
    }
    return;
  }

  if (currentPhase == PHASE_BOSS) {
    /* ── Resupply Truck (sub-state during boss fight) ──── */
    if (player.missileCount == 0 && !truck.active && !orb.active &&
        boss.active && boss.state != 4) {
      bool activeMissiles = false;
      for (unsigned int m = 0; m < playerMissiles.size(); m++)
        if (playerMissiles[m].active)
          activeMissiles = true;

      if (!activeMissiles) {
        truckDelayTimer++;
        if (truckDelayTimer > 300) { /* 5 Second Delay (60fps) */
          /* Spawn Truck — boss stays active */
          truck.active = true;
          truck.x = 960 - 128;
          truck.y = -300;
          truck.state = 0;
          truck.timer = 0;
          truck.animFrame = 0;
          truckDelayTimer = 0;
        }
      }
    } else {
      truckDelayTimer = 0;
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
        /* Truck respawn handled by main loop delay now */
      }
    }
    /* Player Missiles */
    for (unsigned int m = 0; m < playerMissiles.size(); m++) {
      if (playerMissiles[m].active) {
        playerMissiles[m].y += 15.0f;
        if (playerMissiles[m].y > 1080)
          playerMissiles[m].active = false;

        /* Boss Collision -- skip if boss is dying */
        if (boss.active && boss.state != 4 &&
            checkAABB(playerMissiles[m].x, playerMissiles[m].y, 40, 40, boss.x,
                      boss.y, 250, 250)) {
          boss.health -= 20;
          if (boss.health < 0)
            boss.health = 0;
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
      /* Move towards player (Aggr. 4.0/3.0) */
      if (boss.y > player.y)
        boss.y -= 4.0f;
      if (boss.x < player.x)
        boss.x += 3.0f;
      if (boss.x > player.x)
        boss.x -= 3.0f;

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
      break;
    case 4: /* Dying / Exploding */
      bossDeathTimer++;
      if (bossDeathTimer % 10 == 0) {
        /* Spawn standard explosion at random offset */
        Explosion e;
        e.x = boss.x + (rand() % 200);
        e.y = boss.y + (rand() % 200);
        e.frameIndex = 0;
        e.active = true;
        explosions.push_back(e);
      }
      if (bossDeathTimer > 180) { /* 3 Seconds */
        boss.active = false;
        boss.health = 0;
        currentPhase = PHASE_HEALTH_TRUCK;
        bossDeathTimer = 0;

        /* Start health truck */
        healthTruck.active = true;
        healthTruck.x = 960 - 128;
        healthTruck.y = -300;
        healthTruck.state = 0;
        healthTruck.timer = 0;
        healthTruck.animFrame = 0;
      }
      return; /* Skip collisions/attacks */
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

    /* Cannon Hits Boss -- skip if boss is dying */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active && boss.state != 4 &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, boss.x, boss.y,
                    250, 250)) {
        projectiles[p].active = false;
        boss.health -= 1;
        if (boss.health < 0)
          boss.health = 0;
      }
    }

    /* Boss Death */
    /* Boss Death Trigger */
    if (boss.health <= 0 && boss.state != 4) {
      player.score += 500;
      boss.state = 4; /* Enter dying state */
      boss.health = 0;
      bossDeathTimer = 0;
      bossMissiles.clear(); /* Remove active missiles */
      /* Don't set PHASE_WIN yet */
    }

    return;
  }

  if (currentPhase == PHASE_HEALTH_TRUCK) {
    /* Health Truck Update */
    if (healthTruck.active) {
      healthTruck.timer++;
      switch (healthTruck.state) {
      case 0: /* Enter -- position itself ahead of player */
        healthTruck.y += 12.0f;
        if (healthTruck.y >= player.y + 300) {
          healthTruck.state = 1; /* Animate */
          healthTruck.timer = 0;
        }
        break;
      case 1: /* Animate Health Deployment (4x4 grid = 16 frames) */
        if (healthTruck.timer % 5 == 0)
          healthTruck.animFrame++;
        if (healthTruck.animFrame >= 16) {
          healthTruck.state = 2; /* Drop */
          healthTruck.timer = 0;
        }
        break;
      case 2: /* Drop Health Orb */
        healthOrb.active = true;
        healthOrb.x = healthTruck.x + 128 - 50;
        healthOrb.y = healthTruck.y + 128;
        healthTruck.state = 3; /* Leave */
        break;
      case 3: /* Leave */
        healthTruck.y += 20.0f;
        if (healthTruck.y > 1200) {
          healthTruck.active = false;
        }
        break;
      }
    }

    /* Health Orb Logic */
    if (healthOrb.active) {
      healthOrb.y -= ROAD_SCROLL_SPEED;
      if (checkAABB(healthOrb.x, healthOrb.y, 100, 100, player.x, player.y,
                    CAR_DRAW_W, CAR_DRAW_H)) {
        /* Picked up -- full health */
        healthOrb.active = false;
        player.health = PLAYER_MAX_HEALTH;
      } else if (healthOrb.y < -150) {
        /* Missed -- no respawn */
        healthOrb.active = false;
      }
    }

    /* If truck gone and orb gone: start cloud transition */
    if (!healthTruck.active && !healthOrb.active) {
      currentPhase = PHASE_CLOUD_IN;
      cloudLeftX = -1920.0f;
      cloudRightX = 1920.0f;
    }
    return;
  }

  /* ── Cloud Transition ──────────────────────────────── */
  if (currentPhase == PHASE_CLOUD_IN) {
    cloudLeftX += CLOUD_SPEED;
    cloudRightX -= CLOUD_SPEED;
    if (cloudLeftX >= 0.0f) {
      cloudLeftX = 0.0f;
      cloudRightX = 0.0f;
      /* Perform road swap while fully covered */
      player.score = 1500;
      lastSpecialScore = 1900;
      enemies.clear();
      puddles.clear();
      npcs.clear();
      roadChangeStage((char *)"Asset/Roads/road 5.png");
      bossFightOver = true;
      currentPhase = PHASE_CLOUD_OUT;
    }
    return;
  }

  if (currentPhase == PHASE_CLOUD_OUT) {
    cloudLeftX -= CLOUD_SPEED;
    cloudRightX += CLOUD_SPEED;
    if (cloudLeftX <= -1920.0f) {
      cloudLeftX = -1920.0f;
      cloudRightX = 1920.0f;
      currentPhase = PHASE_NORMAL;
    }
    return;
  }

  /* ── Stage 2 Boss Phases ─────────────────────────────── */

  if (currentPhase == PHASE_WARNING2) {
    bool allClear = true;
    for (unsigned int i = 0; i < enemies.size(); i++) {
      if (enemies[i].active) {
        enemies[i].y += 15.0f;
        enemies[i].x += (enemies[i].x < 960) ? -5.0f : 5.0f;
        if (enemies[i].y > 1200)
          enemies[i].active = false;
        allClear = false;
      }
    }
    for (unsigned int i = 0; i < puddles.size(); i++)
      puddles[i].active = false;
    for (unsigned int i = 0; i < npcs.size(); i++) {
      if (npcs[i].active) {
        npcs[i].y += 15.0f;
        if (npcs[i].y > 1200)
          npcs[i].active = false;
        allClear = false;
      }
    }
    if (allClear) {
      currentPhase = PHASE_TRUCK2;
      truck.active = true;
      truck.x = 960 - 128;
      truck.y = -300;
      truck.state = 0;
      truck.timer = 0;
      truck.animFrame = 0;
    }
    return;
  }

  if (currentPhase == PHASE_TRUCK2) {
    truck.timer++;
    switch (truck.state) {
    case 0:
      truck.y += 12.0f;
      if (truck.y >= player.y + 300) {
        truck.state = 1;
        truck.timer = 0;
      }
      break;
    case 1:
      if (truck.timer % 5 == 0)
        truck.animFrame++;
      if (truck.animFrame >= 16) {
        truck.state = 2;
        truck.timer = 0;
      }
      break;
    case 2:
      orb.active = true;
      orb.x = truck.x + 128 - 50;
      orb.y = truck.y + 128;
      truck.state = 3;
      break;
    case 3:
      truck.y += 20.0f;
      if (truck.y > 1200)
        truck.active = false;
      break;
    }

    if (orb.active) {
      orb.y -= ROAD_SCROLL_SPEED;
      if (checkAABB(orb.x, orb.y, 100, 100, player.x, player.y, CAR_DRAW_W,
                    CAR_DRAW_H)) {
        orb.active = false;
        player.missileCount = 10;
      } else if (orb.y < -150) {
        orb.active = false;
        truck.active = true;
        truck.y = -300;
        truck.state = 0;
        truck.timer = 0;
        truck.animFrame = 0;
      }
    }

    if (!truck.active && !orb.active) {
      currentPhase = PHASE_BOSS2;
      boss2.active = true;
      boss2.x = 960 - BOSS2_W / 2;
      boss2.y = 1200;
      boss2.health = 300;
      boss2.maxHealth = 300;
      boss2.state = 0;
      boss2.timer = 0;
      boss2.fireTimer = 0;
      boss2.targetX =
          (float)(ROAD_LEFT_LIMIT +
                  rand() % (ROAD_RIGHT_LIMIT - BOSS2_W - ROAD_LEFT_LIMIT));
      boss2.targetY = 100.0f + (float)(rand() % 800);
      boss2.missilesFired = 0;
      boss2.missileDelay = 0;
    }
    return;
  }

  if (currentPhase == PHASE_BOSS2) {
    /* ── Resupply Truck (during boss2 fight) ──── */
    if (player.missileCount == 0 && !truck.active && !orb.active &&
        boss2.active && boss2.state != 4) {
      bool activeMissiles = false;
      for (unsigned int m = 0; m < playerMissiles.size(); m++)
        if (playerMissiles[m].active)
          activeMissiles = true;

      if (!activeMissiles) {
        boss2TruckDelayTimer++;
        if (boss2TruckDelayTimer > 300) {
          truck.active = true;
          truck.x = 960 - 128;
          truck.y = -300;
          truck.state = 0;
          truck.timer = 0;
          truck.animFrame = 0;
          boss2TruckDelayTimer = 0;
        }
      }
    } else {
      boss2TruckDelayTimer = 0;
    }

    /* Truck Update (during boss2 fight) */
    if (truck.active) {
      truck.timer++;
      switch (truck.state) {
      case 0:
        truck.y += 12.0f;
        if (truck.y >= 200) {
          truck.state = 1;
          truck.timer = 0;
        }
        break;
      case 1:
        if (truck.timer % 5 == 0)
          truck.animFrame++;
        if (truck.animFrame >= 16) {
          truck.state = 2;
          truck.timer = 0;
        }
        break;
      case 2:
        orb.active = true;
        orb.x = truck.x + 128 - 50;
        orb.y = truck.y + 128;
        truck.state = 3;
        break;
      case 3:
        truck.y += 20.0f;
        if (truck.y > 1200)
          truck.active = false;
        break;
      }
    }

    /* Orb Logic (during boss2 fight) */
    if (orb.active) {
      orb.y -= ROAD_SCROLL_SPEED;
      if (checkAABB(orb.x, orb.y, 100, 100, player.x, player.y, CAR_DRAW_W,
                    CAR_DRAW_H)) {
        orb.active = false;
        player.missileCount = 10;
      } else if (orb.y < -150) {
        orb.active = false;
      }
    }

    /* Player Missiles vs Boss2 */
    for (unsigned int m = 0; m < playerMissiles.size(); m++) {
      if (playerMissiles[m].active) {
        playerMissiles[m].y += 15.0f;
        if (playerMissiles[m].y > 1080)
          playerMissiles[m].active = false;

        if (boss2.active && boss2.state != 4 &&
            checkAABB(playerMissiles[m].x, playerMissiles[m].y, 40, 40, boss2.x,
                      boss2.y, BOSS2_W, BOSS2_H)) {
          boss2.health -= 15;
          if (boss2.health < 0)
            boss2.health = 0;
          playerMissiles[m].active = false;
        }
      }
    }

    /* Boss2 Projectiles update */
    for (unsigned int i = 0; i < boss2Projectiles.size(); i++) {
      if (boss2Projectiles[i].active) {
        boss2Projectiles[i].x += boss2Projectiles[i].dx;
        boss2Projectiles[i].y += boss2Projectiles[i].dy;

        if (boss2Projectiles[i].x < -50 || boss2Projectiles[i].x > 1920 ||
            boss2Projectiles[i].y < -50 || boss2Projectiles[i].y > 1080)
          boss2Projectiles[i].active = false;

        if (player.active &&
            checkAABB(boss2Projectiles[i].x, boss2Projectiles[i].y, 20, 20,
                      player.x, player.y, CAR_DRAW_W, CAR_DRAW_H)) {
          player.health -= 5;
          boss2Projectiles[i].active = false;
        }
      }
    }

    /* Boss2 Missiles update */
    for (unsigned int i = 0; i < boss2Missiles.size(); i++) {
      if (boss2Missiles[i].active) {
        boss2Missiles[i].x += boss2Missiles[i].dx;
        boss2Missiles[i].y += boss2Missiles[i].dy;

        if (boss2Missiles[i].x < -50 || boss2Missiles[i].x > 1920 ||
            boss2Missiles[i].y < -50 || boss2Missiles[i].y > 1080)
          boss2Missiles[i].active = false;

        if (player.active &&
            checkAABB(boss2Missiles[i].x, boss2Missiles[i].y, 40, 40, player.x,
                      player.y, CAR_DRAW_W, CAR_DRAW_H)) {
          player.health -= 10;
          boss2Missiles[i].active = false;
        }
      }
    }

    /* Turret tracking — aim at player */
    float bcx = boss2.x + BOSS2_W / 2.0f;
    float bcy = boss2.y + BOSS2_H / 2.0f;
    float pcx = player.x + CAR_DRAW_W / 2.0f;
    float pcy = player.y + CAR_DRAW_H / 2.0f;
    float tdx = pcx - bcx;
    float tdy = pcy - bcy;
    float tAngleDeg = atan2(tdy, tdx) * 180.0f / PI;
    boss2.turretAngle = tAngleDeg;
    /* Use exact same formula as Player.h (getFrameFromAngle(90-deg)),
       plus +31 frame offset because Tank.png Up (away) = frame 31 */
    boss2.turretFrame = (getFrameFromAngle(90.0f - tAngleDeg) + 31) % 36;

    /* Boss2 State Machine */
    boss2.timer++;
    switch (boss2.state) {
    case 0: { /* Move randomly + Fire projectiles for 5s (300f) */
      float ddx = boss2.targetX - boss2.x;
      float ddy = boss2.targetY - boss2.y;
      float dist = sqrt(ddx * ddx + ddy * ddy);
      if (dist < 50.0f) {
        int rangeX = ROAD_RIGHT_LIMIT - BOSS2_W - ROAD_LEFT_LIMIT;
        if (rangeX < 1)
          rangeX = 1;
        boss2.targetX = ROAD_LEFT_LIMIT + (float)(rand() % rangeX);
        boss2.targetY = 100.0f + (float)(rand() % 800);
      } else {
        boss2.x += (ddx / dist) * BOSS2_MOVE_SPEED;
        boss2.y += (ddy / dist) * BOSS2_MOVE_SPEED;
      }

      /* Fire projectile from barrel tip */
      boss2.fireTimer++;
      if (boss2.fireTimer >= BOSS2_FIRE_COOLDOWN) {
        boss2.fireTimer = 0;
        float angleRad = boss2.turretAngle * PI / 180.0f;
        float barrelLen = BOSS2_W / 2.0f;
        BossMissile bp;
        bp.x = bcx + cos(angleRad) * barrelLen - 10;
        bp.y = bcy + sin(angleRad) * barrelLen - 10;
        bp.active = true;
        float pLen = sqrt(tdx * tdx + tdy * tdy);
        if (pLen < 1)
          pLen = 1;
        bp.dx = (tdx / pLen) * BOSS2_PROJ_SPEED;
        bp.dy = (tdy / pLen) * BOSS2_PROJ_SPEED;
        boss2Projectiles.push_back(bp);
      }

      if (boss2.timer >= 300) {
        boss2.state = 1;
        boss2.timer = 0;
      }
      break;
    }
    case 1: /* Retreat to top + wait 2s */
      if (boss2.y < 800)
        boss2.y += 5.0f;
      else if (boss2.timer >= 120) {
        boss2.state = 2;
        boss2.timer = 0;
        boss2.missilesFired = 0;
        boss2.missileDelay = 0;
      }
      break;
    case 2: /* Fire 3 sequential missiles at player position */
      boss2.missileDelay++;
      if (boss2.missilesFired < 3 && boss2.missileDelay >= 30) {
        boss2.missileDelay = 0;
        BossMissile bm;
        bm.x = bcx - 20;
        bm.y = boss2.y;
        bm.active = true;
        float mdx = (player.x + CAR_DRAW_W / 2.0f) - bm.x;
        float mdy = (player.y + CAR_DRAW_H / 2.0f) - bm.y;
        float mLen = sqrt(mdx * mdx + mdy * mdy);
        if (mLen < 1)
          mLen = 1;
        bm.dx = (mdx / mLen) * BOSS2_MISSILE_SPEED;
        bm.dy = (mdy / mLen) * BOSS2_MISSILE_SPEED;
        boss2Missiles.push_back(bm);
        boss2.missilesFired++;
      }
      if (boss2.missilesFired >= 3 && boss2.missileDelay >= 30) {
        boss2.state = 3;
        boss2.timer = 0;
      }
      break;
    case 3: /* Wait 2s */
      if (boss2.timer >= 120) {
        boss2.state = 0;
        boss2.timer = 0;
        boss2.fireTimer = 0;
        int rangeX = ROAD_RIGHT_LIMIT - BOSS2_W - ROAD_LEFT_LIMIT;
        if (rangeX < 1)
          rangeX = 1;
        boss2.targetX = ROAD_LEFT_LIMIT + (float)(rand() % rangeX);
        boss2.targetY = 100.0f + (float)(rand() % 800);
      }
      break;
    case 4: /* Dying — 3s explosion */
      boss2DeathTimer++;
      if (boss2DeathTimer % 10 == 0) {
        Explosion e;
        e.x = boss2.x + (float)(rand() % (BOSS2_W - 50));
        e.y = boss2.y + (float)(rand() % (BOSS2_H - 50));
        e.frameIndex = 0;
        e.active = true;
        explosions.push_back(e);
      }
      if (boss2DeathTimer > 180) {
        boss2.active = false;
        boss2.health = 0;
        currentPhase = PHASE_WIN;
        boss2DeathTimer = 0;
      }
      return;
    }

    /* Boss2 Collision with player */
    if (player.active && boss2.active && boss2.state != 4 &&
        checkAABB(boss2.x, boss2.y, BOSS2_W, BOSS2_H, player.x, player.y,
                  CAR_DRAW_W, CAR_DRAW_H)) {
      player.health -= 10;
      player.y -= 50;
    }

    /* Cannon hits Boss2 — skip if dying */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active && boss2.state != 4 &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, boss2.x,
                    boss2.y, BOSS2_W, BOSS2_H)) {
        projectiles[p].active = false;
        boss2.health -= 1;
        if (boss2.health < 0)
          boss2.health = 0;
      }
    }

    /* Boss2 Death Trigger */
    if (boss2.health <= 0 && boss2.state != 4) {
      boss2.state = 4;
      boss2.health = 0;
      boss2DeathTimer = 0;
      boss2Projectiles.clear();
      boss2Missiles.clear();
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

  /* NPC Update (Stage 2 only — logic in NPC.h) */
  npcUpdate();

  enemySpawn();

  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (!enemies[i].active)
      continue;

    if (enemies[i].type == TYPE_NORMAL) {
      /* Normal Logic: Move Down + Mild Ramming */
      enemies[i].y += enemies[i].speed;

      /* Steer away from NPCs first */
      for (unsigned int n = 0; n < npcs.size(); n++) {
        if (npcs[n].active) {
          float dx = enemies[i].x - npcs[n].x;
          float dy = enemies[i].y - npcs[n].y;
          if (dx < 0)
            dx = -dx;
          if (dy < 0)
            dy = -dy;
          if (dx < NPC_WIDTH + 40 && dy < NPC_HEIGHT + 80) {
            /* Steer away from NPC */
            if (enemies[i].x < npcs[n].x)
              enemies[i].x -= 2.0f;
            else
              enemies[i].x += 2.0f;
          }
        }
      }

      /* Try to Ram (Mild tracking, but NPC avoidance takes priority) */
      if (enemies[i].x < player.x)
        enemies[i].x += 1.0f;
      if (enemies[i].x > player.x)
        enemies[i].x -= 1.0f;

      if (enemies[i].y > 1080)
        enemies[i].active = false;

      /* Avoid NPCs */
      for (unsigned int n = 0; n < npcs.size(); n++) {
        if (npcs[n].active &&
            checkAABB(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT,
                      npcs[n].x, npcs[n].y, NPC_WIDTH, NPC_HEIGHT)) {
          /* Bounce apart (no damage) */
          if (enemies[i].x < npcs[n].x)
            enemies[i].x -= 15;
          else
            enemies[i].x += 15;
          enemies[i].y -= 20;
        }
      }
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

      /* Avoid NPCs */
      for (unsigned int n = 0; n < npcs.size(); n++) {
        if (npcs[n].active &&
            checkAABB(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT,
                      npcs[n].x, npcs[n].y, NPC_WIDTH, NPC_HEIGHT)) {
          if (enemies[i].x < npcs[n].x)
            enemies[i].x -= 15;
          else
            enemies[i].x += 15;
          enemies[i].y -= 20;
        }
      }

      /* Special Enemy stays on screen unless destroyed */
      /* Boundaries */
      if (enemies[i].x < ROAD_LEFT_LIMIT)
        enemies[i].x = ROAD_LEFT_LIMIT;
      if (enemies[i].x > ROAD_RIGHT_LIMIT - ENEMY_WIDTH)
        enemies[i].x = ROAD_RIGHT_LIMIT - ENEMY_WIDTH;
    } else if (enemies[i].type == TYPE_SPECIAL2) {
      /* Special Enemy 2: NPC-like phase then aggressive chase */
      enemies[i].stateTimer++;

      if (enemies[i].state == S_COOLDOWN) {
        /* Phase 1: Scroll down like an NPC (slightly less than road speed) */
        enemies[i].y -= (ROAD_SCROLL_SPEED - 3.0f + (float)(rand() % 3));

        /* Switch to chase only when near the bottom of the visible screen */
        if (enemies[i].y <= 100) {
          enemies[i].state = S_CHASE;
          enemies[i].stateTimer = 0;
        }
      } else {
        /* Phase 2: Aggressive chase (faster + more accurate than police) */
        /* Chase speed: 2.5f X, 5.0f Y (police: 1.0f X, ~4.0f Y) */
        if (enemies[i].y > player.y)
          enemies[i].y -= 5.0f;
        if (enemies[i].y < player.y)
          enemies[i].y += 5.0f;
        if (enemies[i].x < player.x)
          enemies[i].x += 2.5f;
        if (enemies[i].x > player.x)
          enemies[i].x -= 2.5f;
      }

      /* Stay on screen */
      if (enemies[i].x < ROAD_LEFT_LIMIT)
        enemies[i].x = ROAD_LEFT_LIMIT;
      if (enemies[i].x > ROAD_RIGHT_LIMIT - ENEMY_WIDTH)
        enemies[i].x = ROAD_RIGHT_LIMIT - ENEMY_WIDTH;
      if (enemies[i].y < 0)
        enemies[i].y = 0;
      if (enemies[i].state != S_COOLDOWN && enemies[i].y > 1080 - ENEMY_HEIGHT)
        enemies[i].y = 1080 - ENEMY_HEIGHT;
    }

    /* Animation (Frame Cycle) */
    enemies[i].frameTimer++;
    if (enemies[i].frameTimer >= 10) {
      enemies[i].frame = (enemies[i].frame + 1) % 3;
      enemies[i].frameTimer = 0;
    }

    /* Collisions */
    /* Vs Player */
    if (player.active && enemies[i].type != TYPE_SPECIAL1 &&
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
          int killScore = 10;
          if (enemies[i].type == TYPE_SPECIAL1)
            killScore = 50;
          if (enemies[i].type == TYPE_SPECIAL2)
            killScore = 20;
          player.score += killScore;
          if (!bossFightOver && player.score > 1000)
            player.score = 1000;
          break;
        }
      }
    }
  }
}

/* ── Draw ───────────────────────────────────────────────── */
/* enemyDraw: Renders all game entities in correct layer order.
 * Draws: NPCs, enemies (with animation), special effect overlays,
 * oil puddles, supply trucks, power orbs, boss projectiles + missiles,
 * boss sprites, and explosion animations on top of everything.
 */
void enemyDraw(void) {
  /* Puddles */
  for (unsigned int i = 0; i < puddles.size(); i++) {
    if (puddles[i].active) {
      iShowImage((int)puddles[i].x, (int)puddles[i].y, PUDDLE_W, PUDDLE_H,
                 texOilPuddle);
    }
  }

  /* NPCs (drawn before enemies so enemies render on top — logic in NPC.h) */
  npcDraw();

  /* Enemies */
  for (unsigned int i = 0; i < enemies.size(); i++) {
    if (enemies[i].active) {
      if (enemies[i].type == TYPE_NORMAL) {
        iShowImage((int)enemies[i].x, (int)enemies[i].y, ENEMY_WIDTH,
                   ENEMY_HEIGHT, texPolice[enemies[i].frame]);
      } else if (enemies[i].type == TYPE_SPECIAL2) {
        iShowImage((int)enemies[i].x, (int)enemies[i].y, ENEMY_WIDTH,
                   ENEMY_HEIGHT, texSpecialEnemy2);
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

  /* Health Truck */
  if (healthTruck.active) {
    iShowImage((int)healthTruck.x, (int)healthTruck.y, 256, 256, texTruck);
    if (healthTruck.state == 1) {
      iShowEffectGrid((int)healthTruck.x, (int)healthTruck.y, 256, 256,
                      texHealthAnim, healthTruck.animFrame, 4, 4);
    }
  }

  /* Health Orb */
  if (healthOrb.active) {
    iShowImage((int)healthOrb.x, (int)healthOrb.y, 100, 100, texHealthOrb);
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

  /* ── Boss2 Drawing ─────────────────────────────────────── */
  if (currentPhase == PHASE_BOSS2 && boss2.active) {
    /* Turret/Body (rotatable sprite sheet, same grid as Car.png) */
    iShowImageGrid((int)boss2.x, (int)boss2.y, BOSS2_W, BOSS2_H, texTank,
                   boss2.turretFrame, CAR_SHEET_ROWS, CAR_SHEET_COLS);
  }

  /* Boss2 Projectiles */
  if (currentPhase == PHASE_BOSS2) {
    for (unsigned int i = 0; i < boss2Projectiles.size(); i++) {
      if (boss2Projectiles[i].active) {
        iShowImage((int)boss2Projectiles[i].x, (int)boss2Projectiles[i].y, 20,
                   20, texProjectile);
      }
    }
    for (unsigned int i = 0; i < boss2Missiles.size(); i++) {
      if (boss2Missiles[i].active) {
        iShowImage((int)boss2Missiles[i].x, (int)boss2Missiles[i].y, 40, 40,
                   texTorpedo);
      }
    }
    /* Player Missiles during Boss2 */
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

/* ── Cloud Overlay (must be drawn LAST, after all HUD/UI) ── */
/* cloudOverlayDraw: Renders the cloud level transition overlay.
 * Two cloud halves slide inward during PHASE_CLOUD_IN to cover
 * the screen, then slide outward during PHASE_CLOUD_OUT to reveal
 * the new Stage 2 road. Drawn ON TOP of everything (topmost layer).
 * This is the LEVEL TRANSITION MECHANISM using clouds. */
void cloudOverlayDraw(void) {
  if (currentPhase == PHASE_CLOUD_IN || currentPhase == PHASE_CLOUD_OUT) {
    iShowImage((int)cloudLeftX, 0, 1920, 1080, texCloudLeft);
    iShowImage((int)cloudRightX, 0, 1920, 1080, texCloudRight);
  }
}

#endif /* ENEMY_H */

/* ── Helpers ────────────────────────────────────────────── */
StagePhase getPhase() { return currentPhase; }

int getBossHealth() { return boss.health; }
int getBoss2Health() { return boss2.health; }

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
