#ifndef ROAD_H
#define ROAD_H

/* ══════════════════════════════════════════════════════════
 *  ROAD MODULE
 *  Management of the scrolling road background.
 *  (Currently static image, scrolling TODO)
 * ══════════════════════════════════════════════════════════ */

/*  Texture handle for the road background.
 *  Loaded once in roadInit(), drawn every frame in roadDraw(). */
static unsigned int texRoad = 0;

/*  roadInit  –  loads road assets.
 *  Call from gameInit() in iMain.cpp. */
void roadInit(void)
{
	/* Road background – scaled to fill the full 1920x1080 window */
	texRoad = iLoadImage("Asset/Roads/road 4.png");
}

/*  roadDraw  –  renders the road background.
 *  Call from gameDraw() in iMain.cpp. */
void roadDraw(void)
{
	/* Draw road 4.png stretched to the entire screen (1920x1080) */
	iShowImage(0, 0, 1920, 1080, texRoad);
}

#endif /* ROAD_H */
