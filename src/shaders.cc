//////////////////////////////////////////////////////////////////////////////
//
// A support source file from Angel 6th Edition
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

#include <iostream>

#include "Angel.h"

using namespace std;

// modified from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// prints out shader info
void printShaderInfoLog(const GLuint shader) {
  GLint infologLength;
  GLsizei charsWritten;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 1) {
    char *infoLog = new char[infologLength];
    glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
    cout << "Shader Log (" << charsWritten << ": " << infoLog << endl;
    delete[] infoLog;
  } else {
#ifdef DEBUG
    cout << "Shader: OK" << endl;
#endif
  }
}

// modified from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// prints out shader info
void printProgramInfoLog(const GLuint program) {
  GLint infologLength;
  GLsizei charsWritten;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 1) {
    char *infoLog = new char[infologLength];
    glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
    cout << "Program Log (" << charsWritten << ": " << infoLog << endl;
    delete[] infoLog;
  } else {
#ifdef DEBUG
    cout << "Program: OK" << endl;
#endif
  }
}

namespace Angel {
  // create a NULL-terminated string by reading the provided file
  static char *readShaderSource(const char *shaderFile) {
    FILE *fp = fopen(shaderFile, "rb");

    if (fp == NULL) return NULL;

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char *buf = new char[size + 1];
    size_t readSize = fread(buf, 1, size, fp);

    buf[readSize] = '\0';
    fclose(fp);

    return buf;
  }

  void attachShader(const GLuint program, const GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**) &source, NULL);
    glCompileShader(shader);

    printShaderInfoLog(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      cerr << "shader failed to compile:" << std::endl;
      GLint logSize;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      char *logMsg = new char[logSize];
      glGetShaderInfoLog(shader, logSize, NULL, logMsg);
      cerr << logMsg << std::endl;
      delete[] logMsg;

      exit(EXIT_FAILURE);
    }

    glAttachShader(program, shader);
  }

  void attachShaderFile(const GLuint program, const GLenum type, const char *file) {
    char *shader = readShaderSource(file);
    attachShader(program, type, shader);
    delete[] shader;
  }

  void linkProgram(const GLuint program) {
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
      std::cerr << "shader program failed to link:" << std::endl;
      GLint logSize;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
      char *logMsg = new char[logSize];
      glGetProgramInfoLog(program, logSize, NULL, logMsg);
      std::cerr << logMsg << std::endl;
      delete[] logMsg;

      exit(EXIT_FAILURE);
    }

    printProgramInfoLog(program);
  }
}  // Close namespace Angel block
