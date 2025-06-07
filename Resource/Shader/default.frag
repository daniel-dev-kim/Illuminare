#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 viewPos;

uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;

uniform vec3 albedoColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 emissiveColor;

uniform float roughness;
uniform float opacity;

uniform int hasAlbedoMap;
uniform int hasNormalMap;
uniform int hasRoughnessMap;

uniform sampler2D shadowMap;
uniform mat4 lightViewProj;
void main()
{
    // Normal Mapping
    vec3 norm = normalize(Normal);
    if (hasNormalMap == 1) {
        vec3 sampledNormal = texture(normalMap, TexCoord).rgb;
        sampledNormal = normalize(sampledNormal * 2.0 - 1.0); // [0,1] -> [-1,1]
        norm = normalize(sampledNormal);
    }

    // Albedo Mapping
    vec3 albedo = albedoColor;
    if (hasAlbedoMap == 1) {
        vec3 texColor = texture(albedoMap, TexCoord).rgb;
        albedo = texColor;
    }

    // Roughness Mapping
    float finalRoughness = roughness;
    if (hasRoughnessMap == 1) {
        float sampledRoughness = texture(roughnessMap, TexCoord).r;
        finalRoughness = sampledRoughness;
    }

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * ambientColor * albedo;

    // Diffuse
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * albedo;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), mix(4.0, 64.0, 1.0 - finalRoughness));
    vec3 specular = spec * specularColor;

    // Emissive
    vec3 emissive = emissiveColor;

      // 변환된 좌표 (0~1 범위로 매핑)
    vec4 fragPosLightSpace = lightViewProj * vec4(FragPos, 1.0);
    fragPosLightSpace /= fragPosLightSpace.w;
    vec3 projCoords = fragPosLightSpace.xyz * 0.5 + 0.5;

    // Shadow calculation
    float shadow = 0.0;

    // UV가 0~1 안에 있을 때만 그림자 계산
    if (projCoords.x >= 0.0 && projCoords.x <= 1.0 &&
        projCoords.y >= 0.0 && projCoords.y <= 1.0)
    {
        float currentDepth = projCoords.z;
        float closestDepth = texture(shadowMap, projCoords.xy).r;

        // Adaptive bias: normal과 lightDir을 이용
        float bias = 0.001;

        // 그림자 여부
        shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    }

    // 결과 합성
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular) + emissive;
    FragColor = vec4(result, opacity);
}