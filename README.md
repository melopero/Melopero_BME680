# Melopero BME680
![melopero logo](images/sensor.jpg?raw=true)

## Getting Started
### Prerequisites
You will need:
- a python3 version, which you can get here: [download python3](https://www.python.org/downloads/)
- the BME680 sensor: [buy here](https://www.melopero.com/categoria-prodotto/melopero-engineering/)

### Installing
You can install the melopero-bme680 module by typing this line in the terminal:
```python
sudo pip3 install melopero-bme680
```

## Module description
The module contains a class to easily access the BME680's sensor functions.

### Usage
First you need to import the module in your file:
```python
import melopero_bme680 as mp
```
Then you can create a simple BME680 object and access it's methods, the sensor object will be initialized with the i2c address set to `0x77` and the i2c bus to `1` alias `(dev/i2c-1)` which is the standard i2c bus in a Raspberry pi.
```python
sensor = mp.BME680()
```
Alternatively you can modify it's parameters by typing
```python
sensor = mp.BME680(i2c_addr = myaddress, i2c_bus = mybus)
```

The sensor has the following methods
```python
sensor.set_parameters(temp_os = BME680_OS_8X, hum_os = BME680_OS_2X,
                     press_os = BME680_OS_4X, filter_size = BME680_FILTER_SIZE_3)
                     # Sets the parameters  for :  
                     # temperature oversampling, humidity oversampling,
                     # pressure oversampling, filter size
                     # you can call this method parameters to use the default parameters
sensor.update_data() # take a measurement and updates the data
sensor.get_temperature() #returns the last measured temperature
sensor.get_humidity() #returns the last measured humidity
sensor.get_pressure() #returns the last measured pressure
sensor.get_gas_resistance() #returns the last measured gas resistance
```
Remember to close the connection after using the sensor
```python
sensor.close_connection()
```

## Example
The following example will take a measure and print it every second for a 100 times
```python
import time
import melopero_bme680 as mp

sensor = mp.BME680()
sensor.set_parameters()
for i in range(100):
    sensor.update_data()
    print('T: {:.2f}C,H: {:.2f}%rH, P: {:.2f}hPa'.format(sensor.get_temperature(), sensor.get_humidity(), sensor.get_pressure()))
    print('Gas resistance: {:.2f} Ohms'.format(sensor.get_gas_resistance()))
    time.sleep(1)

print('Closing connection')
sensor.close_connection()
```
### Attention

The module is written in `python3` and by now supports only `python3`, remember to use always `sudo pip3 install melopero-bme680` when you install the module and `sudo python3 your_code.py` when you run the code.
