#version 330 core

uniform sampler2D cr_Screen;

in float cr_Time;
in vec2 cr_ScreenSize;

uniform vec2 aberration = vec2(0.0025, 0.0015);
uniform vec2 curvature = vec2(7.0, 7.5);
uniform float scanline_density = 2;
uniform float blur_distance = 0.04;
uniform float vignette_radius = 0.11;
uniform float brightness = 0.2;

void scanlines(inout vec4 pixel, in vec2 uv) {
    float scanline  = clamp(0.95 + 0.05 * cos(3.14 * (uv.y + 0.008 * cr_Time) * 240.0 * 1.0), 0.0, 1.0);
    pixel *= scanline;

    float actualPixelY = uv.y * cr_ScreenSize.y;
    int modulus = int(mod(actualPixelY, scanline_density));
    pixel /= 1.75;

    if (modulus == 0 && actualPixelY != 0.0)
    {
        pixel /= 1.5;
    }
}

void vignette(inout vec4 color, in vec2 uv) {
    float vignette = (uv.x + vignette_radius) *
    (uv.y + vignette_radius) *
    (1.0 - uv.x + vignette_radius) *
    (1.0 - uv.y + vignette_radius);

    vignette = clamp(pow(16.0 * vignette, 0.4), 0.0, 1.0);
    color *= vignette;
}

vec4 blur_5(in vec2 uv, in vec2 direction) {
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.3333333333333333) * direction;

    color += texture2D(cr_Screen, uv) * 0.29411764705882354;
    color += texture2D(cr_Screen, uv + (off1 / cr_ScreenSize)) * 0.35294117647058826;
    color += texture2D(cr_Screen, uv - (off1 / cr_ScreenSize)) * 0.35294117647058826;

    return color;
}

void glow(inout vec4 pixel, in vec2 uv) {
    pixel /= 1.8;
    pixel += blur_5(uv, vec2(blur_distance, 0));
    pixel += blur_5(uv, vec2(-blur_distance, 0));
}

vec4 chromatic_aberration(in vec2 uv) {
    vec3 col;
    col.r = texture(cr_Screen, vec2(uv.x + aberration.x, uv.y - aberration.y)).g;
    col.g = texture(cr_Screen, vec2(uv.x - aberration.x, uv.y + aberration.y)).r;
    col.b = texture(cr_Screen, vec2(uv.x - aberration.x, uv.y + aberration.y)).b;

    col *= (1.0 - (aberration.x + aberration.y) * 0.5);
    return vec4(col, 1.0);
}

vec2 cubic_curvature(in vec2 uv) {
    uv = uv * 2.0-1.0;

    vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;

    return uv;
}

vec4 effect(vec4 pixel, vec2 tex_coords) {
    vec2 curved_uv = cubic_curvature(tex_coords);
    vec4 base_color = chromatic_aberration(curved_uv);
    
    glow(base_color, curved_uv);
    scanlines(base_color, tex_coords);
    vignette(base_color, curved_uv);

    vec4 new_pixel;

    if (curved_uv.x < 0.0 || curved_uv.y < 0.0 || curved_uv.x > 1.0 || curved_uv.y > 1.0) {
        new_pixel = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        new_pixel = base_color * vec4(vec3(1.0 + brightness), 1.0);
    }

    return new_pixel;
}