#version 330

// Input
in vec2 texture_coord;

// Uniform properties
uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_color;
uniform sampler2D texture_depth;
uniform sampler2D texture_light;

uniform int output_type;

// Output
layout(location = 0) out vec4 out_color;


vec3 depth()
{
    float t2 = pow(texture(texture_depth, texture_coord).x , 256);
    return vec3(t2, t2, t2);
}


vec3 color()
{
    return texture(texture_color, texture_coord).xyz;
}


vec3 world_normal()
{
    return texture(texture_normal, texture_coord).xyz;
}


vec3 world_position()
{
    return texture(texture_position, texture_coord).xyz;
}


vec3 light_accumulation()
{
    return texture(texture_light, texture_coord).xyz;
}


void main()
{

    vec4 wNorm = texture(texture_normal, texture_coord);

    // Check if this is a skybox pixel
    if (wNorm.a == 1.0) {
        out_color = vec4(color(), 1);
        return;
    }

    switch (output_type)
    {
    case 1:
        out_color = vec4(light_accumulation() * color(), 1);
        break;
    case 2:
        out_color = vec4(color(), 1);
        break;
    case 3:
        out_color = vec4(world_normal() * color(), 1);
        break;

    default:
        out_color = vec4(light_accumulation() * world_normal() * color(), 1);
    }
}
