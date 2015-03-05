#version 410

const vec4 skyColour = vec4(0.3, 0.5, 1, 1);
const vec3 scale = vec3(8, 4, 8);
const ivec3 worldSize = ivec3(16, 16, 16);

// The noise function. Should only be sampled at 1/scale intervals.
uniform sampler3D world;

// The location of the eye in space
uniform vec3 eye;

// The 3 basis vectors for the 3-space which we are looking in
// INVARIANT: All 3 vectors must be perpendicular
uniform vec3 forward;
uniform vec3 up;
uniform vec3 right;

in vec2 screenPos;

out vec4 fragColour;

bool sampleWorld(vec3 point)
{
  // First we have to figure out what block it is
  vec3 roundedPt = round(point);
  if (point.x < 0 || point.y < 0 || point.z < 0) {
    // There is nothing to the negative of the world
    return false;
  }
  // if (point.x < 0 || point.y < 0 || point.z < 0)
  return false;
}

vec3 raycast(vec3 raydir)
{
  // TODO(michael): Test if textures are working
  vec4 texResult = texture(world, vec3(0.5, 0.5, 0.5));
  if (texResult.x >= 0.5 && texResult.y <= 0.5) {
    return vec3(-1, -1, -1);
  } else {
    return vec3(0.5, 0.5, 0.5);
  }
}

void main()
{
  // First, we determine the direction to fire the ray in
  vec3 rayDir = forward + up * screenPos.x + right * screenPos.y;

  // fragColour = texture(world, vec3(screenPos, 0));
  fragColour = texture(world, vec3((screenPos + vec2(1, 1))/2, 0));
  // fragColour = texelFetch(world, ivec3(0, 0, 0));
  // fragColour = vec4(screenPos, 0.5, 1);
  // vec4 texResult = texture(world, vec3(0.5, 0.5, 0.5));
  // fragColour = texResult;
  // if (texResult == vec4(0, 0, 0, 1)) {
  //   fragColour = vec4(0, 0, 0, 1);
  // } else {
  //   fragColour = skyColour;
  // }
  // fragColour = texResult;
  /* fragColour = skyColour;
     fragColour.x = texResult.x; */
  // fragColour = texture(world, vec3(screenPos, 0.5));
  /*
  vec4 texResult = texture(world, vec3(0.5, 0.5, 0.5));


  vec3 hit = raycast(rayDir);
  if (hit.x < 0) {
    // We didn't hit anything
    fragColour = skyColour;
  } else {
    // We hit something!
    fragColour = vec4(0, 0, 0, 1);
    } */
}
