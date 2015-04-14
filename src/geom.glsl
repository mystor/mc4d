#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// the 3d eye position
uniform vec3 eyePos3;

in vec4 vcolor[];
out vec4 color;

in vec3 vpos3[];
out vec3 pos3;

out vec3 normal;

void main()
{
  // Calculate a normal
  vec3 a = vpos3[1] - vpos3[0];
  vec3 b = vpos3[2] - vpos3[0];
  normal = normalize(cross(a, b));

  // Ensure it is pointing toward the camera
  if (dot(normal, pos3 - eyePos3) > 0) {
    normal = -normal;
  }

  for(int i = 0; i < gl_in.length(); i++)
  {
     // copy attributes
    gl_Position = gl_in[i].gl_Position;
    color = vcolor[i];
    pos3 = vpos3[i];

    // done with the vertex
    EmitVertex();
  }

  EndPrimitive();
}
