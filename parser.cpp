#include "parser.h"

const int SCREEN_WIDTH  = 500;
const int SCREEN_HEIGHT = 500;
using namespace std;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;

int main(int argc, char **argv) {
    if (argc != 2)
        return usage(argv);

	init();

	if (!parse(argc, argv)) {
		printf("error encountered while parsing\n");
		return 1;
	}

    SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
    SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    clean_up();
    return 0;
}

bool usage(char **argv) {
    cout << "error: no input file\n";
    cout << argv[0] << " inputfile\n";
    return false;
}

void error(string error_message) {
	cout << "error: " << error_message << endl;
	cout << "SDL_GetError(): " << SDL_GetError() << endl;
}

bool getLine(FILE *fin, char *command_buffer, float *val_buffer) {
	int i, j, c;
	char fval[16];

	// read the command
	for (i = 0; (c = fgetc(fin)) != '\n' && c != ' '; i++) {
		command_buffer[i] = c;
	}

	// read each float value from the line
	for (j = 0; c != '\n' && c != EOF; j++) {
		i = 0;
		while ((c = fgetc(fin)) == ' ');
		while ((c = fgetc(fin)) != ' ' && c != '\n') fval[i++] = c;
		fval[i] = '\0';
		val_buffer[j] = atof(fval);
	}
}

bool parse(int argc, char **argv) {
    Uint32 pixelColr = SDL_MapRGB(drawSurface->format, 0, 0, 0);
    unsigned char color[3] = {0, 0, 0};
    char command[64];
    float vals[16];

	// get the screen's surface

	if (argc < 2) {
		return usage(argv);
	}

	return true;
}

// set up SDL and related components
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
	SDL_DestroyTexture(drawTexture);
	SDL_FreeSurface(drawSurface);
	SDL_Quit();
}
