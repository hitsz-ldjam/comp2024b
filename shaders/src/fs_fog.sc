
#include <bgfx_shader.sh>

SAMPLER2D(s_color, 0);
SAMPLER2D(s_depth, 1);
uniform vec4 u_params[4];

#define u_camera_pos u_params[0].xyz
#define u_box_min u_params[1].xyz
#define u_box_max u_params[2].xyz

vec3 to_screen_space(vec4 frag_coord) {
    return vec3(frag_coord.xy / u_viewRect.zw, frag_coord.z);
}

float to_device_depth(float depth)
{
#if BGFX_SHADER_LANGUAGE_GLSL
	return depth * 2.0 - 1.0;
#else
	return depth;
#endif // BGFX_SHADER_LANGUAGE_GLSL
}

vec3 screen_to_world_space(vec3 coord) {
    vec4 clip = vec4(coord.x * 2.0f - 1.0f,
                 1.0f - coord.y * 2.0f,
                 to_device_depth(coord.z),
                 1.0f);

    vec4 view = mul(u_invProj, clip);
    view /= view.w;
    
    vec4 world = mul(u_invView, view);
    return world.xyz;
}

vec3 world_to_view_space(vec3 world_coord) {
    return mul(u_view, vec4(world_coord, 1.0f)).xyz;
}

vec3 view_to_world_space(vec3 view_coord) {
    return mul(u_invView, vec4(view_coord, 1.0f)).xyz;
}

float linear_depth(float depth) {
    float a23 = u_proj[2][3];
    float a22 = u_proj[2][2];
    float near = -a23 / a22;
    float far = a23 / (1.0f - a22);
    return (depth - near) / (far - near);
}

bool is_in_box(vec3 pos) {
    return all(u_box_min <= pos) && all(pos <= u_box_max);
}

float random(float x) {
    return fract(sin(x) * 100000.0);
}

float3 random3(vec3 p) {
	p = vec3(dot(p, vec3(127.1,311.7, 74.7)),
			dot(p, vec3(269.5,183.3,246.1)),
			dot(p, vec3(113.5,271.9,124.6)));

	return fract(sin(p) * 43758.5453123);
}

float sample_fog(vec3 world_pos, vec3 camera_pos) {
    vec3 view_dir = normalize(world_pos - camera_pos);
    vec3 step_size = view_dir * 0.25f;

    vec3 curr_pos = camera_pos - step_size * random3(world_pos).x;
    float density = 0;
    for (int i = 0; i < 100; ++i) {
        curr_pos += step_size;
        if(!is_in_box(curr_pos))
            continue;

        density += 0.01;
    }

    return density;
}

void main() {
    vec3 screen_space = to_screen_space(gl_FragCoord);
    float scene_depth = texture2D(s_depth, screen_space.xy).x;
    vec3 scene_color = texture2D(s_color, screen_space.xy);

    screen_space.z = scene_depth;
    vec3 world_pos = screen_to_world_space(screen_space);
    vec3 fog_color = vec3(1, 1, 1);
    float density = sample_fog(world_pos, u_camera_pos);

    gl_FragColor = vec4(scene_color * (1.0f - density) + density * fog_color, 1.0f);
}