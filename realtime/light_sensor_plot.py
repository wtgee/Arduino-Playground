#!/usr/bin/python3

import time
import serial
import matplotlib.pyplot as plt
import sys
from PyQt4 import QtGui
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg \
  import FigureCanvasQTAgg as FigureCanvas

import SerialData as LightSensor

class LightMonitor(FigureCanvas):
    """Realtime plotting of Arduino light sensor data"""
    def __init__(self):
        # initialize the iteration counter
        self.cnt = 0

        # Get the class for getting data from light sensor
        self.datagen = LightSensor.SerialData()

        self._setup_image()

        # Timer
        self.timerEvent(None)
        self.timer = self.startTimer(100)

    def _setup_image(self):
        # Image setup
        self.fig = Figure()
        self.ax = self.fig.add_subplot(111)
        FigureCanvas.__init__(self, self.fig)
        self.ax.set_xlim(0, 30)
        self.ax.set_ylim(0, 600)

        # Initial empty plot
        self.light = []
        self.l_light, = self.ax.plot([],self.light, label='Light')
        self.ax.legend()
        self.fig.canvas.draw()


    def _prepare_sensor_data(self):
        """Helper function to return light sensor info"""
        l_value = self.datagen.next()

        print("l_value: {0}".format(l_value))
        return [l_value]

    def get_light(self):
        """Get the light reading from the sensor"""
        # take the current light sensor information
        light_value = self._prepare_sensor_data()

        return [light_value]

    def timerEvent(self, evt):
        """Custom timerEvent code, called at timer event receive"""
        # get the light
        result = self.get_light()

        # append new data to the datasets
        self.light.append(result[0])

        # update lines data using the lists with new data
        self.l_light.set_data(range(len(self.light)), self.light)

        # force a redraw of the Figure - we start with an initial
        # horizaontal axes but 'scroll' as time goes by
        if(self.cnt >= 30):
            self.ax.set_xlim(self.cnt - 30, self.cnt + 1)
        self.fig.canvas.draw()

        self.cnt += 1


# Build and run the actual appliation
app = QtGui.QApplication(sys.argv)
widget = LightMonitor()
widget.setWindowTitle("Light Sensor Data")
widget.show()
sys.exit(app.exec_())
