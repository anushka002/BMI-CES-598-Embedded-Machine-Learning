import pandas as pd
import matplotlib.pyplot as plt

"""
Analyze IMU accelerometer data for posture detection.
Adds comparison plots for all postures.
Handles:
- First non-numeric line
- Header row
- Mixed tabs/spaces
"""

# ----------------------------------------------------------
# 1. Choose your files
# ----------------------------------------------------------
files = {
    "supine": "supine_data.csv",
    "side_left": "left_data.csv",
    "side_right": "right_data.csv",
    "prone": "prone_data.csv"
}

results = {}
datasets = {}   # <-- store cleaned data for plotting

# ----------------------------------------------------------
# 2. Load + clean each dataset
# ----------------------------------------------------------
for posture, fname in files.items():

    df = pd.read_csv(
        fname,
        sep=r"\s+|\t|,",
        engine="python",
        skiprows=1   # Skip sample rate line
    )

    df.columns = ["time_ms", "ax", "ay", "az"]

    for col in ["time_ms", "ax", "ay", "az"]:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    df.dropna(inplace=True)

    df["time_s"] = df["time_ms"] / 1000.0
    datasets[posture] = df

    # Compute statistics
    results[posture] = {
        "mean_ax": df["ax"].mean(),
        "mean_ay": df["ay"].mean(),
        "mean_az": df["az"].mean(),
        "std_ax": df["ax"].std(),
        "std_ay": df["ay"].std(),
        "std_az": df["az"].std()
    }

# ----------------------------------------------------------
# 3. Print summarized results
# ----------------------------------------------------------
print("\n=== Posture Statistics ===")
for posture, stats in results.items():
    print(f"\nPosture: {posture}")
    for k, v in stats.items():
        print(f"{k}: {v:.3f}")

# ----------------------------------------------------------
# 4. Visualize mean values
# ----------------------------------------------------------
postures = list(results.keys())
mean_ax = [results[p]["mean_ax"] for p in postures]
mean_ay = [results[p]["mean_ay"] for p in postures]
mean_az = [results[p]["mean_az"] for p in postures]

plt.figure(figsize=(10, 5))
plt.plot(postures, mean_ax, marker="o", linewidth=2, label="ax mean")
plt.plot(postures, mean_ay, marker="o", linewidth=2, label="ay mean")
plt.plot(postures, mean_az, marker="o", linewidth=2, label="az mean")
plt.title("Mean Accelerometer Values for Each Posture")
plt.ylabel("Acceleration (g)")
plt.grid(True)
plt.legend()
plt.xticks(rotation=15)
plt.tight_layout()
plt.show()

# ----------------------------------------------------------
# 5. COMPARISON PLOT: All postures side-by-side
# ----------------------------------------------------------

fig, axs = plt.subplots(4, 1, figsize=(12, 12), sharex=True, sharey=True)

fig.suptitle("Comparison of Accelerometer Signals for All Postures", fontsize=16)

for idx, posture in enumerate(postures):
    df = datasets[posture]

    axs[idx].plot(df["time_s"], df["ax"], label="ax")
    axs[idx].plot(df["time_s"], df["ay"], label="ay")
    axs[idx].plot(df["time_s"], df["az"], label="az")

    axs[idx].set_title(posture.replace("_", " ").title())
    axs[idx].grid(True)
    axs[idx].legend(loc="upper right")

axs[-1].set_xlabel("Time (s)")
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
