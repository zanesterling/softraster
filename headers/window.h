#ifndef WINDOW
#define WINDOW

void drawToScreen();
bool init(int width, int height);
void clean_up();

void renderParallelTriangles(SDL_Surface *, const Matrix4f *);
void renderPerspectiveCyclopsTriangles(SDL_Surface *,const Matrix4f *, const float *);
void renderPerspectiveStereoTriangles(SDL_Surface *, const Matrix4f *, const float *);

#endif
