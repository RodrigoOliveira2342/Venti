#pragma once

#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)

#define abs(x) ((x)>0?(x):-(x))

#define ADDRESS_HSC  0x78
#define ADDRESS_SDP31  0x21
#define ADDRESS_LPS27HHW  0x5c

extern const struct device *dev1;
extern bool led_is_on;
extern int ret1;

extern int escalaBufferSDP;



//  int WriteSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len);

// int ReadSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len);

extern const struct device *i2c_dev; 

extern uint8_t tabflow[43];


float MapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);
float FlowCalc(float dpvalue, float tabela_calib[43]);

void ConfigurePin();