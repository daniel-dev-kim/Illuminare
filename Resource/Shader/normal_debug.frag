#version 460 core

in vec3 Normal;
out vec4 FragColor;

void main()
{
    // Normal은 [-1, 1] 범위이므로, [0, 1]로 변환
    vec3 color = normalize(Normal) * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}