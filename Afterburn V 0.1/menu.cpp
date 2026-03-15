/*==========================================================
 *  menu.cpp  —  Main-menu implementation for Afterburn V 0.1
 *
 *  IMPORTANT:  This file must NOT #include "iGraphics.h"
 *  because that header contains STB_IMAGE_IMPLEMENTATION
 *  and all function bodies.  Including it from two .cpp
 *  files would cause multiple-definition linker errors.
 *
 *  Instead we forward-declare the iGraphics functions we
 *  need, then include only our own compact "menu.h".
 *=========================================================*/

/* ── Forward declarations of iGraphics helpers ─────────── */
/* These are defined in iGraphics.h (included by iMain.cpp) */
extern int iScreenWidth, iScreenHeight;

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iClear(void);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);
void iFilledRectangle(double left, double bottom, double dx, double dy);

/* GLUT bitmap fonts (resolved at link time via glut.h) */

/* ── Standard headers ──────────────────────────────────── */
#include <stdlib.h>   /* exit() */
#include <time.h>     /* clock(), CLOCKS_PER_SEC */
#include <stdio.h>    /* fopen, fread, fwrite */

/* GLUT/OpenGL headers for scalable stroke text rendering */
#include "glut.h"

/* ── Our own header ────────────────────────────────────── */
#include "menu.h"

/* ==========================================================
 *  ASSET LAYOUT  (all values in pixels, 1920x1080 canvas)
 *
 *  Positions are in iGraphics coordinates where (0, 0) is
 *  the BOTTOM-LEFT corner of the window.
 *
 *  Derived from the Figma reference (menu_reference.png).
 * ========================================================== */

/* Margins & gaps  (tuned to match menu_reference.png) */
#define MARGIN_LEFT   88
#define MARGIN_BOTTOM 75
#define GAP           20

/* Column x-origins */
#define COL1_X   MARGIN_LEFT                           /* 88   */
#define COL2_X   (COL1_X + 655 + GAP)                 /* 763  */
#define COL3_X   (COL2_X + 521 + GAP)                 /* 1304 */

/* Row y-origins  (bottom-left of each card) */
#define ROW_FULL_Y     MARGIN_BOTTOM                   /* 75   */

/* Column 2: leaderboard on top, options on bottom */
#define COL2_BOT_Y     MARGIN_BOTTOM                   /* 75   */
#define COL2_TOP_Y     (COL2_BOT_Y + 421 + GAP)        /* 516  */

/* Column 3: about on top, quit on bottom */
#define COL3_BOT_Y     MARGIN_BOTTOM                   /* 75   */
#define COL3_TOP_Y     (COL3_BOT_Y + 213 + GAP)        /* 308  */

/* Title logo position and dimensions (purely visual, NOT interactive) */
#define TITLE_X  95
#define TITLE_Y  905
#define TITLE_W  214
#define TITLE_H  162

/* Hover scale factor */
#define HOVER_SCALE   1.08f
#define NORMAL_SCALE  1.00f

/* Time-based hover animation speed (scale units per second) */
/* 0.08 range / 4.0 speed ≈ 300ms for full transition       */
#define SCALE_SPEED   4.0f

/* ── Module-level data ─────────────────────────────────── */
static unsigned int texBackground = 0;
static unsigned int texTitle      = 0;
static unsigned int texCreditPage = 0;
static unsigned int texStartPage  = 0;

static MenuButton buttons[MENU_BUTTON_COUNT];
static GameState   currentState = STATE_MENU;
static clock_t     lastFrameTime = 0;

/* ── Leaderboard data ──────────────────────────────────── */
#define LB_MAX_SCORES 100
static unsigned int texLeaderboardBg = 0;
static int lbScores[LB_MAX_SCORES];
static int lbCount = 0;

/* ── Helper: point-in-rect test (native size) ─────────── */
static int pointInButton(const MenuButton *b, int mx, int my)
{
    return (mx >= b->x && mx <= b->x + b->w &&
            my >= b->y && my <= b->y + b->h);
}

/* ── Helper: is cursor over the title/logo area? ────────── */
static int pointInTitle(int mx, int my)
{
    return (mx >= TITLE_X && mx <= TITLE_X + TITLE_W &&
            my >= TITLE_Y && my <= TITLE_Y + TITLE_H);
}

/* ── Public API ────────────────────────────────────────── */

void menuInit(void)
{
    /* Load textures (paths relative to working directory) */
    texBackground = iLoadImage("Asset/Menu files/background.png");
    texTitle      = iLoadImage("Asset/Menu files/title.png");
    texCreditPage = iLoadImage("Asset/credit page.png");
    texStartPage  = iLoadImage("Asset/starting page.png");

    /* --- New Game (column 1, full height) --- */
    buttons[0].texture = iLoadImage("Asset/Menu files/new_game.png");
    buttons[0].x       = COL1_X;
    buttons[0].y       = ROW_FULL_Y;
    buttons[0].w       = 655;
    buttons[0].h       = 920;
    buttons[0].scale       = NORMAL_SCALE;
    buttons[0].targetScale = NORMAL_SCALE;
    buttons[0].target      = STATE_GAME;

    /* --- Leaderboard (column 2, top) --- */
    buttons[1].texture = iLoadImage("Asset/Menu files/leaderboard.png");
    buttons[1].x       = COL2_X;
    buttons[1].y       = COL2_TOP_Y;
    buttons[1].w       = 521;
    buttons[1].h       = 424;
    buttons[1].scale       = NORMAL_SCALE;
    buttons[1].targetScale = NORMAL_SCALE;
    buttons[1].target      = STATE_LEADERBOARD;

    /* --- Options (column 2, bottom) --- */
    buttons[2].texture = iLoadImage("Asset/Menu files/options.png");
    buttons[2].x       = COL2_X;
    buttons[2].y       = COL2_BOT_Y;
    buttons[2].w       = 521;
    buttons[2].h       = 421;
    buttons[2].scale       = NORMAL_SCALE;
    buttons[2].targetScale = NORMAL_SCALE;
    buttons[2].target      = STATE_OPTIONS;

    /* --- About (column 3, top) --- */
    buttons[3].texture = iLoadImage("Asset/Menu files/about.png");
    buttons[3].x       = COL3_X;
    buttons[3].y       = COL3_TOP_Y;
    buttons[3].w       = 520;
    buttons[3].h       = 632;
    buttons[3].scale       = NORMAL_SCALE;
    buttons[3].targetScale = NORMAL_SCALE;
    buttons[3].target      = STATE_ABOUT;

    /* --- Quit (column 3, bottom) --- */
    buttons[4].texture = iLoadImage("Asset/Menu files/quit.png");
    buttons[4].x       = COL3_X;
    buttons[4].y       = COL3_BOT_Y;
    buttons[4].w       = 520;
    buttons[4].h       = 213;
    buttons[4].scale       = NORMAL_SCALE;
    buttons[4].targetScale = NORMAL_SCALE;
    buttons[4].target      = STATE_EXIT;
}

/* ── Drawing ──────────────────────────────────────────── */

void menuDraw(void)
{
    iClear();

    /* ── Compute delta time (seconds since last frame) ── */
    clock_t now = clock();
    float dt = 0.016f;  /* default ~60fps on first frame */
    if (lastFrameTime != 0)
    {
        dt = (float)(now - lastFrameTime) / (float)CLOCKS_PER_SEC;
        if (dt > 0.1f)  dt = 0.1f;   /* cap to avoid jumps */
        if (dt < 0.0001f) dt = 0.0001f;
    }
    lastFrameTime = now;

    /* Full-screen background */
    iShowImage(0, 0, 1920, 1080, texBackground);

    /* Buttons (with time-based smooth hover scale from center) */
    int i;
    for (i = 0; i < MENU_BUTTON_COUNT; i++)
    {
        MenuButton *b = &buttons[i];

        /* Time-based linear interpolation toward target scale */
        float diff = b->targetScale - b->scale;
        float maxStep = SCALE_SPEED * dt * (HOVER_SCALE - NORMAL_SCALE);
        if (diff > maxStep)
            b->scale += maxStep;
        else if (diff < -maxStep)
            b->scale -= maxStep;
        else
            b->scale = b->targetScale;

        int drawW = (int)(b->w * b->scale);
        int drawH = (int)(b->h * b->scale);

        /* Offset so scaling happens from the center of the button */
        int drawX = b->x - (drawW - b->w) / 2;
        int drawY = b->y - (drawH - b->h) / 2;

        iShowImage(drawX, drawY, drawW, drawH, b->texture);
    }

    /* Title / logo drawn LAST (on top), purely visual, no interaction */
    iShowImage(TITLE_X, TITLE_Y, TITLE_W, TITLE_H, texTitle);
}

/* ── Hover handling ───────────────────────────────────── */

void menuMouseMove(int mx, int my)
{
    /* If cursor is over the title logo, no button should hover */
    int overTitle = pointInTitle(mx, my);

    int i;
    for (i = 0; i < MENU_BUTTON_COUNT; i++)
    {
        if (!overTitle && pointInButton(&buttons[i], mx, my))
            buttons[i].targetScale = HOVER_SCALE;
        else
            buttons[i].targetScale = NORMAL_SCALE;
    }
}

/* ── Click handling ───────────────────────────────────── */

GameState menuMouseClick(int mx, int my)
{
    /* Clicking the title/logo does nothing */
    if (pointInTitle(mx, my))
        return STATE_MENU;

    int i;
    for (i = 0; i < MENU_BUTTON_COUNT; i++)
    {
        if (pointInButton(&buttons[i], mx, my))
        {
            if (buttons[i].target == STATE_EXIT)
            {
                exit(0);
            }
            currentState = buttons[i].target;
            return currentState;
        }
    }
    return STATE_MENU;  /* no button hit */
}

/* ── Placeholder sub-screens ─────────────────────────── */

static void drawPlaceholderScreen(char *title, int r, int g, int b)
{
    iClear();
    /* Show Background.png instead of a solid color */
    iShowImage(0, 0, 1920, 1080, texBackground);

    iSetColor(255, 255, 255);
    iText(760, 560, title, GLUT_BITMAP_TIMES_ROMAN_24);

    char *hint = "Press ESC to return to menu";
    iText(740, 500, hint, GLUT_BITMAP_HELVETICA_18);
}

void drawPlaceholderGame(void)
{
    drawPlaceholderScreen("[ GAME ]", 20, 30, 50);
}

/* ── Leaderboard: file I/O ──────────────────────────────── */

static void leaderboardSave(void)
{
    FILE *fp = NULL;
    fopen_s(&fp, "leaderboard_scores.dat", "wb");
    if (!fp) return;
    fwrite(&lbCount, sizeof(int), 1, fp);
    fwrite(lbScores, sizeof(int), lbCount, fp);
    fclose(fp);
}

void leaderboardInit(void)
{
    texLeaderboardBg = iLoadImage("Asset/leaderboard.png");
    lbCount = 0;

    FILE *fp = NULL;
    fopen_s(&fp, "leaderboard_scores.dat", "rb");
    if (fp) {
        fread(&lbCount, sizeof(int), 1, fp);
        if (lbCount < 0) lbCount = 0;
        if (lbCount > LB_MAX_SCORES) lbCount = LB_MAX_SCORES;
        fread(lbScores, sizeof(int), lbCount, fp);
        fclose(fp);
    }
}

void leaderboardAddScore(int score)
{
    if (lbCount < LB_MAX_SCORES) {
        lbScores[lbCount] = score;
        lbCount++;
    } else if (score > lbScores[LB_MAX_SCORES - 1]) {
        lbScores[LB_MAX_SCORES - 1] = score;
    } else {
        return; /* Score doesn't qualify for leaderboard */
    }
    /* Insertion sort descending */
    int i, j, tmp;
    for (i = 1; i < lbCount; i++) {
        tmp = lbScores[i];
        j = i - 1;
        while (j >= 0 && lbScores[j] < tmp) {
            lbScores[j + 1] = lbScores[j];
            j--;
        }
        lbScores[j + 1] = tmp;
    }
    leaderboardSave();
}

void leaderboardReset(void)
{
    lbCount = 0;
    leaderboardSave();
}

bool leaderboardQualifies(int score)
{
    if (lbCount < 5) return true;           /* Less than 5 entries — always qualifies */
    return score > lbScores[4];             /* Must beat the 5th place score */
}

/* ── Leaderboard: stroke text helper ──────────────────── */

static void drawStrokeText(float x, float y, float scale, const char *text)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    while (*text) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *text);
        text++;
    }
    glPopMatrix();
}

/* Measure width of stroke text for centering */
static float strokeTextWidth(float scale, const char *text)
{
    float w = 0;
    while (*text) {
        w += 104.76f; /* approximate advance width of GLUT_STROKE_ROMAN chars */
        text++;
    }
    return w * scale;
}

/* ── Leaderboard: draw ─────────────────────────────────── */

void drawLeaderboard(void)
{
    iClear();
    iShowImage(0, 0, 1920, 1080, texLeaderboardBg);

    int count = lbCount;
    if (count > 5) count = 5;

    /* Layout: scores centered vertically in the middle area */
    float textScale = 0.35f;          /* Scale for GLUT_STROKE_ROMAN */
    float lineHeight = 60.0f;         /* Vertical gap between entries */
    float totalHeight = count * lineHeight;
    /* Lowered startY from -30.0f to -70.0f to visually center under title */
    float startY = (1080.0f / 2.0f) + (totalHeight / 2.0f) - 70.0f;

    iSetColor(255, 255, 255);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    /* Center X value to align around */
    float centerX = 1920.0f / 2.0f;

    int i;
    for (i = 0; i < count; i++) {
        char rankStr[16];
        char scoreStr[32];
        sprintf_s(rankStr, 16, "%d.", i + 1);
        sprintf_s(scoreStr, 32, "%d", lbScores[i]);

        /* Measure rank string width so we can right-align it */
        float rankW = strokeTextWidth(textScale, rankStr);

        /* Two columns: rank is right-aligned 20px left of center,
           score is left-aligned 20px right of center. */
        float rankX = centerX - 20.0f - rankW;
        float scoreX = centerX + 20.0f;
        float y = startY - i * lineHeight;

        drawStrokeText(rankX, y, textScale, rankStr);
        drawStrokeText(scoreX, y, textScale, scoreStr);
    }

    glLineWidth(1.0f);
}

void drawPlaceholderOptions(void)
{
    drawPlaceholderScreen("[ OPTIONS ]", 30, 30, 30);
}

void drawPlaceholderAbout(void)
{
    iClear();
    iShowImage(0, 0, 1920, 1080, texCreditPage);
}

void drawStartScreen(void)
{
    iClear();
    iShowImage(0, 0, 1920, 1080, texStartPage);
}
