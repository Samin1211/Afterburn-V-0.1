#include "iGraphics.h"
#include "menu.h"

/* ══════════════════════════════════════════════════════════
 *  GAME STAGE  –  variables, init, draw, input
 *
 *  Everything between the "GAME STAGE BEGIN" and
 *  "GAME STAGE END" markers can later be moved into
 *  a separate game.h / game.cpp pair.
 * ══════════════════════════════════════════════════════════ */

/* ── GAME STAGE BEGIN ─────────────────────────────────────── */

/* ── Texture handles ──────────────────────────────────────
 *  Loaded once in gameInit(), drawn every frame in gameDraw().
 *  (Move to game.h as extern declarations)                      */
static unsigned int texRoad       = 0;  /* road background       */
static unsigned int texHealthBar  = 0;  /* health bar frame/skin */
static unsigned int texStarSystem = 0;  /* star sprite sheet     */

/* ── HUD layout constants (pixels, 1920x1080 canvas) ─────
 *  All positions use iGraphics coords: (0,0) = bottom-left.
 *  Tweak these #defines to reposition or resize the HUD.        */

/*  Health Bar draw size  (original 860x218, ~50% scale)         */
#define HB_DRAW_W   430
#define HB_DRAW_H   109

/*  Health Bar position – top-left of screen with 20px margin    */
#define HB_X        20
#define HB_Y        (1080 - 20 - HB_DRAW_H)            /* 951   */

/*  Red fill rectangle – sits INSIDE the bar frame.
 *  These offsets are relative to (HB_X, HB_Y) and were measured
 *  from the Health Bar.png artwork (195-848 x, 60-183 y in the
 *  860x218 source), then scaled to the draw size.               */
#define HB_FILL_OX   97    /* left offset inside the bar    */
#define HB_FILL_OY   30    /* bottom offset inside the bar  */
#define HB_FILL_W    327   /* fill width  at full health    */
#define HB_FILL_H    58    /* fill height                   */

/*  Star System draw size for ONE row.
 *  Original sheet: 808x988, 6 rows → each row ~165 px tall.
 *  We draw one row at a width that roughly matches the bar.     */
#define STAR_DRAW_W  350
#define STAR_DRAW_H  71

/*  Star System position – below health bar with 10px gap        */
#define STAR_X       20
#define STAR_Y       (HB_Y - 10 - STAR_DRAW_H)          /* 870  */

/*  Star sprite sheet metrics                                    */
#define STAR_ROWS    6     /* total rows in Star System.png      */

/* ── Game-state variables ─────────────────────────────────
 *  (Move to game.h when extracting into a header)               */

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
 *  iGraphics.h (V goes from 0 at bottom to -1 at top).
 *  (Move to game.h as: void iShowImageSub(...);)                */
static void iShowImageSub(int x, int y, int w, int h,
                           unsigned int texture,
                           int rowIndex, int totalRows)
{
	/* Compute vertical texture coords for the requested row.
	 * In iGraphics' convention:
	 *   V = -1  → top of the image   (row 0 top edge)
	 *   V =  0  → bottom of the image (last row bottom edge)
	 *
	 * Row 0  top    : V = -1
	 * Row 0  bottom : V = -1 + 1/totalRows
	 * Row r  top    : V = -1 + r/totalRows
	 * Row r  bottom : V = -1 + (r+1)/totalRows                */
	float rowH = 1.0f / (float)totalRows;
	float vTop    = -1.0f + (float)rowIndex * rowH;
	float vBottom = vTop + rowH;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, vBottom); glVertex2f((float)x,     (float)y);
		glTexCoord2f(1.0f, vBottom); glVertex2f((float)(x+w), (float)y);
		glTexCoord2f(1.0f, vTop);    glVertex2f((float)(x+w), (float)(y+h));
		glTexCoord2f(0.0f, vTop);    glVertex2f((float)x,     (float)(y+h));
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

/* ── gameInit ─────────────────────────────────────────────
 *  Call once from main(), after iInitialize().
 *  Loads all game-stage assets.
 *  (Move to game.h as: void gameInit(void);)                    */
void gameInit(void)
{
	/* Road background – scaled to fill the full 1920x1080 window */
	texRoad       = iLoadImage("Asset/Roads/road 4.png");

	/* HUD elements */
	texHealthBar  = iLoadImage("Asset/Health Bar.png");
	texStarSystem = iLoadImage("Asset/Star System.png");
}

/* ── gameDraw ─────────────────────────────────────────────
 *  Render the game-stage scene.
 *  Called from iDraw() when gameState == STATE_GAME.
 *  (Move to game.h as: void gameDraw(void);)                    */
void gameDraw(void)
{
	iClear();

	/* ── 1. Road background ──────────────────────────────── */
	iShowImage(0, 0, 1920, 1080, texRoad);

	/* ── 2. Health bar ───────────────────────────────────── */
	/*  Step A: red fill rectangle (drawn first, behind the frame).
	 *  Width is proportional to playerHealth (1.0 = full).       */
	iSetColor(255, 0, 0);
	iFilledRectangle(
		HB_X + HB_FILL_OX,
		HB_Y + HB_FILL_OY,
		(int)(HB_FILL_W * playerHealth),
		HB_FILL_H
	);

	/*  Step B: health bar frame / skin on top (has transparency) */
	iShowImage(HB_X, HB_Y, HB_DRAW_W, HB_DRAW_H, texHealthBar);

	/* ── 3. Star system (row 0 = all 5 black stars) ──────── */
	iShowImageSub(STAR_X, STAR_Y, STAR_DRAW_W, STAR_DRAW_H,
	              texStarSystem, starRow, STAR_ROWS);

	/* TODO: draw cars, enemies, score text, etc. */
}

/* ── GAME STAGE END ───────────────────────────────────────── */


/* ── Current game state ─────────────────────────────────── */
static GameState gameState = STATE_MENU;

/* ── iDraw: route rendering to the active state ─────────── */
void iDraw()
{
	iClear();

	switch (gameState)
	{
	case STATE_MENU:
		menuDraw();
		break;

	case STATE_GAME:
		/* ── Use the real game stage instead of the placeholder ── */
		gameDraw();
		break;

	case STATE_LEADERBOARD:
		drawPlaceholderLeaderboard();
		break;

	case STATE_OPTIONS:
		drawPlaceholderOptions();
		break;

	case STATE_ABOUT:
		drawPlaceholderAbout();
		break;

	default:
		break;
	}
}

/* ── Mouse drag (unused for now) ────────────────────────── */
void iMouseMove(int mx, int my)
{
}

/* ── Passive mouse move: forward to menu for hover ──────── */
void iPassiveMouseMove(int mx, int my)
{
	if (gameState == STATE_MENU)
	{
		menuMouseMove(mx, my);
	}
}

/* ── Mouse click ────────────────────────────────────────── */
void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (gameState == STATE_MENU)
		{
			gameState = menuMouseClick(mx, my);
		}
	}
}

/* ── Fixed update: keyboard polling ─────────────────────── */
void fixedUpdate()
{
	/* ESC returns to the main menu from any state */
	if (isKeyPressed(27))  /* 27 = ASCII Escape */
	{
		if (gameState != STATE_MENU)
		{
			gameState = STATE_MENU;
		}
	}
}

/* ── Entry point ────────────────────────────────────────── */
int main()
{
	/* NOTE: Audio loading is preserved but playback is
	   disabled during the menu state.  Uncomment when
	   gameplay is implemented.
	   mciSendString("open \"Audios//background.mp3\" alias bgsong", NULL, 0, NULL);
	   mciSendString("open \"Audios//gameover.mp3\" alias ggsong", NULL, 0, NULL);
	   mciSendString("play bgsong repeat", NULL, 0, NULL);
	*/

	iInitialize(1920, 1080, "Afterburn V 0.1");

	/* Load all menu assets before entering the main loop */
	menuInit();

	/* Load all game-stage assets (road texture, etc.) */
	gameInit();

	iStart();
	return 0;
}