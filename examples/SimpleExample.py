#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@author: Leonardo La Rocca
"""

import melopero_bme680.BME680 as mp

sensor = mp.BME680()
sensor.set_parameters()
for i in range(100):
    sensor.update_data()
    print('T: {:.2f}C,H: {:.2f}%rH, P: {:.2f}hPa'.format(sensor.get_temperature(), sensor.get_humidity(), sensor.get_pressure()))
    print('Gas resistance: {:.2f}'.format(sensor.get_gas_resistance()))

print('closing connection')
sensor.close_connection()
print('ending program')