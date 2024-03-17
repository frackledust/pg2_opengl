#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require 

// outputs
layout ( location = 0 ) out vec4 FragColor;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;

in vec3 world_normal;
in vec4 world_position;
in vec3 camera_pos;

in vec3 frag_color;
in vec2 texcoord;
in vec3 tangent;

flat in int material_index;

struct Material
{
vec3 diffuse; // (1,1,1) or albedo
uvec2 tex_diffuse; // albedo texture
vec3 rma; // (1,1,1) or (roughness, metalness, 1)
uvec2 tex_rma; // rma texture - roughness, metalness, ambient occlusion
vec3 normal; // (1,1,1) or (0,0,1)
uvec2 tex_normal; // bump texture
};
layout ( std430, binding = 0 ) readonly buffer Materials
{
Material materials[];  //only the last member can be unsized array
};


uniform sampler2D texture_0;
uniform sampler2D texture_1;
uniform sampler2D texture_2;

vec2 to_spherical(vec3 input_vector){

    float theta = acos(input_vector.z);
    float phi = atan(input_vector.y, input_vector.x);
    if (input_vector.y < 0)

    theta = theta / PI;
    phi = phi / (2 * PI);
    return vec2(phi, theta);
}

void main( void )
{
    // irradiance
    vec2 irradiance_coords = to_spherical(world_normal);
    vec4 irradiance_color = texture(texture_0, irradiance_coords);
    vec4 diffuse_color = irradiance_color  * vec4(frag_color / PI, 1.0f);

    Material material = materials[material_index];
    
    // diffuse from material
    uvec2 md_texture_id = materials[material_index].tex_diffuse;
    vec3 m_diffuse_texture = texture( sampler2D(md_texture_id), texcoord ).rgb;
    vec4 m_diffuse = vec4(material.diffuse.rgb * m_diffuse_texture, 1.0f);
    m_diffuse = m_diffuse * diffuse_color;

    // normal from material
    uvec2 md_normal_id = materials[material_index].tex_normal;
	vec3 m_normal_texture = texture( sampler2D(md_normal_id), texcoord ).rgb;
    vec4 m_normal = vec4(material.normal * m_normal_texture, 1.0f);

    // rma from material
    uvec2 md_rma_id = materials[material_index].tex_rma;
    vec3 m_rma_texture = texture( sampler2D(md_rma_id), texcoord ).rgb;
    vec4 m_rma = vec4(material.rma * m_rma_texture, 1.0f);


    // ------------------------------------------
    float roughness = m_rma.x;
    float metalness = m_rma.y;
    float ambient_occlusion = m_rma.z;

    vec3 world_pos_3 = vec3(world_position.xyz / world_position.w);
    vec3 omega_o = normalize(camera_pos - world_pos_3);
    vec3 omega_r = normalize(reflect(-omega_o, world_normal));
    vec2 ref_uv_coords = to_spherical(omega_r);
    vec3 prefiltered_color = textureLod(texture_2, ref_uv_coords, roughness * MAX_REFLECTION_LOD).rgb;

    //vec3 F = vec3(0.2, 0.2, 0.2);
    vec3 F0 = mix(vec3(0.04), m_diffuse.rgb, metalness);
    // TODO: omega h?
    vec3 omega_h = world_normal.xyz;
    float theta_h = max(dot(omega_h, omega_o), 0.0);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - theta_h, 5.0);
    vec3 Fd = (vec3(1.0) - F) * (1.0 - metalness);

    float cos_theta_o = max(dot(world_normal, omega_o), 0.0);
    vec2 brdf_coords = vec2(cos_theta_o, roughness);
    vec2 brdf = texture(texture_1, brdf_coords).rg;

    vec4 specular_color = vec4(prefiltered_color * (F0 * brdf.x + brdf.y), 1.0);
    
    m_diffuse = m_diffuse * vec4(Fd, 1.0);

    // 01 - without material
    //FragColor = diffuse_color + specular_color;
    // 02 - just material diffuse tex_diffuse
    FragColor = m_diffuse;

    //FragColor = (m_diffuse + specular_color) * ambient_occlusion;
    //FragColor = m_rma;
}