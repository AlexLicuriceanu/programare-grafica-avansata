#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 texCoord;

void main()
{
    texCoord = v_texCoord;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
