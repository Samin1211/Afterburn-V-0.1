#ifndef OPTIONS_H
#define OPTIONS_H

/* ══════════════════════════════════════════════════════════
 *  OPTIONS MODULE
 *  Handles the Options menu, Music Toggle, and Controls screen.
 * ══════════════════════════════════════════════════════════ */

/*  Global Music State (for future audio integration) */
extern bool isMusicOn;

/*  Functions to be called from iMain.cpp */
void optionsInit(void);
void optionsDraw(void);
void optionsMouseMove(int mx, int my);
void optionsMouseClick(int mx, int my);

#endif /* OPTIONS_H */
