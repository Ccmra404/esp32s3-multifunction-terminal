#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  48000
#define AUDIO_OUTPUT_SAMPLE_RATE 48000

/* Pin assignment follows the upstream "movecall/moji-esp32s3" board, which is
 * the reference design for this ES8311 + NS4150B module.
 *
 * I2S:
 *   MCLK IO6, WS IO12, BCLK IO14, DIN IO13, DOUT IO11
 * I2C:
 *   SDA IO5, SCL IO4
 * PA enable:
 *   IO9
 */
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_6
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_14
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_13
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_9
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_5
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_4
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

#define BOOT_BUTTON_GPIO        GPIO_NUM_0

/* ST7789 1.54" 240x240, 8pin 排针
 *   SCK  IO41
 *   MOSI IO40   (板上丝印为 SDA)
 *   RES  IO7    (IO11 is used by the audio codec DOUT)
 *   DC   IO39
 *   CS   IO10
 *   BL   IO42
 */
#define DISPLAY_SCL GPIO_NUM_41
#define DISPLAY_SDA GPIO_NUM_40
#define DISPLAY_RES GPIO_NUM_7
#define DISPLAY_DC  GPIO_NUM_39
#define DISPLAY_CS  GPIO_NUM_10

#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  240
#define DISPLAY_SWAP_XY  false
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_42
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#endif // _BOARD_CONFIG_H_
