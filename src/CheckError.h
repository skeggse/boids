//////////////////////////////////////////////////////////////////////////////
//
// A support header file from Angel 6th Edition
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

#include <stdio.h>
#include <GL/gl.h>

static const char* ErrorString(GLenum error) {
  const char *msg;
  switch (error) {
#define Case(Token) case Token: msg = #Token; break;
    Case(GL_NO_ERROR);
    Case(GL_INVALID_VALUE);
    Case(GL_INVALID_ENUM);
    Case(GL_INVALID_OPERATION);
    Case(GL_STACK_OVERFLOW);
    Case(GL_STACK_UNDERFLOW);
    Case(GL_OUT_OF_MEMORY);
#undef Case
  }

  return msg;
}

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif
static void _CheckError(const char* file, int line) {
  GLenum error = glGetError();

  do {
    fprintf(stderr, "[%s:%d] %s\n", file, line, ErrorString(error));
  } while ((error = glGetError()) != GL_NO_ERROR);
}
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#define CheckError() _CheckError(__FILE__, __LINE__)
