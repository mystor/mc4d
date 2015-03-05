#version 410

layout (location = 0) in vec2 position;

// This is the location
out vec2 screenPos;

void main() {
  screenPos = position; // Ranges from -1 to 1 in each dimension
  gl_Position = vec4(position, 0, 1);
}
