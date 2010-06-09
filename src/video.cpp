#include "video.h"
#include "shaders.h"
#include "mpq.h"
#include "wowmapview.h"
#include <GL/glu.h>

Video video;

void Video::init(int xres, int yres, bool fullscreen)
{
	initShaders();

	this->xres = xres;
	this->yres = yres;

	glViewport(0,0,xres,yres);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, (GLfloat)xres/(GLfloat)yres, 0.01f, 1024.0f);
	gluPerspective(45.0f, (GLfloat)xres/(GLfloat)yres, 1.0f, 1024.0f);

	// hmmm...
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Video::clearScreen()
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Video::set3D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)xres/(GLfloat)yres, 1.0f, 1024.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Video::set2D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, xres, yres, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


#pragma pack(push,1)
struct TGAHeader {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
};
#pragma pack(pop)


GLuint loadTGA(const char *filename, bool mipmaps)
{
	FILE *f = fopen(filename,"rb");
	TGAHeader h;
	fread(&h,18,1,f);
	if (h.datatypecode != 2) return 0;
	size_t s = h.width * h.height;
	GLint bppformat;
	GLint format;
	int bypp = h.bitsperpixel / 8;
	if (h.bitsperpixel == 24) {
		s *= 3;
		format = GL_RGB;
		bppformat = GL_RGB8;
	} else if (h.bitsperpixel == 32) {
		s *= 4;
		format = GL_RGBA;
		bppformat = GL_RGBA8;
	} else return 0;

	unsigned char *buf = new unsigned char[s], *buf2;
	//unsigned char *buf2 = new unsigned char[s];
	fread(buf,s,1,f);
	fclose(f);

	buf2 = buf;

	GLuint t;
	glGenTextures(1,&t);
	glBindTexture(GL_TEXTURE_2D, t);

	if (mipmaps) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps (GL_TEXTURE_2D, bppformat, h.width, h.height, format, GL_UNSIGNED_BYTE, buf2);
	} else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, bppformat, h.width, h.height, 0, format, GL_UNSIGNED_BYTE, buf2);
	}
    delete[] buf;
	//delete[] buf2;
	return t;
}
