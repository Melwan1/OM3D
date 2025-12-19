#version 450

#include "utils.glsl"
#include "lighting.glsl"

// fragment shader of the main lighting pass

// #define DEBUG_NORMAL
// #define DEBUG_METAL
// #define DEBUG_ROUGH
// #define DEBUG_ENV

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_position;
layout(location = 4) in vec3 in_tangent;
layout(location = 5) in vec3 in_bitangent;

layout(binding = 0) uniform sampler2D in_texture;
layout(binding = 1) uniform sampler2D in_normal_texture;
layout(binding = 2) uniform sampler2D in_metal_rough;
layout(binding = 3) uniform sampler2D in_emissive;

layout(binding = 4) uniform sampler2D in_albedo_roughness;
layout(binding = 5) uniform sampler2D in_normal_metal;
layout(binding = 6) uniform sampler2D in_depth;


uniform vec3 base_color_factor;
uniform vec2 metal_rough_factor;
uniform vec3 emissive_factor;
uniform float alpha_cutoff;
uniform uint light_id;

layout(binding = 4) uniform samplerCube in_envmap;
layout(binding = 5) uniform sampler2D brdf_lut;
layout(binding = 6) uniform sampler2DShadow in_shadow;

layout(binding = 0) uniform Data {
    FrameData frame;
};

layout(binding = 1) buffer PointLights {
    PointLight point_lights[];
};

void main() {
    const vec3 normal_map = unpack_normal_map(texture(in_normal_texture, in_uv).xy);
    const vec3 normal = normal_map.x * in_tangent +
    normal_map.y * in_bitangent +
    normal_map.z * in_normal;

    vec4 albedo_roughness = texelFetch(in_albedo_roughness, ivec2(gl_FragCoord.xy), 0);
    vec4 normal_metalness = texelFetch(in_normal_metal, ivec2(gl_FragCoord.xy), 0);
    float depth = texelFetch(in_depth, ivec2(gl_FragCoord.xy), 0).r;

    const vec3 to_view = (frame.camera.position - in_position);
    const vec3 view_dir = normalize(to_view);

    vec3 acc = vec3(0.0);

    for (uint i = 0; i != frame.point_light_count; ++i) {
        PointLight light = point_lights[i];
        const vec3 to_light = (light.position - in_position);
        const float dist = length(to_light);
        const vec3 light_vec = to_light / dist;

        const float att = attenuation(dist, light.radius);
        if (att <= 0.0f) {
            continue;
        }

        acc += eval_brdf(normal_metalness.rgb, view_dir, light_vec, albedo_roughness.rgb, normal_metalness.a, albedo_roughness.a) * att * light.color;
    }

//        out_color = vec4(acc, 0.5);
    out_color = vec4(0.1, 0.1, 0.0, 0.1);
}