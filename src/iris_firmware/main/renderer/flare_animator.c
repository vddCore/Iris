#include <renderer/flare_animator.h>

typedef struct flare_animator
{
    double_t time_step;
    double_t pixel_scale;

    flare_animation_callback_f animation_callback;
    flare_animation_context_p animation;
    bool animation_running;

    flare_renderer_p renderer;
} flare_animator_t, *flare_animator_p;

static iris_status_t __draw_pixel(
    const flare_animation_context_p ctx,
    const uint32_t x,
    const uint32_t y,
    const uint32_t color)
{
    return flare_draw_pixel(ctx->animator->renderer, x, y, color);
}

static iris_status_t __draw_line(
    const flare_animation_context_p ctx,
    const uint32_t x0,
    const uint32_t y0,
    const uint32_t x1,
    const uint32_t y1,
    const uint32_t color)
{
    return flare_draw_line(ctx->animator->renderer, x0, y0, x1, y1, color);
}

static iris_status_t __draw_rectangle(
    const flare_animation_context_p ctx,
    const uint32_t x0,
    const uint32_t y0,
    const uint32_t x1,
    const uint32_t y1,
    const uint32_t color,
    const bool filled)
{
    return flare_draw_rectangle(ctx->animator->renderer, x0, y0, x1, y1, color, filled);
}

static iris_status_t __clear(const flare_animation_context_p ctx)
{
    return led_matrix_clear(ctx->animator->renderer->matrix);
}

static void __destroy_animation(const flare_animator_p animator)
{
    animator->animation_running = false;
    free(animator->animation);
    animator->animation = NULL;
}

iris_status_t flare_anim_init(
    const flare_renderer_p renderer,
    flare_animator_p* out_animator)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    const flare_animator_p animator = calloc(1, sizeof(flare_animator_t));
    if (animator == NULL) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_MEMORY);
    }

    animator->time_step = FLARE_DEFAULT_TIME_STEP;
    animator->pixel_scale = FLARE_DEFAULT_PIXEL_SCALE;
    animator->renderer = renderer;

    *out_animator = animator;

__exit:
    return ret;
}

iris_status_t flare_anim_set_callback(
    const flare_animator_p animator,
    const flare_animation_callback_f callback)
{
    iris_status_t ret = IRIS_OK;

    if (animator == NULL || callback == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (animator->animation != NULL) {
        __destroy_animation(animator);
        animator->animation = NULL;
    }

    const flare_animation_context_p ctx = calloc(1, sizeof(flare_animation_context_t));
    if (ctx == NULL) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_MEMORY);
    }

    flare_get_dimensions(animator->renderer, &ctx->w, &ctx->h);
    ctx->pixel_scale = animator->pixel_scale;
    ctx->animator = animator;
    ctx->pixel = __draw_pixel;
    ctx->line = __draw_line;
    ctx->rect = __draw_rectangle;
    ctx->clear = __clear;
    ctx->frame_time = FLARE_DEFAULT_FRAME_TIME;

    animator->animation_callback = callback;
    animator->animation = ctx;
__exit:
    return ret;
}

iris_status_t flare_anim_reset(const flare_animator_p animator)
{
    iris_status_t ret = IRIS_OK;

    if (animator == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (animator->animation == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_OPERATION);
    }

    if (flare_get_dimensions(animator->renderer, &animator->animation->w, &animator->animation->h) != IRIS_OK) {
        IRIS_FAIL(__exit, IRIS_GENERAL_ERROR);
    }

    animator->animation->x = 0;
    animator->animation->y = 0;
    animator->animation->frame = 0;
    animator->animation->time = 0.0;
    animator->animation->pixel_scale = animator->pixel_scale;

__exit:
    return ret;
}

// todo: rework to spawn tasks instead
iris_status_t flare_anim_run(const flare_animator_p animator)
{
    iris_status_t ret = IRIS_OK;

    if (animator == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (animator->animation == NULL || animator->animation_callback == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_OPERATION);
    }

    bool running = animator->animation_running = true;

    do {
        for (uint32_t y = 0; y < animator->animation->h; y++) {
            for (uint32_t x = 0; x < animator->animation->w; x++) {
                animator->animation->x = x;
                animator->animation->y = y;

                running = animator->animation_callback(
                    animator->animation
                );
            }
        }

        if (running) {
            flare_render(animator->renderer);

            animator->animation->frame++;
            animator->animation->time += animator->time_step;
            vTaskDelay(pdMS_TO_TICKS(animator->animation->frame_time));
        }

        animator->animation_running = running;
    } while (animator->animation_running);

__exit:
    return ret;
}

void flare_anim_destroy(const flare_animator_p animator)
{
    if (animator != NULL) {
        animator->animation_callback = NULL;

        if (animator->animation != NULL) {
            __destroy_animation(animator);
            animator->animation = NULL;
        }

        free(animator);
    }
}
