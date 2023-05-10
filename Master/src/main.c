
/**
 * 
 * 
 * **********	MASTER CODE			***********
 * 
 * 
 *
*/

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#define I2C_NODE    DT_NODELABEL(i2c1) 	
#define I2C_TARGET_ADDRESS 0x07
#define I2C_BUF_LEN 12
#define DELAY_MS  500

void main(void)
{
    int ret;

    uint8_t i2c_rx_data[I2C_BUF_LEN];
    int16_t soil_moisture,air_quality;
    int32_t temperature, humidity;

    const struct device *const dev = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(dev)) { 
        printk("error");
        return;
	}
    else{
        printk("\nDevice ready\n");
    }

    while (1) {
        ret = i2c_read(dev,i2c_rx_data,12,I2C_TARGET_ADDRESS);
        if (ret < 0) {
            printk("Error\n");
        } 
        else {
            soil_moisture = (int16_t)(((int16_t)i2c_rx_data[0] << 8) | i2c_rx_data[1]);
            air_quality = (int16_t)(((int16_t)i2c_rx_data[2] << 8) | i2c_rx_data[3]);
            humidity = (int32_t)((int32_t)i2c_rx_data[4] << 24) | ((int32_t)i2c_rx_data[5] << 16) | ((int16_t)i2c_rx_data[6] << 8) | (i2c_rx_data[7]);
            temperature = (int32_t)((int32_t)i2c_rx_data[8] << 24) | ((int32_t)i2c_rx_data[9] << 16) | ((int16_t)i2c_rx_data[10] << 8) | (i2c_rx_data[11]);

            printk("\nSoil Moisture value = %d", soil_moisture);
            printk("\nAir Quality value = %d", air_quality);
            printk("\nHumidity = %d.%06d",(humidity / 1000),((humidity % 1000)*1000));
            printk("\nTemperature = %d.%06d degC", (temperature / 100), ((temperature % 100) * 1000));

        }
        k_sleep(K_MSEC(1000));
    }
}
