#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform vec3 generator_position;
uniform float deltaTime;

uniform vec3 P0; // Control point 0
uniform vec3 P1; // Control point 1
uniform vec3 P2; // Control point 2
uniform vec3 P3; // Control point 3

out float vert_lifetime;
out float vert_iLifetime;

out int show_particle;

struct Particle
{
    vec4 position;
    vec4 speed;
    vec4 iposition;
    vec4 ispeed;
    float delay;
    float iDelay;
    float lifetime;
    float iLifetime;
};

layout(std430, binding = 0) buffer particles {
    Particle data[];
};

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 EvaluateBezier(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3) {
    float u = 1.0 - t;
    return 
        u * u * u * P0 +
        3.0 * u * u * t * P1 +
        3.0 * u * t * t * P2 +
        t * t * t * P3;
}

void main()
{

    vec3 pos = data[gl_VertexID].position.xyz;
    vec3 spd = data[gl_VertexID].speed.xyz;
    vec3 iposition = data[gl_VertexID].iposition.xyz;
    float delay = data[gl_VertexID].delay;

    delay -= deltaTime;

    if (delay > 0) {
        data[gl_VertexID].delay = delay;
        gl_Position = Model * vec4(iposition + generator_position + vec3(0, -1000, 0), 1);
        return;
    }

    float lifetime = data[gl_VertexID].lifetime;
    float iLifetime = data[gl_VertexID].iLifetime;
    float t = 1.0 - (lifetime / iLifetime);

    vec3 bezierPosition = EvaluateBezier(t, P3, P2, P1, P0);

    if (pos == iposition) {
        show_particle = 0;
    }
    else {
        show_particle = 1;
    }

    if (lifetime == iLifetime) {
        show_particle = 0;
	}
    else {
        show_particle = 1;
	}

    pos = bezierPosition + iposition;
    lifetime -= 0.002;

    if (lifetime <= 0.0) {
        lifetime = iLifetime;
        delay = data[gl_VertexID].iDelay;
        pos = data[gl_VertexID].iposition.xyz;
       
    }

    if (pos.y < 3) {
        data[gl_VertexID].position.xyz = iposition;
        data[gl_VertexID].iposition.xyz = iposition;
        data[gl_VertexID].lifetime = data[gl_VertexID].iLifetime;
        data[gl_VertexID].delay =  data[gl_VertexID].iDelay;
    }

    data[gl_VertexID].position.xyz = pos;
    data[gl_VertexID].iposition.xyz = iposition;
    data[gl_VertexID].speed.xyz = spd;
    data[gl_VertexID].lifetime = lifetime;
    data[gl_VertexID].delay = delay;

    gl_Position = Model * vec4(pos, 1);
}

