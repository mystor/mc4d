#version 410

const vec4 skyColour = vec4(0.3, 0.5, 1, 1);
const vec4 worldSize = ivec4(128, 64, 128, 0);
const int rtMaxDist = 128;

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

  return false;
}

bool gipHandler(vec4 point, vec4 normal)
{
  // Right now, we are dealing with 3d volumes,
  // so we're ignoring the 4th element of the vector
  // TODO(michael): Actually use point.w

  // Are we looking at something outside of the volume? Nothing here...
  /* if (any(lessThan(point, vec4(0, 0, 0, 0)))) {
    // if (point < vec4(0, 0, 0, 0)) {
    // if (point.x < 0 || point.y < 0 || point.z < 0) {
    return false;
  }
  if (any(greaterThan(point, worldSize))) {
    // if (point > worldSize) {
    // if (point.x > worldSize.x || point.y > worldSize.y || point.z > worldSize.z) {
    return false;
  } */

  // Look up the point in the texture
  vec3 texPoint = point.xyz / worldSize.xyz;
  float data = texture(world, texPoint).r;
  if (data < 0.2) { // Arbitrary block cutoff
    // We are looking at a block! Let's change the fragment colour
    fragColour = vec4(abs(normal.xyz) * 0.5, 1);
    return true;
  }

  return false;
}

void gip(vec4 start, vec4 dir)
{
  // TODO(michael):
  // This is being used instead of sign() because sign(0) = 0, which means that
  // we could get a NaN for a deltaInDirs argument, which will totally bugger everything up
  vec4 dirSigns = vec4(dir.x < 0 ? -1 : 1,
                       dir.y < 0 ? -1 : 1,
                       dir.z < 0 ? -1 : 1,
                       dir.w < 0 ? -1 : 1);
  // vec4 dirSigns = sign(dir);

  vec4 pos = start;

  for (int i=0; i<rtMaxDist; i++) {
    // int remaining = rtMaxDist;
  // while (remaining-- > 0) { // Clamp the max # of things to look up (max view distance)
    vec4 posInt = floor(pos); // Round the coordinate down to the closest block

    // The value gotten by moving to one block further in each dimension
    vec4 targets = posInt + dirSigns;
    // The distance which must be traveled in each direction to get this
    vec4 deltas = targets - pos;
    // How many times dir must be traveled to cover this distance
    vec4 deltaInDirs = deltas / dir;

    // TODO(michael): Make this less terrible
    // Choose the smallest one
    float minDeltaInDirs = min(min(deltaInDirs.x, deltaInDirs.y),
                               min(deltaInDirs.z, deltaInDirs.w));


    // A small fudge to make sure that the next block is actually entred
    // in case floating point errors cause the same block to be tested repeatedly
    // minDeltaInDirs += 1.0/16.0;

    // Move along minDeltaInDirs!
    pos += minDeltaInDirs * dir;

    vec4 newPosInt = floor(pos); // Round the coordinate down to the closest block
    vec4 normal = newPosInt - posInt;
    if (gipHandler(newPosInt, normal)) { // Check if the block should count as a hit
      break;
    }
  }
}

void main()
{
  // First, we determine the direction to fire the ray in
  vec3 rayDir = normalize(forward + (up * screenPos.x) + (right * screenPos.y));

  // Default the fragment to the sky colour
  fragColour = skyColour;

  // Send out the ray, and calculate the color which would be recieved
  gip(vec4(eye, 0), vec4(rayDir, 0));
}
