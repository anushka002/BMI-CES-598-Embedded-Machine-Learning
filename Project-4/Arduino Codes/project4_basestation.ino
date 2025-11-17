/*
  project4_basestation.ino
  Nano 33 BLE Sense Rev2
  - Reads BMI270/BMM150 IMU via Arduino_BMI270_BMM150
  - Normalizes per-sensor using scaler.json values (embedded below)
  - Collects WINDOW samples at SAMPLE_RATE, runs TFLite Micro model (g_model_data)
  - Prints single-line result: "DONE <class_index> <confidence>"
*/

#include <Arduino.h>
#include <Arduino_BMI270_BMM150.h>

#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model_data.h" // must provide extern g_model_data[] and g_model_data_len

// ---------------- TFLM arena ----------------
constexpr int kTensorArenaSize = 60000; // increase if AllocateTensors fails
static uint8_t tensor_arena[kTensorArenaSize];

// Error reporter (older API)
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;

// Model/interpreter
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input_tensor = nullptr;
TfLiteTensor* output_tensor = nullptr;

// ---------------- Normalizer values (from your scaler.json) ----------------
const float mean_accel[3] = { 0.1333749970349976f, -0.004738968528588029f, 0.09365147146108725f };
const float std_accel[3]  = { 0.5790330769780665f,  0.5530097689095863f,  0.5730673944081529f };

const float mean_gyro[3]  = { -0.9496915418041441f, 0.38017769265518736f, -0.3906607707326229f };
const float std_gyro[3]   = { 22.876948531367177f,  12.44489971808402f,   8.912320877062669f };

const float mean_mag[3]   = { -25.5315f, 7.238857142857142f, 31.785357142857144f };
const float std_mag[3]    = { 30.058837669405293f, 24.55193804063178f, 27.480036907614338f };

// ---------------- Window config (match your training scaler.json)
constexpr int SAMPLE_RATE = 100;              // Hz (from scaler.json sr=100)
constexpr int WINDOW_SEC  = 2;                // seconds
constexpr int WINDOW_SIZE = SAMPLE_RATE * WINDOW_SEC; // 200 samples

static float window_buffer[WINDOW_SIZE][3];

// ---------------- Helpers ----------------
void normalize_sample(const float raw[3], int mode, float out[3]) {
  if (mode == 1) { // accel
    for (int i = 0; i < 3; ++i) out[i] = (raw[i] - mean_accel[i]) / std_accel[i];
  } else if (mode == 2) { // gyro
    for (int i = 0; i < 3; ++i) out[i] = (raw[i] - mean_gyro[i]) / std_gyro[i];
  } else { // mag
    for (int i = 0; i < 3; ++i) out[i] = (raw[i] - mean_mag[i]) / std_mag[i];
  }
}

// Read one raw reading from IMU for chosen sensor mode
// mode: 1=accel, 2=gyro, 3=mag
bool read_sensor_raw(int mode, float out[3]) {
  if (mode == 1) {
    if (IMU.accelerationAvailable()) {
      return IMU.readAcceleration(out[0], out[1], out[2]);
    }
  } else if (mode == 2) {
    if (IMU.gyroscopeAvailable()) {
      return IMU.readGyroscope(out[0], out[1], out[2]);
    }
  } else {
    float mx, my, mz;
    if (IMU.magneticFieldAvailable()) {
      IMU.readMagneticField(mx, my, mz);
      out[0] = mx; out[1] = my; out[2] = mz;
      return true;
    }
  }
  return false;
}

// Quantize helper for int8/uint8 models
inline int32_t quantize_value(float value, float scale, int zero_point, bool is_signed) {
  float q = value / scale + zero_point;
  int32_t qi = (int32_t)round(q);
  if (is_signed) {
    if (qi < -128) qi = -128;
    if (qi > 127) qi = 127;
  } else {
    if (qi < 0) qi = 0;
    if (qi > 255) qi = 255;
  }
  return qi;
}

// Dequantize output value if needed
inline float dequantize_value(int32_t q, float scale, int zero_point) {
  return (q - zero_point) * scale;
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial console (optional)

  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }

  // Load model
  model = tflite::GetModel(g_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch");
    while (1);
  }

  // Use AllOpsResolver to avoid missing op problems
  static tflite::AllOpsResolver resolver;

  // Create interpreter (older API constructor - matches your library)
  static tflite::MicroInterpreter static_interpreter(
    model,
    resolver,
    tensor_arena,
    kTensorArenaSize,
    error_reporter
  );
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors() failed - increase kTensorArenaSize");
    while (1);
  }

  input_tensor = interpreter->input(0);
  output_tensor = interpreter->output(0);

  Serial.println("System ready.");
  Serial.println("Send sensor mode: 1=accel, 2=gyro, 3=mag");
}

// ---------------- Main loop ----------------
void loop() {
  // Wait for a command
  if (!Serial.available()) {
    delay(5);
    return;
  }

  int mode = Serial.parseInt();
  if (mode < 1 || mode > 3) {
    Serial.println("Invalid mode (send 1/2/3)");
    while (Serial.available()) Serial.read(); // flush
    return;
  }

  // Collect window
  int collected = 0;
  const unsigned long sample_interval_ms = (1000u / SAMPLE_RATE);
  unsigned long last_time = millis();

  while (collected < WINDOW_SIZE) {
    float raw[3];
    if (read_sensor_raw(mode, raw)) {
      float norm[3];
      normalize_sample(raw, mode, norm);
      window_buffer[collected][0] = norm[0];
      window_buffer[collected][1] = norm[1];
      window_buffer[collected][2] = norm[2];
      ++collected;
      // wait to maintain sample rate
      unsigned long now = millis();
      if (last_time + sample_interval_ms > now) {
        delay(last_time + sample_interval_ms - now);
      }
      last_time = millis();
    } else {
      // no sample available yet
      delay(1);
    }
  }

  // Prepare input tensor depending on tensor type
  const TfLiteType in_type = input_tensor->type;

  if (in_type == kTfLiteFloat32) {
    // input->data.f length must be <= input_tensor->bytes/4
    // model expects [1, WINDOW_SIZE, 3] flattened row-major
    for (int i = 0; i < WINDOW_SIZE; ++i) {
      for (int c = 0; c < 3; ++c) {
        int idx = i * 3 + c;
        input_tensor->data.f[idx] = window_buffer[i][c];
      }
    }
  } else if (in_type == kTfLiteInt8) {
    // quantized signed int8
    const float scale = input_tensor->params.scale;
    const int32_t zp = input_tensor->params.zero_point;
    for (int i = 0; i < WINDOW_SIZE; ++i) {
      for (int c = 0; c < 3; ++c) {
        int idx = i * 3 + c;
        int32_t q = quantize_value(window_buffer[i][c], scale, zp, true);
        input_tensor->data.int8[idx] = (int8_t)q;
      }
    }
  } else if (in_type == kTfLiteUInt8) {
    const float scale = input_tensor->params.scale;
    const int32_t zp = input_tensor->params.zero_point;
    for (int i = 0; i < WINDOW_SIZE; ++i) {
      for (int c = 0; c < 3; ++c) {
        int idx = i * 3 + c;
        int32_t q = quantize_value(window_buffer[i][c], scale, zp, false);
        input_tensor->data.uint8[idx] = (uint8_t)q;
      }
    }
  } else {
    Serial.println("Unsupported input tensor type");
    return;
  }

  // Invoke
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Inference failed");
    return;
  }

  // Read outputs and find best class
  int num_output = 1;
  for (int i = 0; i < output_tensor->dims->size; ++i) {
    num_output *= output_tensor->dims->data[i];
  }
  // In classification models, last dim is number of classes; if batch included, compute properly:
  // Typically output dims = [1, num_classes] or [num_classes]
  int num_classes = (output_tensor->dims->size >= 2) ? output_tensor->dims->data[output_tensor->dims->size - 1] : num_output;

  float best_score = -1e9f;
  int best_idx = -1;

  if (output_tensor->type == kTfLiteFloat32) {
    for (int i = 0; i < num_classes; ++i) {
      float v = output_tensor->data.f[i];
      if (v > best_score) {
        best_score = v;
        best_idx = i;
      }
    }
  } else if (output_tensor->type == kTfLiteInt8) {
    float scale = output_tensor->params.scale;
    int32_t zp = output_tensor->params.zero_point;
    for (int i = 0; i < num_classes; ++i) {
      int32_t q = output_tensor->data.int8[i];
      float v = dequantize_value((int32_t)q, scale, zp);
      if (v > best_score) {
        best_score = v;
        best_idx = i;
      }
    }
  } else if (output_tensor->type == kTfLiteUInt8) {
    float scale = output_tensor->params.scale;
    int32_t zp = output_tensor->params.zero_point;
    for (int i = 0; i < num_classes; ++i) {
      int32_t q = output_tensor->data.uint8[i];
      float v = dequantize_value((int32_t)q, scale, zp);
      if (v > best_score) {
        best_score = v;
        best_idx = i;
      }
    }
  } else {
    Serial.println("Unsupported output tensor type");
    return;
  }

  // Print the single-line result expected by base_station.py
  Serial.print("DONE ");
  Serial.print(best_idx);
  Serial.print(" ");
  Serial.println(best_score, 6);

  // flush any leftover serial input
  while (Serial.available()) Serial.read();
}



// 
// 
// 
// 
// 








// #include <Arduino.h>
// #include <Arduino_BMI270_BMM150.h>
// #include "TensorFlowLite.h"
// #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/schema/schema_generated.h"
// // #include "tensorflow/lite/version.h"
// #include "model_data.h"   // contains g_model_data and g_model_data_len

// // ------------------------------------------------------
// // Model / TFLM Setup
// // ------------------------------------------------------
// constexpr int kTensorArenaSize = 60000;
// static uint8_t tensor_arena[kTensorArenaSize];

// tflite::ErrorReporter* error_reporter = nullptr;
// const tflite::Model* model = nullptr;
// tflite::MicroInterpreter* interpreter = nullptr;
// TfLiteTensor* input_tensor = nullptr;
// TfLiteTensor* output_tensor = nullptr;

// tflite::MicroMutableOpResolver<10> resolver;

// // ------------------------------------------------------
// // Update these values from scaler JSON
// // ------------------------------------------------------

// float mean_accel[3] = {
//     0.1333749970349976,
//     -0.004738968528588029,
//     0.09365147146108725
// };
// float std_accel[3] = {
//     0.5790330769780665,
//     0.5530097689095863,
//     0.5730673944081529
// };

// float mean_gyro[3] = {
//     -0.9496915418041441,
//     0.38017769265518736,
//     -0.3906607707326229
// };
// float std_gyro[3] = {
//     22.876948531367177,
//     12.44489971808402,
//     8.912320877062669
// };

// float mean_mag[3] = {
//     -25.5315,
//     7.238857142857142,
//     31.785357142857144
// };
// float std_mag[3] = {
//     30.058837669405293,
//     24.55193804063178,
//     27.480036907614338
// };

// // float mean_accel[3] = { /* paste mean ax, ay, az */ };
// // float std_accel[3]  = { /* paste std ax, ay, az */ };

// // float mean_gyro[3]  = { /* paste mean gx, gy, gz */ };
// // float std_gyro[3]   = { /* paste std gx, gy, gz */ };

// // float mean_mag[3]   = { /* paste mean mx, my, mz */ };
// // float std_mag[3]    = { /* paste std mx, my, mz */ };

// // ------------------------------------------------------
// // Window Parameters
// // ------------------------------------------------------
// constexpr int SAMPLE_RATE = 50;         // 50 Hz
// constexpr int WINDOW_SEC = 2;           // 2 seconds
// constexpr int WINDOW_SIZE = SAMPLE_RATE * WINDOW_SEC;   // 100 samples

// float window_buffer[WINDOW_SIZE][3];
// int sample_index = 0;

// // ------------------------------------------------------
// // Normalize a reading based on selected sensor mode
// // ------------------------------------------------------
// void normalize_sample(float raw[3], int mode, float out[3]) {
//     if (mode == 1) {         // Accel
//         for (int i = 0; i < 3; i++)
//             out[i] = (raw[i] - mean_accel[i]) / std_accel[i];
//     }
//     else if (mode == 2) {    // Gyro
//         for (int i = 0; i < 3; i++)
//             out[i] = (raw[i] - mean_gyro[i]) / std_gyro[i];
//     }
//     else {                   // Mag
//         for (int i = 0; i < 3; i++)
//             out[i] = (raw[i] - mean_mag[i]) / std_mag[i];
//     }
// }

// // ------------------------------------------------------
// // Setup
// // ------------------------------------------------------
// void setup() {
//     Serial.begin(115200);
//     while (!Serial);

//     // Initialize IMU
//     if (!IMU.begin()) {
//         Serial.println("IMU initialization failed");
//         while (1);
//     }

//     // Load TFLite model
//     model = tflite::GetModel(g_model_data);
//     if (model->version() != TFLITE_SCHEMA_VERSION) {
//         Serial.println("Model schema mismatch");
//         while (1);
//     }

//     // Add ops
//     resolver.AddFullyConnected();
//     resolver.AddReshape();
//     resolver.AddConv2D();
//     resolver.AddSoftmax();

//     // Create interpreter
//     interpreter = new tflite::MicroInterpreter(
//         model,
//         resolver,
//         tensor_arena,
//         kTensorArenaSize,
//         error_reporter
//     );

//     // Allocate tensors
//     if (interpreter->AllocateTensors() != kTfLiteOk) {
//         Serial.println("Tensor allocation failed");
//         while (1);
//     }

//     input_tensor = interpreter->input(0);
//     output_tensor = interpreter->output(0);

//     Serial.println("System ready.");
//     Serial.println("Send sensor mode: 1=Accel, 2=Gyro, 3=Mag");
// }

// // ------------------------------------------------------
// // Read sensor based on mode
// // ------------------------------------------------------
// bool read_sensor(int mode, float out[3]) {
//     if (mode == 1) {
//         if (IMU.accelerationAvailable())
//             return IMU.readAcceleration(out[0], out[1], out[2]);
//     }
//     else if (mode == 2) {
//         if (IMU.gyroscopeAvailable())
//             return IMU.readGyroscope(out[0], out[1], out[2]);
//     }
//     else {
//         float mx, my, mz;
//         if (IMU.magneticFieldAvailable()) {
//             IMU.readMagneticField(mx, my, mz);
//             out[0] = mx; out[1] = my; out[2] = mz;
//             return true;
//         }
//     }
//     return false;
// }

// // ------------------------------------------------------
// // Loop
// // ------------------------------------------------------
// void loop() {
//     // Wait for sensor mode command from base-station PC
//     if (Serial.available()) {
//         int mode = Serial.parseInt();
//         if (mode < 1 || mode > 3) {
//             Serial.println("Invalid mode");
//             return;
//         }

//         Serial.print("Mode set: ");
//         Serial.println(mode);

//         // Collect a full window
//         sample_index = 0;
//         while (sample_index < WINDOW_SIZE) {
//             float raw[3];
//             if (read_sensor(mode, raw)) {

//                 float norm[3];
//                 normalize_sample(raw, mode, norm);

//                 window_buffer[sample_index][0] = norm[0];
//                 window_buffer[sample_index][1] = norm[1];
//                 window_buffer[sample_index][2] = norm[2];

//                 sample_index++;
//                 delay(20); // 50 Hz
//             }
//         }

//         // Copy window to model input
//         for (int i = 0; i < WINDOW_SIZE; i++) {
//             input_tensor->data.f[i * 3 + 0] = window_buffer[i][0];
//             input_tensor->data.f[i * 3 + 1] = window_buffer[i][1];
//             input_tensor->data.f[i * 3 + 2] = window_buffer[i][2];
//         }

//         // Run inference
//         if (interpreter->Invoke() != kTfLiteOk) {
//             Serial.println("Inference failed");
//             return;
//         }

//         // Get predicted class
//         float best_score = -9999;
//         int best_idx = -1;
//         for (int i = 0; i < 5; i++) { // 5 classes
//             float v = output_tensor->data.f[i];
//             if (v > best_score) {
//                 best_score = v;
//                 best_idx = i;
//             }
//         }

//         // Print result
//         Serial.print("Predicted posture: ");
//         Serial.print(best_idx);
//         Serial.print("  Confidence: ");
//         Serial.println(best_score, 4);
//     }
// }
