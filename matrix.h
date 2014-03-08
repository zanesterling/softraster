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
	Matrix4f operator *(const Matrix4f m);
	float get(const int, const int);
	void set(const int, const int, const float);
	void transform(const Matrix4f*);
	void addCol(Vec4f);
	void clear();
	int width;
private:
	std::vector<Vec4f> _cols;
};

class Vec4f {
public:
	Vec4f(const float []);
	Vec4f(const float, const float, const float, const float);
	~Vec4f() {};
	float  operator [](const int) const;
	float& operator [](const int);
private:
	float _data[4];
};
#endif
