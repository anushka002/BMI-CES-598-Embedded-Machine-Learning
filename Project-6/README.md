# Project 6: Food-11 Image Classification Using Convolutional Neural Networks
Author: Anushka Gangadhar Satav  
Course: BMI/CES 598 Embedded Machine Learning  
ASU ID: 1233530170 (asatav1)

This project explores the application of deep-learning models—specifically Convolutional Neural Networks (CNNs) and ShuffleNet-based transfer learning—to perform image classification on the Food-11 dataset. The project is divided into four experimental phases, each introducing architectural improvements designed to enhance generalization, reduce overfitting, and optimize performance.

The final ShuffleNet transfer-learning model achieves 80.31% test accuracy, surpassing the course benchmark of 80%.

---

## Contents
- A. System Design  
- B. Experiment and Algorithms  
- C. Results  
- D. Discussion  
- Appendices (A–D)

---

# A. System Design

## Motivation
Food image classification enables a wide range of applications, including diet tracking, calorie estimation, smart kitchen automation, allergy detection, and educational tools for nutrition awareness. Machine learning models capable of identifying food from images can significantly reduce manual user input by automatically tagging food items and estimating nutritional information.

This project evaluates multiple CNN architectures and transfer learning strategies to build an efficient and accurate Food-11 classifier suitable for potential embedded deployment.

## High-Level Design
The classification pipeline evolves through four phases:

1. Phase 1 — Basic CNN  
   Baseline model using convolutional layers and fully connected layers.

2. Phase 2 — CNN + Pooling  
   Introduces MaxPooling to improve generalization and computational efficiency.

3. Phase 3 — CNN + Pooling + Dropout  
   Incorporates dropout (p=0.5) to address overfitting.

4. Phase 4 — ShuffleNet Transfer Learning  
   A pre-trained ShuffleNet V2 model is adapted for 11-class prediction by replacing the final layer.

All images are resized to 224x224, augmented (flip, rotation, color jitter), and normalized. Phases 1–3 use dataset-specific normalization; Phase 4 uses ImageNet normalization.

## Dataset
Food-11: 16,643 images, 11 classes  
Train/Validation/Test splits: 9,866 / 3,430 / 3,347 images  

Classes include bread, dairy product, dessert, egg, fried food, meat, noodles pasta, rice, seafood, soup, and vegetable fruit.

## Observations and Difficulties
- Phase 1 showed overfitting due to limited feature extraction ability.  
- Phase 2 improved accuracy but increased overfitting due to larger FC layers.  
- Phase 3 reduced overfitting through dropout but required hyperparameter tuning.  
- Phase 4 achieved exceptional generalization with a negative overfitting gap.  
- Challenges included GPU memory limits, SOL resource constraints, and troubleshooting ShuffleNet training stability.

---

# B. Experiment and Algorithms

## Experimental Setup
- Framework: PyTorch 2.4.1  
- Hardware: GPU (Tesla T4 on Google Colab)  
- Notebook: FINAL-Anushka-PROJECT-06.ipynb  
- Batch size: 32  
- Optimizer: Adam (lr=0.001)  
- Epochs: 15–30 depending on phase  

### Preprocessing
Phases 1–3:
- Resize 224x224  
- Random flip, rotation, color jitter  
- Normalize using dataset mean & std  

Phase 4:
- Resize 224x224  
- ImageNet normalization  
- Same augmentations  

---
Food11 Dataset after Pre-processing

<img width="616" height="624" alt="image" src="https://github.com/user-attachments/assets/2d8420f2-b76c-44bf-9f97-ed86a1544107" />


[Link To Dataset](https://www.kaggle.com/datasets/trolukovich/food11-image-dataset/data)

---

## Algorithms and Model Architectures

### Phase 1: Basic CNN
- Five Conv2D layers (stride=2)  
- Two fully connected layers (512 → 11)  
- Establishes baseline capability  
- Flattened feature size: 25,088  
- Train accuracy ~0.85; val ~0.78  
- Test accuracy: 52.67%  

### Phase 2: CNN + Pooling
- Four Conv2D layers with MaxPooling  
- Three fully connected layers  
- Train accuracy ~0.90; val ~0.75  
- Overfitting increased (0.1484 gap)  
- Test accuracy: 58.08%  

### Phase 3: CNN + Pooling + Dropout
- Same as Phase 2 but with Dropout(p=0.5)  
- Reduced overfitting (gap: 0.0958)  
- Test accuracy: 61.25%  

### Phase 4: ShuffleNet Transfer Learning
- Pretrained ShuffleNetV2_x1.0 from ImageNet  
- Replaced FC 1024→11  
- Frozen backbone (only ~11K parameters trained)  
- Overfitting gap: -0.0131  
- Test accuracy: 80.31%  

### Theory Notes
- MaxPooling improves feature invariance and reduces computation.  
- Dropout enhances generalization by preventing co-adaptation.  
- Transfer learning accelerates convergence and improves performance with limited data.  
- ShuffleNet uses channel shuffle and depthwise separable convolutions for mobile efficiency.

---

# C. Results

## Phase 1: Basic CNN
- Test Accuracy: 52.67%  
- Overfitting gap: 0.0745  
- Struggled with visually similar classes  

## Phase 2: CNN + Pooling
- Test Accuracy: 58.08%  
- Highest overfitting gap: 0.1484  
- Improved feature extraction but required regularization  

## Phase 3: CNN + Dropout
- Test Accuracy: 61.25%  
- Reduced overfitting  
- Better class separation  

## Phase 4: ShuffleNet Transfer Learning
- Test Accuracy: 80.31%  
- Negative overfitting gap (excellent generalization)  
- Strong diagonal in confusion matrix  

## Phase Comparison Table

| No. | Architecture | Overfitting Gap | Trainable Params | Epochs | Test Accuracy | Summary |
|----|--------------|----------------|------------------|--------|---------------|---------|
| 1 | Basic CNN | 0.0745 | ~1.5M | 15 | 52.67% | 5 Conv layers + 2 FC |
| 2 | CNN + Pooling | 0.1484 | ~1.2M | 15 | 58.08% | 4 Conv + MaxPool + 3 FC |
| 3 | CNN + Pooling + Dropout | 0.0958 | ~1.2M | 20 | 61.25% | Phase 2 + Dropout |
| 4 | ShuffleNet Transfer Learning | -0.0131 | ~11K | 30 | 80.31% | Pretrained ShuffleNet + custom FC |

Key Plots (saved in notebook):
- Overfitting Analysis  
- Test Accuracy Comparison  
- Accuracy/Loss Curves Across Phases  
- Confusion Matrices
  
---
Overfitting Analysis Bar Chart

<img width="696" height="413" alt="image" src="https://github.com/user-attachments/assets/6b8020c9-1f39-44c4-8156-888f664fd43b" />

---
Test Accuracy Bar Chart

<img width="700" height="406" alt="image" src="https://github.com/user-attachments/assets/cc6739c5-3249-45da-800c-1da7596f2d82" />

---
Accuracy/Loss Comparison Plots Across Phases

<img width="904" height="677" alt="image" src="https://github.com/user-attachments/assets/8318da98-414a-419d-8131-107d43e2f413" />

---
Evaluation Confusion Matrix for Phase 4 (Test accuracy = 80.31%)

<img width="695" height="619" alt="image" src="https://github.com/user-attachments/assets/3cfa17c4-b2b5-4abf-891f-a45e462513d7" />

---

# D. Discussion

## Key Findings
- Each architectural enhancement increased accuracy and reduced overfitting.  
- Transfer learning provided the most significant improvement (+19% over Phase 3).  
- ShuffleNet generalized extremely well with minimal trainable parameters.  

## Why ShuffleNet Was Best
- Pretrained ImageNet features  
- Efficient mobile architecture (depthwise convolution, channel shuffle)  
- Small number of trainable parameters prevented overfitting  

## Difficult Classes
Common misclassifications:
- Rice vs noodles  
- Meat vs fried foods  
- Bread vs dessert  
These pairs have strong visual similarity.

## Challenges
- Hyperparameter tuning for ShuffleNet  
- Low accuracy with initial SGD setup  
- Need for proper normalization and training length  

---

# Appendices

## Appendix A – File Inventory

### Jupyter Notebook
- FINAL-ANUSHKA-PROJECT-06.ipynb  
  - Loads dataset  
  - Trains all four phases  
  - Generates plots  
  - Saves model weights and training history  

### Dataset Files
Food-11 Directory:
- training  
- validation  
- testing  

### Saved Outputs
- phase1_basic_cnn.pth  
- phase2_pooling_cnn.pth  
- phase3_dropout_cnn.pth  
- phase4_shufflenet.pth  
- training_history.pkl  
- Comparison plots (PNG)

---

## Appendix B – How to Run the Pipeline

1. Load Food-11 dataset  
2. Compute dataset statistics (mean/std)  
3. Apply augmentations and normalization  
4. Train Phase 1 → Phase 4 sequentially  
5. Evaluate accuracy, loss, and confusion matrices  
6. Compare model performances  
7. Save outputs

---

## Appendix C – References
- Food-11 Dataset (Kaggle)  
- PyTorch Documentation  
- Torchvision ShuffleNet Model Docs  
- Torchvision Transforms  
- Course: BMI/CES 598 Embedded Machine Learning (ASU)

---

## Appendix D – Use of AI Tools
AI tools assisted with:

- Debugging optimizer issues in ShuffleNet  
- Fixing missing imports and shape mismatches  
- Refining training strategies  
- Organizing report structure  
- Clarifying architectural descriptions  

All modeling decisions, coding, and evaluations were performed by the student.

---


