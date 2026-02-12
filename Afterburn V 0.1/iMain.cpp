#include "iGraphics.h"
#include "menu.h"

/* ══════════════════════════════════════════════════════════
 *  GLOBAL STATE & MODULES
 * ══════════════════════════════════════════════════════════ */

/*  Current game state (Must be declared before use) */
GameState gameState = STATE_MENU;

#include "Enemy.h"
#include "Player.h"
#include "Road.h"
#include "UI.h"
#include "options.h"

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

    /* Check Game Over (Player Health <= 0) */
    /* Only trigger Game Over if not winning/boss dying */
    if (player.health <= 0 && getPhase() != PHASE_WIN && !isBossDying())
      gameState = STATE_GAMEOVER;
  }
}

/*  gameInit  –  Call once from main() (loads all textures)   */
void gameInit(void) {
  roadInit();
  uiInit();
  optionsInit();
  playerInit();
  enemyInit();

  /* Also reset state on first init */
  roadReset();
  playerReset();
  enemyReset();
}

/*  gameReset – Lightweight state-only reset (no I/O)         */
void gameReset(void) {
  roadReset();
  playerReset();
  enemyReset();
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
    optionsDraw();
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
  } else if (gameState == STATE_OPTIONS) {
    optionsMouseMove(mx, my);
  }
}

static bool mouseLeftDown = false;

/* ── Mouse click ────────────────────────────────────────── */
void iMouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseLeftDown = true;
    }
    if (state == GLUT_UP) {
      mouseLeftDown = false;
    }
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    firePlayerMissile();
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == STATE_MENU) {
      GameState next = menuMouseClick(mx, my);
      if (next == STATE_GAME) {
        gameReset(); /* Reset everything for a fresh game */
      }
      gameState = next;
    } else if (gameState == STATE_GAME) {
      /* Fire cannon! */
      playerShoot();
    } else if (gameState == STATE_OPTIONS) {
      optionsMouseClick(mx, my);
    }
  }
}

/* ── Keyboard input (polled every frame by iGraphics) ───── */
void fixedUpdate() {
  /* Restart (R key, case-insensitive) */
  if (isKeyPressed('r') || isKeyPressed('R')) {
    if (gameState == STATE_GAMEOVER ||
        (gameState == STATE_GAME && getPhase() == PHASE_WIN)) {
      gameReset();
      gameState = STATE_GAME;
    }
  }

  /* ESC returns to the main menu from any state */
  if (isKeyPressed(27)) /* 27 = ASCII Escape */
  {
    if (gameState != STATE_MENU) {
      gameReset(); /* Reset so returning to game later is clean */
      gameState = STATE_MENU;
    }
  }

  /* Fire Control: Mouse OR Space */
  if (gameState == STATE_GAME) {
    player.isFiring = mouseLeftDown || isKeyPressed(' ');
  }
}

/* ── Keyboard input (iGraphics callback) ────────────────── */
void iKeyboard(unsigned char key) {
  if (gameState == STATE_GAMEOVER ||
      (gameState == STATE_GAME && getPhase() == PHASE_WIN)) {
    if (key == 'r' || key == 'R') {
      gameReset(); /* Reset Everything */
      gameState = STATE_GAME;
    }
    if (key == 27) { /* ESC */
      gameState = STATE_MENU;
    }
    return;
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