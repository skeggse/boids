//////////////////////////////////////////////////////////////////////////////
//
// The main header file for all examples from Angel 6th Edition
//
// This file is provided under the MIT License
// (http://opensource.org/licenses/MIT).
//
// Original work Copyright (c) <2014-2015> <Ed Angel and Dave Shreiner>
// Modified work Copyright (c) 2017 Eli Skeggs
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

// Visual Studio warns about deprecated fopen. Get rid of that warning
#define _CRT_SECURE_NO_WARNINGS

#include <cmath>
#include <iostream>
#include <vector>

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace Angel {
  void attachShader(GLuint program, GLenum type, const char *source);
  void attachShaderFile(GLuint program, GLenum type, const char *file);
  void linkProgram(GLuint program);

  // Defined constant for when numbers are too small to be used in the
  // denominator of a division operation.  This is only used if the
  // DEBUG macro is defined.
  const GLfloat DivideByZeroTolerance = GLfloat(1.0e-07f);

  //  Degrees-to-radians constant
  const GLfloat DegreesToRadians = (GLfloat) M_PI / (GLfloat) 180.0;
} // namespace Angel

#include "vec.h"
#include "mat.h"
#include "CheckError.h"

#define Print(x) do { std::cerr << #x " = " << (x) << std::endl; } while(0)

  // Globally use our namespace in our example programs.
using namespace Angel;
