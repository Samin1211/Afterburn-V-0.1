#include "iGraphics.h"
#include "menu.h"

/* ══════════════════════════════════════════════════════════
 *  GLOBAL STATE & MODULES
 * ══════════════════════════════════════════════════════════ */

/*  Current game state (Must be declared before use) */
static GameState gameState = STATE_MENU;

#include "Enemy.h"
#include "Player.h"
#include "Road.h"
#include "UI.h"

/* ══════════════════════════════════════════════════════════
 *  GAME LOOP & LOGIC
 * ══════════════════════════════════════════════════════════ */

/*  gameUpdate  –  Called by timer every ~16ms (60 FPS)      */
void gameUpdate(void) {
  /* Only update game logic if in Playing State */
  if (gameState == STATE_GAME) {
    /* Update Player (Movement, Rotation, Projectiles) */
    playerUpdate();

    /* Update Road (Scrolling) */
    roadUpdate();

    /* Update Enemies (Spawn, Move, Collisions) */
    enemyUpdate();

    /* TODO: Check Game Over (Player Health <= 0) */
    /* if (player.health <= 0) gameState = STATE_GAMEOVER; */
  }
}

/*  gameInit  –  Call once from main()                       */
void gameInit(void) {
  roadInit();
  uiInit();
  playerInit();
  enemyInit();
}

/*  gameDraw  –  Render the game scene                       */
void gameDraw(void) {
  iClear();

  /* 1. Road Background */
  roadDraw();

  /* 2. Game Entities (Player, Enemies, Projectiles) */
  playerDraw();
  enemyDraw();

  /* 3. HUD Overlay */
  uiDraw();
}

/* ══════════════════════════════════════════════════════════
 *  MAIN / CALLBACKS
 * ══════════════════════════════════════════════════════════ */

/* ── iDraw: route rendering to the active state ─────────── */
void iDraw() {
  iClear();

  switch (gameState) {
  case STATE_MENU:
    menuDraw();
    break;

  case STATE_GAME:
  case STATE_GAMEOVER:
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

/* ── Mouse drag (unused) ────────────────────────────────── */
void iMouseMove(int mx, int my) {}

/* ── Passive mouse move: update global mouse pos ────────── */
void iPassiveMouseMove(int mx, int my) {
  /* Update iGraphics globals if not auto-updated */
  iMouseX = mx;
  iMouseY = my;

  if (gameState == STATE_MENU) {
    menuMouseMove(mx, my);
  }
}

/* ── Mouse click ────────────────────────────────────────── */
void iMouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == STATE_MENU) {
      gameState = menuMouseClick(mx, my);
    } else if (gameState == STATE_GAME) {
      /* Fire cannon! */
      playerShoot();
    }
  }
}

/* ── Keyboard input (polled in fixedUpdate or checks) ───── */
void fixedUpdate() {
  /* ESC returns to the main menu from any state */
  if (isKeyPressed(27)) /* 27 = ASCII Escape */
  {
    if (gameState != STATE_MENU) {
      gameState = STATE_MENU;
    }
  }

  /* R to restart logic */
  if (gameState == STATE_GAMEOVER) {
    if (isKeyPressed('r') || isKeyPressed('R')) {
      playerInit();
      enemyInit();
      roadInit();
      gameState = STATE_GAME;
    }
  }
}

/* ── Entry point ────────────────────────────────────────── */
int main() {
  /* Audio loading (commented out) */
  /* mciSendString("...", NULL, 0, NULL); */

  iInitialize(1920, 1080, "Afterburn V 0.1");

  /* Initialize Timer for Game Loop (60 FPS) */
  iSetTimer(16, gameUpdate);

  /* Initialize Systems */
  menuInit();
  gameInit();

  iStart();
  return 0;
}