#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D font_bitmap;
uniform vec3 textColor;

void main()
{
    vec4 texColor = texture(font_bitmap, TexCoord);

    if (texColor.r < 0.1)
    {
        discard;
    }

    FragColor = vec4(textColor, 1.0);;
}
