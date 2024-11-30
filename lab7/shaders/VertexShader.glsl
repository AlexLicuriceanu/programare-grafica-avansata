#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
// TODO (student): Add the new attributes neccessary for skinning
layout(location = 3) in ivec4 v_bone_ids;
layout(location = 4) in vec4 v_bone_weights; 

const int MAX_BONES = 200;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
// TODO (student): Declare a new uniform variable array, which will
// receive the bone transformations
uniform mat4 Bones[MAX_BONES];


// Output
out vec2 texture_coord;
// TODO (student): Send the normal to the fragment shader
out vec3 frag_normal;

void main()
{
    // TODO (student): Compute the final bone transformation
    mat4 boneTransform =
        Bones[v_bone_ids.x] * v_bone_weights.x +
        Bones[v_bone_ids.y] * v_bone_weights.y +
        Bones[v_bone_ids.z] * v_bone_weights.z +
        Bones[v_bone_ids.w] * v_bone_weights.w;

    vec4 transformedPosition = boneTransform * vec4(v_position, 1.0);

    gl_Position = Projection * View * Model * transformedPosition;

    texture_coord = v_texture_coord;
    // TODO (student): Compute the normal

    // TODO (student): Apply the bone transformation on the vertex position
    frag_normal = normalize(mat3(Model) * mat3(boneTransform) * v_normal);

    //gl_Position = Projection * View * Model * vec4(v_position, 1.0);

}
