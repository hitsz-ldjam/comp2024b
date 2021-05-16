#include <bgfx_shader.sh>

SAMPLER2D(s_color, 0);

void main() {
    gl_FragColor = texture2D(s_color, gl_FragCoord.xy / u_viewRect.zw);;
}