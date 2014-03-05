#ifndef MATRIX
#define MATRIX
#include <new>
#include <stdlib.h>

class Matrix4f {
public:
	Matrix4f(int width, int height);
	Matrix4f(int width, int height, float things[]);
	~Matrix4f();
	Matrix4f operator *(Matrix4f m);
	float get(int x, int y);
	void set(int x, int y, float f);
	void transform(Matrix4f*);
	int width;
	int height;
private:
	float *data;
};
#endif
