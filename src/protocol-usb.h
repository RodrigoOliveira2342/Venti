#pragma once

#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <sys/ring_buffer.h>

#include <usb/usb_device.h>
#include <logging/log.h>

#include <kernel.h>


#include  "tablesFS.h"
#include  "sensor_controller.h"


#define RING_BUF_SIZE 1024

#define HEADER 0x76
#define FOOTER 0xFF
#define NACK   0x15
#define ACK    0x06

#define abs(x) ((x)>0?(x):-(x))

extern const struct device *uart_dev;
extern uint8_t flagMsgRx;

extern struct ring_buf ringbuf;
extern uint8_t ring_buffer[RING_BUF_SIZE];

typedef void StatesCMD(char * Dados);


void CMD1(char * data); // Calibração do sensor de oxigênio
void CMD2(char * data); // Leitura da carga de bateria  
void CMD3(char * data); // Leituras do fluxo, pressão e FiO2
void CMD4(char * data); // Zerar os transdutores
void CMD5(char * data); // Calibrar um ponto de fluxo
void CMD6(char * data); // Leitura da tabela de calibração atualizada


uint16_t crc16calc(char* pData, int length);
void ConfigureUSB();
void ConfigureSDP();
void ReadMsg();
void EncapsulationMsgs(char *data1,char *data2,int len);
void SendMsg(char*msg1,int len);
void ProceduresMsg(char *data);


