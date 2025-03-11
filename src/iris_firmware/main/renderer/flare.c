#include <stdint.h>

#include <renderer/flare.h>

typedef struct flare_config
{
    double_t time_step;
    double_t pixel_scale;

    uint32_t width;
    uint32_t height;
} flare_config_t, *flare_config_p;

static void __set_pixel_fast(
    const flare_renderer_p renderer,
    const uint32_t x,
    const uint32_t y,
    const uint32_t color)
{
    led_matrix_set_led(
        renderer->matrix,
        (renderer->config->height - y) * renderer->config->width - x - 1,
        (color & 0x00FF0000) >> 16,
        (color & 0x0000FF00) >> 8,
        (color & 0x000000FF) >> 0
    );
}

iris_status_t flare_init(
    const lm_context_p matrix,
    const uint32_t width,
    const uint32_t height,
    flare_renderer_p* out_renderer)
{
    iris_status_t ret = IRIS_OK;

    if (matrix == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (width == 0 || height == 0) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    flare_config_p config = calloc(1, sizeof(flare_config_t));
    if (config == NULL) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_MEMORY);
    }
    config->width = width;
    config->height = height;
    config->time_step = FLARE_DEFAULT_TIME_STEP;
    config->pixel_scale = FLARE_DEFAULT_PIXEL_SCALE;

    flare_renderer_p renderer = calloc(1, sizeof(flare_renderer_t));
    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_MEMORY);
    }
    renderer->matrix = matrix;
    renderer->config = config;

    *out_renderer = renderer;

__exit:
    return ret;
}

iris_status_t flare_configure(
    const flare_renderer_p renderer,
    const uint32_t width,
    const uint32_t height,
    const double_t pixel_scale,
    const double_t time_step)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (pixel_scale < 0.0 || time_step < 0.0) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_RANGE);
    }

    if (width != 0) {
        renderer->config->width = width;
    }

    if (height != 0) {
        renderer->config->height = height;
    }

    if (pixel_scale != 0.0) {
        renderer->config->pixel_scale = pixel_scale;
    }

    if (time_step != 0.0) {
        renderer->config->time_step = time_step;
    }

__exit:
    return ret;
}

iris_status_t flare_draw_pixel(
    const flare_renderer_p renderer,
    const uint32_t x,
    const uint32_t y,
    const uint32_t color)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (x >= renderer->config->width || y >= renderer->config->height) {
        IRIS_FAIL(__exit, IRIS_OUT_OF_RANGE);
    }

    __set_pixel_fast(renderer, x, y, color);

__exit:
    return ret;
}

iris_status_t flare_draw_line(
    const flare_renderer_p renderer,
    int32_t x0,
    int32_t y0,
    const int32_t x1,
    const int32_t y1,
    const uint32_t color)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    const int32_t dx = abs(x1 - x0);
    const int32_t dy = abs(y1 - y0);

    const int sx = (x0 < x1)
                   ? 1
                   : -1;
    const int sy = (y0 < y1)
                   ? 1
                   : -1;

    int err = dx - dy;

    while (true) {
        if ((x0 == x1 && y0 == y1)
            || x0 < 0
            || y0 < 0
            || x0 >= renderer->config->width
            || y0 >= renderer->config->height) {
            break;
        }

        __set_pixel_fast(renderer, x0, y0, color);

        int e2 = err << 1;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

__exit:
    return ret;
}

iris_status_t flare_draw_rectangle(
    const flare_renderer_p renderer,
    const uint32_t x,
    const uint32_t y,
    const uint32_t w,
    const uint32_t h,
    const uint32_t color,
    const bool filled)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    if (filled) {
        for (uint32_t i = 0; i < h; i++) {
            for (uint32_t j = 0; j < w; j++) {
                __set_pixel_fast(renderer, x + j, y + i, color);
            }
        }
    } else {
        for (uint32_t i = 0; i < w; i++) {
            __set_pixel_fast(renderer, x + i, y, color);
            __set_pixel_fast(renderer, x + i, y + h - 1, color);
        }

        for (uint32_t i = 0; i < h; i++) {
            __set_pixel_fast(renderer, x, y + i, color);
            __set_pixel_fast(renderer, x + w - 1, y + i, color);
        }
    }

__exit:
    return ret;
}

iris_status_t flare_render(const flare_renderer_p renderer)
{
    iris_status_t ret = IRIS_OK;

    if (renderer == NULL) {
        IRIS_FAIL(__exit, IRIS_INVALID_ARGUMENT);
    }

    led_matrix_flush(renderer->matrix);

__exit:
    return ret;
}

void flare_destroy(const flare_renderer_p renderer)
{
    if (renderer != NULL) {
        if (renderer->config != NULL) {
            free(renderer->config);
            renderer->config = NULL;
        }

        free(renderer);
    }
}
