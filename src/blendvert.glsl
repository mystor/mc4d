#version 410

layout (location = 0) in vec2 position;

// Passing the color to the geometry shader
out vec2 uv;

void main() {
  uv = vec2((position + 1) / 2);
  gl_Position = vec4(position, 0, 1);
}
