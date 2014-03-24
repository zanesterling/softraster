#include "triangles.h"

const int SCREEN_WIDTH  = 250;
const int SCREEN_HEIGHT = 250;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;
Uint32 pixelColor;

int main(int argc, char **argv) {
	using namespace std;
	if (!init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
		cout << "initialization failed" << endl;
		return 1;
	}

	Matrix4f triangleMatrix;
	int w2 = SCREEN_WIDTH / 2;
	int h2 = SCREEN_HEIGHT / 2;

	triangleMatrix.addCol(Vec4f(w2,     h2*1.5, 0, 1));
	triangleMatrix.addCol(Vec4f(w2*1.5, h2*0.6, 0, 1));
	triangleMatrix.addCol(Vec4f(w2*0.5, h2*0.4, 0, 1));
	pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
	//screenTransform(&triangleMatrix, 500, 500, -2, -2, 2, 2);
	fillTriangle(drawSurface, &triangleMatrix, pixelColor);

	drawToScreen();
	bool quit = false;
	bool pause = false;
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				pause = !pause;
			}
		}

		if (!pause) {
			clear(drawSurface);
			translate(&triangleMatrix, -w2, -h2, 0);
			rotatex(&triangleMatrix, 0.01*M_PI);
			translate(&triangleMatrix, w2, h2, 0);
			fillTriangle(drawSurface, &triangleMatrix, pixelColor);
			drawToScreen();
		}
		SDL_Delay(10);
	}
}

void printMatrix(Matrix4f *matrix) {
	using namespace std;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < matrix->width; i++) {
			cout << matrix->get(i, j) << " "; 
		}
		cout << endl;
	}
}

void drawToScreen() {
    SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
    SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void error(std::string error_message) {
	using namespace std;
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
