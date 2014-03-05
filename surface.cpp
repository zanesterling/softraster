#include "surface.h"

Surface::Surface(int w, int h) {
	width = w;
	height = h;
	data = (unsigned char *)malloc(w * h * 3 * sizeof(unsigned char *));
}

Surface::~Surface() {
	free(data);
}

void Surface::clear(unsigned char *color) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			setPixel(j, i, color);
		}
	}
}

void Surface::setPixel(int x, int y, unsigned char *color) {
	int colorIndex = (y * width + x) * 3;
	for (int i = 0; i < 3; i++) {
		data[colorIndex + i] = color[i];
	}
}

unsigned char *Surface::getPixel(int x, int y) {
	return data + (y * width + x) * sizeof(unsigned char) * 3;
}

