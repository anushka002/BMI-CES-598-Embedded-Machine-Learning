# Project 4: Real-Time, Sensor-Agnostic Posture Classification on the Arduino Nano 33 BLE Sense Using IMU Windowing, Normalization, and TensorFlow Lite Micro
**Author:** Anushka Gangadhar Satav  
**Course:** BMI/CES 598 – Embedded Machine Learning  
**ASU ID:** 1233530170 (asatav1)

---


## Abstract

This project presents the design, training, deployment, and evaluation of a real-time posture classification system using the Arduino Nano 33 BLE Sense Rev2 IMU. The goal is to create a sensor-agnostic supervised learning model capable of classifying five postures—supine, prone, side-lying, sitting, and unknown—based solely on any three-axis sensor input (accelerometer, gyroscope, or magnetometer).  

The system includes:
1. Multimodal IMU data collection  
2. Windowing and normalization for both offline training and real-time inference  
3. Development of a lightweight neural network trained on interchangeable sensor triplets  
4. Deployment on the Arduino with BLE-based live streaming of predictions  

The final system achieves high accuracy (>90% test accuracy across sensor combinations), stable real-time predictions, and generalization across different IMU modalities, validating the feasibility of embedded sensor-agnostic posture detection for health monitoring and wearable robotics.

---

## Contents

- **A. System Design**  
- **B. Experiment**  
- **C. Algorithm**  
- **D. Results**  
- **E. Discussion**  
- **Appendices**: Code listing, file inventory, run instructions  

---

## A. System Design

### Motivation

Clinical and consumer health applications, such as sleep monitoring, pressure injury prevention, and patient mobility assessment, require high-reliability posture classification. This project focuses on a generalized, sensor-agnostic learning system that accepts any 3-channel IMU input and produces consistent posture predictions.

### High-Level Design

- Sensor data acquisition using on-board 9-axis IMU (accelerometer, gyroscope, magnetometer)  
- Window-based dataset construction with normalization and label generation  
- Training a 3-channel fully connected neural network, independent of sensor type  
- Embedded deployment using TensorFlow Lite Micro on Arduino Nano 33 BLE Sense Rev2  
- Real-time posture prediction with on-device preprocessing  
- Wireless transmission of predictions to a laptop via BLE  

### System Components

**1. Arduino Nano 33 BLE Sense (Embedded Device)**
- Collects IMU data (ACC, GYR, MAG)  
- Implements preprocessing, feature scaling, and 2s windowing with 50% overlap  
- Runs TensorFlow Lite Micro CNN model for inference  
- Sends predictions via serial communication  

**2. Base Station (Python GUI)**
- Receives real-time predictions  
- Sends sensor-selection commands to Arduino  
- Displays predicted posture and confidence  
- Supports dynamic real-time testing  

**Observations and Difficulties**
- Memory constraints required a compact CNN with quantization  
- Orientation-independent classification required normalization and careful sensor fusion  
- Collecting synchronized IMU data was challenging due to varying sensor rates  
- Magnetometer required stabilization time, affecting latency  

**Hardware & Software**

**Hardware:**  
- Arduino Nano 33 BLE Sense Rev2 (Bosch BMI270/BMM150 IMU)  
- USB serial connection  
- Laptop running Python  

**Software:**  
- Arduino IDE 2.3.2  
- Arduino_BMI270_BMM150 library  
- TensorFlow Lite Micro  
- Python: pandas, numpy, matplotlib, scikit-learn  
- BLE visualization script  

---

## B. Experiment

### Goals and Constraints

- Collect realistic posture data using all IMU modalities  
- Construct a windowed, normalized dataset for sensor-agnostic learning  
- Train a small neural network deployable on Arduino  
- Deploy trained model for real-time inference  
- Stream live posture classifications via BLE  

**Posture Categories:**  
1. Supine  
2. Prone  
3. Side (left + right)  
4. Sitting (USB up + USB down)  
5. Unknown (rolling, leaning, arbitrary orientations)  

**Data Collection:**  
- Each trial ~60s at ~100 Hz  
- Arduino sketch `project3_imudatacollection.ino` collected raw IMU data: ax, ay, az; gx, gy, gz; mx, my, mz; timestamp  

**Orientation-Insensitivity:**  
- Merged labels for side and sitting to enforce orientation-independent learning  

**Challenges:**  
- Simulating realistic movements while maintaining controlled data  
- Distribution shift between lab and real-world data  

---

## C. Algorithm

### Input Features

Each training example consists of 3D feature vectors:

- Composite X (ax, gx, mx)  
- Composite Y (ay, gy, my)  
- Composite Z (az, gz, mz)  

### Machine Learning Algorithm Design

- **1D-CNN** classifies 2s windows of time-series data (200 samples)  
- Dense network used as baseline for comparison  

**Training Parameters:**  
- Input shape: (200, 3)  
- Loss: sparse_categorical_crossentropy  
- Optimizer: Adam  
- Epochs: 100  
- Batch size: 64  
- Early stopping and dropout for regularization  

**Windowing & Normalization:**  
- Sliding 2s windows with 50% overlap  
- StandardScaler applied per sensor  

### Model Architectures

**1. Dense Fully Connected Network**  
- Flattened input → Dense 256 → 128 → 5 output (SoftMax)  
- Dropout: 0.3 after first dense, 0.2 after second  

**2. 1D Convolutional Neural Network (Selected)**  
- Input: 200×3  
- Conv1D layers: 32→64→128 filters  
- MaxPool1D and GlobalAveragePooling  
- Dropout 0.3  
- Dense: 64 → 5 output  

**Training & Evaluation:**  
- CNN achieved 92.73% test accuracy, outperforming Dense network (72.73%)  

### Model Deployment on Arduino

1. Convert Keras model to TensorFlow Lite (float → INT8 quantized)  
2. Convert TFLite to C array (`model_data.cc/h`)  
3. Integrate normalization constants (`scaler.json`)  
4. Arduino sketch implements preprocessing, inference, and serial communication  

---

## D. Results

**Offline Test Accuracy:** 92.73%  

**Real-Time Prediction:**  
- Stable predictions across all postures using ACC, GYR, or MAG inputs  
- Sensor-agnostic performance confirmed  

**Command Protocol Example:**  

| Command | Sensor | Prediction |
|---------|--------|-----------|
| '1'     | ACC    | Posture based on ACC |
| '2'     | GYR    | Posture based on GYR |
| '3'     | MAG    | Posture based on MAG |

**Demonstration Video:**  
[Insert YouTube Video Link Here]  

---

## E. Discussion & Conclusion

- Successfully developed and deployed a real-time, sensor-agnostic posture classification system  
- 1D-CNN extracted robust features from 2s IMU windows, achieving 92.73% offline accuracy  
- Orientation-insensitive label merging and per-sensor normalization ensured generalization  
- Future work: use temporal models (LSTM) for subtle movements and prone vs. supine distinction  

---

## Appendices

**Appendix A – File Inventory**

**Python Scripts:**  
- `project4_data_collection.py` – Collects raw IMU data  
- `data_collection.py` – Applies windowing, normalization, label merging  
- `Project04-BMI598-Colab.ipynb` – Model training, quantization, and C-array generation  
- `base_station.py` – Base station interface for real-time predictions  

**Arduino & Model Artifacts:**  
- `project4_basestation.ino` – Arduino sketch with TFLM runtime  
- `model_data.h/cc` – INT8 quantized TFLite model  
- `scaler.json` – Sensor normalization constants  

**Dataset & Results:**  
- Raw CSVs: `prone.csv`, `supine.csv`, `side_left.csv`, `side_right.csv`, `sitting_up.csv`, `sitting_down.csv`, `unknown.csv`  
- `best_cnn_int8.tflite` – Final optimized TFLite model  
- `cm_CNN.png` – Confusion matrix  

**Appendix B – Running the Pipeline**  
1. Collect and prepare data: `project4_data_collection.py` → `data_collection.py`  
2. Train and convert model: `Project04-BMI598-Colab.ipynb`  
3. Deploy and test: Arduino sketch + `base_station.py`  

**Appendix C – References**  
1. Arduino Nano 33 BLE Sense Rev2 Documentation: https://docs.arduino.cc/hardware/nano-33-ble-sense/  
2. Arduino BMI270 IMU Library Documentation: https://docs.arduino.cc/libraries/arduino_bmi270_bmm150  
3. TensorFlow/Keras Documentation: https://www.tensorflow.org/api_docs/python/tf/keras  
4. Course Material: BMI/CES 598, ASU, Fall B 2025  

**Appendix D – Use of AI Tools**  
- ChatGPT and Gemini Pro assisted with debugging, code generation, and data visualization  

---
