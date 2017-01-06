#include "../src/uniforms.h"

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VOut {
  vec3 vColor;
} gs_in[];

flat out vec3 fColor;

void main() {
  fColor = gs_in[0].vColor;

  vec2 position = gl_in[0].gl_Position.xy,
    velocity = gl_in[0].gl_Position.zw;

  vec2 direction = mix(normalize(velocity), vec2(1.0f, 0.0f), velocity == vec2(0.0f, 0.0f));

  // ten pixels forward (relative to the velocity) from the origin
  vec2 point = position + direction * 10.0f;
  gl_Position = projection * vec4(point, 0.0f, 1.0f);
  EmitVertex();

  // 5 pixels left from the origin
  vec2 left = vec2(-direction.y, direction.x) * 5.0f;
  point = position + left;
  gl_Position = projection * vec4(point, 0.0f, 1.0f);
  EmitVertex();

  // 5 pixels right from the origin
  point = position - left;
  gl_Position = projection * vec4(point, 0.0f, 1.0f);
  EmitVertex();

  EndPrimitive();
}
