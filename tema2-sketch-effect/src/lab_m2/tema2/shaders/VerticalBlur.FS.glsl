#version 330

in vec2 texture_coord;

uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int blurRadius = 12;

float sigma = 4.0;

layout(location = 0) out vec4 out_color;

float gaussian(float x, float sigma) {
    return exp(-0.5 * (x * x) / (sigma * sigma)) / (sigma * sqrt(6.28));
}

void main() {

    vec2 aspectRatio;

    if (screenSize.x > screenSize.y) {
        aspectRatio = vec2(float(screenSize.x) / screenSize.y, 1.0);
    } else {
        aspectRatio = vec2(1.0, float(screenSize.y) / screenSize.x);
    }

    vec2 texture_coord = texture_coord * aspectRatio;

    vec2 texelSize = 1.0 / vec2(screenSize);
    vec4 sum = vec4(0);
    float weightSum = 0.0;

    for (int i = -blurRadius; i <= blurRadius; i++) {
        float weight = gaussian(float(i), sigma);
        vec2 offset = vec2(0, i) * texelSize;
        sum += texture(textureImage, texture_coord + offset) * weight;
        weightSum += weight;
    }

    out_color = sum / weightSum;
}
