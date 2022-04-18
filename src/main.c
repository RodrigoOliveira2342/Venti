/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Sample echo app for CDC ACM class
 *
 * Sample app for USB CDC ACM class driver. The received data is echoed back
 * to the serial port.
 */

#include <zephyr.h>
#include  "protocol-usb.h"
void main(void)
{
	ConfigureUSB();
	ConfigureLFS();
	ConfigureSDP();

	uint8_t lFSCV[4];
	memcpy(lFSCV,&escalaBufferSDP,4); 
	SendMsg(lFSCV,4);
	
	while(1){
		if(uart_irq_rx_ready(uart_dev)){
			ReadMsg();
		}
		k_sleep(K_MSEC(100));
	}
}
