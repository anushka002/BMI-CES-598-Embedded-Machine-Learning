# Project 5: Real-Time Embedded Keyword Spotting of Absolutist Language on Arduino Nano 33 BLE Sense  
**Author:** Anushka Gangadhar Satav  
**Course:** BMI/CES 598 Embedded Machine Learning  
**ASU ID:** 1233530170 (asatav1)  

---

## Demo Video  

[![YouTube Demo Video](https://img.youtube.com/vi/gCpXxAnSdtk/hqdefault.jpg)](https://youtu.be/gCpXxAnSdtk)

---

## Abstract  
The primary goal of this project was to design, train, and deploy a lightweight machine learning model capable of real-time keyword spotting (KWS) on a resource-constrained microcontroller, specifically the Arduino Nano 33 BLE Sense. Unlike general speech recognition systems, the model was trained to detect a set of **five absolutist-language keywords**: **“never”, “none”, “all”, “must”, and “only”**, while distinguishing these from **silence** and **unknown** speech.

The final embedded system continuously listens to incoming audio using the onboard microphone, generates spectrogram-based feature fingerprints, performs low-latency inference using a quantized TensorFlow Lite Micro CNN, and provides real-time feedback by illuminating the onboard RGB LED with a color associated with the detected keyword.

---

## Contents  
- **A. System Design**  
- **B. Experiment**  
- **C. Algorithm**  
- **D. Results**  
- **E. Discussion**

---

# A. System Design  

## Motivation  
This project explores the feasibility of creating an always-listening, low-power embedded system capable of detecting *absolutist language markers*, which are linked in research to mental health indicators. While not intended for diagnosis, the system demonstrates the potential for future privacy-preserving, form-factor-restricted devices (e.g., necklace, pen) capable of on-device speech pattern analysis.

The KWS model is trained to recognize the five target keywords: **never, none, all, must, only**, chosen because of their known correlation to absolutist speech styles.

Running entirely on the Arduino Nano 33 BLE Sense, this system maintains strict memory, compute, and latency constraints typical of embedded applications.

## High-Level Design  
The system performs the following pipeline:

1. Capture 16 kHz audio using the onboard PDM microphone  
2. Convert audio to spectrogram-based “fingerprint” features  
3. Run real-time CNN inference using TensorFlow Lite Micro  
4. Smooth results using the RecognizeCommands algorithm  
5. Trigger RGB LED feedback for keyword detection  

### Components  
**Arduino Nano 33 BLE Sense**  
- Captures 16 kHz audio  
- Runs preprocessing & spectrogram generation  
- Executes the tiny_conv CNN  
- Sends serial debugging output  
- Controls RGB LED responses  

**Google Colab (Training Environment)**  
- Model development, training, evaluation  
- Post-training quantization (int8)  
- Converts `.tflite` model into a deployable C-array  

## Observations and Difficulties  
- Tight memory constraints required **full int8 quantization** and use of TensorFlow’s **tiny_conv** model.  
- Initial crashes were traced to insufficient **tensor arena size**, fixed by increasing from **10 KB → 14 KB**.  
- Real-world detection confidence was lower than test confidence, requiring **threshold tuning** (200 → ~130–150).  

## Hardware & Software  
**Hardware:**  
- Arduino Nano 33 BLE Sense  
- USB cable  
- Laptop running Colab + Arduino IDE  

**Software:**  
- Arduino IDE 2.3.2  
- Arduino_TensorFlowLite library  
- Python (TensorFlow, Keras, NumPy)  

---

# B. Experiment  

## Goals and Constraints  
A 7-class audio classifier was constructed using:  
- Five absolutist keywords  
- `_unknown_` (non-target speech)  
- `_background_noise_`  

These were combined with portions of the **Google Speech Commands dataset** to ensure robustness.

## 1. Audio Data Collection  
The final dataset contained **3,970 .wav files**, standardized to **16 kHz**, **1-second** duration.  
Structured into 7 folders corresponding to the classes.

## 2. Data Augmentation & Splitting  
Using six long `_background_noise_` audio files, randomized noise overlays were applied to simulate realistic conditions.

Dataset split:  
- **Training:** 80% (~3,176 files)  
- **Validation:** 10% (~397 files)  
- **Testing:** 10% (~397 files)  

## 3. Challenges  
A major challenge was a **Google Colab file-system hang** during dataset scanning. This was solved by executing a “priming” debug block that enumerated files before launching the training script.

---

# C. Algorithm  

## 1. Pre-processing & Feature Extraction  
Spectrogram features derived from:  
- Window length: **30 ms**  
- Stride: **20 ms**  
- MFCC bins: **40**  

Produces a **49 × 40** feature “image”.

## 2. Model Architecture  
TensorFlow’s **tiny_conv** CNN:  
- Conv2D (8×4 kernel, 8 filters) + MaxPool  
- Conv2D (4×4 kernel, 16 filters) + MaxPool  
- Flatten  
- Dense (16 units)  
- Softmax output (7 classes)

## 3. Training Process  
- Steps: **15,000**  
- Optimizer: **Adam**  
- LR: **0.001**  
- Loss: Categorical cross-entropy  
- Metrics: accuracy (train/val/test)

## 4. Post-Training Quantization  
The 32-bit model (~116 KB) was quantized to **int8** (~29.5 KB), achieving a **74% reduction** with **no loss in accuracy**, using a representative dataset of 100 real samples.

---

# D. Results  

## Offline Performance  
Observed using TensorBoard:

- **Training Accuracy:** → 97%  
- **Validation Accuracy:** stable, beginning at 88.89%  
- **Test Accuracy:** **95.0%**  
- **Quantized Model Accuracy:** **95.01%**

## Training Curves  
*(Insert Figures in final PDF submission)*  
- Figure 1: Training vs Validation Accuracy  
- Figure 2: Training vs Validation Loss  

## Real-Time Prediction and Demo  
Deployed to the Arduino Nano 33 BLE Sense using the `anushka-speech-recognition.ino` sketch.

### LED Color Mapping  
| Keyword  | LED Color |
|----------|-----------|
| never    | Red       |
| none     | Blue      |
| all      | Green     |
| must     | Cyan      |
| only     | Pink      |
| unknown  | White     |
| silence  | Yellow    |

## Real-Time Performance  
Initially poor due to `detection_threshold = 200`, while real-world scores were in the **140–160** range.  
Lowering threshold to **130–150** restored real-time detection reliability.

### Solution Demonstration Links  
- **YouTube Video:** https://youtu.be/gCpXxAnSdtk  

---

# E. Discussion  

## Conclusion  
This project successfully:  
- Trained a **95% accurate** CNN for five absolutist keywords  
- Achieved **74% model size reduction** via int8 quantization  
- Deployed working inference on Arduino Nano 33 BLE Sense  
- Demonstrated real-time LED-based keyword detection  

## Challenges Encountered  
- **Colab File-System Hang** during dataset scanning  
- **Quantization Graph Pollution** from augmentation code  
- **Memory Crashes** requiring tensor arena size adjustments  
- **Low Real-World Confidence**, requiring careful threshold tuning  

## Future Improvements  
- Collect significantly **more diverse custom data**  
- Expand `_unknown_` dataset  
- Conduct systematic threshold sweeps  
- Explore noise-robust architectures or data augmentation strategies  

## Key Takeaway  
**High test accuracy does not guarantee real-world performance.**  
Embedded ML requires tuning the entire end-to-end system — preprocessing, quantization, smoothing, thresholds, and hardware behavior — to achieve reliable real-time results.


## Appendices
### Appendix A - File inventory (key files)

Python Scripts & Colab Notebooks

ANUSHKA-BMI598-PROJECT05.ipynb
The primary Google Colab notebook containing the full offline TinyML pipeline:
•	Loads the combined 7-class dataset
•	Performs dataset preprocessing and augmentation
•	Defines and trains the tiny_conv CNN for 12,000 steps
•	Logs accuracy and loss for TensorBoard
•	Performs INT8 post-training quantization
•	Generates the deployable C-array model file for Arduino
•	Exports both float32 and int8 .tflite models

Arduino Code & Model Artifacts

anushka-speech-recognition.ino
The main Arduino sketch used for real-time inference.
Modifications include:
•	Increasing kTensorArenaSize to 90 * 1024 bytes
•	Setting the tuned detection threshold using:
static RecognizeCommands static_recognizer(1000, 130, 1000, 3)
•	Implementing continuous audio streaming + inference loop
•	Integrating TFLM runtime and prediction smoothing

micro_features_model.h / micro_features_model.cc
•	Stores the compiled TFLite Micro model as a C-array
•	micro_features_model.cc replaced with the exported
speech_recognition_final_model.cc from Colab
•	Model array variable renamed to g_model for TFLM compatibility

micro_features_micro_model_settings.h / micro_features_micro_model_settings.cpp
Updated to reflect the 7-class taxonomy

arduino_command_responder.cpp / arduino_command_responder.h
Contains the LED response logic. Updated to:
•	Map each of the 7 classes to a specific RGB LED color
•	Handle silence and unknown cases
•	Allow quick visual debugging during real-time testing

recognize_commands.cpp
Modified to:
•	Enable detailed debugging via #define DEBUG_MICRO_SPEECH
•	Print all 7 class scores on one atomic line to avoid serial interference
•	Support tuned detection threshold and smoothing windows


Dataset and Results Files

combined_dataset/
Contains all 3,970 .wav files, organized in 7 subfolders:
•	never/
•	none/
•	all/
•	must/
•	only/
•	unknown/
Includes custom recordings + ASU EML Audio Dataset entries.

models/model.tflite
Final int8 quantized TFLite model (~29 KB).
models/float_model.tflite
Original float32 TFLite model (~116 KB).
Used for size/performance comparison.

logs/
TensorBoard training logs containing:
•	Training accuracy curve
•	Validation accuracy curve
•	Training/testing cross-entropy loss curves


### Appendix B: Run The Pipeline

The full workflow runs in three phases:

1. Data Collection & Preparation: in Google Colab
•	Upload all .wav files to your combined_dataset/ directory
•	Ensure consistent naming + folder structure
•	Convert any .ogg custom recordings into .wav
•	Verify 7-class directory layout for training

2. Train & Convert Model: ANUSHKA-BMI598-PROJECT05.ipynb
Running the notebook performs:
•	Dataset loading & preprocessing
•	MFCC/spectrogram feature generation
•	Training tiny_conv for 15,000 steps
•	Saving TensorBoard logs
•	Running representative_dataset_gen for quantization
•	Exporting the final speech_recognition_final_model.cc C-array
This file is then downloaded to your local machine for Arduino deployment.

3. Deploy & Test Model: on Arduino Nano 33 BLE Sense
Steps:
1.	Open anushka-speech-recognition.ino in Arduino IDE
2.	Replace micro_features_model.cc with
speech_recognition_final_model.cc content
3.	Update class labels in
micro_features_micro_model_settings.h/cpp
4.	Update LED logic in arduino_command_responder.cpp
5.	Set: kTensorArenaSize = 90 * 1024.
6.	static RecognizeCommands static_recognizer(1000, 130, 1000, 3);
7.	Upload to the board
8.	Test by speaking each keyword and observing:
•	RGB LED colors
•	Serial Monitor scores for all 7 classes

### Appendix C: References
1.	Arduino Nano 33 BLE Sense Rev2 Documentation: https://docs.arduino.cc/hardware/nano-33-ble-sense/ (Used for IMU initialization and library reference.)
2.	TensorFlow Lite Micro:  micro_speech Example
https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech , https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/examples/micro_speech/train/train_micro_speech_model.ipynb
3.	Google Speech Commands Dataset
https://www.tensorflow.org/datasets/catalog/speech_commands
4.	TensorFlow. “Keras API Documentation.” https://www.tensorflow.org/api_docs/python/tf/keras
5.	ASU EML Audio Dataset (Provided in course materials)
6.	Open Speech Recording Tool: https://github.com/petewarden/open-speech-recording
7.	Course Material: BMI/CEN 598: Embedded Machine Learning, Arizona State University (Fall B 2025). 
8.	Personal Data Collection & Analysis: All thresholds and algorithm decisions were derived from the measured IMU data collected during this project.

Appendix D: Use of AI Tools
The development process utilized large language models to enhance efficiency and troubleshoot complex technical challenges. ChatGPT and Gemini Pro models were specifically employed for:
•	Debugging and Error Resolution: Identifying and correcting errors in data processing scripts and Arduino inference code.
•	Code Modification and Generation: Assisting with the structure and syntax of specialized functions, such as serial communication protocols and TFLM C array handling.



