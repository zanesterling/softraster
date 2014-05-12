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

	waitUntilQuit();
	//spinDisplayUntilQuit();

    clean_up();
	return 0;
}

void waitUntilQuit() {
	while (processEvents()) SDL_Delay(10);
}

bool processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			return true;
		}
	}
	return false;
}

void spinDisplayUntilQuit() {
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
		rotatey(&triangleMatrix, 0.03);
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
}

bool parse(char **argv) {
	Matrix4f transformMatrix;
	stack<Matrix4f> transformStack;
	map<string, Vec4f> var_args;
	map<string, Matrix4f> savedSystems;
	char command[64];
	char **str_args;
	float float_args[16];
	int max_frames, cur_frame = -1;
	FILE *fp;

	str_args = (char **) malloc(sizeof(char *) * 4);
    for (int i = 0; i < 4; i++) {
		str_args[i] = (char *) calloc(64, sizeof(char));
    }

	// initialize transformMatrix to be identity matrix
	transformMatrix.addCol(Vec4f(1, 0, 0, 0));
	transformMatrix.addCol(Vec4f(0, 1, 0, 0));
	transformMatrix.addCol(Vec4f(0, 0, 1, 0));
	transformMatrix.addCol(Vec4f(0, 0, 0, 1));

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		cout << "no such input file\n";
		return false;
	}

	cout << "parsing\n";
	while (getLine(fp, &var_args, cur_frame, command, str_args, float_args)) {
		if (processEvents()) return true;
		//cout << command << endl;
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
		} else if (strcmp(command, "push") == 0) {
			// push the current transformation (matrix?) to the stack
			transformStack.push(transformMatrix);
		} else if (strcmp(command, "pop") == 0) {
			// pop a transformation matrix from the stack and make it current
			if (transformStack.empty()) {
				cout << "request to pop empty transform stack\n";
				return false;
			}
			transformMatrix = transformStack.top();
			transformStack.pop();
		} else if (strcmp(command, "sphere-t") == 0) {
			// compose a sphere made from triangles, then scale, rotate, and move it
			Matrix4f sphere;
			makeSphere(&sphere);
			scale(&sphere, float_args[0], float_args[1], float_args[2]);
			rotatex(&sphere, dtor(float_args[3]));
			rotatey(&sphere, dtor(float_args[4]));
			rotatez(&sphere, dtor(float_args[5]));
			translate(&sphere, float_args[6], float_args[7], float_args[8]);
			sphere.transform(&transformMatrix);
			triangleMatrix.extend(&sphere);
		} else if (strcmp(command, "box-t") == 0) {
			// compose a box made from triangles, then scale, rotate, and move it
			Matrix4f box;
			makeBox(&box);
			scale(&box, float_args[0], float_args[1], float_args[2]);
			rotatex(&box, dtor(float_args[3]));
			rotatey(&box, dtor(float_args[4]));
			rotatez(&box, dtor(float_args[5]));
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
			if (window == NULL) {
				// initialize the surface
				screen_dimensions[0] = float_args[0];
				screen_dimensions[1] = float_args[1];
				/*for (int i = 0; i < 6; i++) {
					cout << screen_dimensions[i] << " ";
				}
				cout << endl;*/
				if (!init(float_args[0], float_args[1])) {
					error("parser could not init");
					return false;
				}
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
		} else if (strcmp(command, "save") == 0) {
			savedSystems[string(str_args[0])] = transformMatrix;
		} else if (strcmp(command, "restore") == 0) {
			transformMatrix.clear();
			transformMatrix.extend(&savedSystems[string(str_args[0])]);
		} else if (strcmp(command, "vary") == 0) {
			if (var_args.find(string(str_args[0])) == var_args.end()) {
				var_args[string(str_args[0])] = Vec4f(float_args);
			}
		} else if (strcmp(command, "frames") == 0) {
			if (cur_frame == -1) {
				cur_frame  = float_args[0];
				max_frames = float_args[1];
			}
		} else if (strcmp(command, "end") == 0) {
			// stop parsing
			if (cur_frame >= max_frames) {
				cur_frame = 1;
			} else if (cur_frame == -1) {
				return true;
			} else {
				cur_frame++;
			}
			rewind(fp);
			triangleMatrix.clear();
			transformMatrix.clear();
			transformMatrix.addCol(Vec4f(1, 0, 0, 0));
			transformMatrix.addCol(Vec4f(0, 1, 0, 0));
			transformMatrix.addCol(Vec4f(0, 0, 1, 0));
			transformMatrix.addCol(Vec4f(0, 0, 0, 1));
			SDL_FillRect(drawSurface, NULL, 0x000000);
		}
	}
}

void usage(char **argv) {
    cout << "error: no input file\n";
    cout << argv[0] << " inputfile\n";
}

bool getLine(FILE *fin, map<string, Vec4f> *var_args, int frame,
             char *command_buffer, char **args_buffer, float *vals_buffer) {
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
			if (var_args->find(string(fvals)) != var_args->end()) {
				// if it's a recognized var, convert it into vals buffer
				Vec4f v = (*var_args)[string(fvals)];
				float val = v[0] + (v[1] - v[0]) / (v[3] - v[2] + 1) * (frame - v[2]);
				//if (strcmp(fvals, "rx") == 0) cout << val << " " << vals_index << endl;
				vals_buffer[vals_index++] = val;
			} else {
				// otherwise put it in args_buffer
				strcpy(args_buffer[args_index++], fvals);
			}
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
	boxMatrix->addCol(Vec4f(-0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, 0.5, 1));

	// back face
	boxMatrix->addCol(Vec4f(0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, -0.5, 1));

	// right face
	boxMatrix->addCol(Vec4f(0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, 0.5, 1));

	// left face
	boxMatrix->addCol(Vec4f(-0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, 0.5, 1));

	// top face
	boxMatrix->addCol(Vec4f(-0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, -0.5, -0.5, 1));

	// bottom face
	boxMatrix->addCol(Vec4f(0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(0.5, 0.5, 0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, -0.5, 1));
	boxMatrix->addCol(Vec4f(-0.5, 0.5, 0.5, 1));
}

void makeSphere(Matrix4f *sphereMatrix) {
	Matrix4f sector;
	Matrix4f side;
	Matrix4f nextSide;

	for (int i = 0; i <= SPHERE_LAT_LINES; i++) {
		float angle = M_PI * i / SPHERE_LAT_LINES;
		side.addCol(Vec4f(sin(angle), 0, -cos(angle), 1));
		nextSide.addCol(*(side[i]));
	}

	rotatez(&nextSide, M_PI * 2 / SPHERE_LON_LINES);
	for (int i = 1; i < SPHERE_LAT_LINES-1; i++) {
		// add body triangles
		sector.addCol(*(side[i]));
		sector.addCol(*(side[i+1]));
		sector.addCol(*(nextSide[i+1]));
		sector.addCol(*(nextSide[i+1]));
		sector.addCol(*(nextSide[i]));
		sector.addCol(*(side[i]));
	}
	// prevent cap doubling
	sector.addCol(*side[0]);
	sector.addCol(*side[1]);
	sector.addCol(*nextSide[1]);
	sector.addCol(*nextSide[SPHERE_LAT_LINES-1]);
	sector.addCol(*side[SPHERE_LAT_LINES-1]);
	sector.addCol(*side[SPHERE_LAT_LINES]);

	for (int i = 0; i < SPHERE_LON_LINES; i++) {
		sphereMatrix->extend(&sector);
		rotatez(&sector, M_PI * 2 / SPHERE_LON_LINES);
	}
}
