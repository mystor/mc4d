#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// The location of the eye in space
uniform vec4 eye;
                  // in vec3 pos3Space[];

in vec4 vcolor[];
out vec4 color;

out vec3 normal;

void main()
{
  // Calculate a normal
  vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  // vec3 a = pos3Space[1] - pos3Space[0];
  // vec3 b = pos3Space[2] - pos3Space[0];
  normal = normalize(cross(a, b));

  if (vcolor[0].w > 0 && vcolor[1].w > 0 && vcolor[2].w > 0) {
    return;
  }

  for(int i = 0; i < gl_in.length(); i++)
  {
    if (isnan(gl_in[i].gl_Position.x) ||
        isnan(gl_in[i].gl_Position.y) ||
        isnan(gl_in[i].gl_Position.z) ||
        isnan(gl_in[i].gl_Position.w) ||
        isinf(gl_in[i].gl_Position.x) ||
        isinf(gl_in[i].gl_Position.y) ||
        isinf(gl_in[i].gl_Position.z) ||
        isinf(gl_in[i].gl_Position.w)) {
      return;
    }
  }


  for(int i = 0; i < gl_in.length(); i++)
  {
     // copy attributes
    gl_Position = gl_in[i].gl_Position;
    color = vcolor[i];

    // done with the vertex
    EmitVertex();
  }

  EndPrimitive();
}
