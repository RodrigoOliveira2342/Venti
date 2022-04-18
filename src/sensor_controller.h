#pragma once

#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/i2c.h>


#define ADDRESS_HSC  0x78
#define ADDRESS_SDP31  0x21
#define ADDRESS_LPS27HHW  0x5c

extern int escalaBufferSDP;

//  int WriteSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len);

// int ReadSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len);

extern const struct device *i2c_dev; 

extern uint8_t tabflow[43];


float MapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);
int FlowCalc(int dpvalue, int tabela_calib[43]);