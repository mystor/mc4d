#version 410

uniform sampler2D waterTex;
// uniform sampler2DShadow waterDepth;
uniform sampler2D solidTex;
// uniform sampler2DShadow solidDepth;

in vec2 uv;

out vec4 fragColour;

void main()
{
  vec4 solid = texture(solidTex, uv);
  vec4 water = texture(waterTex, uv);

  // fragColour = vec4(water.rgb, 1);

  fragColour = vec4(water.a * water.rgb + (1 - water.a) * solid.rgb, 1);
}
