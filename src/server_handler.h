#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <esp_http_server.h>

void init_server();
esp_err_t stream_handler(httpd_req_t *req);
esp_err_t html_handler(httpd_req_t *req);

#endif // SERVER_HANDLER_H