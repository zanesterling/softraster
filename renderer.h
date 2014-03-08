#ifndef RENDERER
#define RENDERER
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include "surface.h"
#include "matrix.h"

void PutPixel32(SDL_Surface *, int, int, Uint32);
void PutPixel32_nolock(SDL_Surface *, int, int, Uint32);

Uint32 GetPixel32(SDL_Surface *, int, int);
Uint32 GetPixel32_nolock(SDL_Surface *, int, int);

void drawLine4i(SDL_Surface *, int, int, int, int, Uint32);
void drawLine2m(SDL_Surface *, Matrix4f *, Matrix4f *, Uint32);
void drawLine2v(SDL_Surface *, Vec4f *, Vec4f *, Uint32);
void drawEdges(SDL_Surface *, Matrix4f *, Uint32);

void clear(SDL_Surface *);
void clear(SDL_Surface *, Uint32);

void savePPM(SDL_Surface *, char *);
void savePPM(SDL_Surface *, std::string);
#endif
