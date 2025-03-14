#ifndef FLARE_ANIMATOR_H
#define FLARE_ANIMATOR_H

#include "flare.h"

#define FLARE_DEFAULT_TIME_STEP 0.04
#define FLARE_DEFAULT_FRAME_TIME 16 // ms

typedef struct flare_animator flare_animator_t,
                              *flare_animator_p;

typedef struct flare_animation_context flare_animation_context_t,
                                       *flare_animation_context_p;

typedef bool (*flare_animation_callback_f)(
    flare_animation_context_p ctx
);

typedef iris_status_t (*flare_anim_draw_pixel_f)(
    flare_animation_context_p ctx,
    uint32_t x,
    uint32_t y,
    uint32_t color
);

typedef iris_status_t (*flare_anim_draw_line_f)(
    flare_animation_context_p ctx,
    uint32_t x0,
    uint32_t y0,
    uint32_t x1,
    uint32_t y1,
    uint32_t color
);

typedef iris_status_t (*flare_anim_draw_rectangle_f)(
    flare_animation_context_p ctx,
    uint32_t x0,
    uint32_t y0,
    uint32_t x1,
    uint32_t y1,
    uint32_t color,
    bool filled
);

typedef iris_status_t (*flare_anim_clear_f)(
    flare_animation_context_p ctx
);

struct flare_animation_context
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;

    uint32_t frame_time;
    double_t time;
    uint64_t frame;
    double_t pixel_scale;

    flare_animator_p animator;

    flare_anim_draw_pixel_f pixel;
    flare_anim_draw_line_f line;
    flare_anim_draw_rectangle_f rect;
    flare_anim_clear_f clear;
};

iris_status_t flare_anim_init(
    flare_renderer_p renderer,
    flare_animator_p* out_animator
);

iris_status_t flare_anim_set_callback(
    flare_animator_p animator,
    flare_animation_callback_f callback
);

iris_status_t flare_anim_reset(
    flare_animator_p animator
);

iris_status_t flare_anim_run(
    flare_animator_p animator
);

void flare_anim_destroy(
    flare_animator_p animator
);

#endif //FLARE_ANIMATOR_H
