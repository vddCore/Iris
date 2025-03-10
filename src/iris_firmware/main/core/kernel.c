#include <math.h>
#include <esp_log.h>
#include <portmacro.h>

#include <FreeRTOSConfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>
#include <freertos/task.h>

#include <drv/led_matrix.h>

void app_main(void)
{
    lm_context_p matrix = NULL;

    double t = 0;
    const uint16_t w = 8;
    const uint16_t h = 8;

    if (led_matrix_init(3, 64, &matrix) == LM_SUCCESS) {
        if (led_matrix_set_dimensions(matrix, w, h) == LM_SUCCESS) {
            while (true) {
                led_matrix_clear(matrix);

                for (uint16_t y = 0; y < h; y++) {
                    for (uint16_t x = 0; x < w; x++) {
                        led_matrix_set_xy(
                            matrix,
                            x,
                            y,
                            (uint8_t)(32.0 * sin(x / 0.0005 + t)),
                            0,
                            0
                        );

                        t += 0.001;
                    }
                }

                led_matrix_render(matrix);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
    }
}
