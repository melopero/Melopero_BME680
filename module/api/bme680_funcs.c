#include <sys/ioctl.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
// Terrible portability hack between arm-linux-gnueabihf-gcc on Mac OS X and native gcc on raspbian.
#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif
#include "bme680.h"

static int i2c_file = -1;
static struct bme680_dev gas_sensor;
static float last_temp = -1;
static float last_hum = -1;
static float last_press = -1;
static float last_gas_res = -1;

void StartI2CConnection(int bus_num);
float GetLastTemperatureMeasurement();
float GetLastHumidityMeasurement();
float GetLastPressureMeasurement();

int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{	
	if (i2c_file < 0){
		StartI2CConnection(1);
	}

    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
    uint8_t outbuf[1], inbuf[len];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = dev_id;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = outbuf;

    msgs[1].addr = dev_id;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len = len;
    msgs[1].buf = inbuf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 2;

    outbuf[0] = reg_addr;

    *reg_data = 0;
    if (ioctl(i2c_file, I2C_RDWR, &msgset) < 0) {
        perror("ioctl(I2C_RDWR) in i2c_read");
        return -1;
    }
    for (int i = 0; i < len ; i++)
        *(reg_data + i) = inbuf[i];

    return rslt;
}

int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	if (i2c_file < 0){
		StartI2CConnection(1);
	}


    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
    uint8_t outbuf[len + 1];

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset[1];

    outbuf[0] = reg_addr;

    for (int i = 1; i < len + 1; i++)
        outbuf[i] = reg_data[i-1];

    msgs[0].addr = dev_id;
    msgs[0].flags = 0;
    msgs[0].len = len + 1;
    msgs[0].buf = outbuf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if (ioctl(i2c_file, I2C_RDWR, &msgset) < 0) {
        perror("ioctl(I2C_RDWR) in i2c_write");
        return -1;
    }

    return rslt;
}

void StartI2CConnection(int bus_num){
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", bus_num);
    i2c_file = open(filename, O_RDWR);
    if (i2c_file < 0) {
        printf("Error occurred while opening file %s! %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
} 

void CloseI2CConnection(){
    close(i2c_file);
}


void delay_ms(uint32_t millis){
	usleep(millis * 1000);
}

void InitDevice(int i2c_addr, int bus_num){
    StartI2CConnection(bus_num);
    gas_sensor.dev_id = i2c_addr;
    gas_sensor.intf = BME680_I2C_INTF;
    gas_sensor.read = i2c_read;
    gas_sensor.write = i2c_write;
    gas_sensor.delay_ms = delay_ms;

     /* amb_temp can be set to 25 prior to configuring the gas sensor 
     * or by performing a few temperature readings without operating the gas sensor.
     */
    gas_sensor.amb_temp = 25;
    int8_t rslt = BME680_OK;
    rslt = bme680_init(&gas_sensor);
}



void Settings(uint8_t temp_os, uint8_t hum_os, uint8_t pres_os, uint8_t filter_size){  
	//********** SETTINGS ************
	uint8_t set_required_settings;
	int8_t rslt = BME680_OK;

    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = hum_os;
    gas_sensor.tph_sett.os_pres = pres_os;
    gas_sensor.tph_sett.os_temp = temp_os;
    gas_sensor.tph_sett.filter = filter_size;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE; 

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL 
        | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings,&gas_sensor);
    //printf("set settings:  %d\n", rslt);

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&gas_sensor);
    //printf("set sensor mode:  %d\n", rslt);

}

void UpdateData(){
	//********* READING DATA ***********
	/* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
    uint16_t meas_period;
    int8_t rslt = BME680_OK;
    bme680_get_profile_dur(&meas_period, &gas_sensor);

    struct bme680_field_data data;

	delay_ms(meas_period); /* Delay till the measurement is ready */

	rslt = bme680_get_sensor_data(&data, &gas_sensor);

	last_temp = data.temperature / 100.0f;
	last_press = data.pressure / 100.0f;
	last_hum =  data.humidity / 1000.0f;
	
	/* Avoid using measurements from an unstable heating setup */
	if(data.status & BME680_GASM_VALID_MSK)
		last_gas_res = data.gas_resistance;

	/* Trigger the next measurement if you would like to read data out continuously */
	if (gas_sensor.power_mode == BME680_FORCED_MODE) {
		rslt = bme680_set_sensor_mode(&gas_sensor);
	}
}
	
float GetLastTemperatureMeasurement(){
	return last_temp;
}

float GetLastHumidityMeasurement(){
	return last_hum;
}

float GetLastPressureMeasurement(){
	return last_press;
}

float GetLastGasResistanceMeasurement(){
	return last_gas_res;
}

void PrintValues(){
	printf("T: %.2f degC, P: %.2f hPa, H %.2f %%rH \n", GetLastTemperatureMeasurement(),
            GetLastPressureMeasurement(), GetLastHumidityMeasurement());
}
/*int main(int argc, char* args){
}*/
