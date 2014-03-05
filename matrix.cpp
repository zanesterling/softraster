#include "matrix.h"
#include <iostream>

Matrix4f::Matrix4f(int w, int h) {
	width = w;
	height = h;
}

Matrix4f::Matrix4f(int w, int h, float things[]) {
	width = w;
	height = h;
	data = new float[width * height];
	for (int i = 0; i < w * h; i++) {
		data[i] = things[i];
	}
}

Matrix4f::~Matrix4f() {
	delete data;
}

float Matrix4f::get(int x, int y) {
	return data[x * 4 + y];
}

// note: does not check x and y
void Matrix4f::set(int x, int y, float f) {
	data[x * 4 + y] = f;
}

// this mallocs but never removes. remember to destruct everything!
Matrix4f Matrix4f::operator *(Matrix4f m) {
	// TODO make in-place version
	if (width != m.height) {
		std::cout << "multiplying incompatible matrices";
	}

	Matrix4f result(this->height, m.width);
	for (int i = 0; i < this->height; i++) {
		for (int j = 0; j < m.width; j++) {
			float val = 0;
			for (int k = 0; k < m.height; k++) {
				val += get(k, i) * m.get(j, k);
			}
			result.set(j, i, val);
		}
	}

	return result;
}

void Matrix4f::transform(Matrix4f *matrix) {
	for (int i = 0; i < this->height; i++) {
		float val = 0;
		for (int j = 0; j < matrix->width; j++) {
			val += get(0, j) * matrix->get(j, i);
		}
		set(0, i, val);
	}
}
