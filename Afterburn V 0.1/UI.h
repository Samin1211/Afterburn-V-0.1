#ifndef UI_H
#define UI_H

#include "Player.h"
#include <stdio.h> /* sprintf */

/* ══════════════════════════════════════════════════════════
 *  UI MODULE
 *  Manages the health bar, star system, and other HUD elements.
 *
 *  HOW IT WORKS:
 *  This module renders all Heads-Up Display (HUD) elements on top of
 *  the game scene. It shows the player's current health (as a red bar),
 *  a star achievement indicator, the score, status effects (control loss),
 *  boss HP during boss fights, missile count, and game-over/win screens.
 *
 *  Dependencies: Player.h (for player state), Enemy.h (for getPhase(),
 *                getBossHealth(), getBoss2Health()).
 * ══════════════════════════════════════════════════════════ */

/* ── Texture handles ────────────────────────────────────── */
/* texHealthBar: OpenGL texture for the health bar frame/skin overlay. */
static unsigned int texHealthBar = 0;
/* texStarSystem: OpenGL texture for the star achievement sprite sheet (6 rows).
 */
static unsigned int texStarSystem = 0;
/* texWinBg: OpenGL texture for the win screen background (same as menu bg). */
static unsigned int texWinBg = 0;

/* ── HUD layout constants (pixels, 1920x1080 canvas) ───── */

/* Health Bar draw size (original 860x218, scaled to ~50%).
 * HOW TO CHANGE: Increase HB_DRAW_W/H for a bigger health bar on screen.
 * EFFECT: Purely visual — makes the health bar more/less prominent. */
#define HB_DRAW_W 430
#define HB_DRAW_H 109

/* Health Bar position — top-left of screen with 20px margin.
 * HOW TO CHANGE: Move HB_X/HB_Y to reposition the health bar.
 * EFFECT: Changes where the health bar appears on the HUD. */
#define HB_X 20
#define HB_Y (1080 - 20 - HB_DRAW_H) /* 951 */

/* Red fill rectangle — sits INSIDE the bar frame.
 * These offsets position the red HP bar inside the decorative frame.
 * HB_FILL_W: Maximum width of the red fill at 100% health (315 pixels).
 * The actual width = HB_FILL_W * (player.health / 100).
 * HOW TO CHANGE: Adjust offsets if using a different health bar image.
 * EFFECT: Wrong offsets will misalign the fill with the bar frame. */
#define HB_FILL_OX 97 /* left offset inside the bar    */
#define HB_FILL_OY 34 /* bottom offset inside the bar  */
#define HB_FILL_W 315 /* fill width at full health     */
#define HB_FILL_H 42  /* fill height                   */

/* Star System draw size for ONE row.
 * The star sprite sheet has 6 rows (0-5 stars). Only one row is shown at a
 * time. Row selection is based on score thresholds (defined in uiDraw). HOW TO
 * CHANGE: Increase STAR_DRAW_W/H for bigger star display. EFFECT: Visual
 * scaling of the star achievement indicator. */
#define STAR_DRAW_W 350
#define STAR_DRAW_H 71

/* Star System position — below health bar with 10px gap.
 * HOW TO CHANGE: Modify STAR_X/STAR_Y to reposition.
 * EFFECT: Changes star indicator location on screen. */
#define STAR_X 20
#define STAR_Y (HB_Y - 10 - STAR_DRAW_H) /* 870 */

/* Star sprite sheet metrics.
 * STAR_ROWS: Total rows in the Star System.png sprite sheet.
 * HOW TO CHANGE: Only change if using a different star sprite sheet. */
#define STAR_ROWS 6

/* Score Text Position — below the star system.
 * HOW TO CHANGE: Modify SCORE_X/SCORE_Y to move the score display.
 * EFFECT: Repositions the score text on the HUD. */
#define SCORE_X 20
#define SCORE_Y (STAR_Y - 40)

/* ── iShowImageSub ──────────────────────────────────────── */
/* iShowImageSub: Draws a single row from a vertically stacked sprite sheet.
 *
 * HOW IT WORKS:
 * Similar to iShowImageGrid but for sheets divided into horizontal rows only.
 * Calculates UV coordinates for the specified row and renders as a textured
 * quad.
 *
 * PARAMETERS:
 *   x, y       — Screen position (bottom-left).
 *   w, h       — Draw size in pixels.
 *   texture    — OpenGL texture ID.
 *   rowIndex   — Which row to display (0 = top row).
 *   totalRows  — Total number of rows in the sprite sheet.
 *
 * USED BY: Star system rendering in uiDraw().
 * HOW TO CHANGE: Only modify if changing the OpenGL rendering approach.
 * EFFECT: Incorrect row/total values show the wrong star level. */
static void iShowImageSub(int x, int y, int w, int h, unsigned int texture,
                          int rowIndex, int totalRows) {
  float rowH = 1.0f / (float)totalRows;
  float vTop = -1.0f + (float)rowIndex * rowH;
  float vBottom = vTop + rowH;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, vBottom);
  glVertex2f((float)x, (float)y);
  glTexCoord2f(1.0f, vBottom);
  glVertex2f((float)(x + w), (float)y);
  glTexCoord2f(1.0f, vTop);
  glVertex2f((float)(x + w), (float)(y + h));
  glTexCoord2f(0.0f, vTop);
  glVertex2f((float)x, (float)(y + h));
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

/* ── uiInit ─────────────────────────────────────────────── */
/* uiInit: Loads all HUD textures into GPU memory.
 * Called once at startup from gameInit().
 * HOW TO CHANGE: Replace file paths to use different HUD artwork.
 * EFFECT: Changes the visual appearance of HUD elements. */
void uiInit(void) {
  texHealthBar =
      iLoadImage("Asset/Health Bar.png"); /* Health bar frame image */
  texStarSystem =
      iLoadImage("Asset/Star System.png"); /* Star achievement sheet */
  texWinBg = iLoadImage("Asset/Menu files/background.png"); /* Win screen bg */
}

/* ── uiDraw ─────────────────────────────────────────────── */
/* uiDraw: Renders all HUD elements on top of the game scene.
 *
 * HOW IT WORKS (in render order):
 * 1. HEALTH BAR: Red fill rectangle scaled by player.health (0-100%),
 *    then the decorative health bar frame on top.
 * 2. STAR SYSTEM: Shows achievement stars based on score thresholds:
 *    - Row 0 (0 stars): score < 500
 *    - Row 1 (1 star):  score >= 500
 *    - Row 2 (2 stars): score >= 2000
 *    HOW TO CHANGE: Adjust the score >= values to change star thresholds.
 *    EFFECT: Stars appear at different score milestones.
 * 3. SCORE & WARNINGS: Shows "BEWARE!!!" during warning phases,
 *    otherwise displays the current score.
 * 4. STATUS EFFECTS: Shows "LOST CONTROL!!!" when oil puddle debuff is active.
 * 5. BOSS HUD: During boss fights, shows boss HP percentage and missile count.
 *    Boss 1 divides by 2 (max 200 HP), Boss 2 multiplies by 100/300.
 * 6. WIN SCREEN: When PHASE_WIN is reached, shows congratulations and
 *    restart/escape instructions.
 * 7. GAME OVER: When gameState == STATE_GAMEOVER, shows "YOU LOSE" and
 *    restart/menu instructions.
 *
 * HOW TO CHANGE: Modify text positions, font choices, or color values.
 * EFFECT: Changes HUD appearance and information display. */
void uiDraw(void) {
  /* ── 1. Health bar ───────────────────────────────────── */
  /* Calculate fill ratio from player.health (0 to 100) */
  float hpRatio = player.health / 100.0f;
  if (hpRatio < 0)
    hpRatio = 0; /* Clamp negative health */
  if (hpRatio > 1)
    hpRatio = 1; /* Clamp overflow */

  /* Draw the red HP fill bar (width proportional to health) */
  iSetColor(255, 0, 0);
  iFilledRectangle(HB_X + HB_FILL_OX, HB_Y + HB_FILL_OY,
                   (int)(HB_FILL_W * hpRatio), HB_FILL_H);

  /* Draw the decorative health bar frame on top of the fill */
  iSetColor(255, 255, 255);
  iShowImage(HB_X, HB_Y, HB_DRAW_W, HB_DRAW_H, texHealthBar);

  /* ── 2. Star system ──────────────────────────────────── */
  /* Select which row of the star sprite sheet to display.
   * Row 0 = 0 stars (default), Row 1 = 1 star (score >= 500),
   * Row 2 = 2 stars (score >= 2000).
   * HOW TO CHANGE: Modify the score thresholds below.
   * EFFECT: Stars appear at different score milestones. */
  int row = 0;
  if (player.score >= 500)
    row = 1; /* 1 star at 500 points */
  if (player.score >= 2000)
    row = 2; /* 2 stars at 2000 points */

  iShowImageSub(STAR_X, STAR_Y, STAR_DRAW_W, STAR_DRAW_H, texStarSystem, row,
                STAR_ROWS);

  /* ── 3. Score & Level ────────────────────────────────── */
  StagePhase phase = getPhase(); /* Get current game phase from Enemy.h */

  /* During warning phases (pre-boss), show "BEWARE!!!" in red */
  if (phase == PHASE_WARNING || phase == PHASE_WARNING2) {
    iSetColor(255, 0, 0);
    iText(SCORE_X, SCORE_Y, "BEWARE!!!", GLUT_BITMAP_TIMES_ROMAN_24);
  } else {
    /* Otherwise show the current score */
    iSetColor(255, 255, 255);
    char scoreStr[32];
    sprintf_s(scoreStr, 32, "Score: %d", player.score);
    /* Draw twice with 1px offset to simulate bold text */
    iText(SCORE_X + 1, SCORE_Y + 1, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
    iText(SCORE_X, SCORE_Y, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 4. Status Effects ────────────────────────────── */
  /* Show warning text when player is under oil puddle debuff */
  if (player.lossControlTimer > 0) {
    iSetColor(255, 0, 0);
    iText(SCORE_X, SCORE_Y - 40, "LOST CONTROL!!!", GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 5. Boss Stage HUD ────────────────────────────── */

  /* Boss 1 HUD: HP percentage and missile count */
  if (phase == PHASE_BOSS) {
    char bossStr[32];
    /* Boss has 200 max HP, divided by 2 gives 0-100% display */
    sprintf_s(bossStr, "BOSS: %d%%", getBossHealth() / 2);
    iSetColor(255, 0, 0);
    iText(960 - 50, 1000, bossStr, GLUT_BITMAP_TIMES_ROMAN_24);

    /* Show player's remaining missiles (max 5 for Boss 1) */
    char misStr[32];
    sprintf_s(misStr, "Missile: %d/5", player.missileCount);
    iSetColor(255, 255, 255);
    iText(50, 50, misStr, GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 5b. Boss2 Stage HUD ────────────────────────────── */
  /* Boss 2 HUD: HP percentage and missile count */
  if (phase == PHASE_BOSS2) {
    char boss2Str[32];
    /* Boss 2 has 300 max HP, *100/300 gives 0-100% display */
    sprintf_s(boss2Str, "BOSS: %d%%", getBoss2Health() * 100 / 300);
    iSetColor(255, 0, 0);
    iText(960 - 50, 1000, boss2Str, GLUT_BITMAP_TIMES_ROMAN_24);

    /* Show player's remaining missiles (max 10 for Boss 2) */
    char mis2Str[32];
    sprintf_s(mis2Str, "Missile: %d/10", player.missileCount);
    iSetColor(255, 255, 255);
    iText(50, 50, mis2Str, GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── Win Screen ─────────────────────────────────────── */
  /* Displayed when both bosses are defeated (PHASE_WIN) */
  if (phase == PHASE_WIN) {
    iShowImage(0, 0, 1920, 1080, texWinBg); /* Full-screen background overlay */
    iSetColor(255, 255, 255);
    iText(960 - 150, 540 + 40, "Congratulations, You win.",
          GLUT_BITMAP_TIMES_ROMAN_24);
    iText(960 - 120, 540 - 10, "Press 'R' to restart.",
          GLUT_BITMAP_TIMES_ROMAN_24);
    iText(960 - 130, 540 - 60, "Press 'ESC' to escape.",
          GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 6. Game Over Overlay ────────────────────────────── */
  /* Displayed when player health reaches 0 */
  if (gameState == STATE_GAMEOVER) {
    iSetColor(255, 0, 0);
    iText(960 - 50, 540 + 20, "YOU LOSE", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 255, 255);
    iText(960 - 80, 540 - 20, "Press 'R' to Restart", GLUT_BITMAP_HELVETICA_18);
    iText(960 - 80, 540 - 50, "Press 'ESC' to Menu", GLUT_BITMAP_HELVETICA_18);
  }
}

#endif /* UI_H */
