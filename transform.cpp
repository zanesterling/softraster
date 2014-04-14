#include "headers/transform.h"

void translate(Matrix4f *edgeMatrix, const float x, const float y, const float z) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(x, y, z, 1));
	edgeMatrix->transform(&transformMatrix);
}

void scale(Matrix4f *edgeMatrix, const float x, const float y, const float z) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(x, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, y, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, z, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatex(Matrix4f *edgeMatrix, const float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0,       0,      0));
	transformMatrix.addCol(Vec4f(0, cos(t),  sin(t), 0));
	transformMatrix.addCol(Vec4f(0, -sin(t), cos(t), 0));
	transformMatrix.addCol(Vec4f(0, 0,       0,      1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatey(Matrix4f *edgeMatrix, const float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(cos(t), 0, -sin(t), 0));
	transformMatrix.addCol(Vec4f(0,      1, 0,       0));
	transformMatrix.addCol(Vec4f(sin(t), 0, cos(t),  0));
	transformMatrix.addCol(Vec4f(0,      0, 0,       1));
	edgeMatrix->transform(&transformMatrix);
}

void rotatez(Matrix4f *edgeMatrix, const float t) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(cos(t),  sin(t), 0, 0));
	transformMatrix.addCol(Vec4f(-sin(t), cos(t), 0, 0));
	transformMatrix.addCol(Vec4f(0,       0,      1, 0));
	transformMatrix.addCol(Vec4f(0,       0,      0, 1));
	edgeMatrix->transform(&transformMatrix);
}

void screenTransform(Matrix4f *edgeMatrix, const int *sdim) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));
	translate(&transformMatrix, -sdim[2], -sdim[3], 0);
	scale(&transformMatrix, sdim[0] / (sdim[4] - sdim[2]),
	                        sdim[1] / (sdim[3] - sdim[5]), 1);
	translate(&transformMatrix, 0, sdim[1], 0);
	edgeMatrix->transform(&transformMatrix);
}

void screenTransform(Matrix4f *edgeMatrix, const int pw, const int ph, const int xl,
                     const int yb, const int xr, const int yt) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));
	translate(&transformMatrix, -xl, -yb, 0);
	scale(&transformMatrix, pw / (xr - xl),
	                        ph / (yb - yt), 1);
	translate(&transformMatrix, 0, ph, 0);
	edgeMatrix->transform(&transformMatrix);
}

void perspectiveTransform(Matrix4f *edgeMatrix, const float eyex, const float eyey,
                          const float eyez) {
	const float screen_dist = eyez; // cross fingers
	scale(edgeMatrix, 1, 1, -1);
	translate(edgeMatrix, -eyex, -eyey, eyez);
	for (int i = 0; i < edgeMatrix->width; i++) {
		edgeMatrix->set(i, 0,
		                screen_dist * edgeMatrix->get(i, 0) / edgeMatrix->get(i, 2));
		edgeMatrix->set(i, 1,
		                screen_dist * edgeMatrix->get(i, 1) / edgeMatrix->get(i, 2));
	}
	translate(edgeMatrix, eyex, eyey, -eyez);
}

void perspectiveTransform(Matrix4f *edgeMatrix, const float *eye) {
	perspectiveTransform(edgeMatrix, eye[0], eye[1], eye[2]);
}
