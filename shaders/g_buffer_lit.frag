#version 450

#include "utils.glsl"
#include "lighting.glsl"

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D in_albedo_roughness;
layout(binding = 1) uniform sampler2D in_normal_metal;
layout(binding = 2) uniform sampler2D in_depth;

layout(binding = 4) uniform samplerCube in_envmap;
layout(binding = 5) uniform sampler2D brdf_lut;
layout(binding = 6) uniform sampler2DShadow in_shadow;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform Data {
    FrameData frame;
};

layout(binding = 1) buffer PointLights {
    PointLight point_lights[];
};

void main() {
    vec4 albedo_roughness = texelFetch(in_albedo_roughness, ivec2(gl_FragCoord.xy), 0);
    vec4 normal_metalness = texelFetch(in_normal_metal, ivec2(gl_FragCoord.xy), 0);
    float depth = texelFetch(in_depth, ivec2(gl_FragCoord.xy), 0).r;

    vec3 acc = vec3(0.0);
    vec3 in_position = unproject(in_uv, depth, frame.camera.inv_view_proj);

    const vec3 to_view = (frame.camera.position - in_position);
    const vec3 view_dir = normalize(to_view);

    acc += eval_ibl(in_envmap, brdf_lut, 2 * normal_metalness.rgb - 1, view_dir, albedo_roughness.rgb, normal_metalness.a, albedo_roughness.a) * frame.ibl_intensity;
    {
        vec3 current_color = frame.sun_color * eval_brdf(2 * normal_metalness.rgb - 1, view_dir, frame.sun_dir, albedo_roughness.rgb, normal_metalness.a, albedo_roughness.a);

        float shadow_coeff = get_shadow_coefficient(in_position, in_shadow, frame.shadow_camera.view_proj);
        current_color *= shadow_coeff;// avoid black shadows
        acc += current_color;
        //
        //        for(uint i = 0; i != frame.point_light_count; ++i) {
        //            PointLight light = point_lights[i];
        //            const vec3 to_light = (light.position - in_position);
        //            const float dist = length(to_light);
        //            const vec3 light_vec = to_light / dist;
        //
        //            const float att = attenuation(dist, light.radius);
        //            if(att <= 0.0f) {
        //                continue;
        //            }
        //
        //            acc += eval_brdf(normal, view_dir, light_vec, base_color, metallic, roughness) * att * light.color;
        //        }
    }


    out_color = vec4(acc, 1.0);
}
