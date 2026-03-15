#ifndef ROAD_H
#define ROAD_H

#include "Player.h"

/* ══════════════════════════════════════════════════════════
 *  ROAD MODULE
 *  Handles the scrolling road background, score generation,
 *  and road texture swapping for stage transitions.
 *
 *  HOW IT WORKS:
 *  Two copies of the road texture are drawn stacked vertically.
 *  Both scroll downward each frame. When the bottom copy fully
 *  exits the screen, it wraps to the top — creating an infinite
 *  loop effect. Each full loop awards 10 score points.
 *
 *  Dependencies: Player.h (for player score and bossFightOver flag)
 * ══════════════════════════════════════════════════════════ */

/* ── Config ─────────────────────────────────────────────── */

/* ROAD_SPEED: How fast the road scrolls downward (pixels/frame).
 * At 60 FPS this is 600 px/sec, creating a fast driving feel.
 * Must match ROAD_SCROLL_SPEED in NPC.h so road-attached objects
 * (puddles, orbs) appear stationary on the road.
 * HOW TO CHANGE: Increase for faster driving, decrease for slower.
 * EFFECT: Also affects score gain rate (faster scroll = faster score). */
#define ROAD_SPEED 10.0f

/* SCREEN_H: Vertical resolution of the game window (1080 pixels).
 * Used for road looping math and dual-image rendering.
 * HOW TO CHANGE: Only change if the window resolution changes.
 * EFFECT: Wrong value will cause visible seams in the road. */
#define SCREEN_H 1080

/* ── Globals ────────────────────────────────────────────── */

/* texRoad: OpenGL texture ID for the current road background image.
 * Changes during gameplay via roadChangeStage() for Stage 2 transition. */
static unsigned int texRoad = 0;

/* roadY: Current vertical scroll offset of the road texture.
 * Continuously decrements by ROAD_SPEED each frame.
 * Resets to 0 when it reaches -SCREEN_H (seamless loop).
 * Each reset awards 10 score points (when allowed). */
static float roadY = 0.0f;

/* ── Init (load textures) ───────────────────────────────── */
/* roadInit: Loads the Stage 1 road texture from disk.
 * Called once at startup from gameInit().
 * HOW TO CHANGE: Replace the file path to use a different starting road.
 * EFFECT: Changes the visual appearance of the Stage 1 road. */
void roadInit(void) { texRoad = iLoadImage("Asset/Roads/road 4.png"); }

/* ── Reset (state only) ─────────────────────────────────── */
/* roadReset: Resets road scroll position and reloads the Stage 1 road.
 * Called by gameReset() on game restart.
 * HOW TO CHANGE: Change the file path if the starting road image changes.
 * EFFECT: Ensures the game always starts on the correct road. */
void roadReset(void) {
  roadY = 0.0f;
  texRoad = iLoadImage("Asset/Roads/road 4.png");
}

/* ── Change Road Stage ──────────────────────────────────── */
/* roadChangeStage: Swaps the road texture to a new image.
 * Called during the cloud transition after Boss 1 is defeated
 * to switch from road 4.png (Stage 1) to road 5.png (Stage 2).
 * HOW TO CHANGE: Call with a different path to change the visual.
 * EFFECT: Purely visual — changes the road background appearance. */
void roadChangeStage(char path[]) { texRoad = iLoadImage(path); }

/* ── Update ─────────────────────────────────────────────── */
/* roadUpdate: Scrolls the road and awards passive score each frame.
 *
 * HOW IT WORKS:
 * 1. Decrements roadY by ROAD_SPEED (10 px/frame) to scroll down.
 * 2. When roadY reaches -SCREEN_H (one full screen scroll), resets to 0.
 * 3. Awards 10 score on each loop, BUT only if:
 *    - Score is below 1000 (Stage 1, pre-boss), OR
 *    - bossFightOver is true (Stage 2).
 *    This freezes the score at 1000 during the boss fight.
 *
 * HOW TO CHANGE:
 * - Score per loop: Change the "+= 10" value.
 * - Score cap: Change "< 1000" to delay/advance boss trigger.
 * - Scroll speed: Change ROAD_SPEED.
 * EFFECT: Score gain ≈ 10 * 60 / (1080/10) ≈ 5.6 pts/sec. */
void roadUpdate(void) {
  /* Scroll the road downward */
  roadY -= ROAD_SPEED;

  /* Loop: when one full screen has scrolled, wrap and award score */
  if (roadY <= -SCREEN_H) {
    roadY = 0;
    /* Score frozen at 1000 during boss1 fight, and at 2500 during boss2 fight.
     * Resumes after each boss dies and cloud transition completes. */
    if (player.score < (1000 + loopCount * 4500) || (bossFightOver && player.score < (2500 + loopCount * 4500)) ||
        (boss2FightOver && player.score < (4000 + loopCount * 4500))) {
      player.score += 10; /* +10 score per completed road loop */
    }
  }
}

/* ── Draw ───────────────────────────────────────────────── */
/* roadDraw: Renders two copies of the road texture for seamless scrolling.
 *
 * HOW IT WORKS:
 * Draws the road at roadY and roadY + SCREEN_H to fill the screen.
 * As roadY scrolls down, the second copy appears from the top.
 * Both span the full 1920px width of the window.
 *
 * HOW TO CHANGE: The 1920 width matches the window. Change if resolution
 * changes. EFFECT: Visual only — the road is decorative background. */
void roadDraw(void) {
  iShowImage(0, (int)roadY, 1920, SCREEN_H, texRoad); /* Current road  */
  iShowImage(0, (int)roadY + SCREEN_H, 1920, SCREEN_H,
             texRoad); /* Next road above */
}

#endif /* ROAD_H */