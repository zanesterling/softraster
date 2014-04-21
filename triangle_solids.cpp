#include "headers/triangle_solids.h"

#define RENDER_PAR 0
#define RENDER_PER 1
#define RENDER_STE 2

using namespace std;

const int SPHERE_LAT_LINES = 20;
const int SPHERE_LON_LINES = 20;

Matrix4f triangleMatrix;
int screen_dimensions[6];
float camera[6];
int render_type;

int main(int argc, char **argv) {
	if (argc != 2) {
		usage(argv);
		return 1;
	}

	if (!parse(argv)) {
		cout << "error occurred while parsing" << endl;
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
		for (int i = 0; i < triangleMatrix.width; i++) {
			avgX += triangleMatrix.get(i, 0);
			avgY += triangleMatrix.get(i, 1);
			avgZ += triangleMatrix.get(i, 2);
		}
		avgX /= triangleMatrix.width;
		avgY /= triangleMatrix.width;
		avgZ /= triangleMatrix.width;

		// rotate the model slightly around its center
		translate(&triangleMatrix, -avgX, -avgY, -avgZ);
		rotatex(&triangleMatrix, 0.02);
		rotatey(&triangleMatrix, 0.02);
		translate(&triangleMatrix, avgX, avgY, avgZ);

		if (render_type == RENDER_PAR) {
			renderParallelTriangles(&triangleMatrix, screen_dimensions);
		} else if (render_type == RENDER_PER) {
			renderPerspectiveCyclopsTriangles(&triangleMatrix, camera,
			                                  screen_dimensions);
		} else if (render_type == RENDER_STE) {
			renderPerspectiveStereoTriangles(&triangleMatrix, camera,
			                                 screen_dimensions);
		}
		drawToScreen();
		SDL_Delay(10);
	}
    //SDL_Delay(4000);

    clean_up();
    return 0;
}

bool parse(char **argv) {
	Matrix4f transformMatrix;
	char command[64];
	char **str_args;
	float float_args[16];
	int frame_count = 0;

	str_args = (char **) malloc(sizeof(char *) * 4);
    for (int i = 0; i < 4; i++) {
		str_args[i] = (char *) calloc(64, sizeof(char));
    }

	// initialize transformMatrix to be identity matrix
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));

	FILE *fp = fopen(argv[1], "r");
	cout << "parsing\n";
	while (getLine(fp, command, str_args, float_args)) {
		cout << command << endl;
		if (strcmp(command, "identity") == 0) {
			transformMatrix.clear();
			transformMatrix.addCol(Vec4f(1, 0, 0, 0));
			transformMatrix.addCol(Vec4f(0, 1, 0, 0));
			transformMatrix.addCol(Vec4f(0, 0, 1, 0));
			transformMatrix.addCol(Vec4f(0, 0, 0, 1));
		} else if (strcmp(command, "move") == 0) {
			translate(&transformMatrix, float_args[0], float_args[1], float_args[2]);
		} else if (strcmp(command, "scale") == 0) {
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
		} else if (strcmp(command, "push") == 0) { // TODO
			// push the current transformation (matrix?) to the stack
		} else if (strcmp(command, "pop") == 0) { // TODO
			// pop a transformation matrix from the stack and make it current
		} else if (strcmp(command, "sphere-t") == 0) {
			// compose a sphere made from triangles, then scale, rotate, and move it
			Matrix4f sphere;
			makeSphere(&sphere);
			scale(&sphere, float_args[0], float_args[1], float_args[2]);
			rotatex(&sphere, float_args[3]);
			rotatey(&sphere, float_args[4]);
			rotatez(&sphere, float_args[5]);
			translate(&sphere, float_args[6], float_args[7], float_args[8]);
			sphere.transform(&transformMatrix);
			triangleMatrix.extend(&sphere);
		} else if (strcmp(command, "box-t") == 0) {
			// compose a box made from triangles, then scale, rotate, and move it
			Matrix4f box;
			makeBox(&box);
			scale(&box, float_args[0], float_args[1], float_args[2]);
			rotatex(&box, float_args[3]);
			rotatey(&box, float_args[4]);
			rotatez(&box, float_args[5]);
			translate(&box, float_args[6], float_args[7], float_args[8]);
			box.transform(&transformMatrix);
			triangleMatrix.extend(&box);
		} else if (strcmp(command, "screen") == 0) {
			// set the lower-left and upper-right positions of the screen
			screen_dimensions[2] = float_args[0];
			screen_dimensions[3] = float_args[1];
			screen_dimensions[4] = float_args[2];
			screen_dimensions[5] = float_args[3];
		} else if (strcmp(command, "pixels") == 0) {
			// initialize the surface
			screen_dimensions[0] = float_args[0];
			screen_dimensions[1] = float_args[1];
			for (int i = 0; i < 6; i++)
				cout << screen_dimensions[i] << " ";
			cout << endl;
			if (!init(float_args[0], float_args[1])) {
				error("parser could not init");
				return false;
			}
		} else if (strcmp(command, "render-parallel") == 0) {
			// perform a parellel projection along the z-axis
			render_type = RENDER_PAR;
			renderParallelTriangles(&triangleMatrix, screen_dimensions);
		} else if (strcmp(command, "render-perspective-cyclops") == 0) {
			// perform a perspective rendering to a single eye
			render_type = RENDER_PER;
			memcpy(camera, float_args, sizeof(float) * 3);
			renderPerspectiveCyclopsTriangles(&triangleMatrix, camera,
			                                  screen_dimensions);
		} else if (strcmp(command, "render-perspective-stereo") == 0) {
			// perform a perspective rendering to each of two eyes
			render_type = RENDER_STE;
			memcpy(camera, float_args, sizeof(float) * 6);
			renderPerspectiveStereoTriangles(&triangleMatrix, camera,
			                                 screen_dimensions);
		} else if (strcmp(command, "clear-triangles") == 0) {
			// clear the triangle matrix
			triangleMatrix.clear();
		} else if (strcmp(command, "clear-pixels") == 0) {
			// clear the screen
			clear(drawSurface);
			drawToScreen();
		} else if (strcmp(command, "file") == 0) {
			// save to a file
			savePPM(drawSurface, str_args[0]);
		} else if (strcmp(command, "files") == 0) { // TODO
			// save a frame
			sprintf(str_args[0] + strlen(str_args[0]), "%s%02i",
			        str_args[0], frame_count++);
		} else if (strcmp(command, "end") == 0) {
			// stop parsing
			return true;
		}
	}
}

void usage(char **argv) {
    cout << "error: no input file\n";
    cout << argv[0] << " inputfile\n";
}

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

	for (i = 0; command_buffer[i] != '\0'; i++) {
		if (command_buffer[i] == '\r') {
			command_buffer[i] = '\0';
			break;
		}
	}

	for (j = 0; j < args_index; j++) {
		for (i = 0; args_buffer[j][i] != '\0'; i++) {
			if (args_buffer[j][i] == '\r') {
				args_buffer[j][i] = '\0';
				break;
			}
		}
	}

	return true;
}

// degrees to radians
float dtor(float degrees) {
	return degrees * M_PI / 180;
}

void makeBox(Matrix4f *boxMatrix) {
	// front face
	boxMatrix->addCol(Vec4f(0, 0, 1, 1));
	boxMatrix->addCol(Vec4f(1,  0, 1, 1));
	boxMatrix->addCol(Vec4f(1,  1,  1, 1));
	boxMatrix->addCol(Vec4f(1,  1,  1, 1));
	boxMatrix->addCol(Vec4f(0, 1,  1, 1));
	boxMatrix->addCol(Vec4f(0, 0, 1, 1));

	// back face
	boxMatrix->addCol(Vec4f(1,  0, 0, 1));
	boxMatrix->addCol(Vec4f(0, 0, 0, 1));
	boxMatrix->addCol(Vec4f(1,  1,  0, 1));
	boxMatrix->addCol(Vec4f(0, 1,  0, 1));
	boxMatrix->addCol(Vec4f(1,  1,  0, 1));
	boxMatrix->addCol(Vec4f(0, 0, 0, 1));

	// right face
	boxMatrix->addCol(Vec4f(1, 0, 1,  1));
	boxMatrix->addCol(Vec4f(1, 0, 0, 1));
	boxMatrix->addCol(Vec4f(1, 1,  0, 1));
	boxMatrix->addCol(Vec4f(1, 1,  0, 1));
	boxMatrix->addCol(Vec4f(1, 1,  1,  1));
	boxMatrix->addCol(Vec4f(1, 0, 1,  1));

	// left face
	boxMatrix->addCol(Vec4f(0, 0, 0, 1));
	boxMatrix->addCol(Vec4f(0, 0, 1,  1));
	boxMatrix->addCol(Vec4f(0, 1,  0, 1));
	boxMatrix->addCol(Vec4f(0, 1,  0, 1));
	boxMatrix->addCol(Vec4f(0, 0, 1,  1));
	boxMatrix->addCol(Vec4f(0, 1,  1,  1));

	// top face
	boxMatrix->addCol(Vec4f(0, 0, 0, 1));
	boxMatrix->addCol(Vec4f(1,  0, 0, 1));
	boxMatrix->addCol(Vec4f(1,  0, 1,  1));
	boxMatrix->addCol(Vec4f(1,  0, 1,  1));
	boxMatrix->addCol(Vec4f(0, 0, 1,  1));
	boxMatrix->addCol(Vec4f(0, 0, 0, 1));

	// bottom face
	boxMatrix->addCol(Vec4f(1,  1, 0, 1));
	boxMatrix->addCol(Vec4f(0, 1, 0, 1));
	boxMatrix->addCol(Vec4f(1,  1, 1,  1));
	boxMatrix->addCol(Vec4f(1,  1, 1,  1));
	boxMatrix->addCol(Vec4f(0, 1, 0, 1));
	boxMatrix->addCol(Vec4f(0, 1, 1,  1));
}

void makeSphere(Matrix4f *sphereMatrix) {
	Matrix4f sector;
	Matrix4f side;
	Matrix4f nextSide;

	for (int i = 0; i <= SPHERE_LAT_LINES; i++) {
		float angle = M_PI * i / SPHERE_LAT_LINES;
		side.addCol(Vec4f(sin(angle), -cos(angle), 0, 1));
		nextSide.addCol(*(side[i]));
	}

	rotatey(&nextSide, M_PI * 2 / SPHERE_LON_LINES);
	for (int i = 0; i < SPHERE_LON_LINES; i++) {
		sector.addCol(*(side[i]));
		sector.addCol(*(side[i+1]));
		sector.addCol(*(nextSide[i+1]));
		sector.addCol(*(nextSide[i+1]));
		sector.addCol(*(nextSide[i]));
		sector.addCol(*(side[i]));
	}

	for (int i = 0; i < SPHERE_LON_LINES; i++) {
		sphereMatrix->extend(&sector);
		rotatey(&sector, M_PI * 2 / SPHERE_LON_LINES);
	}
}
