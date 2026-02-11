#ifndef ROAD_H
#define ROAD_H

#include "Player.h"

/* ══════════════════════════════════════════════════════════
 *  ROAD MODULE
 *  Management of the scrolling road background.
 *  Dependencies: Player.h (for score global)
 * ══════════════════════════════════════════════════════════ */

/* ── Config ─────────────────────────────────────────────── */
#define ROAD_SPEED 10.0f
#define SCREEN_H 1080

/* ── Globals ────────────────────────────────────────────── */
static unsigned int texRoad = 0;
static float roadY = 0.0f;

/* ── Init (load textures — call once) ────────────────────── */
void roadInit(void) {
  /* Road background – scaled to fill the full 1920x1080 window */
  texRoad = iLoadImage("Asset/Roads/road 4.png");
}

/* ── Reset (state only — call on restart) ──────────────── */
void roadReset(void) { roadY = 0.0f; }

/* ── Update ─────────────────────────────────────────────── */
void roadUpdate(void) {
  /* Scroll down */
  roadY -= ROAD_SPEED;

  /* Loop logic */
  if (roadY <= -SCREEN_H) {
    roadY = 0;
    /* Add 10 points per loop */
    player.score += 10;
  }
}

/* ── Draw ───────────────────────────────────────────────── */
void roadDraw(void) {
  /* Draw primary road */
  iShowImage(0, (int)roadY, 1920, SCREEN_H, texRoad);

  /* Draw secondary road above it for seamless loop */
  iShowImage(0, (int)roadY + SCREEN_H, 1920, SCREEN_H, texRoad);
}

#endif /* ROAD_H */
