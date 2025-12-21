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
layout(location = 6) in vec2 in_screen_uv;

layout(binding = 0) uniform sampler2D in_texture;
layout(binding = 1) uniform sampler2D in_normal_texture;
layout(binding = 2) uniform sampler2D in_metal_rough;
layout(binding = 3) uniform sampler2D in_emissive;

layout(binding = 4) uniform samplerCube in_envmap;
layout(binding = 5) uniform sampler2D brdf_lut;
layout(binding = 6) uniform sampler2DShadow in_shadow;

layout(binding = 7) uniform sampler2D in_albedo_roughness;
layout(binding = 8) uniform sampler2D in_normal_metal;
layout(binding = 9) uniform sampler2D in_depth;

uniform vec3 base_color_factor;
uniform vec2 metal_rough_factor;
uniform vec3 emissive_factor;
uniform float alpha_cutoff;
uniform uint light_id;

layout(binding = 0) uniform Data {
    FrameData frame;
};

layout(binding = 1) buffer PointLights {
    PointLight point_lights[];
};


#define COLOR_NUMBER 8
vec3[COLOR_NUMBER] color_map = { vec3(1.0, 0.0, 0.0) /* Red */, vec3(1.0, 0.5, 0.0) /* Orange */, vec3(1.0, 1.0, 0.0) /* Yellow */,
vec3(0.0, 1.0, 0.0) /* Green */, vec3(0.0, 1.0, 1.0) /* Cyan */, vec3(0.0, 0.0, 1.0) /* Blue */,
vec3(1.0, 0.0, 1.0) /* Magenta */, vec3(1.0, 1.0, 1.0) /* White */,
};

void main() {
    const vec3 normal_map = unpack_normal_map(texture(in_normal_texture, in_uv).xy);
    const vec3 normal = normal_map.x * in_tangent +
    normal_map.y * in_bitangent +
    normal_map.z * in_normal;

    ivec2 pixel_coord = ivec2(gl_FragCoord.xy);
    vec4 albedo_roughness = texelFetch(in_albedo_roughness, pixel_coord, 0);
    vec4 normal_metalness = texelFetch(in_normal_metal, pixel_coord, 0);
    float depth           = texelFetch(in_depth, pixel_coord, 0).r;

    vec3 position = unproject(in_screen_uv, depth, frame.camera.inv_view_proj);

    const vec3 to_view = (frame.camera.position - position);
    const vec3 view_dir = normalize(to_view);

    PointLight light = point_lights[light_id];
    const vec3 to_light = (light.position - position);
    const float dist = length(to_light);
    const vec3 light_vec = to_light / dist;

    const float att = attenuation(dist, light.radius);
    if (att <= 0.0f) {
        discard;
    }

    vec3 acc = eval_brdf((normal_metalness.rgb - 0.5) * 2.0, view_dir, light_vec, albedo_roughness.rgb, normal_metalness.a, albedo_roughness.a) * att * light.color;

    #ifdef DEBUG_COLORMAP
    // visualize the lights area with distinct colors
    out_color = vec4(color_map[light_id % COLOR_NUMBER] / 5, 0.0);
    #else
    out_color = vec4(acc, 0.0);
    #endif
}