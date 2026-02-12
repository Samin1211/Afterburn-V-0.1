#include "menu.h" /* For GameState enum and switching back to STATE_MENU */
#include "options.h"
#include <iostream>

/* ══════════════════════════════════════════════════════════
 *  FORWARD DECLARATIONS (Avoid including iGraphics.h)
 * ══════════════════════════════════════════════════════════ */
extern int iScreenWidth, iScreenHeight;
extern int iMouseX, iMouseY;

unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iClear(void);
void iSetColor(double r, double g, double b);
void iText(double x, double y, char *str, void *font);


/* ══════════════════════════════════════════════════════════
 *  INTERNAL STATE & ASSETS
 * ══════════════════════════════════════════════════════════ */

/*  Internal Enum for Sub-screens */
enum OptionsScreen { SCREEN_MAIN, SCREEN_CONTROLS };

static OptionsScreen currentScreen = SCREEN_MAIN;
bool isMusicOn = true; /* Default to ON */

/*  Texture IDs */
static unsigned int texOptBg = 0;
static unsigned int texOptControlsBg = 0; /* control_menu.png */
static unsigned int texBtnMusicOn = 0;
static unsigned int texBtnMusicOff = 0;
static unsigned int texBtnControls = 0;
static unsigned int texBtnBack = 0;         /* Main Options Back */
static unsigned int texBtnBackControls = 0; /* Controls Screen Back */


/*  Layout Constants (Adjusted for 1920x1080) */
/*  Music & Controls: Centered List
    Back: Bottom-Left (88, 75) matching margins */
#define START_Y 600

/*  Coordinates (Internal tracking for hover) */
struct Button {
  int x, y, w, h;
  bool hovered;
};

/*  Button Instances with EXACT dimensions from assets */
/*  Music: 364x87 */
static Button btnMusic = {1920 / 2 - 364 / 2, START_Y, 364, 87, false};

/*  Controls: 262x73 */
/*  Placed below Music with gap */
static Button btnControls = {1920 / 2 - 262 / 2, START_Y - 87 - 30, 262, 73,
                             false};

/*  Main Options Back: 137x73 */
/*  Placed below Controls with gap */
static Button btnBack = {1920 / 2 - 137 / 2, START_Y - 87 - 30 - 73 - 30, 137,
                         73, false};

/*  Controls Screen Back: 283x141 */
/*  Placed at bottom-left corner */
static Button btnBackCtrl = {88, 75, 283, 141,
                             false}; /* Matching menu margin (88, 75) */




/* ══════════════════════════════════════════════════════════
 *  FUNCTIONS
 * ══════════════════════════════════════════════════════════ */

void optionsInit(void) {
  /* Load Backgrounds */
  texOptBg = iLoadImage("Asset/Options files/Buttons/background.png");
  texOptControlsBg =
      iLoadImage("Asset/Options files/Buttons/control_menu.png"); /* Static Controls BG */

  /* Load Buttons */
  texBtnMusicOn = iLoadImage("Asset/Options files/Buttons/music_on.png");
  texBtnMusicOff = iLoadImage("Asset/Options files/Buttons/music_off.png");
  texBtnControls = iLoadImage("Asset/Options files/Buttons/controls.png");
  texBtnBack = iLoadImage("Asset/Options files/Buttons/back.png");
  texBtnBackControls =
      iLoadImage("Asset/Options files/Buttons/back_controls.png");
}

/* Helper to draw button with hover scale */
static void drawButton(unsigned int tex, Button &b) {
  if (b.hovered) {
    /* Scale up slightly from center */
    int inflate = 10;
    iShowImage(b.x - inflate / 2, b.y - inflate / 2, b.w + inflate,
               b.h + inflate, tex);
  } else {
    iShowImage(b.x, b.y, b.w, b.h, tex);
  }
}

static void drawMainOptions(void) {
  iShowImage(0, 0, 1920, 1080, texOptBg);

  /* Music Toggle */
  if (isMusicOn)
    drawButton(texBtnMusicOn, btnMusic);
  else
    drawButton(texBtnMusicOff, btnMusic);

  /* Controls */
  drawButton(texBtnControls, btnControls);

  /* Back */
  drawButton(texBtnBack, btnBack);
}

static void drawControlsScreen(void) {
  /* Draw Static Background (Full Screen) */
  iShowImage(0, 0, 1920, 1080, texOptControlsBg);

  /* Draw ONLY Back Button */
  drawButton(texBtnBackControls, btnBackCtrl);
}

void optionsDraw(void) {
  if (currentScreen == SCREEN_MAIN) {
    drawMainOptions();
  } else {
    drawControlsScreen();
  }
}

/* Helper for AABB check */
static bool isInside(int mx, int my, Button b) {
  return (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
}

void optionsMouseMove(int mx, int my) {
  if (currentScreen == SCREEN_MAIN) {
    btnMusic.hovered = isInside(mx, my, btnMusic);
    btnControls.hovered = isInside(mx, my, btnControls);
    btnBack.hovered = isInside(mx, my, btnBack);
  } else {
    btnBackCtrl.hovered = isInside(mx, my, btnBackCtrl);
  }
}

extern GameState gameState; /* Access global state from iMain.cpp/menu.h to switch back */

void optionsMouseClick(int mx, int my) {
  if (currentScreen == SCREEN_MAIN) {
    if (isInside(mx, my, btnMusic)) {
      isMusicOn = !isMusicOn; /* Toggle */
    } else if (isInside(mx, my, btnControls)) {
      currentScreen = SCREEN_CONTROLS;
    } else if (isInside(mx, my, btnBack)) {
      gameState = STATE_MENU; /* Return to Main Menu */
    }
  } else {
    if (isInside(mx, my, btnBackCtrl)) {
      currentScreen = SCREEN_MAIN; /* Return to Options Main */
    }
  }
}
