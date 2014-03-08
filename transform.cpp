#include "transform.h"

void translate(Matrix4f *edgeMatrix, float x, float y, float z) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(x, y, z, 1));
	edgeMatrix->transform(&transformMatrix);
}

void scale(Matrix4f *edgeMatrix, float x, float y, float z) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(x, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, y, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, z, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatex(Matrix4f *edgeMatrix, float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0,       0,      0));
	transformMatrix.addCol(Vec4f(0, cos(t),  sin(t), 0));
	transformMatrix.addCol(Vec4f(0, -sin(t), cos(t), 0));
	transformMatrix.addCol(Vec4f(0, 0,       0,      1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatey(Matrix4f *edgeMatrix, float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(cos(t), 0, -sin(t), 0));
	transformMatrix.addCol(Vec4f(0,      1, 0,       0));
	transformMatrix.addCol(Vec4f(sin(t), 0, cos(t),  0));
	transformMatrix.addCol(Vec4f(0,      0, 0,       1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatez(Matrix4f *edgeMatrix, float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(cos(t),  sin(t), 0, 0));
	transformMatrix.addCol(Vec4f(-sin(t), cos(t), 0, 0));
	transformMatrix.addCol(Vec4f(0,       0,      1, 0));
	transformMatrix.addCol(Vec4f(0,       0,      0, 1));
	edgeMatrix->transform(&transformMatrix);
}
