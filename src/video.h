#ifndef VIDEO_H
#define VIDEO_H

#include <GL/gl.h>
#include <map>

#include "vec3d.h"

#include "font.h"

#define PI 3.14159265358f

#define GL_BUFFER_OFFSET(i) ((char *)(0) + (i))

////////// VIDEO CLASS

struct Video
{
	void init(int xres, int yres);

	void clearScreen();
	void set3D();
	void set2D();

	int xres, yres;
};

extern Video video;

GLuint loadTGA(const char *filename, bool mipmaps);

#endif
