# Project 3: Design, Data Collection, and Neural Network-Based Classification of Human Postures Using IMU Signals  
**Author:** Anushka Gangadhar Satav  
**Course:** BMI/CES 598 – Embedded Machine Learning  
**ASU ID:** 1233530170 (asatav1)

---

## **Abstract**
This project presents an end-to-end supervised machine learning pipeline for detecting human postures using IMU data collected from the Arduino Nano 33 BLE Sense Rev2. The system follows the required project phases: (1) IMU data collection; (2) controlled posture-specific data generation; (3) dataset construction and feature engineering; (4) neural network training with multiple activation functions; and (5) evaluation of test accuracy and robustness. The pipeline emphasizes reproducibility, robust feature creation, data-driven design, and explainable ML behavior suitable for embedded applications.

---

## **Contents**
- A. System Design  
- B. Experiment  
- C. Algorithm  
- D. Results  
- E. Discussion  
- Appendices (code, data, instructions)

---

# **A. System Design**

### **Motivation**
Identifying human posture (supine, prone, side, sitting, unknown) is vital for applications such as sleep monitoring, fall-risk assessment, pressure-injury prevention, and smart-health systems. This project designs an embedded ML approach that processes tri-axial IMU data—acceleration, angular velocity, and magnetic field—to classify five posture categories.

### **High-Level Architecture**
1. **Embedded IMU acquisition** using accelerometer, gyroscope, and magnetometer.  
2. **Dataset construction** across multiple controlled trials and orientations.  
3. **Neural network classifier** trained on fused, windowed, noise-augmented IMU features.  
4. **Performance analysis** with Sigmoid, Tanh, and ReLU activations.

### **Hardware & Software**
- **Hardware:** Arduino Nano 33 BLE Sense Rev2 (BMI270 + BMM150), USB cable  
- **Arduino Library:** `Arduino_BMI270_BMM150.h`  
- **Python Packages:** `pyserial`, `pandas`, `numpy`, `matplotlib`  
- **ML Framework:** TensorFlow / Keras on Google Colab  

A fixed sampling frequency of **50 Hz** was achieved using controlled sampling (20 ms delay). Key challenges included sensor synchronization, orientation independence, multi-axis fusion, and dataset balance.

---

# **B. Experiment**

### **Goals and Constraints**
Data collection was performed **without wearing the sensor**, per project instructions. Instead, the board was placed manually in orientations that simulate real postures. Each posture was recorded for **approximately 60 seconds**, across multiple trials.

### **Postures Collected**
1. **Supine**  
2. **Prone**  
3. **Side** (combined left + right)  
4. **Sitting** (USB-port up + USB-port down)  
5. **Unknown** (leaning, rolling, arbitrary orientations)

### **1. IMU Data Collection**
Using `project3_imudatacollection.ino`, the following were logged at ~50 Hz:
- Accelerometer: `ax, ay, az`  
- Gyroscope: `gx, gy, gz`  
- Magnetometer: `mx, my, mz`  
- Timestamp: `time_ms`  

### **2. PC-Side Logging into Three Separate CSVs**
Using `project3_data_logger.py`, each trial produced:
- `<posture>_accl.csv`  
- `<posture>_gyro.csv`  
- `<posture>_mag.csv`

**Why three files?**
- Cleaner debugging  
- Easy to merge using `time_ms`  
- Modular if sensors need to be added or removed  

### **3. Synchronization and Merging**
All CSVs contain the same `time_ms` column, enabling perfect alignment of the three modalities into a single unified dataset with 10 columns per sample.

### **4. Dataset Generation Strategy (project3_dataset_generation.py)**

#### **Step 1 — Global Scaling (Z-Score Standardization)**
Each sensor axis is standardized using:  
`scaled = (x − mean) / std`  
Global mean and std values are saved in `scaler_params.json`.

#### **Step 2 — Composite Feature Construction**
Scaled accelerometer, gyroscope, and magnetometer axes are fused into:
- **X = ax + gx + mx**  
- **Y = ay + gy + my**  
- **Z = az + gz + mz**

This reduces 9 IMU features → 3 direction-based composite features.

#### **Step 3 — Windowing with 1-Second Windows**
- Window size: **50 samples**  
- Overlap: **50% (25 samples)**  

For each window, the mean of composite X/Y/Z is computed.

#### **Step 4 — Label Assignment**
Labels are inferred from folder names:

| Label | Posture |
|------|---------|
| 1 | Supine |
| 2 | Prone |
| 3 | Side |
| 4 | Sitting |
| 5 | Unknown |

#### **Step 5 — Gaussian Noise Augmentation**
Two noisy samples per window are created to mimic micro-movements.

#### **Step 6 — Final Dataset Output**
Two datasets generated:
- `unscaled_final_dataset.csv`  
- `final_dataset.csv`  

Figures (for report):
- **Figure 1:** Raw Composite Feature Distribution  
- **Figure 2:** Scaled Composite Feature Distribution  

---

# **C. Algorithm**

### **1. Input Features**
Each training example consists of:
- Composite **X**
- Composite **Y**
- Composite **Z**

### **2. Neural Network Architecture**
Built with TensorFlow / Keras Functional API:

- **Input:** 3 features  
- **Hidden Layer 1:** 64 neurons  
- **Hidden Layer 2:** 32 neurons  
- **Hidden Layer 3:** 16 neurons  
- **Output Layer:** 5-neuron SoftMax  

Loss: **categorical cross-entropy**  
Optimizer: **Adam**  
Metric: **accuracy**

### **3. Activation Functions Tested**
- Sigmoid  
- Tanh  
- ReLU  

Each model was trained with all hidden layers using the same activation.

### **4. Training Details**
Dataset split:
- 70% Training  
- 15% Validation  
- 15% Testing  

Hyperparameters:
- Epochs: **100**  
- Batch size: **32**  

---

# **D. Results**

Three models were trained for 100 epochs using Sigmoid, Tanh, and ReLU respectively.

### **Final Test Accuracy Comparison (100 Epochs)**

| Activation | Test Accuracy |
|------------|----------------|
| **ReLU** | **0.9801** |
| **Tanh** | **0.9765** |
| **Sigmoid** | **0.9657** |

ReLU produced the best performance.

---

## **1. Tanh Model — 97.65% Accuracy**

- Perfect results for Supine, Prone, Sitting  
- Minor overlap between Side and Unknown  
- Validation curves show stable convergence  

(Figure: Training/Validation Loss – Tanh)

---

## **2. ReLU Model — 98.01% Accuracy (Best)**

- Nearly perfect classification across all classes  
- Best robustness with Unknown category  
- Very stable training and fast convergence  

(Figure: Training/Validation Loss – ReLU)

---

## **3. Sigmoid Model — 96.57% Accuracy**

- Good for stable classes  
- Weaker performance on Unknown  
- Saturation effects reduced gradient flow  

(Figure: Training/Validation Loss – Sigmoid)

---

## **Validation Loss Comparison for All Activations**
(Figure: 3-Curve Validation Loss Plot)

---

### **Interpretation of Results**
- **Sigmoid** saturates and limits gradient flow → weaker generalization  
- **Tanh** performs better due to symmetric range (−1 to 1)  
- **ReLU** avoids saturation and supports strong gradient propagation → best accuracy  

---

# **E. Discussion / Conclusion**

This project successfully implemented a full supervised ML system for 5-class posture recognition using IMU data from the Arduino Nano 33 BLE Sense Rev2. The work addressed significant real-world challenges including sensor synchronization, multimodal fusion, and posture variability.

The pipeline incorporated:
- Sensor fusion (acc + gyro + mag)  
- Z-score scaling  
- 1s windowing with overlap  
- Noise augmentation  
- A compact, efficient neural network  
- Activation function comparison  

ReLU achieved the strongest performance (98.01% accuracy), making it the recommended activation for embedded posture recognition. Future work includes TensorFlow Lite deployment, real-time inference, and temporal models (LSTM, TCN).

---

# **Appendices**

## **Appendix A — File Inventory**

### Arduino Code
- `project3_imudatacollection.ino`

### Python Scripts
- `project3_data_logger.py`  
- `project3_dataset_generation.py`  
- 3D visualization scripts  

### Colab Notebook
- `project3_training.ipynb`

### Dataset Files
- `unscaled_final_dataset.csv`  
- `final_dataset.csv`  
- `scaler_params.json`  
- Raw posture folders with accel/gyro/mag CSVs  

---

## **Appendix B — Running the Pipeline**

### Collect IMU Data
```
Upload project3_imudatacollection.ino
```

### Log Data
```
python project3_data_logger.py
```

### Generate Dataset
```
python project3_dataset_generation.py
```

### Train in Colab
Open:
```
project3_training.ipynb
```

---

## **Appendix C — References**
1. Arduino Nano 33 BLE Sense Documentation  
2. Arduino BMI270/BMM150 IMU Library  
3. TensorFlow Keras API  
4. Course notes: BMI/CEN 598 – Embedded Machine Learning  
5. Personal recorded IMU datasets  

