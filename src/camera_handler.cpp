#include "camera_handler.h"
#include <Arduino.h>

camera_config_t config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000, // XCLK frequency (20 MHz)
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_GRAYSCALE,

    .frame_size = FRAMESIZE_QQVGA, // Choose the appropriate frame size
    //.jpeg_quality = 20,            // JPEG quality (0-63)
    .fb_count = 1, // Frame buffer count
};

void init_camera()
{
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera initialization failed with error 0x%x", err);
        return;
    }
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);

    Serial.println("Camera initialized");
}

void cropResizeImage(const uint8_t *inputBuffer, uint8_t *outputBuffer)
{
    int inputWidth = 160;
    int inputHeight = 120;
    int croppedSize = 120;
    int resizedSize = 28;

    int cropStartX = (inputWidth - croppedSize) / 2;
    int cropStartY = 0;

    int resizedRatioX = croppedSize / resizedSize;
    int resizedRatioY = inputHeight / resizedSize;

    for (int y = 0; y < resizedSize; y++)
    {
        for (int x = 0; x < resizedSize; x++)
        {
            int inputX = cropStartX + x * resizedRatioX;
            int inputY = cropStartY + y * resizedRatioY;
            outputBuffer[y * resizedSize + x] = inputBuffer[inputY * inputWidth + inputX];
        }
    }
}

void invertImageColors(uint8_t *image)
{
    for (int i = 0; i < 28; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            image[i * 28 + j] = 255 - image[i * 28 + j];
        }
    }
}

void threshold_image(uint8_t input_image[28][28], uint8_t threshold)
{
    for (int i = 0; i < 28; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            input_image[i][j] = (input_image[i][j] > threshold) ? 255 : 0;
        }
    }
}

void add_black_border(uint8_t input_image[28][28], uint8_t border_width)
{
    for (int i = 0; i < 28; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            if (i < border_width || i >= 28 - border_width || j < border_width || j >= 28 - border_width)
            {
                input_image[i][j] = 0; // Set border pixels to black
            }
        }
    }
}

void applyHighPassFilter(uint8_t inputImage[28][28], uint8_t outputImage[28][28])
{
    int kernel[3][3] = {
        {-2, -2, -2},
        {-2, 17, -2},
        {-2, -2, -2}};

    for (int i = 1; i < 27; i++)
    {
        for (int j = 1; j < 27; j++)
        {
            int sum = 0;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    sum += kernel[x + 1][y + 1] * inputImage[i + x][j + y];
                }
            }
            outputImage[i][j] = (uint8_t)(sum < 0 ? 0 : (sum > 255 ? 255 : sum));
        }
    }
}

uint8_t print_image(camera_fb_t *fb)
{
  uint8_t *pixels = fb->buf;
  uint32_t width = fb->width;
  uint32_t height = fb->height;
  Serial.printf("Image size: %dx%d\n", width, height);
  for (int i = 0; i < height; i++)
  {
    Serial.println();
    for (int j = 0; j < width; j++)
    {
      Serial.printf("%d ", pixels[i * width + j]);
    }
  }
  Serial.println();
  return 0;
}
