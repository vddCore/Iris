#include <math.h>
#include <esp_log.h>
#include <portmacro.h>

#include <FreeRTOSConfig.h>
#include <freertos/FreeRTOS.h>

#include <drv/led_matrix.h>
#include <renderer/flare.h>
#include <renderer/flare_animator.h>

#define MATRIX_GPIO 3
#define MATRIX_LED_COUNT 64
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

bool animation_test(const flare_animation_context_p ctx)
{
    double_t x = ctx->x;
    double_t y = ctx->y;
    double_t w = ctx->w;
    double_t h = ctx->h;
    double_t t = ctx->time;
    double_t s = ctx->pixel_scale;

    const uint8_t r = (uint8_t)(255.0 * (cos(sin(x / 2.0 + 1.0 + t / 2.0) + sin(y / 2.0 + 4.0 + t / 1.25) + t / 0.85))) * 0;
    const uint8_t g = (uint8_t)(255.0 * (sin(cos(x / 2.0 + 2.0 + t / 2.0) + sin(y / 2.0 + 2.0 + t / 1.50) + t / 0.95)));
    const uint8_t b = (uint8_t)(127.0 * (1.0 - sin(sin(y / 2.0 + 4.0 + t / 2.0) + cos(x / 2.0 + 1.0 + t / 2.00) + t / 1.05)));
    // const uint8_t b = (uint8_t)(255.0 * sin(sin(x - 4 + cos(t * 4)) + cos(y + 4 + t) + t * 1.5)) * 0;

    ctx->pixel(ctx, x, y, FLARE_RGB(r, g, b));

    return true;
}

void app_main(void)
{
    lm_context_p matrix = NULL;
    flare_renderer_p renderer = NULL;
    flare_animator_p animator = NULL;

    if (led_matrix_init_simple(MATRIX_GPIO, MATRIX_LED_COUNT, &matrix) == LM_OK) {
        led_matrix_set_global_brightness(matrix, 32);

        if (flare_init(matrix, MATRIX_WIDTH, MATRIX_HEIGHT, &renderer) == IRIS_OK) {
            if (flare_anim_init(renderer, &animator) == IRIS_OK) {
                flare_anim_set_callback(animator, animation_test);
                flare_anim_run(animator);
            }
        }
    }
}
