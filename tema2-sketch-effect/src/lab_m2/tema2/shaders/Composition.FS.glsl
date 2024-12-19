#version 330 core

in vec2 texture_coord;

uniform sampler2D pattern1Texture;
uniform sampler2D pattern2Texture;
uniform sampler2D pattern3Texture;
uniform ivec2 screenSize;
uniform int patternCount = 3;

layout(location = 0) out vec4 out_color;

void main() {
    vec2 aspectRatio;

    if (screenSize.x > screenSize.y) {
        aspectRatio = vec2(float(screenSize.x) / screenSize.y, 1.0);
    } else {
        aspectRatio = vec2(1.0, float(screenSize.y) / screenSize.x);
    }

    vec2 texture_coord = texture_coord * aspectRatio;

    float pattern1 = texture(pattern1Texture, texture_coord).r;
    float pattern2 = texture(pattern2Texture, texture_coord).r;

    if (patternCount == 3) {
        float pattern3 = texture(pattern3Texture, texture_coord).r;

        float combinedPattern = min(min(pattern1, pattern2), pattern3);

        out_color = vec4(vec3(combinedPattern), 1.0);
    }
    else if (patternCount == 2) {
        float combinedPattern = min(pattern1, pattern2);

        out_color = vec4(vec3(combinedPattern), 1.0);
    }
}
