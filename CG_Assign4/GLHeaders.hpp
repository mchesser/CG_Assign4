//! Basic includes for OpenGL and glut under different systems
#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif