# Project 1: Embedded State Machine using Arduino Nano 33 BLE Sense

**Name:** Anushka Gangadhar Satav  
**Course:** BMI/CES 598 Embedded Machine Learning  
**Project:** 01  

---

## 1. Problem Statement

Embedded systems form the computational backbone of intelligent environments, ranging from wearable devices to autonomous robotics. These systems are inherently reactive, continuously observing their environment through sensors and actuating responses based on programmed logic.  
A **Finite State Machine (FSM)** provides a formal framework for describing such behavior, where a system’s current state and input events determine its next state.

The objective of this project is to design and implement an FSM on the **Arduino Nano 33 BLE Sense**, using the onboard **RGB LED** to visually represent system states and the **Serial Monitor input** as a virtual actuator. The FSM models a cyclic color-transition sequence governed by both manual input events and time-driven transitions.

### Expected Behavioral Model
- **RED → DARK** after 5 seconds  
- **BLUE → RED** after 4 seconds  
- **GREEN → BLUE** after 3 seconds  


This exercise demonstrates the integration of **temporal logic**, **software debouncing**, and **reactive scheduling** in low-power embedded controllers.

---

## 2. State Transition Overview

Figure 1. State Transition Diagram

<img width="638" height="278" alt="image" src="https://github.com/user-attachments/assets/825457cf-08a8-4a6e-b94f-43eac78513cf" />


1. Initially, the system starts in the **DARK** state (LED off).  
2. From **DARK**, pressing the virtual switch transitions to **RED**.  
3. In **RED**, the LED automatically returns to **DARK** after 5 seconds.  
4. Pressing the switch again transitions from **RED → BLUE**.  
5. In **BLUE**, the LED automatically transitions to **RED** after 4 seconds.  
6. Pressing the switch transitions **BLUE → GREEN**.  
7. In **GREEN**, the LED automatically transitions to **BLUE** after 3 seconds.  
8. Pressing the switch in **GREEN** transitions back to **DARK**.

---

## 3. Hardware Configuration

| Component | Description |
|------------|-------------|
| **Microcontroller** | Arduino Nano 33 BLE Sense Rev2 |
| **Processor** | nRF52840 SoC – ARM Cortex-M4F @ 64 MHz |
| **Memory** | 1 MB Flash, 256 KB SRAM |
| **Power** | USB 5 V (regulated to 3.3 V I/O) |
| **Integrated Peripherals** | IMU (9-axis), Microphone, APDS9960 Light + Gesture Sensor |
| **Output Device** | Onboard RGB LED (common anode) |
| **Pin Mapping** | R → Pin 22, G → Pin 23, B → Pin 24 |
| **Input Mechanism** | Serial Monitor (‘C’ key) acting as soft switch |

---

## 4. Software Configuration

- **IDE:** Arduino IDE 2.x  
- **Core:** Arduino mbed-enabled board (Nano 33 BLE Sense Rev2)  
- **Language:** C++ (Arduino Sketch)  
- **Communication Interface:** USB Serial @ 115200 bps  
- **Timing Framework:** `millis()`-based non-blocking scheduling  
- **Operating Environment:** Windows 11 / macOS / Linux  

---

## 5. Code Implementation

[View Code]()

---

## 6. System Design Rationale

The FSM logic is abstracted through an indexed array (`states[]`) and a pointer variable (`stateIndex`). The `loop()` function continuously monitors elapsed time and serial inputs, maintaining **non-blocking concurrency**.

- **Temporal Control:** Implemented using `millis()` for deterministic timing.  
- **Manual Events:** Simulated via serial keypress (‘C’) to emulate interrupt-like behavior.  
- **Synchronization:** Each transition resets `lastChange` to maintain precise timing.  
- **Hardware Abstraction:** `setColorLED()` isolates LED control for modular debugging.

This hybrid reactive design — blending **event-driven** and **time-triggered** mechanisms — models real-world embedded architectures like motor control, sensor-fusion systems, and robotic behavior modules.

---

## 7. Experimental Design and Validation

### Case 1: Manual Transition Validation  
- **Procedure:** Sequentially press ‘C’.  
- **Expected Sequence:** DARK → RED → BLUE → GREEN → DARK  
- **Observation:** LED transitions matched serial output logs.

### Case 2: Automatic Timeout Validation  
- **Procedure:** No input during operation.  
- **Expected:**  
  - RED → DARK (5 s)  
  - BLUE → RED (4 s)  
  - GREEN → BLUE (3 s)  
- **Observation:** Automatic transitions verified.

### Case 3: Mixed Reactive Mode  
- **Procedure:** Interleave manual inputs during automatic timeouts.  
- **Result:** Immediate state changes and timer resets — no deadlocks or drift.

---

## 8. Solution Demonstration Links
 
[![Watch the video](https://img.youtube.com/vi/hP2Bn6jy1pI/0.jpg)](https://youtu.be/hP2Bn6jy1pI)




## 9. Observations, Challenges, and Lessons Learned

1. Reinforced understanding of **reactive embedded control** and **finite state abstraction**.  
2. Main challenge: maintaining timing precision while processing asynchronous serial events.  
3. Modular design improved debugging and future scalability.  
4. Demonstrated robustness against timing conflicts through consistent timer resets.  
5. Architecture extensible to **multi-sensor FSMs** (e.g., IMU or gesture-triggered transitions).  

---

## 10. References and Helpful Resources

1. [Arduino Nano 33 BLE Sense Documentation](https://docs.arduino.cc/hardware/nano-33-ble-sense)  
2. [RGB LED Pin Mapping Discussion](https://forum.arduino.cc/t/rgb-led-on-nano-33-ble/671327)  
3. FSM Concepts in Embedded Control Systems – *IEEE Access, Vol. 9 (2022), pp. 10045–10058*  
4. [Arduino Timing Function Reference](https://www.arduino.cc/en/Tutorial/BuiltInExamples/BlinkWithoutDelay)  

---

## 11. Conclusion

1. Successfully implemented a deterministic, reactive FSM on a constrained embedded platform.  
2. Integrated time-based and user-triggered transitions to emulate intelligent system behavior.  
3. Achieved predictable and scalable state management suitable for embedded ML extensions.  
4. Provides a foundational framework for **sensor-driven** and **gesture-responsive** systems in future work.

---
