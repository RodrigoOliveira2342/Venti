#pragma once

#include <stdio.h>
#include <zephyr.h>
#include <device.h>
#include <fs/fs.h>
#include <fs/littlefs.h>
#include <storage/flash_map.h>

#include <string.h>

#define  FLPS  0x00
#define  FHSC  0x01
#define  FSDP  0x02
#define  FFIO2 0x03 

void configureLFS(void);

uint32_t testcount(void);


void readdata(int tipo);
void savedata(int tipo);


