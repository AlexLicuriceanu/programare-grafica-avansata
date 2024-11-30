#version 330

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
layout(triangle_strip, max_vertices = 256) out;

// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
// TODO(student): Declare any other uniforms here
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;

// Input
in int instance[2];


vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z *sin(u);
    float z = point.x * sin(u) + point.z *cos(u);
    return vec3(x, point.y, z);
}


vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}


// This function computes B(t) with 4 control points. For a visual example,
// see [1]. For an interactive Javascript example with the exact points in
// this code, see [2].
//
// [1] https://www.desmos.com/calculator/cahqdxeshd
// [2] https://jsfiddle.net/6yuv9htf/
vec3 bezier(float t)
{
    return  control_p0 * pow((1 - t), 3) +
            control_p1 * 3 * t * pow((1 - t), 2) +
            control_p2 * 3 * pow(t, 2) * (1 - t) +
            control_p3 * pow(t, 3);
}


// TODO(student): If you want to take things a step further, try drawing a
// Hermite spline. Hint: you can repurpose two of the control points. For a
// visual example, see [1]. For an interactive Javascript example with the
// exact points in this code, see [2].
// 
// Unlike the Javascript function, you MUST NOT call the Bezier function.
// There is another way to draw a Hermite spline, all you need is to find
// the formula.
//
// [1] https://www.desmos.com/calculator/5knm5tkr8m
// [2] https://jsfiddle.net/6yuv9htf/

void main()
{
    const int SURFACE_TYPE_ROTATION     = 0;
    const int SURFACE_TYPE_TRANSLATION  = 1;
    const int SURFACE_TYPE = SURFACE_TYPE_ROTATION;

    if (instance[0] < no_of_instances - 1)
    {
        for (int i = 0; i < no_of_generated_points - 1; i++)
        {
            float t1 = float(i) / float(no_of_generated_points - 1);
            float t2 = float(i + 1) / float(no_of_generated_points - 1);

            vec3 point_row1_a = bezier(t1);
            vec3 point_row1_b = bezier(t2);

            vec3 point_row2_a = bezier(t1);
            vec3 point_row2_b = bezier(t2);

            if (SURFACE_TYPE == SURFACE_TYPE_ROTATION)
            {
                point_row1_a = rotateY(point_row1_a, max_rotate * instance[0] / (no_of_instances - 1));
                point_row1_b = rotateY(point_row1_b, max_rotate * instance[0] / (no_of_instances - 1));
                point_row2_a = rotateY(point_row2_a, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = rotateY(point_row2_b, max_rotate * (instance[0] + 1) / (no_of_instances - 1));
            }
            else if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION)
            {
                point_row1_a = translateX(point_row1_a, max_translate * instance[0] / (no_of_instances - 1));
                point_row1_b = translateX(point_row1_b, max_translate * instance[0] / (no_of_instances - 1));
                point_row2_a = translateX(point_row2_a, max_translate * (instance[0] + 1) / (no_of_instances - 1));
                point_row2_b = translateX(point_row2_b, max_translate * (instance[0] + 1) / (no_of_instances - 1));
            }

            
            gl_Position = Projection * View * vec4(point_row1_a, 1);
            EmitVertex();

            gl_Position = Projection * View * vec4(point_row2_a, 1);
            EmitVertex();

            gl_Position = Projection * View * vec4(point_row1_b, 1);
            EmitVertex();

            gl_Position = Projection * View * vec4(point_row2_b, 1);
            EmitVertex();

            EndPrimitive();
        }
    }
}
