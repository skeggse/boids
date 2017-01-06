#include "../src/uniforms.h"

in vec4 position;

out VOut {
  vec3 vColor;
};

void main() {
  gl_Position = position;
  vColor = vec3(0.8f, 0.0f, 0.0f);
}
