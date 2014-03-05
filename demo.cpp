#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "renderer.h"
#include "matrix.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;

using namespace std;
void error(string error_message) {
	cout << "error: " << error_message << endl;
	cout << "SDL_GetError(): " << SDL_GetError() << endl;
}

bool init() {
    // if the init fails, display an error message and exit
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error("SDL could not initialize");
        return false;
    }

    // if the window fails to load, display an error message and exit
    window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
		error("Window could not be created");
		return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
		error("Renderer could not be initialized");
		return false;
    }

    drawSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    if (drawSurface == NULL) {
		error("Draw surface could not be created");
		return false;
    }

    drawTexture = SDL_CreateTextureFromSurface(renderer, drawSurface);
    if (drawTexture == NULL) {
		error("Draw texture could not be created");
		return false;
    }

    return true;
}

void clean_up() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(drawSurface);
    SDL_Quit();
}

int main(int argc, char **argv) {
	if (!init()) {
		return 1;
	}

	// You should work with SDL_Surface all time then use SDL_UpdateTexture(sdlTexture, NULL, pSurface->pixels, pSurface->pitch);
	// when your drawing is done, and finally call SDL_RenderCopy(renderer, pTexture, NULL, NULL);
	Uint32 RED = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);
	SDL_RenderClear(renderer);

	float data[4] = {320, 240, 0, 1};
	Matrix4f p1(1, 4, data);
	float data2[4] = {350, 270, 0, 1};
	Matrix4f p2(1, 4, data2);
	drawline2m(drawSurface, &p1, &p2, RED);

	float data3[16] = {1, 0, 0, 0,
	                   0, 1, 0, 0,
	                   0, 0, 1, 0,
	                   10, -10, 0, 1};
	Matrix4f t(4, 4, data3);
	p1.transform(&t);
	p2.transform(&t);
	drawline2m(drawSurface, &p1, &p2, RED);

	drawline4i(drawSurface, 0, 0, 0, 0, RED);

	printf("%x\n%x\n", GetPixel32(drawSurface, 0, 0), GetPixel32(drawSurface, 1, 0));

	// show
	SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
	SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

	cout << "saving screenshot" << endl;
	savePPM(drawSurface, "screenshot.ppm");
	cout << "screenshot saved" << endl;

    // wait so the user can see it
    SDL_Delay(2000);

	// clean up
    clean_up();
    return 0;
}
