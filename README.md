# Embedded Machine Learning Projects – Fall B 2025  
**Author:** Anushka Satav  
**Course:** BMI/CEN 598 – Embedded Machine Learning  
**University:** Arizona State University  
**Instructor:** Dr. Hassan Ghasemzadeh

---

## Overview  
This repository contains all project submissions and experiments completed as part of the **BMI/CEN 598 – Embedded Machine Learning** course at **Arizona State University (Fall B 2025)**.  
Each project explores a unique embedded systems concept combining **microcontroller programming**, **state machine design**, **sensor-based data collection**, and **intelligent decision-making** on low-power devices such as the **Arduino Nano 33 BLE Sense Rev2**.

---

## Project List  

---

### 🔹 [Project 1: Embedded State Machine using Arduino Nano 33 BLE Sense](./Project1/)

**Description:**  
A finite state machine was implemented using the onboard RGB LED of the Arduino Nano 33 BLE Sense.  
The system reacts to serial input (`C` key) and internal timers to cycle through LED color states representing  
**DARK → RED → BLUE → GREEN → DARK**.

**Features:**  
- Event-driven design using `millis()` (non-blocking timing)  
- Serial-based virtual input trigger  
- Automatic timeout transitions per state  
- Demonstrated real-time reactive embedded scheduling  

**Demo:**  
[![Solution Video](https://img.youtube.com/vi/TrUIcJQiAgE/0.jpg)](https://youtu.be/TrUIcJQiAgE)


**Folder:** [`Project1/`](./Project1/)

---

### 🔹 [Project 2: IMU-Based Lying Posture Detection Using Arduino Nano 33 BLE Sense](./Project2/)

**Description:**  
This project implements a complete sensing-to-classification pipeline using the onboard **BMI270 IMU**.  
Accelerometer data is collected for **supine, prone, and side-lying** postures, analyzed offline, and used to design a simple, data-driven threshold classifier.  
The real-time Arduino system communicates detected posture via **LED blink patterns**:

- ✅ *1 blink* → Supine  
- ✅ *2 blinks* → Prone  
- ✅ *3 blinks* → Side (Left or Right)

**Features:**  
- IMU data acquisition and CSV logging  
- Data visualization using Python (pandas, matplotlib)  
- Threshold-based classification from real measurements  
- Real-time posture detection with on-board LED output  

**Demo:**  
[![Solution Video](https://img.youtube.com/vi/y1_X-Y1JYJQ/0.jpg)](https://youtu.be/y1_X-Y1JYJQ)

**Folder:** [`Project2/`](./Project2/)


---

### 🔹 [Project 3: Design, Data Collection, and Neural Network-Based Classification of Human Postures Using IMU Signals](./Project3/)

**Description:**  
An end-to-end supervised machine learning pipeline for human posture classification using IMU data.  
The project includes data collection, dataset construction, feature engineering, neural network training, and evaluation.

**Features:**  
- Controlled posture-specific data collection  
- Feature extraction from IMU signals  
- Neural network training with multiple activation functions  
- Accuracy and robustness evaluation  
- Emphasis on explainable ML for embedded systems  

**Folder:** [`Project3/`](./Project3/)

---

### 🔹 [Project 4: Real-Time, Sensor-Agnostic Posture Classification on the Arduino Nano 33 BLE Sense Using IMU Windowing, Normalization, and TensorFlow Lite Micro](./Project4/)

**Description:**  
A real-time embedded posture classification system capable of classifying **five postures**  
(supine, prone, side-lying, sitting, unknown) using **any three-axis IMU sensor**.

The system is sensor-agnostic and supports accelerometer, gyroscope, or magnetometer inputs, with real-time inference deployed using **TensorFlow Lite Micro**.

**Features:**  
- Multimodal IMU data collection  
- Windowing and normalization for real-time inference  
- Lightweight neural network optimized for embedded deployment  
- BLE-based live streaming of predictions  
- >90% test accuracy across sensor combinations  

**Demo Video:**  
[![Watch the demo](https://img.youtube.com/vi/NzS9q4fE_0s/maxresdefault.jpg)](https://youtu.be/NzS9q4fE_0s)


**Folder:** [`Project4/`](./Project4/)

---

### 🔹 [Project 5: Real-Time Embedded Keyword Spotting of Absolutist Language on Arduino Nano 33 BLE Sense](./Project5/)

**Description:**  
A full **TinyML audio pipeline** that performs real-time keyword spotting directly on a microcontroller.  
The system detects five custom *absolutist-language* keywords—**“never”, “none”, “all”, “must”, “only”**—and distinguishes them from silence and unknown speech.

The model runs continuously on-device and provides immediate visual feedback using the onboard RGB LED.

**Features:**  
- On-device audio acquisition at 16 kHz  
- Spectrogram / MFCC-based feature extraction  
- CNN (tiny_conv) trained and deployed with TensorFlow Lite Micro  
- INT8 post-training quantization (~74% model size reduction)  
- Real-time inference with application-level threshold tuning  
- ~95% test accuracy with stable real-time performance  

**Demo Video:**  
[![YouTube Demo Video](https://img.youtube.com/vi/gCpXxAnSdtk/hqdefault.jpg)](https://youtu.be/gCpXxAnSdtk)


**Folder:** [`Project5/`](./Project5/)

---

### 🔹 [Project 6: Food-11 Image Classification Using Convolutional Neural Networks and Transfer Learning](./Project6/)

**Description:**  
An exploration of deep learning techniques for image classification using the **Food-11 dataset**.  
The project compares baseline CNN models with **ShuffleNet-based transfer learning** to improve generalization and reduce overfitting.

**Features:**  
- CNN-based image classification  
- Transfer learning with ShuffleNet  
- Regularization and optimization techniques  
- Final test accuracy: **80.31%** (exceeding course benchmark)  

**Folder:** [`Project6/`](./Project6/)

---

## Tools & Technologies  

- **Hardware:** Arduino Nano 33 BLE Sense Rev2 (nRF52840 ARM Cortex-M4F)  
- **ML Frameworks:** TensorFlow, TensorFlow Lite Micro  
- **Software:** Arduino IDE 2.x, Python, Google Colab  
- **Languages:** C++, Python  
- **Sensors:** IMU, Microphone  
- **Version Control:** Git & GitHub  

---

## Learning Outcomes  

Through these projects, I developed hands-on experience in:

- Embedded systems design and finite state machines  
- Sensor data acquisition and annotation  
- Feature engineering for time-series and audio data  
- Neural network training and evaluation  
- Model optimization and quantization for embedded devices  
- Real-time inference on microcontrollers  
- System-level debugging and performance tuning  

---

## About the Author  

**Anushka Satav**  
- M.S. in Robotics & Autonomous Systems (AI), Arizona State University  
- Interests: Embedded AI, TinyML, robot intelligence, perception systems  
- Skills: **C++, Python, ROS2, Sensors, Control, Embedded Machine Learning**

📫 Connect with me:  
[LinkedIn](https://www.linkedin.com/in/anushka-satav/) | [GitHub](https://github.com/anushkasatav)

---

⭐ *If you found these projects helpful or interesting, please consider starring the repository!*

