#include "renderer.h"

void PutPixel32(SDL_Surface *surface, int x, int y, Uint32 color)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	PutPixel32_nolock(surface, x, y, color);
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void PutPixel32_nolock(SDL_Surface *surface, int x, int y, Uint32 color)
{
	Uint8 * pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	*((Uint32*)pixel) = color;
}

Uint32 GetPixel32(SDL_Surface *surface, int x, int y) {
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	Uint32 pixel = GetPixel32_nolock(surface, x, y);
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	return pixel;
}

Uint32 GetPixel32_nolock(SDL_Surface *surface, int x, int y) {
	Uint8 *pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	return *((Uint32*)pixel);
}

void drawLine4i(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel) {
	int dx, dy;
	int adx, ady;
	int temp;
	int i;

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

void drawLine2m(SDL_Surface *surface, Matrix4f *p1, Matrix4f *p2, Uint32 pixel) {
	int x1, x2;
	int y1, y2;

	x1 = p1->get(0, 0);
	x2 = p2->get(0, 0);
	y1 = p1->get(0, 1);
	y2 = p2->get(0, 1);

	drawLine4i(surface, x1, y1, x2, y2, pixel);
}

void drawLine2v(SDL_Surface *surface, Vec4f *v1, Vec4f *v2, Uint32 pixel) {
	drawLine4i(surface, (*v1)[0], (*v1)[1], (*v2)[0], (*v2)[1], pixel);
}

void drawEdges(SDL_Surface *surface, Matrix4f *edgeMatrix, Uint32 pixel) {
	for (int i = 0; i < edgeMatrix->width / 2; i++) {
		drawLine2v(surface, (*edgeMatrix)[i*2], (*edgeMatrix)[i*2 + 1], pixel);
	}
}

// fills the surface with black
void clear(SDL_Surface *surface) {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
}

// fills the surface with $color
void clear(SDL_Surface *surface, Uint32 color) {
	SDL_FillRect(surface, NULL, color);
}

using namespace std;
// save the surface to a ppm file
void savePPM(SDL_Surface *surface, string fn) {
	savePPM(surface, fn.c_str());
}

// save the surface to a ppm file
void savePPM(SDL_Surface *surface, char *fn) {
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
