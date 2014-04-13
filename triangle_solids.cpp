#include "headers/triangle_solids.h"

using namespace std;

Matrix4f triangleMatrix;
int screen_dimensions[6];
//int pix_width, pix_height;
//int xleft, ybot, xright, ytop;
float camera[6];
int render_type;

int main(int argc, char **argv) {
	if (argc != 2) {
		usage(argv);
		return 1;
	}

	if (!init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
		cout << "initialization failed" << endl;
		return 1;
	}

	if (!parse(argc, argv)) {
		cout << "error occurred while parsing" << endl;
		return 1;
	}
}

bool parse(char **argv) {
	Matrix4f transformMatrix;
	char command[64];
	char **str_args;
	float float_args[16];
	int frame_count = 0;

	str_args = (char **) malloc(sizeof(char *) * 4);
    for (i = 0; i < 4; i++) {
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
			rotatex(&transformMatrix, float_args[0]);
		} else if (strcmp(command, "rotate-y") == 0) {
			rotatey(&transformMatrix, float_args[0]);
		} else if (strcmp(command, "rotate-z") == 0) {
			rotatez(&transformMatrix, float_args[0]);
		} else if (strcmp(command, "push") == 0) { // TODO
		} else if (strcmp(command, "pop") == 0) { // TODO
		} else if (strcmp(command, "sphere-t") == 0) { // TODO
		} else if (strcmp(command, "box-t") == 0) { // TODO
			Matrix4f box;
			box.addCol(Vec4f(-1, -1, 1, 1);
		} else if (strcmp(command, "screen") == 0) {
			// set the lower-left and upper-right positions of the screen
			screen_dimensions[2] = float_args[2];
			screen_dimensions[3] = float_args[3];
			screen_dimensions[4] = float_args[4];
			screen_dimensions[5] = float_args[5];
		} else if (strcmp(command, "pixels") == 0) {
			// initialize the surface
			screen_dimensions[0] = float_args[0];
			screen_dimensions[1] = float_args[1];
			if (!init(float_args[0], float_args[1])) {
				error("parser could not init");
				return false;
			}
		} else if (strcmp(command, "render-parallel") == 0) {
			// perform a parellel projection along the z-axis
			render_type = RENDER_PAR;
			renderParallelTriangles(&triangleMatrix);
		} else if (strcmp(command, "render-perspective-cyclops") == 0) {
			// perform a perspective rendering to a single eye
			render_type = RENDER_PER;
			memcpy(camera, float_args, sizeof(float) * 3);
			renderPerspectiveCyclopsTriangles(&triangleMatrix, camera);
		} else if (strcmp(command, "render-perspective-stereo") == 0) {
			// perform a perspective rendering to each of two eyes
			render_type = RENDER_STE;
			memcpy(camera, float_args, sizeof(float) * 6);
			renderPerspectiveStereoTriangles(&triangleMatrix, camera);
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
			return 0;
		}
	}
}

void usage(char **argv) {
    cout << "error: no input file\n";
    cout << argv[0] << " inputfile\n";
    return false;
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

	return true;
}
