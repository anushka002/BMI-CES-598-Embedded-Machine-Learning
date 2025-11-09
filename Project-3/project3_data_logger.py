# project3_data_logger.py
# ------------------------------------------------------------
# Automated IMU Data Logger for Project 3
# Records accelerometer, gyroscope, and magnetometer data
# into separate CSV files for 30 seconds.
#
# Output:
#   data/<posture>_accl.csv
#   data/<posture>_gyro.csv
#   data/<posture>_mag.csv
#
# Author: Anushka Satav
# ------------------------------------------------------------

import serial, time, os

PORT = "COM11"  
BAUD = 115200
RECORD_TIME = 30   # seconds

# ---------------------------------------------
# Ask user for posture label
# ---------------------------------------------
posture = input("Enter posture label (supine/prone/side/sitting/unknown): ").strip()

# ---------------------------------------------
# Create output directory
# ---------------------------------------------
os.makedirs("data", exist_ok=True)

# Open files
f_acc  = open(f"data/{posture}_accl.csv", "w")
f_gyro = open(f"data/{posture}_gyro.csv", "w")
f_mag  = open(f"data/{posture}_mag.csv", "w")

# Write headers
f_acc.write("time_ms,ax,ay,az\n")
f_gyro.write("time_ms,gx,gy,gz\n")
f_mag.write("time_ms,mx,my,mz\n")

# ---------------------------------------------
# Connect to serial port
# ---------------------------------------------
ser = serial.Serial(PORT, BAUD)
time.sleep(2)

print(f"\nRecording will run for {RECORD_TIME} seconds...")
start_time = time.time()

# ---------------------------------------------
# Logging Loop
# ---------------------------------------------
try:
    while True:
        now = time.time()
        elapsed = now - start_time

        if elapsed >= RECORD_TIME:
            print("\n Time limit reached. Stopping logging...")
            break

        # Show countdown
        if int(elapsed) % 5 == 0:
            print(f"Elapsed: {int(elapsed)}s", end="\r")

        line = ser.readline().decode().strip()
        if not line:
            continue
        
        if line.startswith("time_ms"): # To skip header lines from duplicating 
            continue  

        parts = line.split(',')
        if len(parts) != 10:
            continue

        t, ax, ay, az, gx, gy, gz, mx, my, mz = parts

        # Write to individual CSVs
        f_acc.write(f"{t},{ax},{ay},{az}\n")
        f_gyro.write(f"{t},{gx},{gy},{gz}\n")
        f_mag.write(f"{t},{mx},{my},{mz}\n")

except KeyboardInterrupt:
    print("\n Interrupted by user.")

finally:
    f_acc.close()
    f_gyro.close()
    f_mag.close()
    ser.close()
    print("Files closed and logging complete")
