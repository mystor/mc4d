#version 410

in vec4 color;
in vec3 normal;

out vec4 fragColour;

void main()
{
  vec3 nNormal = abs(normal);
  fragColour = color;// vec4(nNormal, 0.5);
}
