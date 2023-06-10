#include "../src/uniforms.h"

in vec4 position;

out VOut {
  vec3 vColor;
};

void main() {
  gl_Position = position;
  vec2 velocity = position.zw;
  float angle = atan(velocity.x, velocity.y) * 0.31830988618387107f * 0.5f;
  vColor = 0.7f * mix(1.0f.xxx, clamp(abs(fract(angle.xxx + vec3(1.0f, 0.666666666666666666666f, 0.333333333333333333333f)) * 6.0f - 3.0f.xxx) - 1.0f.xxx, 0.0f, 1.0f) * vec3(0.8f, 1.0f.xx), 0.7f);
  //vColor = vec3(0.8f, 0.0f, 0.0f);
}


/*vec3 hsv2rgb(vec3 c)
{
    vec3 p = abs(fract(angle.xxx + vec3(1.0f, 0.666666666666666666666f, 0.333333333333333333333f) * 6.0 - 3.0f.www);
    return clamp(p - 1.0f.xxx, 0.0, 1.0);

    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}*/
