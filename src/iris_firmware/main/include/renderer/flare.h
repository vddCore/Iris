#ifndef FLARE_H
#define FLARE_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <iris.h>
#include <drv/led_matrix.h>

#define FLARE_DEFAULT_TIME_STEP 0.0001
#define FLARE_DEFAULT_PIXEL_SCALE 0.00000000001

#define FLARE_RGB(r, g, b) \
    ((uint32_t)r << 16)    \
    | ((uint32_t)g << 8)   \
    | ((uint32_t) b << 0)  \

typedef struct flare_config flare_config_t, *flare_config_p;

typedef struct flare_renderer {
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
    uint32_t height,
    double_t pixel_scale,
    double_t time_step
);

iris_status_t flare_draw_pixel(
    flare_renderer_p renderer,
    uint32_t x,
    uint32_t y,
    uint32_t color
);

iris_status_t flare_draw_line(
    flare_renderer_p renderer,
    uint32_t x0,
    uint32_t y0,
    uint32_t x1,
    uint32_t y1,
    uint32_t color
);

iris_status_t flare_draw_rectangle(
    flare_renderer_p renderer,
    uint32_t x,
    uint32_t y,
    uint32_t w,
    uint32_t h,
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
