# """
# project3_dataset_generation.py

# Creates an ML-ready dataset with 3 input features (composite x,y,z) per window
# and integer labels (1..5). Steps:
#  - Merge accelerometer, gyro, magnetometer CSVs by time_ms
#  - Compute global mean/std per sensor-axis and z-score each sensor axis
#  - Compose composite axes: X = ax_z + gx_z + mx_z, etc.
#  - Window the composite series (1s windows at Fs, 50% overlap)
#  - For each window compute mean(X), mean(Y), mean(Z) -> 3 features
#  - Augment by adding Gaussian noise (optional copies)
#  - Save X.npy (N,3) and y.npy (N,) and scaler params

# Assumptions:
#  - Data root folder "data" contains subfolders with names like:
#      supine/, prone/, left side/, right side/, sitting upward/, sitting downward/, unknown/
#  - In each posture folder there exist matching triples: *_accl.csv, *_gyro.csv, *_mag.csv
#  - Each CSV has a header and columns: time_ms,ax,ay,az  (gyro: time_ms,gx,gy,gz; mag: time_ms,mx,my,mz)
#  - Target sampling rate Fs is provided (50 Hz)
# """

"""
project3_dataset_generation.py

Adds:
 1. unscaled_final_dataset.csv (raw composite X,Y,Z without scaling/noise)
 2. 2D scatter plots of X_raw,Y_raw,Z_raw
 3. final_dataset.csv (scaled + noise + windowed)

"""

import os, glob, json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# ---------------- USER SETTINGS ----------------
DATA_ROOT = "data"
FS = 50
WINDOW_SEC = 1.0
STEP_SEC = 0.5
NOISE_STD = 0.02
NOISE_COPIES = 2

UNSCALED_CSV = "unscaled_final_dataset.csv"
OUT_CSV = "final_dataset.csv"
SCALER_JSON = "scaler_params.json"

WEIGHTS = {"acc":1.0, "gyro":1.0, "mag":1.0}

LABEL_MAP = {
    "supine": 1,
    "prone": 2,
    "side": 3,
    "sitting": 4,
    "unknown": 5
}

WINDOW_SAMPLES = int(WINDOW_SEC * FS)
STEP_SAMPLES = int(STEP_SEC * FS)

# ---------------- Helper ----------------
def infer_label(folder):
    f = folder.lower()
    if "supine" in f: return 1
    if "prone" in f: return 2
    if "side" in f or "left" in f or "right" in f: return 3
    if "sit" in f or "up" in f or "down" in f: return 4
    return 5

def find_triples(folder):
    acc = sorted(glob.glob(os.path.join(folder, "*accl*.csv")))
    gyro = sorted(glob.glob(os.path.join(folder, "*gyro*.csv")))
    mag = sorted(glob.glob(os.path.join(folder, "*mag*.csv")))
    return list(zip(acc, gyro, mag))

def zscore(arr, mean, std):
    return (arr - mean) / std

# ---------------- Load all raw data ----------------
folders = [d for d in os.listdir(DATA_ROOT) if os.path.isdir(os.path.join(DATA_ROOT, d))]
acc_all, gyro_all, mag_all = [], [], []
trials = []

for folder in folders:
    folder_path = os.path.join(DATA_ROOT, folder)
    triples = find_triples(folder_path)
    label = infer_label(folder)

    for a_f, g_f, m_f in triples:
        df_a = pd.read_csv(a_f)
        df_g = pd.read_csv(g_f)
        df_m = pd.read_csv(m_f)

        df = df_a.merge(df_g, on="time_ms").merge(df_m, on="time_ms")

        acc_all.append(df[["ax","ay","az"]].values)
        gyro_all.append(df[["gx","gy","gz"]].values)
        mag_all.append(df[["mx","my","mz"]].values)

        trials.append({"df": df, "label": label})

# stack for scaler
acc_all = np.vstack(acc_all)
gyro_all = np.vstack(gyro_all)
mag_all = np.vstack(mag_all)

# Save scaler
scaler = {
    "acc_mean": acc_all.mean(axis=0).tolist(),
    "acc_std": (acc_all.std(axis=0)+1e-12).tolist(),
    "gyro_mean": gyro_all.mean(axis=0).tolist(),
    "gyro_std": (gyro_all.std(axis=0)+1e-12).tolist(),
    "mag_mean": mag_all.mean(axis=0).tolist(),
    "mag_std": (mag_all.std(axis=0)+1e-12).tolist(),
    "weights": WEIGHTS
}
json.dump(scaler, open(SCALER_JSON,"w"), indent=2)

# ---------------- BUILD BOTH DATASETS ----------------

unscaled_rows = []   # raw composite features
scaled_features = [] # scaled window-means
scaled_labels = []

for trial in trials:
    df = trial["df"]
    label = trial["label"]

    # raw values
    acc_raw = df[["ax","ay","az"]].values
    gyro_raw = df[["gx","gy","gz"]].values
    mag_raw = df[["mx","my","mz"]].values

    # ----- RAW COMPOSITE (NO SCALING) -----
    X_raw = acc_raw[:,0] + gyro_raw[:,0] + mag_raw[:,0]
    Y_raw = acc_raw[:,1] + gyro_raw[:,1] + mag_raw[:,1]
    Z_raw = acc_raw[:,2] + gyro_raw[:,2] + mag_raw[:,2]

    # window raw
    n = len(X_raw)
    start = 0
    while start + WINDOW_SAMPLES <= n:
        end = start + WINDOW_SAMPLES
        unscaled_rows.append([
            X_raw[start:end].mean(),
            Y_raw[start:end].mean(),
            Z_raw[start:end].mean(),
            label
        ])
        start += STEP_SAMPLES

    # ----- NOW SCALED VERSION -----
    acc = zscore(acc_raw, np.array(scaler["acc_mean"]), np.array(scaler["acc_std"]))
    gyro = zscore(gyro_raw, np.array(scaler["gyro_mean"]), np.array(scaler["gyro_std"]))
    mag = zscore(mag_raw, np.array(scaler["mag_mean"]), np.array(scaler["mag_std"]))

    Xc = acc[:,0] + gyro[:,0] + mag[:,0]
    Yc = acc[:,1] + gyro[:,1] + mag[:,1]
    Zc = acc[:,2] + gyro[:,2] + mag[:,2]

    n = len(Xc)
    start = 0
    while start + WINDOW_SAMPLES <= n:
        end = start + WINDOW_SAMPLES
        X_mean = Xc[start:end].mean()
        Y_mean = Yc[start:end].mean()
        Z_mean = Zc[start:end].mean()

        base_feat = np.array([X_mean, Y_mean, Z_mean])

        scaled_features.append(base_feat)
        scaled_labels.append(label)

        for _ in range(NOISE_COPIES):
            noise = np.random.normal(0, NOISE_STD, size=3)
            scaled_features.append(base_feat + noise)
            scaled_labels.append(label)

        start += STEP_SAMPLES

# ---------------- SAVE UNSCALED DATASET ----------------
df_unscaled = pd.DataFrame(unscaled_rows, columns=["X","Y","Z","label"])
df_unscaled.to_csv(UNSCALED_CSV, index=False)
print("\n✅ Saved:", UNSCALED_CSV)
print(df_unscaled.label.value_counts())

# ---------------- SAVE FINAL (SCALED) DATASET ----------------
df_final = pd.DataFrame({
    "X": [f[0] for f in scaled_features],
    "Y": [f[1] for f in scaled_features],
    "Z": [f[2] for f in scaled_features],
    "label": scaled_labels
})
df_final[["X","Y","Z"]] = df_final[["X","Y","Z"]].round(5)
df_final.to_csv(OUT_CSV, index=False)
print("\n✅ Saved:", OUT_CSV)
print(df_final.label.value_counts())

# ---------------- PLOTS (Unscaled data) ----------------

# ---------------- BEAUTIFUL 3D SCATTER PLOT ----------------
from mpl_toolkits.mplot3d import Axes3D

label_names = {
    1: "Supine",
    2: "Prone",
    3: "Side",
    4: "Sitting",
    5: "Unknown"
}

colors = {
    1: "red",
    2: "blue",
    3: "green",
    4: "purple",
    5: "orange"
}

fig = plt.figure(figsize=(12, 9))
ax = fig.add_subplot(111, projection='3d')

for label in sorted(df_unscaled["label"].unique()):
    sub = df_unscaled[df_unscaled["label"] == label]
    ax.scatter(
        sub["X"], sub["Y"], sub["Z"],
        s=12, alpha=0.6,
        color=colors[label],
        label=label_names[label]
    )

ax.set_title("3D Feature Distribution (Unscaled Composite Axes)", fontsize=14)
ax.set_xlabel("X_raw")
ax.set_ylabel("Y_raw")
ax.set_zlabel("Z_raw")

ax.legend(title="Posture Class", fontsize=10)
plt.tight_layout()
plt.savefig("scatter_3d_unscaled.png", dpi=300)
plt.show()

print(" Saved: scatter_3d_unscaled.png")


# ------------------------------------------------------------

# ---------------- BEAUTIFUL 3D SCATTER PLOT (SCALED DATASET) ----------------
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Load the scaled dataset
df_scaled = pd.read_csv("final_dataset.csv")

# Label names & colors
label_names = {
    1: "Supine",
    2: "Prone",
    3: "Side",
    4: "Sitting",
    5: "Unknown"
}

colors = {
    1: "red",
    2: "blue",
    3: "green",
    4: "purple",
    5: "orange"
}

# Create 3D scatter
fig = plt.figure(figsize=(12, 9))
ax = fig.add_subplot(111, projection='3d')

for label in sorted(df_scaled["label"].unique()):
    sub = df_scaled[df_scaled["label"] == label]
    ax.scatter(
        sub["X"], sub["Y"], sub["Z"],
        s=12, alpha=0.6,
        color=colors[label],
        label=label_names[label]
    )

ax.set_title("3D Feature Distribution (Scaled Composite Axes)", fontsize=14)
ax.set_xlabel("X_scaled")
ax.set_ylabel("Y_scaled")
ax.set_zlabel("Z_scaled")

ax.legend(title="Posture Class", fontsize=10)
plt.tight_layout()
plt.savefig("scatter_3d_scaled.png", dpi=300)
plt.show()

print("✅ Saved: scatter_3d_scaled.png")
# ------------------------------------------------------------