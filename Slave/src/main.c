/**
 * 
 * 
 * **********	SLAVE CODE			***********
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
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>



#define I2C_TARGET_ADDRESS  0x07                  //ADDRESS OF THE SLAVE 

#define LED0_NODE DT_ALIAS(led0)

#define I2C_NODE    DT_NODELABEL(i2c1) 
#define BUF_LEN 12
#define DELAY_MS  500	
#define STACK_SIZE  500 

const struct device *const dev = DEVICE_DT_GET(I2C_NODE);
int ret;

// dummy data
uint8_t data[BUF_LEN];
uint32_t buff_idx;
bool m_read;

int16_t soilbuf;
int16_t airbuf;
int32_t hum_comp;
int32_t temp_comp;

// read request callback
int master_read(struct i2c_target_config *config, uint8_t *val)
{
    m_read = true;
    *val = data[buff_idx];
    printf("Read value = %d", *val);
    return 0;
};

// read processed callback for incrementing the data index
int master_continue(struct i2c_target_config *config, uint8_t *val)
{
    buff_idx = (buff_idx + 1) % BUF_LEN;
	*val = data[buff_idx];
    printf("Read value = %d", *val);
    return 0;
};

// callbacks
struct i2c_target_callbacks cb = {
    .read_requested = master_read,
    .read_processed = master_continue,
};

// slave configurations
struct i2c_target_config slave_config = {
    .address = I2C_TARGET_ADDRESS,
    .callbacks = &cb,
};


void main(){
    if (!device_is_ready(dev)) { 
        printk("error");
        return;
	}
    else{
        printk("Device ready");
    }

    ret = i2c_target_register(dev, &slave_config);
    if (ret < 0) { 
        printk("error value %d",ret); 
        return;
    }
    else{
        printk("Successfully registered!!");
    }
    soilbuf = 1023;
    airbuf = 700;
    hum_comp = 258976;
    temp_comp = 24657;
    while (1)
    {
        if(!m_read){
            data[0] = (uint8_t)((soilbuf & 0xFF00) >> 8);       //soilbuf MSB
            data[1] = (uint8_t)((soilbuf & 0x00FF));            //soilbuf LSB

            data[2] = (uint8_t)((airbuf & 0xFF00) >> 8);        //airbuf MSB
            data[3] = (uint8_t)((airbuf & 0x00FF));             //airbuf LSB

            data[4] = (uint8_t)((hum_comp >> 24) & 0xFF);       //humidity MSB
            data[5] = (uint8_t)((hum_comp >> 16) & 0xFF);
            data[6] = (uint8_t)((hum_comp >> 8) & 0xFF);
            data[7] = (uint8_t)(hum_comp & 0xFF);               //humidity LSB

            data[8] = (uint8_t)((temp_comp >> 24) & 0xFF);      //temp MSB     
            data[9] = (uint8_t)((temp_comp >> 16) & 0xFF);
            data[10] = (uint8_t)((temp_comp >> 8) & 0xFF);
            data[11] = (uint8_t)(temp_comp & 0xFF);             //temp LSB
        }
    }

    k_sleep(K_MSEC(1000));
    

}








