#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;

    vec4 ambientLightColor;
    vec4 lightPosition;
    vec4 lightColor;
} ubo;

void main() {
    vec3 normal = normalize(fragNormalWorld);

    vec3 lightOffset = ubo.lightPosition.xyz - fragPositionWorld;

    float distanceSquared = max(dot(lightOffset, lightOffset), 0.001);

    vec3 directionToLight = normalize(lightOffset);

    float diffuseStrength = max(dot(normal, directionToLight), 0.0);

    vec3 ambientLight = ubo.ambientLightColor.rgb * ubo.ambientLightColor.a;

    vec3 pointLight =
        ubo.lightColor.rgb *
        ubo.lightColor.a *
        diffuseStrength /
        distanceSquared;

    vec3 finalColor = (ambientLight + pointLight) * fragColor;

    outColor = vec4(finalColor, 1.0);
}