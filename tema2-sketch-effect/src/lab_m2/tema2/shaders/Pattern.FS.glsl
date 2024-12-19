#version 330

in vec2 texture_coord;

uniform sampler2D textureImage;
uniform float c;
uniform vec2 direction;
uniform float intensityThreshold;
uniform int pattern;
uniform ivec2 screenSize;

layout(location = 0) out vec4 out_color;

void main() {

    vec2 aspectRatio;

    if (screenSize.x > screenSize.y) {
        aspectRatio = vec2(float(screenSize.x) / screenSize.y, 1.0);
    } else {
        aspectRatio = vec2(1.0, float(screenSize.y) / screenSize.x);
    }

    vec2 texture_coord = texture_coord * aspectRatio;


    vec4 texColor = texture(textureImage, texture_coord);
    float intensity = dot(texColor.rgb, vec3(0.21, 0.72, 0.07));

    float linePattern = sin(dot(texture_coord * 100.0, direction));

    if (pattern == 0) {
        if (intensity < intensityThreshold) {

            if (linePattern > c) {
                out_color = vec4(1.0, 1.0, 1.0, 1.0); // White lines
            }
            else {
                out_color = vec4(0.0, 0.0, 0.0, 1.0); // Black background
            }

        }
        else {
            out_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }


    else if (pattern == 1) {

        if (intensity < intensityThreshold && linePattern > c) {
            out_color = vec4(0.0, 0.0, 0.0, 1.0);

        }
        else {
            out_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }
    
}
