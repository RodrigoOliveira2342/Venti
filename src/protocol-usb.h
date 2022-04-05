#pragma once

#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <sys/ring_buffer.h>

#include <usb/usb_device.h>
#include <logging/log.h>

#define RING_BUF_SIZE 1024

#define HEADER 0x76
#define FOOTER 0xFF
#define NACK   0x15
#define ACK    0x06

#define PZERO {0x00,{0x00}}

typedef struct{
	uint8_t *msg;
	uint8_t crc[2];
}Protocolo_t;


// static void interrupt_handler(const struct device *dev, void *user_data);
void configureUSB();
void ReadMsg();
void EncapsulationMsgs(char *data1,char *data2,int len);
// uint16_t crc16(char* pData, int length);
void SendMsg(char*msg1,int len);