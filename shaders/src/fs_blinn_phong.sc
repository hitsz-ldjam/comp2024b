$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

// SAMPLER2D(s_tex_diffuse, 0);
uniform vec4 u_diffuse_color;  // a channel is gloss/shininess

uniform vec3 u_ambient_light;

uniform vec3 u_dir_light_dir;  // world direction point to light
uniform vec3 u_dir_light_color;

uniform vec3 u_camera_pos;

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
