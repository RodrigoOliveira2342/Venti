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
	configureUSB();
	while(1){
		if(flagMsgRx){
	// 		// ReadMsg();
			uint8_t buffer[3] = "OK\n";
			/*ECO PARA TESTES*/ uart_fifo_fill(uart_dev, buffer, 3);
			flagMsgRx = 0;
		}
		// k_sleep(K_MSEC(100));
	}
}
