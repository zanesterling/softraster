#include "matrix.h"

Matrix4f::Matrix4f() {
	width = 0;
}

Matrix4f::~Matrix4f() {
}

void Matrix4f::addCol(Vec4f v) {
	this->_cols.push_back(v);
	this->width++;
}

const float Matrix4f::get(const int x, const int y) const {
	return _cols[x][y];
}

// note: does not check x and y
void Matrix4f::set(const int x, const int y, const float f) {
	_cols[x][y] = f;
}

const Vec4f *Matrix4f::operator [](const int i) const {
	return &(this->_cols[i]);
}

void Matrix4f::transform(const Matrix4f *matrix) {
	for (int k = 0; k < width; k++) {
		Vec4f newVec(0, 0, 0, 0);
		for (int i = 0; i < 4; i++) {
			float val = 0;
			for (int j = 0; j < matrix->width; j++) {
				val += get(k, j) * matrix->get(j, i);
			}
			newVec[i] = val;
		}
		for (int i = 0; i < 4; i++) {
			set(k, i, newVec[i]);
		}
	}
}

void Matrix4f::extend(const Matrix4f *matrix) {
	for (int i = 0; i < matrix->width; i++) {
		addCol(matrix->_cols[i]);
	}
}

void Matrix4f::clear() {
	 _cols.clear();
	 width = 0;
}

Vec4f::Vec4f(const float data[]) {
	memcpy(this->_data, data, sizeof(float)*4);
}

Vec4f::Vec4f(const float f1, const float f2, const float f3, const float f4) {
	_data[0] = f1;
	_data[1] = f2;
	_data[2] = f3;
	_data[3] = f4;
	// I wish this worked Q_Q
	//memcpy(this->_data, &f1, sizeof(float)*4);
}

float Vec4f::operator [](const int i) const {
	return _data[i];
}

float& Vec4f::operator [](const int i) {
	return _data[i];
}
