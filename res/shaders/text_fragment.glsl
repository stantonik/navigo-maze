#version 330 core

out vec4 FragColor;
  
in vec2 texCoord;
in vec4 color;

uniform sampler2D font_bitmap;

void main()
{
    vec4 texColor = texture(font_bitmap, texCoord);

    if (texColor.r < 0.1)
    {
        discard;
    }

    FragColor = color;
}
