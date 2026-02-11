#ifndef UI_H
#define UI_H

#include "Player.h"
#include <stdio.h> /* sprintf */

/* ══════════════════════════════════════════════════════════
 *  UI MODULE
 *  Manages the health bar, star system, and other HUD elements.
 *  Dependencies: Player.h (for player state)
 * ══════════════════════════════════════════════════════════ */

/* ── Texture handles ────────────────────────────────────── */
static unsigned int texHealthBar = 0;  /* health bar frame/skin */
static unsigned int texStarSystem = 0; /* star sprite sheet     */
static unsigned int texWinBg = 0;      /* win screen background */

/* ── HUD layout constants (pixels, 1920x1080 canvas) ───── */

/*  Health Bar draw size  (original 860x218, ~50% scale)         */
#define HB_DRAW_W 430
#define HB_DRAW_H 109

/*  Health Bar position – top-left of screen with 20px margin    */
#define HB_X 20
#define HB_Y (1080 - 20 - HB_DRAW_H) /* 951   */

/*  Red fill rectangle – sits INSIDE the bar frame.
 *  Adjusted to fit cleanly within the black outline.            */
#define HB_FILL_OX 97 /* left offset inside the bar    */
#define HB_FILL_OY 34 /* bottom offset inside the bar  */
#define HB_FILL_W 315 /* fill width at full health     */
#define HB_FILL_H 42  /* fill height                   */

/*  Star System draw size for ONE row.
 *  Original sheet: 808x988, 6 rows → each row ~165 px tall.     */
#define STAR_DRAW_W 350
#define STAR_DRAW_H 71

/*  Star System position – below health bar with 10px gap        */
#define STAR_X 20
#define STAR_Y (HB_Y - 10 - STAR_DRAW_H) /* 870  */

/*  Star sprite sheet metrics                                    */
#define STAR_ROWS 6 /* total rows in Star System.png      */

/*  Score Text Position */
#define SCORE_X 20
#define SCORE_Y (STAR_Y - 40)

/* ── iShowImageSub ──────────────────────────────────────── */
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
void uiInit(void) {
  texHealthBar = iLoadImage("Asset/Health Bar.png");
  texStarSystem = iLoadImage("Asset/Star System.png");
  texWinBg = iLoadImage("Asset/Menu files/background.png");
}

/* ── uiDraw ─────────────────────────────────────────────── */
void uiDraw(void) {
  /* ── 1. Health bar ───────────────────────────────────── */
  /*  Calculate fill ratio from player.health (0-100) */
  float hpRatio = player.health / 100.0f;
  if (hpRatio < 0)
    hpRatio = 0;
  if (hpRatio > 1)
    hpRatio = 1;

  iSetColor(255, 0, 0);
  iFilledRectangle(HB_X + HB_FILL_OX, HB_Y + HB_FILL_OY,
                   (int)(HB_FILL_W * hpRatio), HB_FILL_H);

  /*  Frame on top */
  iSetColor(255, 255, 255);
  iShowImage(HB_X, HB_Y, HB_DRAW_W, HB_DRAW_H, texHealthBar);

  /* ── 2. Star system ──────────────────────────────────── */
  /*  Update based on score. > 500 = Row 1 (1 Star). Else Row 0 (0 Stars).
   *  Assuming Row 0 is "Empty" or "0 Stars" (User said "replace with second
   * row... show player has attained 1 star") So Row 0 is default. Row 1 is 1
   * Star.
   */
  int row = 0;
  if (player.score >= 500)
    row = 1;
  /* Future: if >= 1000 row = 2 etc. */

  iShowImageSub(STAR_X, STAR_Y, STAR_DRAW_W, STAR_DRAW_H, texStarSystem, row,
                STAR_ROWS);

  /* ── 3. Score ────────────────────────────────────────── */
  /* ── 3. Score & Level ────────────────────────────────── */
  StagePhase phase = getPhase(); /* Helper from Enemy.h */

  if (phase == PHASE_WARNING) {
    iSetColor(255, 0, 0);
    iText(SCORE_X, SCORE_Y, "BEWARE!!!", GLUT_BITMAP_TIMES_ROMAN_24);
  } else {
    iSetColor(255, 255, 255);
    char scoreStr[32];
    sprintf_s(scoreStr, 32, "Score: %d", player.score);
    iText(SCORE_X, SCORE_Y, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 4. Status Effects ────────────────────────────── */
  if (player.lossControlTimer > 0) {
    iSetColor(255, 0, 0);
    iText(SCORE_X, SCORE_Y - 40, "LOST CONTROL!!!", GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 5. Boss Stage HUD ────────────────────────────── */
  /* BEWARE handled above */

  if (phase == PHASE_BOSS) {
    /* Boss HP */
    char bossStr[32];
    sprintf_s(bossStr, "BOSS: %d%%", getBossHealth() / 2); /* 200 -> 100% */
    iSetColor(255, 0, 0);
    iText(960 - 50, 1000, bossStr, GLUT_BITMAP_TIMES_ROMAN_24);

    /* Missiles */
    char misStr[32];
    sprintf_s(misStr, "Missile: %d/5", player.missileCount);
    iSetColor(255, 255, 255);
    iText(50, 50, misStr, GLUT_BITMAP_TIMES_ROMAN_24);
  }

  if (phase == PHASE_WIN) {
    /* Win Screen: Background overlay + text */
    iShowImage(0, 0, 1920, 1080, texWinBg);
    iSetColor(255, 255, 255);
    iText(960 - 150, 540 + 40, "Congratulations, You win.",
          GLUT_BITMAP_TIMES_ROMAN_24);
    iText(960 - 120, 540 - 10, "Press 'R' to restart.",
          GLUT_BITMAP_TIMES_ROMAN_24);
    iText(960 - 130, 540 - 60, "Press 'ESC' to escape.",
          GLUT_BITMAP_TIMES_ROMAN_24);
  }

  /* ── 6. Game Over Overlay ────────────────────────────── */
  if (gameState == STATE_GAMEOVER) {
    iSetColor(255, 0, 0);
    iText(960 - 50, 540 + 20, "YOU LOSE", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 255, 255);
    iText(960 - 80, 540 - 20, "Press 'R' to Restart", GLUT_BITMAP_HELVETICA_18);
    iText(960 - 80, 540 - 50, "Press 'ESC' to Menu", GLUT_BITMAP_HELVETICA_18);
  }
}

#endif /* UI_H */
