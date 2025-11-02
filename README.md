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

## Tools & Technologies  
- **Hardware:** Arduino Nano 33 BLE Sense Rev2 (nRF52840 ARM Cortex-M4F)  
- **Software:** Arduino IDE 2.x, Python  
- **Languages:** C++, Python  
- **Testing Tools:** Serial Monitor, IMU accelerometer, Onboard LED  
- **Version Control:** Git & GitHub  

---

## Learning Outcomes  
Through these projects, I developed strong practical skills in:  
- Designing reactive embedded architectures (FSMs)  
- Gathering and annotating sensor data from IMUs  
- Creating data-driven embedded algorithms  
- Implementing real-time classification on microcontrollers  
- Plotting and analyzing sensor signals for threshold selection  
- Writing modular embedded C++ and Python scripts  
- Validating designs through experiments and demonstration videos  

---

## Future Additions  
Upcoming projects may include:  
- Sensor fusion (IMU + vision + audio)  
- TinyML inference using TensorFlow Lite Micro  
- Gesture recognition on embedded hardware  
- Multi-modal embedded perception pipelines  

---

### About the Author  
**Anushka Satav**  
- M.S. in Robotics & Autonomous Systems (AI) @ Arizona State University  
- Interests: Robot intelligence, embedded AI, autonomous navigation, perception systems  
- Skills: **C++, Python, ROS2, Sensors, Control, Embedded ML**  

📫 *Connect with me:*  
[LinkedIn](https://www.linkedin.com/in/anushka-satav/) | [GitHub](https://github.com/anushkasatav)

---

⭐ *If you found the projects helpful or interesting, please consider starring the repository!*
