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

	cout << "initializing... ";
	init();
	cout << "initialized\n";


	cout << "ready to parse... ";
	if (!parse(argc, argv)) {
		printf("error encountered while parsing\n");
		return 1;
	}

    SDL_Delay(3000);

    clean_up();
    return 0;
}

bool parse(int argc, char **argv) {
    Uint32 pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
    Matrix4f transformMatrix;
    Matrix4f edgeMatrix;
    Matrix4f tempMatrix;
    char command[64];
    char **str_args;
    float float_args[16];
    float point[4];
    int i;

	str_args = (char **) malloc(sizeof(char *) * 4);
    for (i = 0; i < 4; i++) {
		str_args[i] = (char *) calloc(64, sizeof(char));
    }

    // initialize the transform matrix to be the identity matrix
    transformMatrix.addCol(Vec4f(1, 0, 0, 0));
    transformMatrix.addCol(Vec4f(0, 1, 0, 0));
    transformMatrix.addCol(Vec4f(0, 0, 1, 0));
    transformMatrix.addCol(Vec4f(0, 0, 0, 1));

	if (argc < 2) {
		return usage(argv);
	}

	FILE *fp = fopen(argv[1], "r");
	cout << "parsing\n";
	while (getLine(fp, command, str_args, float_args)) {
		cout << command << endl;
		if (strcmp(command, "line") == 0) {
			// add a line to the edge matrix
			edgeMatrix.addCol(Vec4f(float_args));
			edgeMatrix.set(edgeMatrix.width-1, 3, 1);
			edgeMatrix.addCol(Vec4f(float_args + 3));
			edgeMatrix.set(edgeMatrix.width-1, 3, 1);
			cout << edgeMatrix.get(0, 0) << " " << edgeMatrix.get(0, 1) << " ";
			cout << edgeMatrix.get(0, 2) << " " << edgeMatrix.get(0, 3) << endl;
			cout << edgeMatrix.get(1, 0) << " " << edgeMatrix.get(1, 1) << " ";
			cout << edgeMatrix.get(1, 2) << " " << edgeMatrix.get(1, 3) << endl;
		} else if (strcmp(command, "identity") == 0) {
			// make the transform matrix the identity matrix
			transformMatrix.clear();
    		transformMatrix.addCol(Vec4f(1, 0, 0, 0));
    		transformMatrix.addCol(Vec4f(0, 1, 0, 0));
    		transformMatrix.addCol(Vec4f(0, 0, 1, 0));
    		transformMatrix.addCol(Vec4f(0, 0, 0, 1));
		} else if (strcmp(command, "move") == 0) {
			// perform a translation on the edge matrix
    		tempMatrix.addCol(Vec4f(1, 0, 0, 0));
    		tempMatrix.addCol(Vec4f(0, 1, 0, 0));
    		tempMatrix.addCol(Vec4f(0, 0, 1, 0));
    		tempMatrix.addCol(Vec4f(float_args[0], float_args[1], float_args[3], 1));
    		transformMatrix.transform(&tempMatrix);
    		tempMatrix.clear();
		} else if (strcmp(command, "scale") == 0) {
			// perform a scaling on the edge matrix
    		tempMatrix.addCol(Vec4f(float_args[0], 0, 0, 0));
    		tempMatrix.addCol(Vec4f(0, float_args[1], 0, 0));
    		tempMatrix.addCol(Vec4f(0, 0, float_args[2], 0));
    		tempMatrix.addCol(Vec4f(0, 0, 0, 1));
    		transformMatrix.transform(&tempMatrix);
    		tempMatrix.clear();
		} else if (strcmp(command, "rotate-x") == 0) { // TODO
			// rotate about the x-axis
		} else if (strcmp(command, "rotate-y") == 0) { // TODO
			// rotate about the y-axis
		} else if (strcmp(command, "rotate-z") == 0) { // TODO
			// rotate about the z-axis
		} else if (strcmp(command, "screen") == 0) { // TODO
			// set the lower-left and upper-right positions of the screen
			cout << "screen description requested\n";
		} else if (strcmp(command, "pixels") == 0) { // TODO
			// set the lower-left and upper-right positions of the pixels
			cout << "pixel description requested\n";
		} else if (strcmp(command, "transform") == 0) {
			// multiply the edge matrix by the transform matrix
			edgeMatrix.transform(&transformMatrix);
		} else if (strcmp(command, "render-parallel") == 0) {
			// perform a parellel projection along the z-axis
			drawEdges(drawSurface, &edgeMatrix, pixelColor);
			drawToScreen();
		} else if (strcmp(command, "render-perspective-cyclops") == 0) { // TODO
			// perform a perspective rendering to a single eye
		} else if (strcmp(command, "render-perspective-stereo") == 0) { // TODO
			// perform a perspective rendering to each of two eyes
		} else if (strcmp(command, "clear-edge") == 0) {
			// clear the edge matrix
			edgeMatrix.clear();
		} else if (strcmp(command, "clear-pixels") == 0) {
			// clear the screen
			clear(drawSurface);
			drawToScreen();
		} else if (strcmp(command, "file") == 0) {
			// save to a file
			savePPM(drawSurface, str_args[0]);
		} else if (strcmp(command, "end") == 0) {
			// stop parsing
			return true;
		} else {
			error("unknown command");
			return false;
		}
	}

	for (i = 0; i < 4; i++) {
		free(str_args[i]);
	}
	free(str_args);

	return true;
}

void drawToScreen() {
    SDL_UpdateTexture(drawTexture, NULL, drawSurface->pixels, drawSurface->pitch);
    SDL_RenderCopy(renderer, drawTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// reads a line and puts all string args in args_buffer and all float args in
// val_buffer. returns true if a line is read
bool getLine(FILE *fin, char *command_buffer, char **args_buffer, float *vals_buffer) {
	int args_index, vals_index;
	int i, j, c;
	char fvals[16];
	char input[1024];

	// read the command
	input[0] = '#';
	while (input[0] == '#') fgets(input, 1024, fin);
	if (input[0] == '\0') return false;
	for (i = 0; i < 1024 && input[i] != ' ' && input[i] != '\n'; i++) {
		command_buffer[i] = input[i];
	}
	command_buffer[i] = '\0';

	// read each argument from the line
	args_index = 0;
	vals_index = 0;
	while (input[i] != '\n') {
		j = 0;
		while (input[i] == ' ') i++; // skip whitespace
		while (input[i] != ' ' && input[i] != '\n') fvals[j++] = input[i++]; // read
		fvals[j] = '\0';

		// if the argument is a float, put it in vals_buffer
		if ('0' <= fvals[0] && fvals[0] <= '9' || fvals[0] == '.') {
			vals_buffer[vals_index++] = atof(fvals);
		} else {
			// otherwise put it in args_buffer
			strcpy(args_buffer[args_index++], fvals);
		}
	}

	return true;
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
