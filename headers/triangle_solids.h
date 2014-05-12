#ifndef TRI_SOLIDS
#define TRI_SOLIDS

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <stack>
#include <map>
#include "renderer.h"
#include "matrix.h"
#include "transform.h"
#include "window.h"

using namespace std;

void usage(char **);
bool parse(char **);
void waitUntilQuit();
bool processEvents();
void spinDisplayUntilQuit();
bool getLine(FILE *, map<string, Vec4f> *, int, char *, char **, float *);
float dtor(float); // degrees to radians
void makeBox(Matrix4f *triangleMatrix);
void makeSphere(Matrix4f *triangleMatrix);

#endif
