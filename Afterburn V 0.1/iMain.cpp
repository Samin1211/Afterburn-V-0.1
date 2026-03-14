#include "iGraphics.h"
#include "menu.h"

/* ══════════════════════════════════════════════════════════
 *  GLOBAL STATE & MODULES
 *
 *  HOW IT WORKS:
 *  This is the main entry point and game loop controller.
 *  It ties together all modules (Player, Enemy, NPC, Road, UI, Menu, Options)
 *  and routes rendering, input, and updates based on the current GameState.
 *
 *  GAME STATES:
 *  STATE_MENU      — Main menu screen (menuDraw/menuMouseClick)
 *  STATE_GAME      — Active gameplay (gameUpdate/gameDraw)
 *  STATE_GAMEOVER  — Player died, showing game over screen
 *  STATE_LEADERBOARD — Placeholder leaderboard screen
 *  STATE_OPTIONS   — Options/settings screen
 *  STATE_ABOUT     — Placeholder about screen
 * ══════════════════════════════════════════════════════════ */

/* gameState: The master state variable controlling which screen is active.
 * All rendering and input routing depends on this value.
 * HOW TO CHANGE: Set to any GameState enum value to switch screens.
 * EFFECT: Controls what the player sees and what logic runs. */
GameState gameState = STATE_MENU;

#include "Enemy.h"
#include "Player.h"
#include "Road.h"
#include "UI.h"
#include "options.h"

/* ══════════════════════════════════════════════════════════
 *  GAME LOOP & LOGIC
 * ══════════════════════════════════════════════════════════ */

/* gameUpdate: The main game tick — called by iSetTimer every ~16ms (60 FPS).
 *
 * HOW IT WORKS:
 * Only runs game logic when in STATE_GAME. Each frame:
 * 1. Updates player (movement, rotation, projectiles) via playerUpdate().
 * 2. Updates road (scrolling, score generation) via roadUpdate().
 * 3. Updates enemies, bosses, NPCs, collisions via enemyUpdate().
 * 4. Checks for game over (player.health <= 0), but NOT during:
 *    - Win screen (PHASE_WIN)
 *    - Boss 1 dying animation (isBossDying())
 *    - Boss 2 dying animation (isBoss2Dying())
 *    This prevents false game-overs during boss death sequences.
 *
 * HOW TO CHANGE: Add new update calls here for new game systems.
 * EFFECT: This is the heartbeat of the game. Order of updates matters
 *         for correct collision detection timing. */
void gameUpdate(void) {
  /* Only update game logic if in Playing State */
  if (gameState == STATE_GAME) {
    /* Update Player (Movement, Rotation, Projectiles) */
    playerUpdate();

    /* Update Road (Scrolling, Score Generation) */
    roadUpdate();

    /* Update Enemies (Spawn, Move, Collisions, Boss Logic) */
    enemyUpdate();

    /* Check Game Over: Player Health <= 0.
     * Skip during win screen, boss dying, or boss2 dying to prevent
     * false game-overs from last-moment damage. */
    if (player.health <= 0 && getPhase() != PHASE_WIN && !isBossDying() &&
        !isBoss2Dying())
      gameState = STATE_GAMEOVER;
  }
}

/* gameInit: One-time initialization — loads ALL textures from disk.
 *
 * HOW IT WORKS:
 * Calls each module's Init function to load its textures into GPU memory.
 * Also calls Reset functions to set initial game state.
 * Called once in main() before the game loop starts.
 *
 * ORDER MATTERS: npcInit() must be called before enemyInit() because
 * Enemy.h's enemy drawing uses textures loaded by NPC.h.
 *
 * HOW TO CHANGE: Add new module init calls here when adding new systems.
 * EFFECT: Missing an init call means missing textures (white rectangles). */
void gameInit(void) {
  roadInit();    /* Load road textures */
  uiInit();      /* Load HUD textures */
  optionsInit(); /* Load options menu textures */
  playerInit();  /* Load player car + projectile textures */
  npcInit();     /* Load NPC car + explosion textures */
  enemyInit();   /* Load enemy, boss, truck, orb, cloud textures */

  /* Also reset state on first init */
  roadReset();
  playerReset();
  enemyReset();
}

/* gameReset: Lightweight state-only reset for restarting the game.
 * Does NOT reload textures (already in GPU memory from gameInit).
 *
 * HOW IT WORKS:
 * Resets road scroll position, player state (health, score, position),
 * and all enemy/NPC/boss state to start fresh.
 * Called when: starting "New Game", pressing 'R' to restart.
 *
 * HOW TO CHANGE: Add new module reset calls here.
 * EFFECT: A clean game restart without the I/O cost of texture loading. */
void gameReset(void) {
  roadReset();
  playerReset();
  enemyReset();
}

/* gameDraw: Renders the complete game scene in correct layer order.
 *
 * HOW IT WORKS (bottom to top):
 * 1. Road background (bottommost layer)
 * 2. Player car + projectiles
 * 3. Enemies, NPCs, bosses, trucks, orbs, explosions
 * 4. HUD overlay (health bar, score, stars, boss HP)
 * 5. Cloud transition overlay (topmost — covers everything during transitions)
 *
 * HOW TO CHANGE: Reorder the draw calls to change layering.
 * EFFECT: Draw order = visual depth. Later draws appear on top. */
void gameDraw(void) {
  iClear();

  /* 1. Road Background (bottommost layer) */
  roadDraw();

  /* 2. Game Entities (Player, then Enemies/NPCs on top) */
  playerDraw();
  enemyDraw();

  /* 3. HUD Overlay (health, score, etc.) */
  uiDraw();

  /* 4. Cloud transition overlay (on top of EVERYTHING during stage transitions)
   */
  cloudOverlayDraw();
}

/* ══════════════════════════════════════════════════════════
 *  MAIN / CALLBACKS
 *  These functions are called by the iGraphics framework.
 * ══════════════════════════════════════════════════════════ */

/* ── iDraw: route rendering to the active state ─────────── */
/* iDraw: Called by iGraphics every frame to render the current screen.
 * Routes to the appropriate draw function based on gameState. */
void iDraw() {
  iClear();

  switch (gameState) {
  case STATE_MENU:
    menuDraw(); /* Main menu with buttons */
    break;

  case STATE_GAME:
  case STATE_GAMEOVER:
    gameDraw(); /* Game scene (game over overlay is drawn by uiDraw) */
    break;

  case STATE_LEADERBOARD:
    drawPlaceholderLeaderboard(); /* Placeholder screen */
    break;

  case STATE_OPTIONS:
    optionsDraw(); /* Options menu (music toggle, controls) */
    break;

  case STATE_ABOUT:
    drawPlaceholderAbout(); /* Placeholder screen */
    break;

  default:
    break;
  }
}

/* ── Mouse drag (unused) ────────────────────────────────── */
void iMouseMove(int mx, int my) {}

/* ── Passive mouse move: update global mouse pos ────────── */
/* iPassiveMouseMove: Called whenever the mouse moves (without buttons held).
 * Updates the global mouse position used for player aiming.
 * Routes to menu/options hover handlers for button highlight effects. */
void iPassiveMouseMove(int mx, int my) {
  /* Update iGraphics global mouse position for player aiming */
  iMouseX = mx;
  iMouseY = my;

  /* Route to menu/options for hover effects on buttons */
  if (gameState == STATE_MENU) {
    menuMouseMove(mx, my);
  } else if (gameState == STATE_OPTIONS) {
    optionsMouseMove(mx, my);
  }
}

/* mouseLeftDown: Tracks whether the left mouse button is currently held.
 * Used by fixedUpdate() to enable continuous fire (isFiring flag). */
static bool mouseLeftDown = false;

/* ── Mouse click ────────────────────────────────────────── */
/* iMouse: Called by iGraphics on mouse button events.
 *
 * HOW IT WORKS:
 * - Left button DOWN: Sets mouseLeftDown for continuous fire tracking.
 * - Left button UP: Clears mouseLeftDown.
 * - Right button DOWN: Fires a heavy missile (boss fight only).
 * - Left click in STATE_MENU: Routes to menu button click handler.
 *   If "New Game" is clicked, gameReset() is called for a fresh start.
 * - Left click in STATE_GAME: Fires a single cannon shot.
 * - Left click in STATE_OPTIONS: Routes to options click handler.
 *
 * HOW TO CHANGE: Add new click handlers for new game states.
 * EFFECT: This is the primary mouse input router for the entire game. */
void iMouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseLeftDown = true;
    }
    if (state == GLUT_UP) {
      mouseLeftDown = false;
    }
  }
  /* Right-click: Fire heavy missile during boss fights */
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    firePlayerMissile(); /* Uses missileCount from power orb pickup */
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == STATE_MENU) {
      GameState next = menuMouseClick(mx, my);
      if (next == STATE_GAME) {
        gameReset(); /* Reset everything for a fresh game */
      }
      gameState = next;
    } else if (gameState == STATE_GAME) {
      playerShoot(); /* Fire a cannon bullet toward mouse cursor */
    } else if (gameState == STATE_OPTIONS) {
      optionsMouseClick(mx, my);
    }
  }
}

/* ── fixedUpdate: polled every frame by iGraphics ───────── */
/* fixedUpdate: Called every frame for keyboard-based logic.
 *
 * HOW IT WORKS:
 * - 'R' key: Restarts the game from game over or win screen.
 * - ESC key: Returns to main menu from any state (with a clean reset).
 * - Continuous fire: Sets player.isFiring based on mouse button or space bar.
 *
 * HOW TO CHANGE: Add new key handlers here for additional controls.
 * EFFECT: This handles game-flow controls (restart, menu) and continuous fire.
 */
void fixedUpdate() {
  /* Restart (R key) — only from Game Over or Win screen */
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

  /* Continuous Fire Control: left mouse held OR space bar held */
  if (gameState == STATE_GAME) {
    player.isFiring = mouseLeftDown || isKeyPressed(' ');
  }
}

/* ── iKeyboard: iGraphics key-down callback ─────────────── */
/* iKeyboard: Called by iGraphics on single key press events.
 * Handles restart and menu-return during game over and win screens.
 * Note: This overlaps with fixedUpdate() for redundancy/responsiveness. */
void iKeyboard(unsigned char key) {
  if (gameState == STATE_GAMEOVER ||
      (gameState == STATE_GAME && getPhase() == PHASE_WIN)) {
    if (key == 'r' || key == 'R') {
      gameReset();            /* Reset Everything */
      gameState = STATE_GAME; /* Start new game */
    }
    if (key == 27) {          /* ESC */
      gameState = STATE_MENU; /* Return to menu */
    }
    return;
  }
}

/* ── Entry point ────────────────────────────────────────── */
/* main: The program entry point.
 *
 * HOW IT WORKS:
 * 1. Initializes the iGraphics window at 1920x1080 resolution.
 * 2. Sets up a 16ms timer (≈60 FPS) that calls gameUpdate() each tick.
 * 3. Initializes the menu system and all game modules.
 * 4. Starts the iGraphics main loop (never returns normally).
 *
 * HOW TO CHANGE:
 * - Window size: Change iInitialize(1920, 1080, ...) for different resolution.
 * - FPS: Change iSetTimer(16, ...) — 16ms = 60FPS, 33ms = 30FPS.
 * - Window title: Change the string "Afterburn V 0.1".
 * EFFECT: Resolution changes affect all position calculations in the game. */
int main() {
  /* Audio loading (commented out for now) */
  /* mciSendString("...", NULL, 0, NULL); */

  iInitialize(1920, 1080, "Afterburn V 0.1"); /* Create 1920x1080 window */

  /* Initialize Timer for Game Loop (~60 FPS) */
  iSetTimer(16, gameUpdate); /* 16ms ≈ 60 FPS */

  /* Initialize Systems */
  menuInit(); /* Load menu textures and button layout */
  gameInit(); /* Load all game textures and reset initial state */

  iStart(); /* Enter the iGraphics main loop (blocks forever) */
  return 0;
}