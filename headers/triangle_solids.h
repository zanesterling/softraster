#ifndef TRI_SOLIDS
#define TRI_SOLIDS

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "renderer.h"
#include "matrix.h"
#include "transform.h"
#include "window.h"

void usage(char **);
bool parse(char **);
bool getLine(FILE *, char *, char **, float *);
float dtor(float); // degrees to radians
void makeBox(Matrix4f *triangleMatrix);
void makeSphere(Matrix4f *triangleMatrix);

#endif
