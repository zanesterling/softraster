#include "renderer.h"
#include <stdlib.h>

#define L_COMMAND 0
#define L_ARGS 1

const int SCREEN_WIDTH  = 500;
const int SCREEN_HEIGHT = 500;
const int SCREEN_BPP    = 32;

int usage(char **argv) {
    using namespace std;
    cout << "error: no input file\n";
    cout << argv[0] << " inputfile\n";
    return 1;
}

int parse(int argc, char **argv) {
    /* everything set up, let's parse! */
    Uint32 pixelColr = SDL_MapRGB(screen->format, 0, 0, 0);
    unsigned char color[3] = {0, 0, 0};
    char input[1024];
    char *temp;
	FILE *fin = fopen(argv[1], "r");
	int linetype = L_COMMAND;
	char command;

	// get the screen's surface
    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                           SDL_SWSURFACE);

	if (argc < 2) {
		return usage(argv);
	}

	while(input[0]) {
		temp = input;
		//printf("%s", input);
		fgets(input, 1024, fin);

		//skip commented lines
		if (input[0] == '#')
			continue;
		if (input[0] == 'q')
			break;

		if (linetype == L_COMMAND) {
			command = input[0];
			linetype = L_ARGS;
		} else if (linetype == L_ARGS) {
			switch (command) {
				case 'c':
					color[0] = atoi(temp);
					for (int i = 1; i < 3; i++) {
						while (temp[0] != ' ') temp++;
						temp++;
						color[i] = atoi(temp);
					}
					pixelColor = SDL_MapRGB(screen->format, color[0], color[1], color[2]);
					break; 
				case 'l':
					int crds[4];
    				crds[0] = atoi(temp);
    				for (int i = 1; i < 4; i++) {
    					while (temp[0] != ' ') temp++;
    					temp++;
    					crds[i] = atoi(temp);
    				}
    				drawline(screen, crds[0], crds[1], crds[2], crds[3], pixelColr);
    				break;
    			case '#':
    				break;
    			case 'q':
    				input[0] = 0;
    				break;
    		}
    		linetype = L_COMMAND;
    	}
	}

	return 0;
}

int main(int argc, char **argv) {
    if (argc != 2)
        return usage(argv);

	// set up sdl
    SDL_Init(0);
    SDL_InitSubSystem(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	int result = parse(argc, argv);

    // pixels drawn, flip the screen
    SDL_Flip(screen);

	// wait, then quit
    SDL_Delay(3000);
    SDL_Quit();
    return result;
}
