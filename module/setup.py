#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@author: Leonardo La Rocca
"""

import setuptools
import os

sources_files = []
lib_dir = ['api']
for file in os.listdir(os.path.join(os.getcwd(), lib_dir[0])):
    if file.endswith('.c'):
        sources_files.append(os.path.join(lib_dir[0],file))

extension = setuptools.Extension(
    'bme680_api',
    define_macros=[],
    include_dirs=lib_dir,
    #extra_compile_args=['-std=c99'],
    libraries=[],
    library_dirs=[],
    sources= sources_files)

setuptools.setup(
    name="melopero_bme680",
    version="0.1.0",
    description="A module to easily access Melopero's BME680 sensor's features",
    url="https://github.com/melopero/Melopero_VL53L1X/tree/master/module",
    author="Melopero",
    author_email="info@melopero.com",
    license="MIT",
    ext_modules=[extension],
    packages=setuptools.find_packages(),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.5",
    ],
)
