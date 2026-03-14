#ifndef NPC_H
#define NPC_H

#include "Player.h"
#include <cstdlib>
#include <vector>

/* ══════════════════════════════════════════════════════════
 *  NPC MODULE
 *  Management of civilian NPC vehicles on the road.
 *
 *  HOW IT WORKS:
 *  NPCs are innocent civilian cars that appear in Stage 2 (after
 *  bossFightOver is true). They scroll down the screen like traffic.
 *  PENALTY SYSTEM: If the player rams or shoots an NPC, the player
 *  LOSES health and score instead of gaining. This creates a risk/reward
 *  mechanic where players must aim carefully to avoid hitting civilians.
 *
 *  This module also hosts shared definitions (PROJECTILE_DMG,
 *  ROAD_SCROLL_SPEED, Explosion struct, checkAABB) used by Enemy.h.
 *
 *  Dependencies: Player.h (for player/projectile globals)
 * ══════════════════════════════════════════════════════════ */

/* ── Shared Config (used by both NPC and Enemy) ────────── */

/* PROJECTILE_DMG: Damage dealt per player cannon bullet hit on an enemy or NPC.
 * Used in both NPC collision and Enemy collision code.
 * HOW TO CHANGE: Increase to make bullets stronger, decrease for weaker.
 * EFFECT: Higher = enemies die faster; lower = more bullets needed to kill.
 *         At 5 damage, a 30HP police enemy dies in 6 shots. */
#define PROJECTILE_DMG 5

/* ROAD_SCROLL_SPEED: Pixels per frame that road-attached objects (puddles,
 * orbs, etc.) scroll downward. Must match Road.h ROAD_SPEED (10.0f)
 * so that objects placed on the road appear stationary relative to it.
 * HOW TO CHANGE: Change in sync with ROAD_SPEED in Road.h.
 * EFFECT: If these don't match, objects will slide on the road surface. */
#define ROAD_SCROLL_SPEED 10.0f

/* ── NPC Config ────────────────────────────────────────── */
#define NPC_WIDTH 100
#define NPC_HEIGHT 100
#define NPC_MAX_ON_SCREEN 3
#define NPC_RAM_HP_PENALTY 3
#define NPC_RAM_SCORE_PENALTY 2
#define NPC_SHOOT_SCORE_PENALTY 10
#define NPC_SHOOT_HP_PENALTY 5
#define NPC_MAX_HP 15

/*  Explosion Config (Grid 4x4)                                  */
#define EXPLOSION_ROWS 4
#define EXPLOSION_COLS 4
#define EXPLOSION_FRAMES 16
#define EXPLOSION_SIZE 128

/* ── Structs ────────────────────────────────────────────── */
struct NPC {
  float x, y;
  float speed;
  int health;
  int texIndex; /* Which NPC texture (0-4) */
  bool active;
};

struct Explosion {
  float x, y;
  int frameIndex;
  bool active;
};

/* ── Globals ────────────────────────────────────────────── */
static std::vector<NPC> npcs;
static std::vector<Explosion> explosions;
static unsigned int texNPC[5];
static unsigned int texExplosion = 0;

/* ── Helpers ────────────────────────────────────────────── */
static bool checkAABB(float x1, float y1, float w1, float h1, float x2,
                      float y2, float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

/* ── Init (load NPC textures — call once) ────────────────── */
void npcInit(void) {
  texNPC[0] = iLoadImage("Asset/NPC 1.png");
  texNPC[1] = iLoadImage("Asset/NPC 2.png");
  texNPC[2] = iLoadImage("Asset/NPC 3.png");
  texNPC[3] = iLoadImage("Asset/NPC 4.png");
  texExplosion = iLoadImage("Asset/Explosion.png");
}

/* ── Reset (state only — call on restart) ──────────────── */
void npcReset(void) {
  npcs.clear();
  explosions.clear();
}

/* ── Spawn Logic (Stage 2 only, called when bossFightOver) ─ */
/* npcSpawn: Spawns new NPC vehicles during Stage 2.
 *
 * HOW IT WORKS:
 * 1. Counts currently active NPCs on screen.
 * 2. If no NPCs are active, spawns one immediately.
 *    If fewer than NPC_MAX_ON_SCREEN (3), has a 1/120 chance per frame
 *    of spawning a new one (random delay for natural feel).
 * 3. Picks a random X position within road limits and Y above the screen.
 * 4. Tries up to 10 random positions to avoid overlapping existing NPCs.
 * 5. Sets speed to roughly match the road scroll speed (5.0-7.0 px/frame)
 *    so NPCs appear as normal traffic.
 *
 * HOW TO CHANGE:
 * - NPC_MAX_ON_SCREEN: Max simultaneous NPCs (currently 3).
 * - rand() % 120: Spawn frequency. Lower = more frequent spawning.
 * - Speed formula: ROAD_SCROLL_SPEED - 5.0f + rand()%3 gives 5-7.
 *   Increase the base for faster NPCs, decrease for slower.
 * - Overlap check: NPC_WIDTH+20 / NPC_HEIGHT+20 is the minimum distance.
 *
 * EFFECT: More NPCs = more traffic = harder to shoot enemies without
 *         accidentally hitting civilians. Faster NPCs scroll past quickly. */
void npcSpawn(void) {
  int npcActive = 0;
  for (unsigned int i = 0; i < npcs.size(); i++)
    if (npcs[i].active)
      npcActive++;

  /* Spawn immediately if none active, else random delay (1/120 chance/frame) */
  bool shouldSpawn = (npcActive == 0) ||
                     (npcActive < NPC_MAX_ON_SCREEN && (rand() % 120 == 0));
  if (shouldSpawn) {
    NPC n;
    int range = ROAD_RIGHT_LIMIT - NPC_WIDTH - ROAD_LEFT_LIMIT;
    if (range < 1)
      range = 1;

    /* Try up to 10 random positions to avoid overlapping with existing NPCs */
    bool validPos = false;
    for (int attempt = 0; attempt < 10; attempt++) {
      n.x = ROAD_LEFT_LIMIT + (float)(rand() % range); /* Random X on road */
      n.y =
          1080.0f + (float)(rand() % 300); /* Start above screen (1080-1380) */
      validPos = true;
      for (unsigned int j = 0; j < npcs.size(); j++) {
        if (npcs[j].active) {
          float dx = n.x - npcs[j].x;
          float dy = n.y - npcs[j].y;
          if (dx < 0)
            dx = -dx;
          if (dy < 0)
            dy = -dy;
          /* Minimum spacing: NPC size + 20px buffer */
          if (dx < NPC_WIDTH + 20 && dy < NPC_HEIGHT + 20) {
            validPos = false;
            break; /* Too close to an existing NPC, retry */
          }
        }
      }
      if (validPos)
        break;
    }

    if (validPos) {
      /* Speed set near road speed so NPC looks like normal traffic.
       * Range: 5.0 to 7.0 (road is 10.0, so NPC scrolls at rate 5-7) */
      n.speed =
          ROAD_SCROLL_SPEED - 5.0f + (float)(rand() % 3); /* 5-7 px/frame */
      n.health = NPC_MAX_HP;                              /* 15 HP */
      n.texIndex = rand() % 4; /* Random visual variant (0-3) */
      n.active = true;
      npcs.push_back(n);
    }
  }
}

/* ── Update ───────────────────────────────────────────── */
/* npcUpdate: Processes all NPC logic each frame.
 *
 * HOW IT WORKS:
 * For each active NPC:
 *   1. MOVEMENT: Moves the NPC downward at its speed (scrolling like traffic).
 *   2. OFF-SCREEN: If the NPC scrolls below the screen, deactivate it.
 *   3. RAM COLLISION: If the NPC overlaps the player (with a reduced hitbox
 *      of NPC_WIDTH-30 x NPC_HEIGHT-30 for forgiveness):
 *      - Player loses NPC_RAM_HP_PENALTY (3) health.
 *      - Player loses NPC_RAM_SCORE_PENALTY (2) score.
 *      - NPC takes 2 self-damage.
 *      - Both bounce apart vertically (20px).
 *      - If NPC dies from ramming, spawn explosion.
 *   4. PROJECTILE COLLISION: If a player bullet hits the NPC:
 *      - Bullet deactivated, NPC takes PROJECTILE_DMG (5) damage.
 *      - If NPC dies: player gets NPC_SHOOT_SCORE_PENALTY (-10 score)
 *        and NPC_SHOOT_HP_PENALTY (-5 HP). Explosion spawns.
 *
 * HOW TO CHANGE:
 * - Ram hitbox: Modify the +15/-30 offsets for tighter/looser collision.
 * - Bounce distance: Change the 20px push value.
 * - Penalties: Modify the NPC_RAM/SHOOT constants at the top.
 *
 * EFFECT: NPCs create a "don't shoot civilians" challenge. Their penalties
 *         balance the game by punishing careless play. */
void npcUpdate(void) {
  for (unsigned int i = 0; i < npcs.size(); i++) {
    if (!npcs[i].active)
      continue;

    /* Move downward from top of screen (simulates oncoming traffic) */
    npcs[i].y -= npcs[i].speed;

    /* Off-screen removal: NPC scrolled past the bottom of the screen */
    if (npcs[i].y < -NPC_HEIGHT) {
      npcs[i].active = false;
      continue;
    }

    /* Collision with Player (Ram) — uses a smaller hitbox (inset 15px each
     * side) for a more forgiving collision feel. NPC takes self damage on
     * contact. */
    if (player.active && checkAABB(npcs[i].x + 15, npcs[i].y + 15,
                                   NPC_WIDTH - 30, NPC_HEIGHT - 30, player.x,
                                   player.y, CAR_DRAW_W, CAR_DRAW_H)) {
      player.health -= NPC_RAM_HP_PENALTY;   /* Player loses 3 HP */
      player.score -= NPC_RAM_SCORE_PENALTY; /* Player loses 2 score */
      npcs[i].health -= 2;                   /* NPC self damage on ram */
      /* Bounce apart vertically to separate them */
      if (npcs[i].y > player.y)
        npcs[i].y += 20; /* NPC above player: push NPC up */
      else
        npcs[i].y -= 20; /* NPC below player: push NPC down */

      /* Check if NPC died from the ram impact */
      if (npcs[i].health <= 0) {
        Explosion ex;
        ex.x = npcs[i].x + NPC_WIDTH / 2 - EXPLOSION_SIZE / 2;
        ex.y = npcs[i].y + NPC_HEIGHT / 2 - EXPLOSION_SIZE / 2;
        ex.frameIndex = 0;
        ex.active = true;
        explosions.push_back(ex);
        npcs[i].active = false;
      }
      continue;
    }

    /* Collision with Player Projectiles (Shot) */
    for (unsigned int p = 0; p < projectiles.size(); p++) {
      if (projectiles[p].active &&
          checkAABB(projectiles[p].x, projectiles[p].y, 20, 20, npcs[i].x,
                    npcs[i].y, NPC_WIDTH, NPC_HEIGHT)) {
        projectiles[p].active = false;    /* Bullet consumed */
        npcs[i].health -= PROJECTILE_DMG; /* NPC takes 5 damage */

        if (npcs[i].health <= 0) {
          /* NPC destroyed by shooting: player is PENALIZED */
          player.score -= NPC_SHOOT_SCORE_PENALTY; /* Lose 10 score */
          player.health -= NPC_SHOOT_HP_PENALTY;   /* Lose 5 HP */

          Explosion ex;
          ex.x = npcs[i].x + NPC_WIDTH / 2 - EXPLOSION_SIZE / 2;
          ex.y = npcs[i].y + NPC_HEIGHT / 2 - EXPLOSION_SIZE / 2;
          ex.frameIndex = 0;
          ex.active = true;
          explosions.push_back(ex);

          npcs[i].active = false;
        }
        break; /* One bullet per NPC per frame */
      }
    }
  }
}

/* ── Draw ───────────────────────────────────────────────── */
void npcDraw(void) {
  /* NPCs (drawn before enemies so enemies render on top) */
  for (unsigned int i = 0; i < npcs.size(); i++) {
    if (npcs[i].active) {
      iShowImage((int)npcs[i].x, (int)npcs[i].y, NPC_WIDTH, NPC_HEIGHT,
                 texNPC[npcs[i].texIndex]);
    }
  }
}

#endif
