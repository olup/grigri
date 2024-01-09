#pragma once

// buttons
#define BTN_1 14
#define BTN_2 9
#define BTN_3 10
#define BTN_4 11
#define BTN_5 12
#define BTN_6 21
#define BTN_7 47

#define WAKE_UP_BTN GPIO_NUM_12

// sound
#define I2S_DOUT 6
#define I2S_BCLK 5
#define I2S_LRC 4

#define AMP_EN 7

// display
// display definition is overriden by the Pio build env, set it there. cf:
// https://github.com/Bodmer/TFT_eSPI/blob/master/docs/PlatformIO/Configuring%20options.txt
#define LCD_BL 13

// sd card
#define CLK 41
#define CMD 42
#define D0 40
#define D1 39
#define D2 1
#define D3 2

// battery
#define BATTERY_READ 18
#define BATTERY_CHARGING_STAT 8

// led
#define LED_RGB_RED 15
#define LED_RGB_GREEN 17
#define LED_RGB_BLUE 16

#define AUDIO_LOG 1