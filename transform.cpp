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

void screenTransform(Matrix4f *edgeMatrix, int width, int height, int xleft, int ybot,
                     int xright, int ytop) {
	Matrix4f transformMatrix;
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));
	translate(&transformMatrix, -xleft, -ybot, 0);
	scale(&transformMatrix, width / (xright - xleft), height / (ybot - ytop), 1);
	translate(&transformMatrix, 0, height, 0);
	edgeMatrix->transform(&transformMatrix);
}

void perspectiveTransform(Matrix4f *edgeMatrix, float eyex, float eyey, float eyez) {
	float screen_dist = eyez; // cross fingers
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

void perspectiveTransform(Matrix4f *edgeMatrix, float *eye) {
	perspectiveTransform(edgeMatrix, eye[0], eye[1], eye[2]);
}
