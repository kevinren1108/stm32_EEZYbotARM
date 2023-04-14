import serial
import matplotlib.pyplot as plt
from drawnow import drawnow

# Initialize variables
time_data = []
ax_data = []
ay_data = []
az_data = []
gx_data = []
gy_data = []
gz_data = []

time_buffer = []
ax_buffer = []
ay_buffer = []
az_buffer = []
gx_buffer = []
gy_buffer = []
gz_buffer = []

MAX_PTS = 1000000  # Max number of points to plot

# Create serial object
ser = serial.Serial('COM4', 112000)

# Define plot function
def plot_data():
    

    plt.plot(time_buffer, ax_buffer, label="ACCx")
    plt.plot(time_buffer, ay_buffer, label="ACCy")
    plt.plot(time_buffer, az_buffer, label="ACCz")
    plt.plot(time_buffer, gx_buffer, label="GYROx")
    plt.plot(time_buffer, gy_buffer, label="GYROy")
    plt.plot(time_buffer, gz_buffer, label="GYROz")


    plt.ylim(-90, 90)  # Set y-axis limits


    if(len(time_buffer) > 200):
        plt.xlim(time_buffer[-200], time_buffer[-1])

    plt.title('MPU6050 Live data')
    plt.legend(loc="upper left")
    plt.xlabel('Time (s)')
    plt.ylabel('Data (V)')

# Read data from serial port and plot in real time
while True:
    try:
        # Read data from serial port


        line = ser.readline().decode().strip()
        time_data.append(float(line))

        line = ser.readline().decode().strip()
        ax_data.append(float(line) * 100)

        line = ser.readline().decode().strip()
        ay_data.append(float(line) * 100)

        line = ser.readline().decode().strip()
        az_data.append(float(line) * 100)

        line = ser.readline().decode().strip()
        gx_data.append(float(line))

        line = ser.readline().decode().strip()
        gy_data.append(float(line))

        line = ser.readline().decode().strip()
        gz_data.append(float(line))

        # Update x and y values
        time_buffer.append(len(time_data) + 1)
        ax_buffer.append(ax_data[-1])
        ay_buffer.append(ay_data[-1])
        az_buffer.append(az_data[-1])

        gy_buffer.append(gy_data[-1])
        gz_buffer.append(gz_data[-1])
        gx_buffer.append(gx_data[-1])

        
        # Limit number of points to plot
        if len(time_buffer) > MAX_PTS:
            time_buffer.pop(0)
            ax_buffer.pop(0)
            ay_buffer.pop(0)
            az_buffer.pop(0)
            gy_buffer.pop(0)
            gz_buffer.pop(0)
            gx_buffer.pop(0)
        
        # Draw plot 2
        drawnow(plot_data)
        
    except KeyboardInterrupt:
        ser.close()
        break
