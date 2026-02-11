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

/*  Texture handle for the road background.
 *  Loaded once in gameInit(), drawn every frame in gameDraw().
 *  (Move to game.h as: extern unsigned int texRoad;)            */
static unsigned int texRoad = 0;

/*  gameInit  –  call once from main(), after iInitialize().
 *  Loads all game-stage assets.
 *  (Move to game.h as: void gameInit(void);)                    */
void gameInit(void)
{
	/* Road background – scaled to fill the full 1920x1080 window */
	texRoad = iLoadImage("Asset/Roads/road 4.png");
}

/*  gameDraw  –  render the game-stage scene.
 *  Called from iDraw() when gameState == STATE_GAME.
 *  (Move to game.h as: void gameDraw(void);)                    */
void gameDraw(void)
{
	iClear();

	/* Draw road 4.png stretched to the entire screen (1920x1080) */
	iShowImage(0, 0, 1920, 1080, texRoad);

	/* TODO: draw cars, HUD, enemies, etc. on top of the road */
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