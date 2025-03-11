#include <stdlib.h>

#include <led_strip.h>
#include <driver/gpio.h>

#include "drv/led_matrix.h"

#include <math.h>

typedef struct lm_context
{
    uint8_t gpio_pin;
    uint16_t led_count;
    uint8_t red_brightness;
    uint8_t green_brightness;
    uint8_t blue_brightness;
    lm_bus_t bus;
    led_strip_handle_t led_strip;
} lm_context_t, *lm_context_p;

lm_status_t led_matrix_init_simple(const uint8_t gpio_pin, const uint16_t led_count, lm_context_p* out_matrix)
{
    return led_matrix_init_user(
        gpio_pin,
        led_count,
        LM_BUS_RMT,
        NULL,
        NULL,
        out_matrix
    );
}

lm_status_t led_matrix_init_user(
    const uint8_t gpio_pin,
    const uint32_t led_count,
    const lm_bus_t bus,
    const led_strip_rmt_config_t* user_rmt_config,
    const led_strip_spi_config_t* user_spi_config,
    lm_context_p* out_matrix)
{
    lm_status_t ret = LM_OK;

    const lm_context_p matrix = calloc(1, sizeof(lm_context_t));

    if (matrix == NULL) {
        LM_FAIL(__hard_error, LM_OUT_OF_MEMORY);
    }

    if (gpio_reset_pin(gpio_pin) != ESP_OK) {
        LM_FAIL(__hard_error, LM_GPIO_ERROR);
    }

    if (gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT) != ESP_OK) {
        LM_FAIL(__hard_error, LM_GPIO_ERROR);
    }

    matrix->gpio_pin = gpio_pin;
    matrix->led_count = led_count;
    matrix->bus = bus;
    matrix->red_brightness = 127;
    matrix->green_brightness = 127;
    matrix->blue_brightness = 127;

    const led_strip_config_t ls_cfg = {
        .led_model = LED_MODEL_WS2812,
        .max_leds = matrix->led_count,
        .strip_gpio_num = matrix->gpio_pin
    };

    if (matrix->bus == LM_BUS_RMT) {
        led_strip_rmt_config_t ls_rmt_config = { 0 };

        if (user_rmt_config != NULL) {
            ls_rmt_config = *user_rmt_config;
        } else {
            ls_rmt_config.resolution_hz = 40 * 1000 * 1000;
            ls_rmt_config.flags.with_dma = false;
        }

        if (led_strip_new_rmt_device(&ls_cfg, &ls_rmt_config, &matrix->led_strip) != ESP_OK) {
            LM_FAIL(__hard_error, LM_BUS_ERROR);
        }
    } else if (matrix->bus == LM_BUS_SPI) {
        led_strip_spi_config_t ls_spi_config = { 0 };

        if (user_spi_config != NULL) {
            ls_spi_config = *user_spi_config;
        } else {
            ls_spi_config.spi_bus = SPI2_HOST;
            ls_spi_config.flags.with_dma = true;
        }

        if (led_strip_new_spi_device(&ls_cfg, &ls_spi_config, &matrix->led_strip) != ESP_OK) {
            LM_FAIL(__hard_error, LM_BUS_ERROR);
        }
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

lm_status_t led_matrix_clear(const lm_context_p matrix)
{
    lm_status_t ret = LM_OK;

    if (matrix == NULL) {
        LM_FAIL(__exit, LM_INVALID_ARGUMENT);
    }

    led_strip_clear(matrix->led_strip);

__exit:
    return ret;
}

lm_status_t led_matrix_set_led(
    const lm_context_p matrix,
    const uint32_t idx,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b)
{
    lm_status_t ret = LM_OK;

    if (matrix == NULL) {
        LM_FAIL(__exit, LM_INVALID_ARGUMENT);
    }

    if (idx >= matrix->led_count) {
        LM_FAIL(__exit, LM_OUT_OF_RANGE);
    }

    led_strip_set_pixel(
        matrix->led_strip,
        idx,
        (uint8_t)(255 * (r * matrix->red_brightness / 255.0)),
        (uint8_t)(255 * (g * matrix->green_brightness / 255.0)),
        (uint8_t)(255 * (b * matrix->blue_brightness / 255.0))
    );

__exit:
    return ret;
}

lm_status_t led_matrix_set_global_brightness(
    const lm_context_p matrix,
    const uint8_t red_brightness,
    const uint8_t green_brightness,
    const uint8_t blue_brightness)
{
    lm_status_t ret = LM_OK;

    if (matrix == NULL) {
        LM_FAIL(__exit, LM_INVALID_ARGUMENT);
    }

    matrix->red_brightness = red_brightness;
    matrix->green_brightness = green_brightness;
    matrix->blue_brightness = blue_brightness;

__exit:
    return ret;
}

lm_status_t led_matrix_flush(const lm_context_p matrix)
{
    lm_status_t ret = LM_OK;

    if (matrix == NULL) {
        LM_FAIL(__exit, LM_INVALID_ARGUMENT);
    }

    led_strip_refresh(matrix->led_strip);

__exit:
    return ret;
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
