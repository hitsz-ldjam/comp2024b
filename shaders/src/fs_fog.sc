
#include <bgfx_shader.sh>

SAMPLER2D(s_depth, 0);
SAMPLER3D(s_noise, 1);
uniform vec4 u_params[8];

#define u_camera_pos u_params[0].xyz
#define u_noise_scale u_params[0].w // vec4 0
#define u_box_min u_params[1].xyz
#define u_density u_params[1].w // vec4 1
#define u_box_max u_params[2].xyz // vec4 2
#define u_color_min u_params[3] // vec4 3
#define u_color_max u_params[4] // vec4 4

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

bool ray_box_intersect(vec3 start, vec3 dir, vec3 b_min, vec3 b_max, out float tmin, out float tmax) {
    vec3 dir_inv = 1.0f / dir;

    float tx1 = (b_min.x - start.x) * dir_inv.x;
    float tx2 = (b_max.x - start.x) * dir_inv.x;

    tmin = min(tx1, tx2);
    tmax = max(tx1, tx2);

    float ty1 = (b_min.y - start.y) * dir_inv.y;
    float ty2 = (b_max.y - start.y) * dir_inv.y;

    tmin = max(tmin, min(ty1, ty2));
    tmax = min(tmax, max(ty1, ty2));

    float tz1 = (b_min.z - start.z) * dir_inv.z;
    float tz2 = (b_max.z - start.z) * dir_inv.z;

    tmin = max(tmin, min(tz1, tz2));
    tmax = min(tmax, max(tz1, tz2));

    return tmax >= tmin;
}

float sample_fog(vec3 current_pos, vec3 backgroud_pos, vec3 camera_pos, vec3 box_extent) {
    const int max_steps = 100;
    const float step_size = 0.25f;

    vec3 view_dir = normalize(current_pos - camera_pos);
    float max_dist = distance(camera_pos, backgroud_pos);
    vec3 vsize = view_dir * step_size;

    vec3 curr_pos = camera_pos - vsize * random3(current_pos).x;
    float density = 0;
    for (int i = 0; i < max_steps; ++i) {
        curr_pos += vsize;

        if(distance(camera_pos, curr_pos) > max_dist)
            break;

        if(!is_in_box(curr_pos))
            continue;

        // sample density here
        float sample = texture3D(s_noise, (curr_pos - u_box_min) / box_extent * u_noise_scale).x;
        density += sample * u_density;
    }

    return density;
}

void main() {
    vec3 screen_space = to_screen_space(gl_FragCoord);
    float scene_depth = texture2D(s_depth, screen_space.xy).x;

    vec3 current_pos = screen_to_world_space(screen_space);
    screen_space.z = scene_depth;
    vec3 backgroud_pos = screen_to_world_space(screen_space);

    vec3 box_extent = u_box_max - u_box_min;
    float density = sample_fog(current_pos, backgroud_pos, u_camera_pos, box_extent);
    vec4 color = mix(u_color_min, u_color_max, density);
    gl_FragColor = vec4(color.xyz, density);
}