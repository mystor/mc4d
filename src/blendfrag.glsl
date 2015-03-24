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

  if (solid.w > water.w) { // W == alpha == depth
    // Blend!
    fragColour = vec4(0.4 * water.rgb + 0.6 * solid.rgb, 1);
  } else {
    fragColour = vec4(solid.rgb, 1);
  }
}
