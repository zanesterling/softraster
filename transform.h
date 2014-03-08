#ifndef TRANSFORM
#define TRANSFORM

#include "matrix.h"
#include "math.h"

void translate(Matrix4f *, float, float, float);
void scale(Matrix4f *, float, float, float);
void rotatex(Matrix4f *, float);
void rotatey(Matrix4f *, float);
void rotatez(Matrix4f *, float);

#endif
