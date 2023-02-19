import serial
import matplotlib.pyplot as plt
from drawnow import drawnow

# Initialize variables
axdata = []
aydata = []
azdata = []

ax_vals = []
ay_vals = []
az_vals = []

gxdata = []
gydata = []
gzdata = []

gx_vals = []
gy_vals = []
gz_vals = []

x_vals = []

MAX_PTS = 1000000  # Max number of points to plot

# Create serial object
ser = serial.Serial('COM4', 112000)

# Define plot function
def plot_data():
    plt.plot(x_vals, ax_vals)
    plt.plot(x_vals, ay_vals)
    plt.plot(x_vals, az_vals)

    plt.plot(x_vals, gx_vals)
    plt.plot(x_vals, gy_vals)
    plt.plot(x_vals, gz_vals)

    plt.ylim(0, 1000)  # Set y-axis limits
    plt.title('Real-time UART Data')
    plt.xlabel('Time (s)')
    plt.ylabel('Data (V)')

# Read data from serial port and plot in real time
while True:
    try:
        # Read data from serial port
        line = ser.readline().decode().strip()
        axdata.append(float(line)+800)

        line = ser.readline().decode().strip()
        aydata.append(float(line)+300)

        line = ser.readline().decode().strip()
        azdata.append(float(line)+500)

        line = ser.readline().decode().strip()
        gxdata.append(float(line)+800)

        line = ser.readline().decode().strip()
        gydata.append(float(line)+300)

        line = ser.readline().decode().strip()
        gzdata.append(float(line)+500)


        # Update x and y values
        x_vals.append(len(x_vals) + 1)
        ax_vals.append(axdata[-1])
        ay_vals.append(aydata[-1])
        az_vals.append(azdata[-1])

        gx_vals.append(gxdata[-1])
        gy_vals.append(gydata[-1])
        gz_vals.append(gzdata[-1])
        
        # Limit number of points to plot
        if len(x_vals) > MAX_PTS:
            x_vals.pop(0)
            ax_vals.pop(0)
            ay_vals.pop(0)
            az_vals.pop(0)
            gx_vals.pop(0)
            gy_vals.pop(0)
            gz_vals.pop(0)
        
        # Draw plot 2
        drawnow(plot_data)
        
    except KeyboardInterrupt:
        ser.close()
        break
