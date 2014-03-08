#ifndef MATRIX
#define MATRIX
#include <new>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

class Matrix4f;
class Vec4f;

class Matrix4f {
public:
	Matrix4f();
	~Matrix4f();
	Matrix4f operator *(Matrix4f m);
	float get(int, int);
	void set(int x, int y, float f);
	void transform(Matrix4f*);
	void addCol(Vec4f);
	void clear();
	int width;
private:
	std::vector<Vec4f> _cols;
};

class Vec4f {
public:
	Vec4f(const float []);
	Vec4f(const float f1, const float f2, const float f3, const float f4);
	~Vec4f() {};
	float  operator [](const int i) const;
	float& operator [](const int i);
private:
	float _data[4];
};
#endif
