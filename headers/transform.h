#ifndef TRANSFORM
#define TRANSFORM

#include "matrix.h"
#include <math.h>

void translate(Matrix4f *, const float, const float, const float);
void scale(Matrix4f *, const float, const float, const float);
void rotatex(Matrix4f *, const float);
void rotatey(Matrix4f *, const float);
void rotatez(Matrix4f *, const float);
void screenTransform(Matrix4f *, const int *);
void screenTransform(Matrix4f *, const int, const int, const int, const int, const int, const int);
void perspectiveTransform(Matrix4f *, const float, const float, const float);
void perspectiveTransform(Matrix4f *, const float *);

#endif
