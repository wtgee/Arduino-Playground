#!/usr/bin/python3

import time
import json
import serial
import matplotlib.pyplot as plt
import sys
from PyQt4 import QtGui
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg \
  import FigureCanvasQTAgg as FigureCanvas

from Arduino.SerialIO import SerialData

class ArduinoSerialMonitor(FigureCanvas):
    """Realtime plotting of Arduino serial sensor data"""
    def __init__(self):
        # initialize the iteration counter for scrolling window
        self.count = 0
        self.window_size = 30

        # Get the class for getting data from serial sensor
        self.serial_reader = SerialData()

        # Hold information on sensors read
        self.sensor_readings = dict()
        self.sensor_plots = dict()

        self._setup_plot()

        # Timer
        self.timerEvent(None)
        self.timer = self.startTimer(100)

    def _setup_plot(self):
        # Image setup
        self.fig = Figure()

        FigureCanvas.__init__(self, self.fig)

        # Do an initial reading of the data to get info
        # on the sensors. Arduino sends a number of blank lines
        # to start so we skip those
        initial_reading = self.get_reading()
        while True:
            if len(initial_reading):
                break

            initial_reading = self.get_reading()

        print("initial_reading: {}".format(initial_reading))
        for pin in range(5):
            # Create an empty array to store data for this sensor
            sensor_values = []

            # Create plot, set x and y axes
            ax = self.fig.add_subplot(111)
            ax.set_xlim(0, self.window_size)
            ax.set_ylim(0, 100)

            s_plot, = ax.plot([],sensor_values, label="Analog {}".format(pin))

            ax.legend()

            # Add the ax and plot to our monitor
            self.sensor_readings[pin] = sensor_values
            plot_dict = {'plot': s_plot, 'ax': ax}
            self.sensor_plots[pin] = plot_dict

        # Draw the initial canvas
        self.fig.canvas.draw()


    def _prepare_sensor_data(self):
        """Helper function to return serial sensor info"""
        sensor_value = self.serial_reader.next()
        sensor_data = dict()
        if len(sensor_value) > 0:
            try:
                sensor_data = json.loads(sensor_value)
            except ValueError:
                print("Bad JSON: {0}".format(sensor_value))

        return sensor_data

    def get_reading(self):
        """Get the serial reading from the sensor"""
        # take the current serial sensor information
        return self._prepare_sensor_data()

    def timerEvent(self, evt):
        """Custom timerEvent code, called at timer event receive"""

        for sensor, value in self.get_reading().items():
            if sensor == 'analog':
                for pin in range(5):
                    # Append new data to the datasets
                    self.sensor_readings[pin].append(value[pin])

                    # Scrolling horizontal axis is calculated - basically
                    # if our number of readings is abover the size, start scrolling.
                    # We add 15 so the right edge of line is not butting up against edge
                    # of graph but has some nice buffer space
                    if(self.count >= self.window_size):
                        self.sensor_plots[pin]['ax'].set_xlim(
                                self.count - self.window_size,
                                self.count + 15
                                )

                    num_readings = len(self.sensor_readings[pin])

                    # Update lines data using the lists with new data
                    plot_data = (range(num_readings),self.sensor_readings[pin])
                    self.sensor_plots[pin]['plot'].set_data(range(num_readings), self.sensor_readings[pin])

        # Force a redraw of the Figure
        self.fig.canvas.draw()

        self.count += 1


# Build and run the actual appliation
app = QtGui.QApplication(sys.argv)
widget = ArduinoSerialMonitor()
widget.setWindowTitle("Serial Monitor")
widget.show()
sys.exit(app.exec_())
