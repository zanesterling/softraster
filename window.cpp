#include "headers/window.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;

void drawToScreen() {
    SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
    SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

using namespace std;
void error(string error_message) {
	cout << "error: " << error_message << endl;
	cout << "SDL_GetError(): " << SDL_GetError() << endl;
}

// set up SDL and related components
bool init(int width, int height) {
    // if the init fails, display an error message and exit
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error("SDL could not initialize");
        return false;
    }

    // if the window fails to load, display an error message and exit
    window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
		error("Window could not be created");
		return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
		error("Renderer could not be initialized");
		return false;
    }

    drawSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (drawSurface == NULL) {
		error("Draw surface could not be created");
		return false;
    }

    drawTexture = SDL_CreateTextureFromSurface(renderer, drawSurface);
    if (drawTexture == NULL) {
		error("Draw texture could not be created");
		return false;
    }

    SDL_RenderClear(renderer);
    return true;
}

void clean_up() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(drawTexture);
	SDL_FreeSurface(drawSurface);
	SDL_Quit();
}

void renderParallelTriangles(Matrix4f *triangleMatrix, const int *screen_dimensions) {
	Matrix4f finalTriangles;

	finalTriangles.extend(triangleMatrix);
	float camera[3] = {0, 0, -100};
	bool *shouldDraw = new bool[finalTriangles.width / 3];
	backfaceCull(&finalTriangles, camera, shouldDraw);
	screenTransform(&finalTriangles, screen_dimensions);
	Uint32 pixel = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
	drawTheseTriangles(drawSurface, &finalTriangles, pixel, shouldDraw);
	drawToScreen();
}

void renderPerspectiveCyclopsTriangles(Matrix4f *triangleMatrix, const float *camera,
                                       const int *screen_dimensions) {
	Matrix4f finalTriangles;

	finalTriangles.extend(triangleMatrix);
	bool *shouldDraw = new bool[finalTriangles.width / 3];
	backfaceCull(&finalTriangles, camera, shouldDraw);
	perspectiveTransform(&finalTriangles, camera);
	screenTransform(&finalTriangles, screen_dimensions);
	Uint32 pixel = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
	drawTheseTriangles(drawSurface, &finalTriangles, pixel, shouldDraw);
	drawToScreen();
}

void renderPerspectiveStereoTriangles(Matrix4f *triangleMatrix, const float *camera,
                                      const int *screen_dimensions) {
	Matrix4f finalTriangles;
	Uint32 pixelColor;

	// draw red
	finalTriangles.extend(triangleMatrix);
	bool *shouldDraw = new bool[finalTriangles.width / 3];
	backfaceCull(&finalTriangles, camera, shouldDraw);
	perspectiveTransform(&finalTriangles, camera);
	screenTransform(&finalTriangles, screen_dimensions);
	pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);
	drawTheseTriangles(drawSurface, &finalTriangles, pixelColor, shouldDraw);

	// draw cyan
	finalTriangles.clear();
	finalTriangles.extend(triangleMatrix);
	perspectiveTransform(&finalTriangles, camera + 3);
	screenTransform(&finalTriangles, screen_dimensions);
	pixelColor = SDL_MapRGB(drawSurface->format, 0, 0xff, 0xff);
	drawTheseTriangles(drawSurface, &finalTriangles, pixelColor, shouldDraw);
	drawToScreen();
}
