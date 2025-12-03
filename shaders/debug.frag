#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D in_albedo_roughness;
layout(binding = 1) uniform sampler2D in_normal_metal;
layout(binding = 2) uniform sampler2D in_depth;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(vec3(0.0), 1.0);

    #ifdef DEBUG_ALBEDO
    vec3 albedo = texture(in_albedo_roughness, in_uv).rgb;
    out_color = vec4(albedo, 1.0);
    #endif

    #ifdef DEBUG_ROUGHNESS
    float roughness = texture(in_albedo_roughness, in_uv).a;
    roughness = pow(roughness, 0.35);
    out_color = vec4(vec3(roughness), 1.0);
    #endif

    #ifdef DEBUG_NORMAL
    vec3 normal = texture(in_normal_metal, in_uv).rgb;
    out_color = vec4(normal * 0.5 + 0.5, 1.0);
    #endif

    #ifdef DEBUG_METALNESS
    float metallic = texture(in_normal_metal, in_uv).a;
    metallic = pow(metallic, 0.35);
    out_color = vec4(vec3(metallic), 1.0);
    #endif

    #ifdef DEBUG_DEPTH
    float depth = texture(in_depth, in_uv).r;
    depth = pow(depth, 0.35);
    out_color = vec4(vec3(depth), 1.0);
    #endif
}
