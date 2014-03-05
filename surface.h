#ifndef SURFACE
#define SURFACE
#include <stdlib.h>
class Surface {
public:
	Surface(int w, int h);
	~Surface();
	void clear(unsigned char *color);
	void setPixel(int x, int y, unsigned char *color);
	unsigned char *getPixel(int x, int y);
	int width;
	int height;
private:
	unsigned char *data;
};
#endif
