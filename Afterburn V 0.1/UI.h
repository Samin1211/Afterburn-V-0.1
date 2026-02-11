#ifndef UI_H
#define UI_H

/* ══════════════════════════════════════════════════════════
 *  UI MODULE
 *  Manages the health bar, star system, and other HUD elements.
 * ══════════════════════════════════════════════════════════ */

/* ── Texture handles ────────────────────────────────────── */
static unsigned int texHealthBar = 0;  /* health bar frame/skin */
static unsigned int texStarSystem = 0; /* star sprite sheet     */

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

/* ── Game-state variables ───────────────────────────────── */

/*  Current health (0.0 – 1.0).  1.0 = full bar, 0.0 = empty.   */
static float playerHealth = 1.0f;

/*  Current star row to display (0-based).
 *  Row 0 = all 5 stars filled black (best rating).              */
static int starRow = 0;

/* ── iShowImageSub ────────────────────────────────────────
 *  Draws a HORIZONTAL STRIP of a texture (one "row" of a
 *  vertically-stacked sprite sheet).
 *
 *  rowIndex : 0-based row from the TOP of the image.
 *  totalRows: total number of equal-height rows in the sheet.
 *
 *  Uses the same OpenGL conventions as iShowImage() in
 *  iGraphics.h (V goes from 0 at bottom to -1 at top).          */
static void iShowImageSub(int x, int y, int w, int h, unsigned int texture,
                          int rowIndex, int totalRows) {
  /* Compute vertical texture coords for the requested row.
   * In iGraphics' convention:
   *   V = -1  → top of the image   (row 0 top edge)
   *   V =  0  → bottom of the image (last row bottom edge) */
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

/* ── uiInit ───────────────────────────────────────────────
 *  Loads all UI assets.
 *  Call from gameInit() in iMain.cpp.                           */
void uiInit(void) {
  texHealthBar = iLoadImage("Asset/Health Bar.png");
  texStarSystem = iLoadImage("Asset/Star System.png");
}

/* ── uiDraw ───────────────────────────────────────────────
 *  Renders the HUD overlay.
 *  Call from gameDraw() in iMain.cpp.                           */
void uiDraw(void) {
  /* ── 1. Health bar ───────────────────────────────────── */
  /*  Step A: red fill rectangle (drawn first, behind the frame).
   *  Width is proportional to playerHealth (1.0 = full).       */
  iSetColor(255, 0, 0);
  iFilledRectangle(HB_X + HB_FILL_OX, HB_Y + HB_FILL_OY,
                   (int)(HB_FILL_W * playerHealth), HB_FILL_H);

  /*  Step B: health bar frame / skin on top (has transparency) */
  iSetColor(255, 255, 255); /* Ensure color is white for texture */
  iShowImage(HB_X, HB_Y, HB_DRAW_W, HB_DRAW_H, texHealthBar);

  /* ── 2. Star system (row 0 = all 5 black stars) ──────── */
  iShowImageSub(STAR_X, STAR_Y, STAR_DRAW_W, STAR_DRAW_H, texStarSystem,
                starRow, STAR_ROWS);
}

#endif /* UI_H */
