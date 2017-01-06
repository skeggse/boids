#pragma once

#include <ostream>

#include "Angel.h"
#include "util.h"

#pragma pack(push, 1)
struct boid {
  vec2 location, velocity;
};

struct range {
  GLuint offset, count;
};

struct Uniforms {
  mat4 projection;
  vec2 winScale;
  uvec2 cellCounts;
  GLuint numBoids, cellCount;
  GLfloat cellSize, mapRatio,
    edgeDist, edgeFactor,
    separateDist2, separateFactor,
    alignDist2, alignFactor,
    attractDist2, attractFactor,
    maxSpeed;

  Uniforms() {}

  friend std::ostream& operator << (std::ostream& os, const Uniforms &u) {
    return os << "( projection " << u.projection
      << " winScale " << u.winScale
      << " cellCounts " << u.cellCounts
      << " numBoids " << u.numBoids
      << " cellCount " << u.cellCount
      << " cellSize " << u.cellSize
      << " mapRatio " << u.mapRatio
      << " edgeDist " << u.edgeDist
      << " edgeFactor " << u.edgeFactor
      << " separateDist2 " << u.separateDist2
      << " separateFactor " << u.separateFactor
      << " alignDist2 " << u.alignDist2
      << " alignFactor " << u.alignFactor
      << " attractDist2 " << u.attractDist2
      << " attractFactor " << u.attractFactor
      << " maxSpeed " << u.maxSpeed
      << " )";
  }
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
