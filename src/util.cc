#include "Angel.h"

#include "util.h"

GLuint divUp(GLuint a, GLuint b) {
  return a / b + (a % b != 0);
}

void zeroBufferData(GLenum target) {
  glClearBufferData(target, GL_R8UI, GL_RED, GL_UNSIGNED_BYTE, NULL);
}
