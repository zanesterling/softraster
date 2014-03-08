#include "demo.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;

using namespace std;

int main(int argc, char **argv) {
	Matrix4f edgeMatrix;
	Matrix4f transformMatrix;

	if (!init()) {
		return 1;
	}
	Uint32 RED = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);

	cout << "drawing\n";
	edgeMatrix.addCol(Vec4f(320, 240, 0, 1));
	edgeMatrix.addCol(Vec4f(350, 270, 0, 1));
	drawEdges(drawSurface, &edgeMatrix, RED);

	cout << "transforming\n";
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(10, -10, 0, 1));
	edgeMatrix.transform(&transformMatrix);

	cout << "drawing transformed\n";
	drawEdges(drawSurface, &edgeMatrix, RED);

	// show
	SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
	SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

    // wait so the user can see it
    SDL_Delay(2000);

	// clean up
    clean_up();
    return 0;
}
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
    window = SDL_CreateWindow("Hello World",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
		error("Window could not be created");
		return false;
    }

    renderer = SDL_CreateRenderer(window, -1,
                                 SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

	SDL_RenderClear(renderer);
    return true;
}

void clean_up() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(drawSurface);
    SDL_Quit();
}
