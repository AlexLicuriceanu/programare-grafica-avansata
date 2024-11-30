#version 330

// Input
in vec2 texture_coord;

// Uniform properties
uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;
uniform int outputMode = 2; // 0: original, 1: grayscale, 2: blur

// Output
layout(location = 0) out vec4 out_color;

// Local variables
vec2 textureCoord = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y); // Flip texture


vec4 grayscale()
{
    vec4 color = texture(textureImage, textureCoord);
    float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b; 
    return vec4(gray, gray, gray,  0);
}


vec4 blur(int blurRadius)
{
    vec2 texelSize = 1.0f / screenSize;
    vec4 sum = vec4(0);
    for(int i = -blurRadius; i <= blurRadius; i++)
    {
        for(int j = -blurRadius; j <= blurRadius; j++)
        {
            sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
        }
    }
        
    float samples = pow((2 * blurRadius + 1), 2);
    return sum / samples;
}

vec4 median()
{
    vec2 texelSize = 1.0f / screenSize;
    const int kernelSize = 7; // 3x3 kernel for simplicity
    const int numSamples = kernelSize * kernelSize;

    vec4 samples[numSamples];
    int index = 0;

    // Collect pixel intensities in the neighborhood
    for (int i = -kernelSize / 2; i <= kernelSize / 2; i++)
    {
        for (int j = -kernelSize / 2; j <= kernelSize / 2; j++)
        {
            samples[index++] = texture(textureImage, textureCoord + vec2(i, j) * texelSize);
        }
    }

    // Sort the samples by luminance
    for (int i = 0; i < numSamples - 1; i++)
    {
        for (int j = i + 1; j < numSamples; j++)
        {
            float luminanceI = 0.21 * samples[i].r + 0.71 * samples[i].g + 0.07 * samples[i].b;
            float luminanceJ = 0.21 * samples[j].r + 0.71 * samples[j].g + 0.07 * samples[j].b;

            if (luminanceI > luminanceJ)
            {
                vec4 temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }

    // Return the median value (middle sample)
    return samples[numSamples / 2];
}

void main()
{
    switch (outputMode)
    {
        case 1:
        {
            out_color = grayscale();
            break;
        }

        case 2:
        {
            out_color = blur(3);
            break;
        }

        case 3:
		{
			out_color = median();
			break;
		}

        default:
            out_color = texture(textureImage, textureCoord);
            break;
    }
}
