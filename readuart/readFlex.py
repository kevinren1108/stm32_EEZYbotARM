import serial
import matplotlib.pyplot as plt
from drawnow import drawnow

# Initialize variables
time_data = []
flex_data = []

flex_buffer = []
x_vals = []

MAX_PTS = 1000000  # Max number of points to plot

# Create serial object
ser = serial.Serial('COM4', 112000)

# Define plot function
def plot_data():
    plt.plot(x_vals, flex_buffer, label="Roll")

    plt.ylim(0000, 3000)  # Set y-axis limits
    if(len(x_vals) > 200):
        plt.xlim(x_vals[-200], x_vals[-1])

    plt.title('Flex Sensor Live data')
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
        flex_data.append(float(line))

        # Update x and y values
        x_vals.append(len(x_vals) + 1)
        flex_buffer.append(flex_data[-1])


        
        # Limit number of points to plot
        if len(x_vals) > MAX_PTS:
            x_vals.pop(0)
            flex_buffer.pop(0)

        # Draw plot 2
        drawnow(plot_data)
        
    except KeyboardInterrupt:
        ser.close()
        break
