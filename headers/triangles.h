#ifndef TRIANGLES
#define TRIANGLES

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#include "matrix.h"
#include "transform.h"

int main(int, char **);
void printMatrix(Matrix4f *);
void drawToScreen();
bool usage(char **);
void error(std::string);
bool init(int, int);
void clean_up();

#endif
