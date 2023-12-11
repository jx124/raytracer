#version 450 core

in vec2 TexCoord;

uniform sampler2D ImageTexture;

out vec4 FragColor;

void main()
{
    const float gamma = 1.0;
    vec4 color = texture(ImageTexture, TexCoord);

    const vec4 gammaCorrection = vec4(1.0 / gamma);
    FragColor = pow(color, gammaCorrection);
}