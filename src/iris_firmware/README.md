# Iris Firmware
ESP32-C3 firmware written in C for the Iris project.

## System components

### Low-level
#### LED Matrix Driver
`drv/led_matrix.c`  

Handles bus initialization and drives the LEDs using `led_strip` component provided by Espressif.
Can be re-used independently of Iris in your own projects.

### High-level
#### FLARE  
`renderer/flare.c`  
`renderer/flare_animator.c`  

**F**ancy **L**ED **A**nimation & **R**endering **E**ngine.  
Handles programmable color blending & timestep management.  
  
#### AURA VM  
`vm/aura.c`  

**A**ssembly **U**plink, **R**untime & **A**PI.   
Provides a way for users to animate the matrix in real time or via prebuilt executables
without having to recompile the firmware every time a user would like to change the damn animation.