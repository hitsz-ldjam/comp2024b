$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

// SAMPLER2D(s_tex_diffuse, 0);
uniform vec4 u_diffuse_color; // a channel is gloss/shininess

uniform vec4 u_light_params[8];  // a channel is gloss/shininess

#define u_ambient_light u_light_params[0].xyz

#define u_dir_light_dir u_light_params[1].xyz  // world direction point to light
#define u_dir_light_color u_light_params[2].xyz

#define u_camera_pos u_light_params[3].xyz

void main() {
    // vec3 albedo = texture2D(s_tex_diffuse, v_texcoord0).rgb;
    vec3 albedo = u_diffuse_color.rgb;

    vec3 ambient = u_ambient_light * albedo;

    vec3 world_normal = normalize(v_normal);
    vec3 light_dir = normalize(u_dir_light_dir);

    vec3 diffuse = u_dir_light_color * albedo * max(dot(world_normal, light_dir), 0);

    vec3 view_dir = normalize(u_camera_pos - v_position);
    vec3 half_dir = normalize(light_dir + view_dir);
    float gloss = u_diffuse_color.a;
    vec3 specular = u_dir_light_color * pow(max(dot(world_normal, half_dir), 0), gloss);

    gl_FragColor.xyz = ambient + diffuse + specular;
    gl_FragColor.w = 1.0;
}
