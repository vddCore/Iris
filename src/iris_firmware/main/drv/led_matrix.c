#include <stdlib.h>

#include <led_strip.h>
#include <driver/gpio.h>

#include "drv/led_matrix.h"

typedef struct lm_context
{
    uint8_t gpio_pin;
    uint16_t led_count;

    uint16_t width;
    uint16_t height;
    bool immediate_render;

    led_strip_handle_t led_strip;
} lm_context_t, *lm_context_p;

lm_status_t led_matrix_init(const uint8_t gpio_pin, const uint32_t led_count, lm_context_p* out_matrix)
{
    lm_status_t ret = LM_SUCCESS;

    const lm_context_p matrix = calloc(1, sizeof(lm_context_t));

    if (matrix == NULL) {
        ret = LM_OUT_OF_MEMORY;
        goto __hard_error;
    }

    if (gpio_reset_pin(gpio_pin) != ESP_OK) {
        ret = LM_GPIO_ERROR;
        goto __hard_error;
    }

    if (gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT) != ESP_OK) {
        ret = LM_GPIO_ERROR;
        goto __hard_error;
    }

    const led_strip_config_t ls_cfg = {
        .led_model = LED_MODEL_WS2812,
        .max_leds = led_count,
        .strip_gpio_num = gpio_pin
    };

    matrix->gpio_pin = gpio_pin;
    matrix->led_count = led_count;

    const led_strip_rmt_config_t ls_rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,
        .flags.with_dma = false
    };

    if (led_strip_new_rmt_device(&ls_cfg, &ls_rmt_config, &matrix->led_strip) != ESP_OK) {
        ret = LM_RMT_ERROR;
        goto __hard_error;
    }

    *out_matrix = matrix;

__exit:
    return ret;

__hard_error:
    if (matrix != NULL) {
        led_matrix_destroy(matrix);
    }

    *out_matrix = NULL;
    goto __exit;
}

lm_status_t led_matrix_set_dimensions(const lm_context_p matrix, const uint16_t width, const uint16_t height)
{
    if (width * height > matrix->led_count) {
        return LM_OUT_OF_RANGE;
    }

    matrix->width = width;
    matrix->height = height;

    return LM_SUCCESS;
}

lm_status_t led_matrix_get_dimensions(const lm_context_p matrix, uint16_t* width, uint16_t* height)
{
    lm_status_t ret = LM_SUCCESS;

    if (matrix == NULL) {
        ret = LM_INVALID_ARGUMENT;
        goto __hard_error;
    }

    *width = matrix->width;
    *height = matrix->height;

__exit:
    return ret;

__hard_error:
    *width = 0;
    *height = 0;
    goto __exit;
}

lm_status_t led_matrix_set_immediate_render(const lm_context_p matrix, const bool enable)
{
    if (matrix == NULL) {
        return LM_INVALID_ARGUMENT;
    }

    matrix->immediate_render = enable;

    return LM_SUCCESS;
}

lm_status_t led_matrix_clear(lm_context_p matrix)
{
    if (matrix == NULL) {
        return LM_INVALID_ARGUMENT;
    }

    led_strip_clear(matrix->led_strip);

    if (matrix->immediate_render) {
        led_strip_refresh(matrix->led_strip);
    }

    return LM_SUCCESS;
}

lm_status_t led_matrix_set_led(
    const lm_context_p matrix,
    const uint32_t idx,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b)
{
    lm_status_t ret = LM_SUCCESS;

    if (matrix == NULL) {
        ret = LM_INVALID_ARGUMENT;
        goto __exit;
    }

    if (idx >= matrix->led_count) {
        ret = LM_OUT_OF_RANGE;
        goto __exit;
    }

    led_strip_set_pixel(matrix->led_strip, idx, r, g, b);

    if (matrix->immediate_render) {
        led_strip_refresh(matrix->led_strip);
    }

__exit:
    return ret;
}

lm_status_t led_matrix_set_xy(
    const lm_context_p matrix,
    const uint16_t x,
    const uint16_t y,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b)
{
    lm_status_t ret = LM_SUCCESS;

    if (matrix == NULL) {
        ret = LM_INVALID_ARGUMENT;
        goto __exit;
    }

    if (x >= matrix->width || y >= matrix->height) {
        ret = LM_OUT_OF_RANGE;
        goto __exit;
    }

    const uint32_t idx = ((matrix->height - y) * matrix->width) - x - 1;
    led_strip_set_pixel(matrix->led_strip, idx, r, g, b);

    if (matrix->immediate_render) {
        led_strip_refresh(matrix->led_strip);
    }

__exit:
    return ret;
}

lm_status_t led_matrix_render(const lm_context_p matrix)
{
    if (matrix == NULL) {
        return LM_INVALID_ARGUMENT;
    }

    led_strip_refresh(matrix->led_strip);
    return LM_SUCCESS;
}

void led_matrix_destroy(const lm_context_p matrix)
{
    if (matrix != NULL) {
        if (matrix->led_strip != NULL) {
            led_strip_del(matrix->led_strip);
        }

        free(matrix);
    }
}
