#ifndef PARSER
#define PARSER

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include "renderer.h"
#include "matrix.h"

int main(int, char **);
bool usage(char **);
void error(std::string);
bool getLine(FILE *, char *, float *);
bool parse(int, char **);
bool init();
void clean_up();
#endif
