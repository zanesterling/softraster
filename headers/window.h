#ifndef WINDOW
#define WINDOW

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "renderer.h"
#include "matrix.h"
#include "transform.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *drawTexture;
extern SDL_Surface *drawSurface;

void drawToScreen();
bool init(int, int);
void clean_up();
void error(std::string);

void renderParallelTriangles(Matrix4f *, const int *);
void renderPerspectiveCyclopsTriangles(Matrix4f *, const float *, const int *);
void renderPerspectiveStereoTriangles(Matrix4f *, const float *, const int *);

#endif
