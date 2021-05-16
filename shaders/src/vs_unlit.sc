$input a_position, a_normal, a_texcoord0
$output v_color0

#include <bgfx_shader.sh>


uniform vec4 u_diffuse_color;


void main() {
    // gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec4 tmp = mul(u_model[0], vec4(a_position, 1.0));
    gl_Position = mul(u_viewProj, tmp);
    // todo: transform normal & uv
    v_color0 = u_diffuse_color;
}
