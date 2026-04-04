#ifndef AUDIO_H
#define AUDIO_H

/* ══════════════════════════════════════════════════════════
 *  AUDIO MODULE
 *  Defines file paths and playback helpers for game audio.
 *
 *  Audio assets (located in the "Audios" folder):
 *    AUDIO_GAMEPLAY  — Looping background music during gameplay.
 *    AUDIO_GAMEOVER  — One-shot sound on player defeat.
 *
 *  Uses the Windows MCI (Media Control Interface) API.
 *
 *  IMPORTANT: Call audioInit() once at startup to pre-load
 *  the game-over sound so it plays with zero delay.
 * ══════════════════════════════════════════════════════════ */

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

/* ── External state from options module ────────────────── */
extern bool isMusicOn;  /* Defined in options.cpp */

/* ── Audio File Paths ──────────────────────────────────── */
#define AUDIO_GAMEPLAY  "Audios/background2.mp3"
#define AUDIO_GAMEOVER  "Audios/gameover2.mp3"

/* ── Track whether gameover alias is pre-loaded ────────── */
static bool gameoverPreloaded = false;

/* ══════════════════════════════════════════════════════════
 *  audioInit  — Call ONCE at startup (from gameInit).
 *  Pre-opens the game-over sound so it can play instantly.
 * ══════════════════════════════════════════════════════════ */
static void audioInit(void) {
  /* Pre-load game-over audio into MCI so play is instant */
  mciSendStringA("open \"" AUDIO_GAMEOVER "\" type mpegvideo alias gameover", NULL, 0, NULL);
  gameoverPreloaded = true;
}

/* ══════════════════════════════════════════════════════════
 *  Gameplay Music
 * ══════════════════════════════════════════════════════════ */

/* Opens and starts the gameplay music loop. */
static void audioPlayGameplay(void) {
  mciSendStringA("close gameplay", NULL, 0, NULL);

  if (!isMusicOn) return;

  mciSendStringA("open \"" AUDIO_GAMEPLAY "\" type mpegvideo alias gameplay", NULL, 0, NULL);
  mciSendStringA("play gameplay repeat", NULL, 0, NULL);
}

/* Stops the gameplay music. */
static void audioStopGameplay(void) {
  mciSendStringA("stop gameplay",  NULL, 0, NULL);
  mciSendStringA("close gameplay", NULL, 0, NULL);
}

/* ══════════════════════════════════════════════════════════
 *  Game-Over Sound  (pre-loaded for ZERO delay)
 * ══════════════════════════════════════════════════════════ */

/* Plays the game-over sound instantly.
 * The alias was already opened by audioInit(), so we just
 * seek to the beginning and play — no file I/O at death. */
static void audioPlayGameOver(void) {
  if (!isMusicOn) return;

  if (!gameoverPreloaded) {
    /* Safety fallback — open if init was somehow missed */
    mciSendStringA("open \"" AUDIO_GAMEOVER "\" type mpegvideo alias gameover", NULL, 0, NULL);
    gameoverPreloaded = true;
  }

  /* Rewind to start and play immediately */
  mciSendStringA("seek gameover to start", NULL, 0, NULL);
  mciSendStringA("play gameover", NULL, 0, NULL);
}

/* Stops the game-over sound (but keeps the alias open for reuse). */
static void audioStopGameOver(void) {
  mciSendStringA("stop gameover", NULL, 0, NULL);
}

/* ══════════════════════════════════════════════════════════
 *  Convenience
 * ══════════════════════════════════════════════════════════ */

/* Stops ALL audio channels. */
static void audioStopAll(void) {
  audioStopGameplay();
  audioStopGameOver();
}

#endif /* AUDIO_H */
