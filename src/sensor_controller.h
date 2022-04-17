#pragma once

#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/i2c.h>

const char ADDRESS_HSC = 0x78;
const char ADDRESS_SDP31 = 0x21;
const char ADDRESS_LPS27HHW = 0x5c;


const unsigned char tabflow[43] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
                                           40, 45, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};

 float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);
 int flowCalc(int dpvalue, int tabela_calib[43]);