#version 410

layout (location = 0) in vec3 position;

// The MVP matrix
uniform mat4 MVP;

// Passing the color to the geometry shader
out vec3 pos;

void main() {
  pos = position;
  gl_Position = MVP * vec4(20 * position, 1);
}
