#version 460 core

// outputs
layout ( location = 0 ) out vec4 FragColor;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;

in vec3 world_normal;
in vec3 frag_color;
in vec3 camera_pos;
in vec4 world_position;

uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_2;

vec2 to_spherical(vec3 input_vector){

    float theta = acos(input_vector.z);
    float phi = atan(input_vector.y, input_vector.x);
    if (input_vector.y < 0)
        phi = phi + PI * 2;

    theta = theta / PI;
    phi = phi / (2 * PI);
    return vec2(phi, theta);
}

void main( void )
{
    vec2 irradiance_coords = to_spherical(world_normal);
    vec4 irradiance_color = texture(texture_0, irradiance_coords);
    vec4 diffuse_color = irradiance_color  * vec4(frag_color / PI, 1.0f);

    float roughness = 0.5;

    vec3 world_pos_3 = vec3(world_position.xyz / world_position.w);
    vec3 omega_o = normalize(camera_pos - world_pos_3);
    vec3 omega_r = normalize(reflect(-omega_o, world_normal));
    vec2 ref_uv_coords = to_spherical(omega_r);
    vec3 prefiltered_color = textureLod(texture_2, ref_uv_coords, roughness * MAX_REFLECTION_LOD).rgb;

    float cos_theta_o = max(dot(world_normal, omega_o), 0.0);
    vec2 brdf_coords = vec2(cos_theta_o, roughness);
    vec2 brdf = texture(texture_1, brdf_coords).rg;

    vec3 F = vec3(0.2, 0.2, 0.2);
    vec3 specular_color = prefiltered_color * (F * brdf.x + brdf.y);

    FragColor = diffuse_color + vec4(specular_color, 1.0);
}