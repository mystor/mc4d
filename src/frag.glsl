#version 410

const vec3 lightDir = vec3(0.3, 1, 0);

// The points in space
uniform sampler2D faceTex;

uniform float hcIndicator;

in vec4 color;
in vec3 normal;

out vec4 fragColour;

void main()
{
  vec3 nNormal = abs(normal);
  fragColour = vec4(0.5, 0.5, 0.5, 1) -
    vec4(0.1, 0.1, 0.1, 1) * (texture(faceTex, color.rg).r + dot(nNormal, lightDir));
  fragColour.g += 0.5 * color.b * hcIndicator;
  // fragColour = vec4(0, 1, 0, 1);
}
