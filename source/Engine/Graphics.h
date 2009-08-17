/*
 *  Graphics.h
 *  SdlSludge
 *
 *  Created by Rikard Peterson on 2009-05-01.
 *
 */

#ifndef __SLUDGE_GRAPHICS_H__
#define __SLUDGE_GRAPHICS_H__

#include "glee.h"

struct texture {
	GLubyte * data;
	GLuint name;
	int w, h;
	double texW, texH;
};


// From Backdrop.cpp, but they're here anyway
extern GLubyte * backdropTexture;
extern double backdropTexW, backdropTexH;


extern int winWidth, winHeight;
extern int viewportHeight, viewportWidth;
extern int viewportOffsetX, viewportOffsetY;
extern int realWinWidth, realWinHeight;

extern bool NPOT_textures;

void setPixelCoords (bool pixels);
void setGraphicsWindow(bool fullscreen, bool restoreGraphics = true);

int getNextPOT(int n);


#endif