#pragma once

#include <ostream>

#include "Angel.h"
#include "util.h"
#include "uniforms.h"

#pragma pack(push, 1)
struct boid {
  vec2 location, velocity;
};

struct range {
  GLuint offset, count;
};

struct Buffers {
  Buffer boid[2],
    uniform,
    counts, offsets, ranges, index;

  Buffers() {
    boid[0].useFor(GL_DYNAMIC_DRAW);
    boid[1].useFor(GL_DYNAMIC_DRAW);
    uniform.useFor(GL_STATIC_DRAW).bindTarget(GL_UNIFORM_BUFFER);
  }
};

struct {
  GLuint draw, cellCounts, spreadCounts, index, spreadIndex, move;
} programs;
#pragma pack(pop)
