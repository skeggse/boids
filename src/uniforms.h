// hybrid header file for c++ and glsl
// concept borrowed from NVIDIA's GameWorks graphics samples

#ifndef __cplusplus
#define GLint int
#define GLuint uint
#define GLint64 int64
#define GLuint64 uint64
#define GLfloat float
#define GLdouble double
layout (std140, binding = 0) uniform
#else
#pragma once

#pragma pack(push, 1)
struct
#endif

Params {
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

#ifdef __cplusplus
  Params() {}

  friend std::ostream& operator << (std::ostream& os, const Params &p) {
    return os << "( projection " << p.projection
      << " winScale " << p.winScale
      << " cellCounts " << p.cellCounts
      << " numBoids " << p.numBoids
      << " cellCount " << p.cellCount
      << " cellSize " << p.cellSize
      << " mapRatio " << p.mapRatio
      << " edgeDist " << p.edgeDist
      << " edgeFactor " << p.edgeFactor
      << " separateDist2 " << p.separateDist2
      << " separateFactor " << p.separateFactor
      << " alignDist2 " << p.alignDist2
      << " alignFactor " << p.alignFactor
      << " attractDist2 " << p.attractDist2
      << " attractFactor " << p.attractFactor
      << " maxSpeed " << p.maxSpeed
      << " )";
  }
#endif
};

#ifdef __cplusplus
#pragma pack(pop)
#endif
