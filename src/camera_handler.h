#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <esp_camera.h>
#include <esp_http_server.h>

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void init_camera();
void cropResizeImage(const uint8_t *inputBuffer, uint8_t *outputBuffer);
void invertImageColors(uint8_t *image);
void threshold_image(uint8_t input_image[28][28], uint8_t threshold);
void add_black_border(uint8_t input_image[28][28], uint8_t border_width);
void applyHighPassFilter(uint8_t inputImage[28][28], uint8_t outputImage[28][28]);
uint8_t print_image(camera_fb_t *fb);

#endif // CAMERA_HANDLER_H