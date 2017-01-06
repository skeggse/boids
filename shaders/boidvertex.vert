#version 430 core

layout (std140, binding = 0) uniform SimParams {
  mat4 projection;
  vec2 winScale;
  uvec2 cellCounts;
  uint numBoids, cellCount;
  float cellSize, mapRatio,
    edgeDist, edgeFactor,
    separateDist2, separateFactor,
    alignDist2, alignFactor,
    attractDist2, attractFactor,
    maxSpeed;
};

in vec4 position;

out VOut {
  vec3 vColor;
};

void main() {
  gl_Position = position;
  vColor = vec3(0.8f, 0.0f, 0.0f);
}
