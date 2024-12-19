#version 330

in vec2 texture_coord;

uniform sampler2D textureImage;
uniform ivec2 screenSize;

float threshold = 0.25;

layout(location = 0) out vec4 out_color;

void main() {
    vec2 aspectRatio;

    if (screenSize.x > screenSize.y) {
        aspectRatio = vec2(float(screenSize.x) / screenSize.y, 1.0);
    } else {
        aspectRatio = vec2(1.0, float(screenSize.y) / screenSize.x);
    }

    vec2 texture_coord = texture_coord * aspectRatio;

    vec2 texelSize = 1.0 / vec2(screenSize);


    vec2 offsets[9] = vec2[](
        vec2(-1, -1) * texelSize, vec2(0, -1) * texelSize, vec2(1, -1) * texelSize,
        vec2(-1,  0) * texelSize, vec2(0,  0) * texelSize, vec2(1,  0) * texelSize,
        vec2(-1,  1) * texelSize, vec2(0,  1) * texelSize, vec2(1,  1) * texelSize
    );

    float kernelX[9] = float[](
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    );

    float kernelY[9] = float[](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );

    float Gx = 0.0;
    float Gy = 0.0;

    for (int i = 0; i < 9; i++) {
        vec4 color = texture(textureImage, texture_coord + offsets[i]);
        float intensity = dot(color.rgb, vec3(0.21, 0.72, 0.07));
        Gx += kernelX[i] * intensity;
        Gy += kernelY[i] * intensity;
    }

    float magnitude = sqrt(Gx * Gx + Gy * Gy);

    if (magnitude > threshold) {
        out_color = vec4(0, 0, 0, 1);
    } else {
        out_color = vec4(1, 1, 1, 1);
    }
}
