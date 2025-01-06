#version 330 core
out vec4 FragColor;
  
in vec2 texCoord;
in vec4 color;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, texCoord);
    if (texColor.a < 0.1) 
    {
        discard;
    }

    vec4 mixedColor = mix(texColor, vec4(color.rgb, 1.0), color.a);

    FragColor = mixedColor;
}
