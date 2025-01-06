#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aModel;
layout (location = 5) in vec4 aColor;
layout (location = 6) in vec2 aTexOffset;
layout (location = 7) in vec2 aTexScale;

// precompute these matrices
uniform mat4 projection;
uniform mat4 view;

out vec2 texCoord;
out vec4 color;

void main()
{
    gl_Position = projection * view * aModel * vec4(aPos, 1.0);
    texCoord = aPos.xy * aTexScale + aTexOffset;
    color = aColor;
}
