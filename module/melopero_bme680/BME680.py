#!/usr/bin/python

from ctypes import CDLL, c_float
import os


# Code to find the c library
#Folder structure :
#some_folder(usr/local/lib/python3.*/):
#   melopero_bme680
#       -BME680.py
#   api
#       -bme680_api.*.so
lib_loc = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
#lib_loc = os.path.join(lib_loc, 'api')
lib_file = ''
for file in os.listdir(lib_loc):
    if file.startswith('bme680_api') and file.endswith('.so'):
        lib_file = file

bme680_api = None

try:
    bme680_api = CDLL(os.path.join(lib_loc, lib_file))
except:
    print('Something went wrong while loading the bme680 api, make sure there is a file named bme680_api.*.so in this directory ', lib_loc)


class BME680():
    
    I2C_PRIMARY_ADDRESS = 0X77
    I2C_SECONDARY_ADDRESS = 0X11
    
    #********** Oversampling
    BME680_OS_NONE	= 0
    BME680_OS_1X		= 1
    BME680_OS_2X		= 2
    BME680_OS_4X		= 3
    BME680_OS_8X    = 4
    BME680_OS_16X   = 5

    #************ IIR filter settings 
    BME680_FILTER_SIZE_0	 = 0
    BME680_FILTER_SIZE_1	 = 1
    BME680_FILTER_SIZE_3	 = 2
    BME680_FILTER_SIZE_7	 = 3
    BME680_FILTER_SIZE_15 = 4
    BME680_FILTER_SIZE_31 = 5
    BME680_FILTER_SIZE_63 = 6
    BME680_FILTER_SIZE_127 = 7
    
    def __init__(self, i2c_addr = 0x77, i2c_bus = 1):
        self.i2c_address = i2c_addr
        self.i2c_bus = i2c_bus
        self._cfuncs = bme680_api
        self._cfuncs.InitDevice(self.i2c_address, self.i2c_bus)
        self._cfuncs.GetLastTemperatureMeasurement.restype = c_float
        self._cfuncs.GetLastHumidityMeasurement.restype = c_float
        self._cfuncs.GetLastPressureMeasurement.restype = c_float
        self._cfuncs.GetLastGasResistanceMeasurement.restype = c_float        
    
    def set_parameters(self, temp_os = BME680_OS_8X, hum_os = BME680_OS_2X, 
                       press_os = BME680_OS_4X, filter_size = BME680_FILTER_SIZE_3):
        '''Sets the parameters for the sensor: temperature oversampling, humidity oversampling,
            pressure oversampling, filter size'''
        self._cfuncs.Settings(temp_os, hum_os, press_os, filter_size)
    
    def update_data(self):
        self._cfuncs.UpdateData()
        
    def get_temperature(self):
        '''returns the last measured temperature in degrees Celsius'''
        return c_float(self._cfuncs.GetLastTemperatureMeasurement()).value
    
    def get_humidity(self):
        '''returns the last measured realitive humidity in %'''
        return c_float(self._cfuncs.GetLastHumidityMeasurement()).value
    
    def get_pressure(self):
        '''returns the last measured pressure in Hectopascals'''
        return c_float(self._cfuncs.GetLastPressureMeasurement()).value
    
    def get_gas_resistance(self):
        '''returns the last measured gas resistance in Ohms'''
        return c_float(self._cfuncs.GetLastGasResistanceMeasurement()).value
    
    def close_connection(self):
        self._cfuncs.CloseI2CConnection()
        return

        