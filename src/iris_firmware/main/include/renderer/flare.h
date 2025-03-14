#ifndef FLARE_H
#define FLARE_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <iris.h>
#include <drv/led_matrix.h>

#define FLARE_DEFAULT_PIXEL_SCALE 0.000000000001

#define FLARE_RGB(r, g, b) \
    ((uint32_t)r << 16)    \
    | ((uint32_t)g << 8)   \
    | ((uint32_t) b << 0)

#define FLARE_HSV(H, S, V) ({                                   \
    uint8_t r, g, b, h = (H) / 60, f = ((H) % 60) * 255 / 60,   \
            p = (V * (255 - S)) >> 8,                           \
            q = (V * (255 - ((S * f) >> 8))) >> 8,              \
            t = (V * (255 - ((S * (255 - f)) >> 8))) >> 8;      \
        switch (h) {                                            \
            default: case 0: r = V, g = t, b = p; break;        \
            case 1: r = q, g = V, b = p; break;                 \
            case 2: r = p, g = V, b = t; break;                 \
            case 3: r = p, g = q, b = V; break;                 \
            case 4: r = t, g = p, b = V; break;                 \
            case 5: r = V, g = p, b = q; break;                 \
        }                                                       \
        0xFF000000 | (r << 16) | (g << 8) | b;                  \
    })

typedef struct flare_config flare_config_t, *flare_config_p;

typedef struct flare_renderer
{
    flare_config_p config;
    lm_context_p matrix;
} flare_renderer_t, *flare_renderer_p;

iris_status_t flare_init(
    lm_context_p matrix,
    uint32_t width,
    uint32_t height,
    flare_renderer_p* out_renderer
);

iris_status_t flare_configure(
    flare_renderer_p renderer,
    uint32_t width,
    uint32_t height
);

iris_status_t flare_get_dimensions(
    flare_renderer_p renderer,
    uint32_t* out_width,
    uint32_t* out_height
);

iris_status_t flare_draw_pixel(
    flare_renderer_p renderer,
    uint32_t x,
    uint32_t y,
    uint32_t color
);

iris_status_t flare_draw_line(
    flare_renderer_p renderer,
    int32_t x0,
    int32_t y0,
    int32_t x1,
    int32_t y1,
    uint32_t color
);

iris_status_t flare_draw_rectangle(
    flare_renderer_p renderer,
    uint32_t x0,
    uint32_t y0,
    uint32_t x1,
    uint32_t y1,
    uint32_t color,
    bool filled
);

iris_status_t flare_render(
    flare_renderer_p renderer
);

void flare_destroy(
    flare_renderer_p renderer
);

#endif //FLARE_H
