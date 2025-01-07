#version 330 core
out vec4 FragColor;
  
in vec2 texCoord;
in vec4 color;

uniform sampler2D texture1;
uniform float u_GrayscaleAmount;

void main()
{
    vec4 texColor = texture(texture1, texCoord);
    if (texColor.a < 0.1) 
    {
        discard;
    }

    vec4 mixedColor = mix(texColor, vec4(color.rgb, 1.0), color.a);

    float gray = dot(mixedColor.rgb, vec3(0.299, 0.587, 0.114));
    vec3 grayscaleColor = vec3(gray);
    vec3 blendedColor = mix(texColor.rgb, grayscaleColor, u_GrayscaleAmount);
    FragColor = vec4(blendedColor, texColor.a);
}
