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

    const uint16_t w = MATRIX_WIDTH;
    const uint16_t h = MATRIX_HEIGHT;

    if (led_matrix_init_simple(MATRIX_GPIO, MATRIX_LED_COUNT, &matrix) == LM_OK) {
        if (flare_init(matrix, w, h, &renderer) == IRIS_OK) {
            // flare_draw_line(renderer, 0, 0, 7, 7, 0x005500);
            // flare_draw_line(renderer, 5, 0, 5, 7, 0x550000);
            flare_draw_line(renderer, 6, 0, 1, 4, 0x550000);
            flare_draw_rectangle(renderer, 4, 4, 3, 3, 0x550055, true);
            flare_draw_rectangle(renderer, 1, 3, 6, 3, 0x005555, false);

            flare_render(renderer);

            while (true) {
                vTaskDelay(pdMS_TO_TICKS(16));
            }
        }
    }
}
