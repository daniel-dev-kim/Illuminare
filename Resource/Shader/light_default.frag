#version 460 core

out vec4 FragColor;

uniform vec3 albedo = vec3(1.0); // 머터리얼 색상

void main() {
    FragColor = vec4(albedo, 1.0); // 그대로 출력
}