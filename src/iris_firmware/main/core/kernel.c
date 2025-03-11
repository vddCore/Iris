#include <math.h>
#include <esp_log.h>
#include <portmacro.h>

#include <FreeRTOSConfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>
#include <freertos/task.h>

#include <drv/led_matrix.h>
#include <renderer/flare.h>

#define MATRIX_GPIO 3
#define MATRIX_LED_COUNT 64
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

void app_main(void)
{
    lm_context_p matrix = NULL;
    flare_renderer_p renderer = NULL;

    double_t t = 0;
    const uint16_t w = MATRIX_WIDTH;
    const uint16_t h = MATRIX_HEIGHT;

    if (led_matrix_init_simple(MATRIX_GPIO, MATRIX_LED_COUNT, &matrix) == LM_OK) {
        if (flare_init(matrix, w, h, &renderer) == IRIS_OK) {
            while (true) {
                for (uint16_t y = 0; y < h; y++) {
                    for (uint16_t x = 0; x < w; x++) {
                        flare_draw_pixel(
                            renderer,
                            x,
                            y,
                            0xFF00FF
                            // FLARE_RGB(
                            //     (uint8_t)(32.0 * sin((x + 0.5) * (y + 0.5) / FLARE_DEFAULT_PIXEL_SCALE + t)),
                            //     0,
                            //     (uint8_t)(32.0 * cos((x + 0.5) * (y + 0.5) / FLARE_DEFAULT_PIXEL_SCALE + t))
                            // )
                            // (uint8_t)(32.0 * sin(
                            //     sqrt(
                            //         (((x + 0.5) - w / 2) * ((x + 0.5) - w / 2) * M_PI) / PIXEL_SCALE
                            //         + (((y + 0.5) - h / 2) * ((y + 0.5) - h / 2) * M_PI) / PIXEL_SCALE
                            //     ) + t)
                            // )
                        );
                        flare_render(renderer);
                        vTaskDelay(pdMS_TO_TICKS(250));

                        t += FLARE_DEFAULT_TIME_STEP;
                    }
                }

            }
        }
    }
}
