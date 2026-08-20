import serial
import time
from datetime import datetime

# Configure the serial port
# Note: Check if your port is '/dev/ttyACM0' or '/dev/ttyUSB0' using the terminal command: ls /dev/tty*
SERIAL_PORT = '/dev/ttyACM0' 
BAUD_RATE = 9600

try:
    # Initialize serial connection
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    # The Arduino resets when a serial connection is opened; pause to allow it to boot
    time.sleep(2) 
    print(f"Connected to Arduino on {SERIAL_PORT}")

    while True:
        # Get the current system date and time formatted as a string
        current_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        # Format string with a newline delimiter and encode to bytes
        payload = f"{current_date}\n".encode('utf-8')
        
        # Send data over serial
        ser.write(payload)
        print(f"Sent: {current_date}")
        
        # Optional: Read response back from Arduino if available
        if ser.in_waiting > 0:
            response = ser.readline().decode('utf-8').rstrip()
            print(f"Response: {response}")
            
        # Wait 5 seconds before sending the updated date
        time.sleep(5)

except serial.SerialException as e:
    print(f"Error opening or communicating on serial port: {e}")
except KeyboardInterrupt:
    print("\nProgram stopped by user.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port connection closed.")