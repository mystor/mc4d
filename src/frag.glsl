#version 410

const vec3 lightDir = vec3(0.3, 1, 0);

// The points in space
uniform sampler2D faceTex;

// The kind of the hypercube
uniform float hcIndicator;

// The sky
uniform samplerCube skybox;

// the 3d eye position
uniform vec3 eyePos3;

in vec4 color;
in vec3 normal;
in vec3 pos3;

//layout(location=0)
out vec4 fragColour;

void main()
{
  vec3 nNormal = abs(normal);
  fragColour = vec4(0.5, 0.5, 0.5, 1) -
    // This is quite possibly the worst implementation of phong diffuse shading ever
    vec4(0.3, 0.3, 0.3, 0) * (texture(faceTex, color.rg).r + 0.1 * dot(nNormal, lightDir));

  // Yeah, this is awful, but I don't have to maintain this program, and this is
  // the fastest & easiest way to get it working, so yeah.
  if (hcIndicator == 1) { // Grass
    fragColour.g += clamp(color.b - 0.5, 0, 1);
  } else if (hcIndicator == 2) { // Water
    vec3 incoming = pos3 - eyePos3;
    vec3 refl = incoming - 2 * dot(normal, incoming) * normal;

    vec4 color = texture(skybox, refl);
    fragColour += color;

    fragColour = color + 0.25 * fragColour;
    fragColour.b += 0.1;

    fragColour.a = 0.5; // Blending factor
  } else if (hcIndicator == 3) { // Sand
    fragColour += 0.5 * vec4(237 / 255.0, 201 / 255.0, 175 / 255.0, 0);
  } else if (hcIndicator == 4) { // Solid grass
    fragColour.g += 0.5;
  } else if (hcIndicator == 5) { // Cloud
    fragColour.rgb += 0.3;
    fragColour.a = 0.4; // Blending factor
  }
}
