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

	// uint8_t lFSCV[4];
	// memcpy(lFSCV,&escalaBufferSDP,4); 
	// SendMsg(lFSCV,4);
	// uint8_t lFSCV[4];
	// uint8_t lFSCVI[4];
	Readdata(1);
	Readdata(2);

		// memcpy(lFSCV,&tabela_HSC[0],4); 
		// for (int i = 0;i<4;i++)lFSCVI[i]=lFSCV[3-i];

		// SendMsg(lFSCVI,4);
		
		// memcpy(lFSCV,&tabela_SDP[0],4); 
		
		// for (int i = 0;i<4;i++)lFSCVI[i]=lFSCV[3-i];
		// SendMsg(lFSCVI,4);
	
	while(1){
		if(uart_irq_rx_ready(uart_dev)){
			ReadMsg();
		}
		k_sleep(K_MSEC(100));

	}
}
