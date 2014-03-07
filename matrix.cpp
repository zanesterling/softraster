#include "matrix.h"

Matrix4f::Matrix4f() {
	width = 0;
}

Matrix4f::~Matrix4f() {
}

void Matrix4f::addCol(Vec4f v) {
	this->_cols.push_back(v);
}

float Matrix4f::get(int x, int y) {
	return _cols[x][y];
}

// note: does not check x and y
void Matrix4f::set(int x, int y, float f) {
	_cols[x][y] = f;
}

// this mallocs but never removes. remember to destruct everything!
Matrix4f Matrix4f::operator *(Matrix4f m) {
	// TODO make in-place version
	if (width != 4) {
		std::cout << "multiplying incompatible matrices";
	}

	Matrix4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < m.width; j++) {
			float val = 0;
			for (int k = 0; k < 4; k++) {
				val += get(k, i) * m.get(j, k);
			}
			result.set(j, i, val);
		}
	}

	return result;
}

void Matrix4f::transform(Matrix4f *matrix) {
	for (int i = 0; i < 4; i++) {
		float val = 0;
		for (int j = 0; j < matrix->width; j++) {
			val += get(0, j) * matrix->get(j, i);
		}
		set(0, i, val);
	}
}

Vec4f::Vec4f(const float data[]) {
	memcpy(this->_data, data, sizeof(float)*4);
}

Vec4f::~Vec4f() {

}

float Vec4f::operator [](const int i) const {
	return _data[i];
}

float& Vec4f::operator [](const int i) {
	return _data[i];
}
