#version 410

uniform samplerCube sb;

in vec3 pos;

out vec4 fragColor;

void main()
{
  fragColor = vec4(1, 0, 1, 1);
  fragColor = texture(sb, pos);
}
