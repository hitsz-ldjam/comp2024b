$input a_position, a_normal, a_texcoord0
$output v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_position = mul(u_invViewProj, gl_Position).xyz;  // world space position
    v_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;  // world space normal (?)
    v_texcoord0 = a_texcoord0;  // ignore texture transform
}

// todo: consider view space shading
// void vs_view() {
//     gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
//     v_pos_vs = mul(u_modelView, vec4(a_position, 1.0)).xyz;
//     v_norm_vs = mul(u_modelView, vec4(a_normal, 0.0)).xyz;
//     v_texcoord0 = a_texcoord0;
// }
