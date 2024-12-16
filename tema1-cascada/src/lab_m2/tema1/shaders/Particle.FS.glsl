#version 430

// Input
layout(location = 0) in vec2 texture_coord;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    vec4 texColor = texture(texture_1, texture_coord);

    if (texColor.a > 0.9)
    {
        discard;
    }

    // Output the final color
    out_color = texColor;
    out_color[3] = 1.0;
}
