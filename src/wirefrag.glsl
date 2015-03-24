#version 410

uniform vec4 eye;

// The depth, used for depthcueing
in float depth;

out vec4 fragColour;

void main()
{
  fragColour = vec4(depth / (length(eye) + 8), 0, 0, 1);
}
