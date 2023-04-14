import serial
import matplotlib.pyplot as plt
from drawnow import drawnow

# Initialize variables
time_data = []
roll_data = []
pitch_data = []
yaw_data = []

roll_buffer = []
pitch_buffer = []
yaw_buffer = []

x_vals = []

MAX_PTS = 1000000  # Max number of points to plot

# Create serial object
ser = serial.Serial('COM4', 112000)

# Define plot function
def plot_data():
    plt.plot(x_vals, roll_buffer, label="Roll")
    plt.plot(x_vals, pitch_buffer, label="Pitch")
    plt.plot(x_vals, yaw_buffer, label="Yaw")

    plt.ylim(-90, 90)  # Set y-axis limits
    if(len(x_vals) > 200):
        plt.xlim(x_vals[-200], x_vals[-1])

    plt.title('MPU6050 Live data')
    plt.legend(loc="upper left")
    plt.xlabel('Time (s)')
    plt.ylabel('Data (V)')

# Read data from serial port and plot in real time
while True:
    try:
        # Read data from serial port
        # First three ACC x, y, z
        line = ser.readline().decode().strip()
        time_data.append(float(line))

        line = ser.readline().decode().strip()
        roll_data.append(float(line))

        line = ser.readline().decode().strip()
        pitch_data.append(float(line))

        line = ser.readline().decode().strip()
        yaw_data.append(float(line))

        # Update x and y values
        x_vals.append(len(x_vals) + 1)
        roll_buffer.append(roll_data[-1])
        pitch_buffer.append(pitch_data[-1])
        yaw_buffer.append(yaw_data[-1])

        
        # Limit number of points to plot
        if len(x_vals) > MAX_PTS:
            x_vals.pop(0)
            roll_buffer.pop(0)
            pitch_buffer.pop(0)
            yaw_buffer.pop(0)
        
        # Draw plot 2
        drawnow(plot_data)
        
    except KeyboardInterrupt:
        ser.close()
        break
