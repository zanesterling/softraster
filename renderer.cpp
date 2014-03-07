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

void drawline4i(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel) {
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

void drawline2m(SDL_Surface *surface, Matrix4f *p1, Matrix4f *p2, Uint32 pixel) {
	int x1, x2;
	int y1, y2;
	int dx, dy;
	int adx, ady;
	int temp;
	int i;

	x1 = p1->get(0, 0);
	x2 = p2->get(0, 0);
	y1 = p1->get(0, 1);
	y2 = p2->get(0, 1);

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

using namespace std;
void savePPM(SDL_Surface *surface, string fn) {
	FILE *fp;
	fp = fopen(fn.c_str(), "w+");

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
