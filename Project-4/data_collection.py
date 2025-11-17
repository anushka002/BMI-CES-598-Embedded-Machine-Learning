import serial
import csv
import time

PORT = "COM11"                   # Change this to your COM port
BAUD = 115200
OUTPUT_FILE = "prone.csv"   # Change filename per posture
RECORD_SECONDS = 240             #  Duration of recording (in seconds)

def main():
    ser = serial.Serial(PORT, BAUD)
    time.sleep(2)  # wait for Arduino reset

    print(f"Collecting IMU data for {RECORD_SECONDS} seconds...")

    start_time = time.time()

    with open(OUTPUT_FILE, "w", newline="") as f:
        writer = csv.writer(f)

        # Read CSV header from Arduino
        header = ser.readline().decode().strip().split(",")
        writer.writerow(header)
        print("Header:", header)

        try:
            while True:
                # Stop after RECORD_SECONDS
                if time.time() - start_time >= RECORD_SECONDS:
                    print("\nTime is up! Recording stopped.")
                    break

                line = ser.readline().decode().strip()
                if line:
                    writer.writerow(line.split(","))
                    print(line)

        except KeyboardInterrupt:
            print("\nStopped manually.")

    ser.close()
    print("Serial connection closed.")
    print(f"Saved file: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
