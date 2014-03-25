#include "renderer.h"

void PutPixel32(SDL_Surface *surface, const int x, const int y, const Uint32 color) {
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	PutPixel32_nolock(surface, x, y, color);
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void PutPixel32_nolock(SDL_Surface *surface, const int x, const int y,
					   const Uint32 color) {
	Uint8 * pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	*((Uint32*)pixel) = color;
}

Uint32 GetPixel32(SDL_Surface *surface, const int x, const int y) {
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	Uint32 pixel = GetPixel32_nolock(surface, x, y);
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	return pixel;
}

Uint32 GetPixel32_nolock(SDL_Surface *surface, const int x, const int y) {
	Uint8 *pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	return *((Uint32*)pixel);
}

void drawLine4i(SDL_Surface *surface, int x1, int y1, int x2, int y2,
				const Uint32 pixel) {
	int dx, dy;
	int adx, ady;
	int temp;
	int i;

	if (x1 < 0 || surface->w <= x1 || y1 < 0 || surface->h <= y1 ||
		x2 < 0 || surface->w <= x2 || y2 < 0 || surface->h <= y2) {
		return;
	}

	dx = x2 - x1;
	dy = y2 - y1;
	// absolute values
	adx = abs(dx);
	ady = abs(dy);

	if (dx == 0 && dy == 0) {
		PutPixel32(surface, x1, y1, pixel);
		return;
	}

	if (adx > ady) {
		if (adx != dx) {
			//swap points
			std::swap(x1, x2);
			std::swap(y1, y2);
		}
		float m = (float) dy / (float) dx;
		for (i = 0; i <= adx; i++) {
			PutPixel32(surface, x1 + i, y1 + i*m, pixel);
		}
	} else {
		if (ady != dy) {
			//swap points
			std::swap(x1, x2);
			std::swap(y1, y2);
		}
		float m = (float) dx / (float) dy;
		for (i = 0; i <= ady; i++) {
			PutPixel32(surface, x1 + i*m, y1 + i, pixel);
		}
	}
}

void drawLine2m(SDL_Surface *surface, const Matrix4f *p1, const Matrix4f *p2,
				const Uint32 pixel) {
	int x1, x2;
	int y1, y2;

	x1 = p1->get(0, 0);
	x2 = p2->get(0, 0);
	y1 = p1->get(0, 1);
	y2 = p2->get(0, 1);

	drawLine4i(surface, x1, y1, x2, y2, pixel);
}

void drawLine2v(SDL_Surface *surface, const Vec4f *v1, const Vec4f *v2,
				const Uint32 pixel) {
	drawLine4i(surface, (*v1)[0], (*v1)[1], (*v2)[0], (*v2)[1], pixel);
}

void drawEdges(SDL_Surface *surface, const Matrix4f *edgeMatrix, const Uint32 pixel) {
	using namespace std;
	for (int i = 0; i < edgeMatrix->width / 2; i++) {
		drawLine2v(surface, (*edgeMatrix)[i*2], (*edgeMatrix)[i*2 + 1], pixel);
	}
}

// fills the surface with black
void clear(SDL_Surface *surface) {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
}

// fills the surface with $color
void clear(SDL_Surface *surface, const Uint32 color) {
	SDL_FillRect(surface, NULL, color);
}

using namespace std;
// save the surface to a ppm file
void savePPM(SDL_Surface *surface, const string fn) {
	savePPM(surface, fn.c_str());
}

// save the surface to a ppm file
void savePPM(SDL_Surface *surface, const char *fn) {
	FILE *fp;
	fp = fopen(fn, "w+");

	// header
	fprintf(fp, "P3\n");
	fprintf(fp, "%d %d\n", surface->w, surface->h);
	fprintf(fp, "255\n");

	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	int flag = 0;
	for (int i = 0; i < surface->h; i++) {
		for (int j = 0; j < surface->w; j++) {
			Uint32 pixel = GetPixel32_nolock(surface, j, i);
			unsigned char r = (pixel & 0xff0000) >> 16;
			unsigned char g = (pixel & 0x00ff00) >> 8;
			unsigned char b = (pixel & 0x0000ff);
			fprintf(fp, "%d %d %d ", r, g, b);
		}
		fprintf(fp, "\n");
	}
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}

// scanline rasterization
void fillTriangle(SDL_Surface *surface, const Matrix4f *verts, const Uint32 color) {
	/* TODO check to see if triangle is facing */
	// sort the three vertices by y-coordinate ascending so v1 is the topmost vertex
	Vec4f v1, v2, v3;
	float val = verts->get(0, 1);
	int index = 0;
	int things[3];
	for (int i = 0; i < 3; i++) {
		things[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		if (verts->get(i, 1) > val) {
			val = verts->get(i, 1);
			index = i;
		}
	}
	v1 = (*verts)[index];
	things[index] = 1;

	for (int i = 0; i < 3; i++) {
		if (verts->get(i, 1) < val) {
			val = verts->get(i, 1);
			index = i;
		}
	}
	v3 = (*verts)[index];
	things[index] = 3;

	for (int i = 0; i < 3; i++) {
		if (!things[i]) {
			v2 = (*verts)[i];
		}
	}

	/* here we know that v1[1] <= v2[1] <= v3[1] */
	if (v2[1] == v3[1]) {
		fillBottomFlatTriangle(surface, v1, v2, v3, color);
	} else if (v1[1] == v2[1]) {
		fillTopFlatTriangle(surface, v1, v2, v3, color);
	} else {
		/* general case - split the triangle in a topflat and bottom-flat one */
		Vec4f v4((int)(v1[0] + ((float)(v2[1] - v1[1]) / (float)(v3[1] - v1[1])) *
		          (v3[0] - v1[0])), v2[1], 0, 1);
		fillBottomFlatTriangle(surface, v1, v2, v4, color);
		fillTopFlatTriangle(surface, v2, v4, v3, color);
	}
}

void fillBottomFlatTriangle(SDL_Surface *surface, const Vec4f v1, const Vec4f v2,
							const Vec4f v3, const Uint32 color) {
	if (v1[0] == v2[0] && v2[0] == v3[0]) return;
	if (v1[1] == v2[1] && v2[1] == v3[1]) return;
	float invslope1 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
	float invslope2 = (v3[0] - v1[0]) / (v3[1] - v1[1]);

	float curx1 = v1[0];
	float curx2 = v1[0];

	for (int scanlineY = v1[1]; scanlineY >= v2[1]; scanlineY--) {
		fillRow(surface, (int)curx2, (int)curx1, scanlineY, color);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void fillTopFlatTriangle(SDL_Surface *surface, const Vec4f v1, const Vec4f v2,
						 const Vec4f v3, const Uint32 color) {
	if (v1[0] == v2[0] && v2[0] == v3[0]) return;
	if (v1[1] == v2[1] && v2[1] == v3[1]) return;
	float invslope1 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
	float invslope2 = (v3[0] - v2[0]) / (v3[1] - v2[1]);
	if (abs(invslope1) < 5 && abs(invslope2) < 5) {
		float curx1 = v3[0];
		float curx2 = v3[0];

		for (int scanlineY = v3[1]; scanlineY < v1[1]; scanlineY++) {
			curx1 += invslope1;
			curx2 += invslope2;
			fillRow(surface, (int)curx1, (int)curx2, scanlineY, color);
		}
	} else {
		invslope1 = (v3[1] - v1[1]) / (v3[0] - v1[0]);
		invslope2 = (v3[1] - v2[1]) / (v3[0] - v2[0]);
		if (invslope2 < 0) {
			fillBottomFlatTriangle(surface, v1, v2, v3, color);
			return;
		}
		float cury1 = v3[1];
		float cury2 = v3[1];

		for (int scanlineX = v3[0]; scanlineX < v2[0]; scanlineX++) {
			cury1 += invslope1;
			cury2 += invslope2;
			fillCol(surface, (int)cury1, (int)cury2, scanlineX, color);
		}
		for (int scanlineX = v2[0]; scanlineX < v1[0]; scanlineX++) {
			cury1 += invslope1;
			fillCol(surface, (int)cury1, (int)cury2, scanlineX, color);
		}
	}
}

void fillRow(SDL_Surface *surface, int x1, int x2, const int y,
			 const Uint32 color) {
	if (x1 > x2) {
		swap(x1, x2);
	}
	if (x1 < 0) x1 = 0;
	if (x2 >= surface->w) x2 = 249;
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	for (int i = x1; i <= x2; i++) {
		PutPixel32_nolock(surface, i, y, color);
	}
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}

void fillCol(SDL_Surface *surface, int y1, int y2, const int x,
			 const Uint32 color) {
	if (y1 > y2) {
		swap(y1, y2);
	}
	if (y1 < 0) y1 = 0;
	if (y2 >= surface->h) y2 = 249;
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	for (int i = y1; i <= y2; i++) {
		PutPixel32_nolock(surface, x, i, color);
	}
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}

void drawRect(SDL_Surface *surface, int x, int y, int w, int h, const Uint32 color) {
	Matrix4f edgeMatrix;
	edgeMatrix.addCol(Vec4f(x,   y,   0, 1));
	edgeMatrix.addCol(Vec4f(x+w, y,   0, 1));
	edgeMatrix.addCol(Vec4f(x,   y,   0, 1));
	edgeMatrix.addCol(Vec4f(x,   y+h, 0, 1));
	edgeMatrix.addCol(Vec4f(x+w, y,   0, 1));
	edgeMatrix.addCol(Vec4f(x+w, y+h, 0, 1));
	edgeMatrix.addCol(Vec4f(x,   y+h, 0, 1));
	edgeMatrix.addCol(Vec4f(x+w, y+h, 0, 1));
	drawEdges(surface, &edgeMatrix, color);
}
