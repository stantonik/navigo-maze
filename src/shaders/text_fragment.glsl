#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D font_bitmap;

void main()
{
    vec4 texColor = texture(font_bitmap, TexCoord);
    FragColor = texColor;
}
