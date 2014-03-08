#ifndef PARSER
#define PARSER

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include "renderer.h"
#include "matrix.h"

int main(int, char **);
bool parse(int, char **);
void drawToScreen();
bool getLine(FILE *, char *, char **, float *);
bool usage(char **);
void error(std::string);
bool init();
void clean_up();
#endif
