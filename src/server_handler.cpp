#include "server_handler.h"
#include <esp_http_server.h>
#include <esp_camera.h>
#include <Arduino.h>
#include "tflite_handler.h"
#include "camera_handler.h"

const char *html_content = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32-CAM Live Stream</title>
    <style>
        body {
            margin: 0;
            overflow: hidden;
        }
        img {
            width: 100%;
            height: 100vh;
        }
    </style>
</head>
<body>
    <img id="streamImage" alt="Camera Stream">
    <script>
        const streamImage = document.getElementById('streamImage');
        const fps = 2; // Desired frames per second

        function refreshImage() {
            streamImage.src = '/stream?' + new Date().getTime(); // Append timestamp to prevent caching
        }

        refreshImage(); // Initial image load

        setInterval(refreshImage, 1000 / fps);
    </script>
</body>
</html>
)";

httpd_handle_t httpd = NULL;

void init_server()
{
    httpd_config_t config_server = HTTPD_DEFAULT_CONFIG();
    config_server.server_port = 80;

    // Start the HTTP server
    if (httpd_start(&httpd, &config_server) == ESP_OK)
    {
        httpd_uri_t stream_uri = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = stream_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(httpd, &stream_uri);

        httpd_uri_t html_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = html_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(httpd, &html_uri);
    }
}

esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Print image sizes
    Serial.printf("Image size: %dx%d\n", fb->width, fb->height);
    // print_image(fb);

    // Create the cropped and resized image buffer
    uint8_t croppedResizedBuffer[28 * 28]; // 28x28 size
    cropResizeImage(fb->buf, croppedResizedBuffer);
    invertImageColors(croppedResizedBuffer);
    uint8_t highPassImage[28 * 28];
    applyHighPassFilter((uint8_t(*)[28])croppedResizedBuffer, (uint8_t(*)[28])highPassImage);
    threshold_image((uint8_t(*)[28])highPassImage, 180);
    add_black_border((uint8_t(*)[28])highPassImage, 2);

    // Print the image
    /*for (int i = 0; i < 28; i++)
    {
      Serial.println();
      for (int j = 0; j < 28; j++)
      {
        Serial.printf("%d ", highPassImage[i * 28 + j]);
      }
    }
    Serial.println();*/

    classify(highPassImage);

    // Create the JPEG buffer
    uint8_t *_jpg_buf;
    size_t _jpg_buf_len;

    bool jpeg_converted = fmt2jpg(highPassImage, 28 * 28, 28, 28, PIXFORMAT_GRAYSCALE, 50, &_jpg_buf, &_jpg_buf_len);

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req, (const char *)_jpg_buf, _jpg_buf_len);

    esp_camera_fb_return(fb);
    free(_jpg_buf);

    return ESP_OK;
}

esp_err_t html_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}