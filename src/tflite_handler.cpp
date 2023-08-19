#include "tflite_handler.h"
#include <Arduino.h>

#include "mnist_model_quantized.h"

// TFlite objects
tflite::ErrorReporter *error_reporter;
tflite::AllOpsResolver resolver;
const tflite::Model *model;
const int tensor_arena_size = 20 * 1024;
uint8_t tensor_arena[tensor_arena_size];
// Build an interpreter to run the model with
tflite::MicroInterpreter *interpreter;


void init_TFLite() {
    // Initialize TF Lite model
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(mnist_model_quant_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  Serial.println("Model loaded");

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, tensor_arena_size, error_reporter);
  interpreter = &static_interpreter;
  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();
  Serial.println("Tensors allocated");

  Serial.println("TF Lite model initialized");
}


int32_t classify(uint8_t *image)
{
  // Obtain a pointer to the model's input tensor
  TfLiteTensor *input = interpreter->input(0);
  // Make sure the input has the properties we expect
  // print the parameters
  /*
  Serial.print("input->dims->size: ");
  Serial.println(input->dims->size);
  Serial.print("input->dims->data[0]: ");
  Serial.println(input->dims->data[0]);
  Serial.print("input->dims->data[1]: ");
  Serial.println(input->dims->data[1]);
  Serial.print("input->dims->data[2]: ");
  Serial.println(input->dims->data[2]);
  Serial.print("input->dims->data[3]: ");
  Serial.println(input->dims->data[3]);
  Serial.print("input->type: ");
  Serial.println(input->type);
  */
  if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
      (input->dims->data[1] != 28) || (input->dims->data[2] != 28) ||
      (input->dims->data[3] != 1) || (input->type != kTfLiteUInt8))
  {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }
  Serial.println("Input parameters checked");

  // Provide an input value
  // image buffer must be 28x28
  memcpy(input->data.uint8, image, input->bytes);

  // Run the model on this input and check that it succeeds
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk)
  {
    error_reporter->Report("Invoke failed\n");
  }
  Serial.println("Invoke succeed");

  TfLiteTensor *output = interpreter->output(0);
  // Serial.println(output->dims->data[0]);
  // Serial.println(output->dims->data[1]);
  //  print the array values
  int maxIndex = 0;
  int maxValue = 0;
  for (int i = 0; i < output->dims->data[1]; i++)
  {
    Serial.print(output->data.uint8[i]);
    Serial.print(" ");
    if (output->data.uint8[i] > maxValue)
    {
      maxValue = output->data.uint8[i];
      maxIndex = i;
    }
  }
  Serial.println();
  Serial.printf("Class is : %d\n", maxIndex);

  return 0;
}