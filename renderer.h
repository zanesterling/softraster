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
void drawline4i(SDL_Surface *, int, int, int, int, Uint32);
void drawline2m(SDL_Surface *, Matrix4f *, Matrix4f *, Uint32);
void savePPM(SDL_Surface *, std::string);
void savePPM(SDL_Surface *, char *);
#endif
