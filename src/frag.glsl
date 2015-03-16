#version 410

in vec3 normal;

out vec4 fragColour;

void main()
{
  vec3 nNormal = abs(normal);
  fragColour = vec4(nNormal, 0.5);
}
