#ifndef MENU_H
#define MENU_H

/*==========================================================
 *  menu.h  —  Main‑menu system for Afterburn V 0.1
 *
 *  This header is safe to include from any translation unit.
 *  It does NOT pull in iGraphics.h (which contains
 *  STB_IMAGE_IMPLEMENTATION and must only be included once).
 *=========================================================*/

/* ── Game states ──────────────────────────────────────── */
enum GameState {
  STATE_MENU,
  STATE_GAME,
  STATE_GAMEOVER,
  STATE_LEADERBOARD,
  STATE_OPTIONS,
  STATE_ABOUT,
  STATE_EXIT
};

/* ── Menu button descriptor ────────────────────────────── */
struct MenuButton {
  unsigned int texture; /* OpenGL texture id              */
  int x, y;             /* bottom-left position (native)  */
  int w, h;             /* native pixel dimensions        */
  float scale;          /* current draw scale (1.0 norm.) */
  float targetScale;    /* desired scale (lerp target)    */
  GameState target;     /* state to switch to on click    */
};

/* ── Total number of clickable buttons ─────────────────── */
#define MENU_BUTTON_COUNT 5

/* ── Public API ────────────────────────────────────────── */

/*  Call once in main() before iStart().
 *  Loads all menu textures.                                */
void menuInit(void);

/*  Draw the full main‑menu screen.
 *  Called from iDraw() when state == STATE_MENU.           */
void menuDraw(void);

/*  Forward iPassiveMouseMove coords here every frame.
 *  Updates hover / scale state on each button.             */
void menuMouseMove(int mx, int my);

/*  Forward left‑click coords here.
 *  Returns the new GameState (may remain STATE_MENU).      */
GameState menuMouseClick(int mx, int my);

/*  Placeholder draw functions for sub‑screens.
 *  Each renders a simple screen with "Press ESC" text.     */
void drawPlaceholderGame(void);
void drawPlaceholderLeaderboard(void);
void drawPlaceholderOptions(void);
void drawPlaceholderAbout(void);

#endif /* MENU_H */
