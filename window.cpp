#include "headers/window.h"

const int SCREEN_WIDTH  = 250;
const int SCREEN_HEIGHT = 250;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;

void drawToScreen() {
    SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
    SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
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

void renderParallelTriangles(SDL_Surface *drawSurface, Matrix4f *triangleMatrix,
                             int *screen_dimensions) {
	Matrix4f finalTriangles;

	finalTriangles.extend(&triangleMatrix);
	screenTransform(&finalTriangles, screen_dimensions);
	drawTriangles(drawSurface, &finalTriangles,
	              SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff));
	drawToScreen();
}

void renderPerspectiveCyclopsTriangles(SDL_Surface *drawSurface,
                                       Matrix4f *triangleMatrix, float *camera
                                       int *screen_dimensions) {
	Matrix4f finalTriangles;

	finalTriangles.extend(&triangleMatrix);
	perspectiveTransform(&finalTriangles, camera);
	screenTransform(&finalTriangles, screen_dimensions);
	drawTriangles(drawSurface, &finalTriangles,
	              SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff));
	drawToScreen();
}

void renderPerspectiveStereoTriangles(SDL_Surface *drawSurface,
                                      Matrix4f *triangleMatrix, float *camera,
                                      int *screen_dimensions) {
	Matrix4f finalTriangles;
	Uint32 pixelColor;

	// draw red
	finalTriangles.extend(triangleMatrix);
	perspectiveTransform(&finalTriangles, camera);
	screenTransform(&finalTriangles, screen_dimensions);
	pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);
	drawTriangles(drawSurface, &finalEdges, pixelColor);

	// draw cyan
	finalTriangles.clear();
	finalTriangles.extend(&triangleMatrix);
	perspectiveTransform(&finalTriangles, float_args+3);
	screenTransform(&finalTriangles, screen_dimensions);
	pixelColor = SDL_MapRGB(drawSurface->format, 0, 0xff, 0xff);
	drawTriangles(drawSurface, &finalTriangles, pixelColor);
	drawToScreen();
}
