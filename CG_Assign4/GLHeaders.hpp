//! Basic includes for OpenGL and glut under different systems
#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#ifndef __APPLE__
  #include <GL/glew.h>
  #include <GL/glut.h>
#else
  #include <OpenGL/gl3.h>
  #include <GLUT/glut.h>
#endif
