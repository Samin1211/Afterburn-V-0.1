#include "iGraphics.h"
#include "menu.h"

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
		drawPlaceholderGame();
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
	/* ESC returns to the main menu from any placeholder */
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

	iStart();
	return 0;
}