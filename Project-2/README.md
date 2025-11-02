# Project 2: IMU-Based Lying Posture Detection Using Arduino Nano 33 BLE Sense

**Name:** Anushka Gangadhar Satav  
**Course:** BMI/CES 598 — Embedded Machine Learning  
**Project:** 02  
**ASU ID:** 1233530170 (asatav1)

---

## 1. Problem Statement

Lying posture plays an important role in assessing comfort, mobility, and clinical risk in sleep environments. Wearable or embedded systems that detect supine, prone, and side-lying positions can support early intervention for hospital-acquired pressure injuries and sleep-quality monitoring.

This project implements a complete embedded workflow using the **BMI270 IMU** on the **Arduino Nano 33 BLE Sense Rev2**. The system identifies lying postures using accelerometer data and communicates results through LED blink patterns:

- **1 blink → Supine**  
- **2 blinks → Prone**  
- **3 blinks → Side (Left or Right)**  

The assignment requires four phases:
1. IMU data collection  
2. Posture-specific data logging  
3. Data visualization & threshold design  
4. On-device detection using Arduino  

---

## 2. System Architecture Overview

### High-Level Workflow
1. Arduino streams raw accelerometer data `(ax, ay, az)` via Serial.  
2. Python script logs the data into posture-labeled CSV files.  
3. Analysis computes means, standard deviations, and generates plots.  
4. Arduino implements a real-time classifier using data-driven thresholds.

### Hardware Setup
| Component | Description |
|----------|-------------|
| Microcontroller | Arduino Nano 33 BLE Sense Rev2 |
| Sensor | BMI270 accelerometer |
| Output | Onboard LED (pin 13) |
| Interface | USB Serial (115200 bps) |

---

## 3. Experiment Design

### Data Collection
Each posture was recorded for ~30–45 seconds:

- `supine_data.csv`  
- `left_data.csv`  
- `right_data.csv`  
- `prone_data.csv`

Each file contained:  
`time_ms, ax, ay, az`

The IMU was placed on a stable flat surface to mimic chest orientation.

### Analysis Steps
Using Python:
- Load CSV data
- Clean headers & parse numeric fields
- Compute mean & standard deviation for each axis
- Generate:
  - Individual time-series plots
  - 4-panel posture comparison plot  
These visualizations directly informed the threshold-based classifier.

---

## 4. Algorithm Design

### Data-Driven Threshold Rules
| Posture | Dominant Axis | Rule |
|---------|---------------|------|
| Supine | +Z | `az > +0.70` |
| Prone | –Z | `az < –0.70` |
| Side-Left | +Y | `ay > +0.70` |
| Side-Right | –Y | `ay < –0.70` |

These were derived from inspection of measured means (≈ ±1 g) and low standard deviations.

### LED Output Mapping
| Posture | LED Pattern |
|---------|-------------|
| Supine | 1 blink |
| Prone | 2 blinks |
| Side | 3 blinks |
| Unknown | LED off |

The classifier updates posture at ~20–50 Hz.

---

## 5. Results

### Summary of Posture Statistics
| Posture | Mean (ax, ay, az) | Std (ax, ay, az) | Dominant |
|--------|--------------------|-------------------|-----------|
| Supine | (-0.098, -0.083, +0.960) | (0.132, 0.188, 0.059) | +Z |
| Side Left | (-0.067, +0.910, -0.206) | (0.087, 0.073, 0.294) | +Y |
| Side Right | (-0.040, -0.930, -0.248) | (0.049, 0.087, 0.243) | –Y |
| Prone | (-0.117, -0.011, -0.915) | (0.161, 0.357, 0.101) | –Z |

### Figures
**Figure 1:** Mean accelerometer values across postures  
**Figure 2:** Time-series comparison of ax/ay/az for all four postures  

### Demonstration
[![Solution Video](https://img.youtube.com/vi/y1_X-Y1JYJQ/0.jpg)](https://youtu.be/y1_X-Y1JYJQ)

---

## 6. Observations & Lessons Learned

1. Axis separation was clear and consistent, enabling simple thresholds.  
2. Accelerometer-only detection is sufficient for static posture recognition.  
3. Board orientation consistency significantly affects data clarity.  
4. The workflow (collection → analysis → deployment) mirrors real-world embedded ML pipelines.  

---

## 7. Appendices

### Appendix A — File Inventory
- `imudatacollection.ino` — IMU data streaming  
- `serial_logger.py` — Data logging  
- `positions_analysis.py` — Signal analysis & plotting  
- `posture_detection.ino` — Final classifier  
- Raw CSV datasets  
- Exported analysis plots  

### Appendix B — How to Run
1. Upload `imudatacollection.ino` → confirm Serial output  
2. Run `serial_logger.py` → save CSV for each posture  
3. Run `positions_analysis.py` → generate plots & stats  
4. Upload `posture_detection.ino` → observe LED output  

### Appendix C — References
1. Arduino Nano 33 BLE Sense Documentation  
2. Arduino BMI270 IMU Library Docs  
3. Python libraries: pandas, matplotlib, pyserial  
4. ASU BMI/CES 598 Course Resources  
5. IMU data collected by the author  

---

## 8. Conclusion

This project successfully implements a complete embedded posture detection system using the Arduino Nano 33 BLE Sense. The threshold-based algorithm reliably differentiates supine, prone, and side-lying postures using only accelerometer data. The workflow highlights core skills in embedded sensing, data annotation, offline analysis, and real-time microcontroller deployment.

