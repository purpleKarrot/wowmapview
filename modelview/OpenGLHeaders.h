#ifndef OPENGLHEADERS_H
#define OPENGLHEADERS_H

#ifdef _WINDOWS
	#include ".\glew\include\GL\glew.h"
	#include ".\glew\include\GL\wglew.h"
#elif __WXMAC__
    #include <GL/glew.h>
#else
	#include <GL/glew.h>
	#include <GL/glxew.h>
#endif

#ifdef _WINDOWS
#include <windows.h>
#endif

// opengl
#ifdef __WXMAC__ //Mac
#   include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#else
#	include <GL/gl.h>
#	include <GL/glu.h>
#endif


#define GL_BUFFER_OFFSET(i) ((char *)(0) + (i))

#endif

