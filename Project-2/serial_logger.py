import serial
import time

# ----------------------------
# USER SETTINGS
# ----------------------------
PORT = "COM11"           # Arduino COM port
BAUD = 115200
OUTPUT_FILE = "right_data.csv" # Output file name
# ----------------------------

print("Connecting to Arduino...")
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # Give Arduino time to reset

print(f"\nLogging data to: {OUTPUT_FILE}")
print("Press CTRL+C to stop.\n")

with open(OUTPUT_FILE, "w") as f:
    try:
        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if line:
                print(line)
                f.write(line + "\n")
    except KeyboardInterrupt:
        print("\n\nStopped logging.")
        ser.close()
