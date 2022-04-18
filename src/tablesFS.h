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

extern float tabela_HSC[43]; 
extern float tabela_SDP[43];
extern float tabela_FIO2[2];
extern float offset_LPS[1];

void ConfigureLFS(void);

void Readdata(int tipo);
void Savedata(int tipo);


