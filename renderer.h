#ifndef RENDERER
#define RENDERER
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include "matrix.h"

void PutPixel32(SDL_Surface *, const int, const int, const Uint32);
void PutPixel32_nolock(SDL_Surface *, const int, const int, const Uint32);

Uint32 GetPixel32(SDL_Surface *, const int, const int);
Uint32 GetPixel32_nolock(SDL_Surface *, const int, const int);

void drawLine4i(SDL_Surface *, int, int, int, int, const Uint32);
void drawLine2m(SDL_Surface *, const Matrix4f *, const Matrix4f *, const Uint32);
void drawLine2v(SDL_Surface *, const Vec4f *, const Vec4f *, const Uint32);
void drawEdges(SDL_Surface *, const Matrix4f *, const Uint32);

void clear(SDL_Surface *);
void clear(SDL_Surface *, const Uint32);

void savePPM(SDL_Surface *, const char *);
void savePPM(SDL_Surface *, const std::string);

void fillTriangle(SDL_Surface *, const Matrix4f *, const Uint32);
void fillBottomFlatTriangle(SDL_Surface *surface, const Vec4f, const Vec4f,
                            const Vec4f, const Uint32);
void fillTopFlatTriangle(SDL_Surface *surface, const Vec4f, const Vec4f,
                         const Vec4f, const Uint32);
void fillRow(SDL_Surface *, const int, const int, const int, const Uint32);

#endif
