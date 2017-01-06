#pragma once

#include <algorithm>
#include <iostream>

#include "Angel.h"

#define arraySize(array) (sizeof(array) / sizeof*(array))

#pragma pack(push, 1)
struct uvec2 {
  GLuint x, y;

  uvec2() : x(0), y(0) {}
  uvec2(const GLuint x, const GLuint y) : x(x), y(y) {}
  uvec2(const GLfloat x, const GLfloat y) : x(GLuint(x)), y(GLuint(y)) {}
  uvec2(const vec2 &v) : uvec2(v.x, v.y) {}

  friend std::ostream& operator << (std::ostream &os, const uvec2 &u) {
    return os << "( " << u.x << ", " << u.y << " )";
  }
};
#pragma pack(pop)

struct Buffer {
  GLuint buffer;
  bool hasBuffer, initialized;
  GLsizeiptr size;
  GLenum usage, target;

  Buffer() : hasBuffer(false), initialized(false),
    usage(GL_DYNAMIC_READ), target(GL_SHADER_STORAGE_BUFFER) {}

  Buffer &useBuffer(const GLuint buffer) {
    if (hasBuffer) abort();
    hasBuffer = true;
    return *this;
  }

  Buffer &bindTarget(const GLenum target) {
    this->target = target;
    return *this;
  }

  Buffer &useFor(const GLenum usage) {
    this->usage = usage;
    return *this;
  }

  void ensureBuffer() {
    if (!hasBuffer) {
      hasBuffer = true;
      glGenBuffers(1, &buffer);
    }
  }

  void bind() {
    bindTo(target);
  }

  void bind(const GLuint index) {
    ensureBuffer();
    glBindBufferBase(target, index, buffer);
  }

  void bindTo(const GLenum target) {
    ensureBuffer();
    glBindBuffer(target, buffer);
  }

  void clear() {
    if (initialized) {
      bind();
      glClearBufferData(target, GL_R8UI, GL_RED, GL_UNSIGNED_BYTE, NULL);
    } else {
      abort();
    }
  }

  void write(const void *data, const GLsizeiptr size) {
    bind();
    if (initialized && size == this->size) {
      glBufferSubData(target, 0, size, data);
    } else {
      initialized = true;
      this->size = size;
      glBufferData(target, size, data, usage);
    }
  }

  void write(const void *data, const GLintptr offset, const GLsizeiptr size) {
    if (!initialized || offset + size > this->size) {
      std::cerr << "invalid write(offset, size) call" << std::endl;
      abort();
    }
    bind();
    glBufferSubData(target, offset, size, data);
  }

  void read(void * const data) {
    if (!initialized) return;
    bind();
    glGetBufferSubData(target, 0, size, data);
  }

  void copyFrom(Buffer &other) {
    if (!other.initialized) {
      std::cerr << "invalid copyFrom() call" << std::endl;
      abort();
    }
    if (!initialized) {
      write(NULL, other.size);
      copyFrom(other, 0, 0, other.size);
    } else {
      copyFrom(other, 0, 0, std::min({size, other.size}));
    }
  }

  void copyFrom(Buffer &other, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    if (!initialized || size > this->size || size > other.size) {
      std::cerr << "invalid copyFrom(offsets, size) call" << std::endl;
      abort();
    }
    bindTo(GL_COPY_WRITE_BUFFER);
    other.bindTo(GL_COPY_READ_BUFFER);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
  }
};

GLuint divUp(const GLuint a, const GLuint b);
