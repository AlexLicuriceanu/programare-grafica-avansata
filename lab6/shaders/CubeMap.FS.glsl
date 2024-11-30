#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform samplerCube texture_cubemap;
uniform int type;

uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;


vec3 myReflect()
{
    // TODO(student): Compute the reflection color value
    vec3 viewDir = normalize(world_position - camera_position);
    vec3 reflectDir = reflect(viewDir, normalize(world_normal));
    return texture(texture_cubemap, reflectDir).rgb;
}


vec3 myRefract(float refractive_index)
{
    // TODO(student): Compute the refraction color value
    vec3 viewDir = normalize(world_position - camera_position);
    vec3 refractDir = refract(viewDir, normalize(world_normal), 1/refractive_index);
    return texture(texture_cubemap, refractDir).rgb;

}


void main()
{

    if (type == 0)
    {
        out_color = vec4(myReflect(), 0);
    }
    else
    {
        out_color = vec4(myRefract(1.33), 0);
    }
}
