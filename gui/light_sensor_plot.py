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
        # initialize the iteration counter
        self.cnt = 0
        self.window_size = 30

        # Get the class for getting data from serial sensor
        self.datagen = SerialData()

        self._setup_image()

        # Timer
        self.timerEvent(None)
        self.timer = self.startTimer(100)

    def _setup_image(self):
        # Image setup
        self.fig = Figure()
        self.ax = self.fig.add_subplot(111)
        FigureCanvas.__init__(self, self.fig)
        self.ax.set_xlim(0, self.window_size)
        self.ax.set_ylim(0, 600)

        # Initial empty plot
        self.serial = []
        self.l_light, = self.ax.plot([],self.serial, label='serial')
        self.ax.legend()
        self.fig.canvas.draw()


    def _prepare_sensor_data(self):
        """Helper function to return serial sensor info"""
        l_value = self.datagen.next()
        sensor_data = json.loads(l_value)

        return sensor_data.temp

    def get_reading(self):
        """Get the serial reading from the sensor"""
        # take the current serial sensor information
        serial_value = self._prepare_sensor_data()

        return [serial_value]

    def timerEvent(self, evt):
        """Custom timerEvent code, called at timer event receive"""
        # get the serial
        result = self.get_reading()

        # append new data to the datasets
        self.serial.append(result[0])

        # update lines data using the lists with new data
        self.l_light.set_data(range(len(self.serial)), self.serial)

        # force a redraw of the Figure - we start with an initial
        # horizontal axes but 'scroll' as time goes by
        if(self.cnt >= self.window_size):
            self.ax.set_xlim(self.cnt - self.window_size, self.cnt + 15)
        self.fig.canvas.draw()

        self.cnt += 1


# Build and run the actual appliation
app = QtGui.QApplication(sys.argv)
widget = ArduinoSerialMonitor()
widget.setWindowTitle("Serial Monitor")
widget.show()
sys.exit(app.exec_())
