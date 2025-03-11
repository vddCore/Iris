# Iris
A mesh-networked LED matrix system based on ESP32-C3 and WS2812B 8x8 LED display modules.

## System components

### Low-level
`drv/led_matrix.c`  
Handles bus initialization and drives the LEDs using `led_strip` component provided by Espressif.
Can be re-used independently of Iris in your own projects.

### High-level
**FLARE**  
Fancy LED Animation & Rendering Engine. Handles programmable color blending & timestep management.