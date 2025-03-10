#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>

typedef struct lm_context lm_context_t, *lm_context_p;

typedef enum lm_status
{
    LM_SUCCESS          = 0,
    LM_GENERAL_ERROR    = -1,
    LM_OUT_OF_MEMORY    = -2,
    LM_GPIO_ERROR       = -3,
    LM_RMT_ERROR        = -4,
    LM_INVALID_ARGUMENT = -5,
    LM_OUT_OF_RANGE     = -6
} lm_status_t;

lm_status_t led_matrix_init(
    uint8_t gpio_pin,
    uint32_t led_count,
    lm_context_p* out_matrix
);

lm_status_t led_matrix_set_dimensions(
    lm_context_p matrix,
    uint16_t width,
    uint16_t height
);

lm_status_t led_matrix_get_dimensions(
    lm_context_p matrix,
    uint16_t* width,
    uint16_t* height
);

lm_status_t led_matrix_set_immediate_render(
    lm_context_p matrix,
    bool enable
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

lm_status_t led_matrix_set_xy(
    lm_context_p matrix,
    uint16_t x,
    uint16_t y,
    uint8_t r,
    uint8_t g,
    uint8_t b
);

lm_status_t led_matrix_render(
    lm_context_p matrix
);

void led_matrix_destroy(
    lm_context_p matrix
);

#endif // LED_MATRIX_H
