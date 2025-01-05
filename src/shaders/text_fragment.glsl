#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D font_bitmap;
uniform vec3 textColor;

void main() {
    vec4 sampled = texture(font_bitmap, TexCoord);
    // FragColor = vec4(textColor, 1.0) * sampled;
    FragColor = sampled;
}
