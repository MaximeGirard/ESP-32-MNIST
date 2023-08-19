#ifndef TFLITE_HANDLER_H
#define TFLITE_HANDLER_H

#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>

void init_TFLite();
int32_t classify(uint8_t *image);

#endif // TFLITE_HANDLER_H