#include "parser.h"

#define RENDER_PAR 0
#define RENDER_PER 1
#define RENDER_STE 2

using namespace std;

const int SPHERE_LAT_LINES = 20;
const int SPHERE_LON_LINES = 20;

int pix_width, pix_height;
int xleft, ybot, xright, ytop;
float camera[6];
int render_type;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *drawTexture = NULL;
SDL_Surface *drawSurface = NULL;
Uint32 pixelColor;
Matrix4f edgeMatrix;

int main(int argc, char **argv) {
    if (argc != 2)
        return usage(argv);

	cout << "ready to parse... ";
	if (!parse(argc, argv)) {
		printf("error encountered while parsing\n");
		return 1;
	}

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		} 

		// clear for the next frame
		clear(drawSurface);

		// find the object's center
		float avgX = 0;
		float avgY = 0;
		float avgZ = 0;
		for (int i = 0; i < edgeMatrix.width; i++) {
			avgX += edgeMatrix.get(i, 0);
			avgY += edgeMatrix.get(i, 1);
			avgZ += edgeMatrix.get(i, 2);
		}
		avgX /= edgeMatrix.width;
		avgY /= edgeMatrix.width;
		avgZ /= edgeMatrix.width;

		// rotate the model slightly around its center
		translate(&edgeMatrix, -avgX, -avgY, -avgZ);
		rotatex(&edgeMatrix, 0.02);
		rotatey(&edgeMatrix, 0.02);
		translate(&edgeMatrix, avgX, avgY, avgZ);

		if (render_type == RENDER_PAR) {
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			screenTransform(&finalEdges, pix_width, pix_height, xleft, ybot, xright, ytop);
			drawEdges(drawSurface, &finalEdges, pixelColor);
		} else if (render_type == RENDER_PER) {
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			screenTransform(&finalEdges, pix_width, pix_height, xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
			drawEdges(drawSurface, &finalEdges, pixelColor);
		} else if (render_type == RENDER_STE) {
			// draw red
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			perspectiveTransform(&finalEdges, camera);
			screenTransform(&finalEdges, pix_width, pix_height, xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);
			drawEdges(drawSurface, &finalEdges, pixelColor);

			// draw cyan
			finalEdges.clear();
			finalEdges.extend(&edgeMatrix);
			perspectiveTransform(&finalEdges, camera+3);
			screenTransform(&finalEdges, pix_width, pix_height, xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0, 0xff, 0xff);
			drawEdges(drawSurface, &finalEdges, pixelColor);
		}
		drawToScreen();
		SDL_Delay(10);
	}
    //SDL_Delay(4000);

    clean_up();
    return 0;
}

void printMatrix(Matrix4f *matrix) {
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < matrix->width; i++) {
			cout << matrix->get(i, j) << " "; 
		}
		cout << endl;
	}
}

// degrees to radians
float dtor(float degrees) {
	return degrees * M_PI / 180;
}

bool parse(int argc, char **argv) {
    Matrix4f transformMatrix;
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
		// cout << command << endl;
		if (strcmp(command, "line") == 0) {
			// add a line to the edge matrix
			edgeMatrix.addCol(Vec4f(float_args));
			edgeMatrix.set(edgeMatrix.width-1, 3, 1);
			edgeMatrix.addCol(Vec4f(float_args + 3));
			edgeMatrix.set(edgeMatrix.width-1, 3, 1);
		} else if (strcmp(command, "sphere") == 0) { // TODO
			// add appropriate edges for a sphere to the edge matrix
			Matrix4f side;
			Matrix4f prvSide;
			for (int i = 0; i <= SPHERE_LON_LINES; i++) {
				float rot_angle = i * M_PI * 2 / SPHERE_LON_LINES;
				for (int j = 0; j < SPHERE_LAT_LINES; j++) {
					float alt_angle = j * M_PI / SPHERE_LAT_LINES;
					float x = float_args[0] * sin(alt_angle);
					float y = float_args[0] * cos(alt_angle);
					side.addCol(Vec4f(x, y, 0, 1));

					alt_angle = (j+1) * M_PI / SPHERE_LAT_LINES;
					x = float_args[0] * sin(alt_angle);
					y = float_args[0] * cos(alt_angle);
					side.addCol(Vec4f(x, y, 0, 1));
				}
				rotatey(&side, rot_angle);
				if (i != 0) {
					for (int j = 1; j < SPHERE_LAT_LINES; j++) {
						side.addCol(*(side[j*2]));
						side.addCol(*(prvSide[j*2]));
						/* uncomment for triangles! woot
						side.addCol(*(side[j*2]));
						side.addCol(*(prvSide[j*2-2]));
						*/
					}
				}
				prvSide.clear();
				prvSide.extend(&side);
				translate(&side, float_args[1], float_args[2], float_args[3]);
				edgeMatrix.extend(&side);
				side.clear();
			}
		} else if (strcmp(command, "identity") == 0) {
			// make the transform matrix the identity matrix
			transformMatrix.clear();
    		transformMatrix.addCol(Vec4f(1, 0, 0, 0));
    		transformMatrix.addCol(Vec4f(0, 1, 0, 0));
    		transformMatrix.addCol(Vec4f(0, 0, 1, 0));
    		transformMatrix.addCol(Vec4f(0, 0, 0, 1));
		} else if (strcmp(command, "move") == 0) {
			// perform a translation on the edge matrix
			translate(&transformMatrix, float_args[0], float_args[1], float_args[2]);
		} else if (strcmp(command, "scale") == 0) {
			// perform a scaling on the edge matrix
			scale(&transformMatrix, float_args[0], float_args[1], float_args[2]);
		} else if (strcmp(command, "rotate-x") == 0) {
			// rotate about the x-axis
			rotatex(&transformMatrix, dtor(float_args[0]));
		} else if (strcmp(command, "rotate-y") == 0) {
			// rotate about the y-axis
			rotatey(&transformMatrix, dtor(float_args[0]));
		} else if (strcmp(command, "rotate-z") == 0) {
			// rotate about the z-axis
			rotatez(&transformMatrix, dtor(float_args[0]));
		} else if (strcmp(command, "screen") == 0) {
			// set the lower-left and upper-right positions of the screen
			xleft = float_args[0];
			ybot = float_args[1];
			xright = float_args[2];
			ytop = float_args[3];
		} else if (strcmp(command, "pixels") == 0) {
			// initialize the surface
			if (!init(float_args[0], float_args[1])) {
				error("parser could not init");
				return false;
			}
			pixelColor = SDL_MapRGB(drawSurface->format, 0, 0xff, 0);
		} else if (strcmp(command, "transform") == 0) {
			// multiply the edge matrix by the transform matrix
			edgeMatrix.transform(&transformMatrix);
		} else if (strcmp(command, "render-parallel") == 0) {
			// perform a parellel projection along the z-axis
			render_type = RENDER_PAR;
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			screenTransform(&finalEdges, pix_width, pix_height,
			                xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
			drawEdges(drawSurface, &finalEdges, pixelColor);
			drawToScreen();
		} else if (strcmp(command, "render-perspective-cyclops") == 0) {
			// perform a perspective rendering to a single eye
			render_type = RENDER_PER;
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			memcpy(camera, float_args, sizeof(float) * 4);
			perspectiveTransform(&finalEdges, float_args);
			screenTransform(&finalEdges, pix_width, pix_height,
			                xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0xff, 0xff);
			drawEdges(drawSurface, &finalEdges, pixelColor);
			drawToScreen();
		} else if (strcmp(command, "render-perspective-stereo") == 0) {
			// perform a perspective rendering to each of two eyes
			render_type = RENDER_STE;
			// draw red
			Matrix4f finalEdges;
			finalEdges.extend(&edgeMatrix);
			memcpy(camera, float_args, sizeof(float) * 6);
			perspectiveTransform(&finalEdges, float_args);
			screenTransform(&finalEdges, pix_width, pix_height,
			                xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0xff, 0, 0);
			drawEdges(drawSurface, &finalEdges, pixelColor);

			// draw cyan
			finalEdges.clear();
			finalEdges.extend(&edgeMatrix);
			perspectiveTransform(&finalEdges, float_args+3);
			screenTransform(&finalEdges, pix_width, pix_height,
			                xleft, ybot, xright, ytop);
			pixelColor = SDL_MapRGB(drawSurface->format, 0, 0xff, 0xff);
			drawEdges(drawSurface, &finalEdges, pixelColor);
			drawToScreen();
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
			error("unknown command: " + std::string(command));
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
		if ('0' <= fvals[0] && fvals[0] <= '9' || fvals[0] == '.' ||
			fvals[0] == '-') {
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
bool init(int width, int height) {
    pix_width = width;
    pix_height = height;

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
