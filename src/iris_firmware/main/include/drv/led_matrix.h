#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>

#include <led_strip.h>

#define LM_FAIL(jump_to, errcode) \
    ret = errcode;                \
    goto jump_to;

typedef struct lm_context lm_context_t;
typedef struct lm_context *lm_context_p;

typedef enum lm_status
{
    LM_OK               = 0,
    LM_GENERAL_ERROR    = -1,
    LM_OUT_OF_MEMORY    = -2,
    LM_GPIO_ERROR       = -3,
    LM_BUS_ERROR        = -4,
    LM_INVALID_ARGUMENT = -5,
    LM_OUT_OF_RANGE     = -6
} lm_status_t;

typedef enum lm_bus
{
    LM_BUS_SPI,
    LM_BUS_RMT
} lm_bus_t;

lm_status_t led_matrix_init_simple(
    uint8_t gpio_pin,
    uint16_t led_count,
    lm_context_p* out_matrix
);

lm_status_t led_matrix_init_user(
    uint8_t gpio_pin,
    uint32_t led_count,
    lm_bus_t bus,
    const led_strip_rmt_config_t* user_rmt_config,
    const led_strip_spi_config_t* user_spi_config,
    lm_context_p* out_matrix
);

lm_status_t led_matrix_clear(
    lm_context_p matrix
);

lm_status_t led_matrix_set_led(
    lm_context_p matrix,
    uint32_t idx,
    uint8_t r,
    uint8_t g,
    uint8_t b
);

lm_status_t led_matrix_set_global_brightness(
    lm_context_p matrix,
    uint8_t brightness
);

lm_status_t led_matrix_flush(
    lm_context_p matrix
);

void led_matrix_destroy(
    lm_context_p matrix
);

#endif // LED_MATRIX_H
