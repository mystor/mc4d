#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

                  // in vec3 pos3Space[];

out vec3 normal;

void main()
{
  // Calculate a normal
  vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  // vec3 a = pos3Space[1] - pos3Space[0];
  // vec3 b = pos3Space[2] - pos3Space[0];
  normal = normalize(cross(a, b));

  for(int i = 0; i < gl_in.length(); i++)
  {
     // copy attributes
    gl_Position = gl_in[i].gl_Position;

    // done with the vertex
    EmitVertex();
  }

  EndPrimitive();
}
