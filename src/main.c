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
		if(uart_irq_rx_ready(uart_dev)){
			// int recv_len, rb_len;
			// uint8_t buffer[64];
			ReadMsg();
			// uint8_t buffer[3] = "OK\n";
			// size_t len = MIN(ring_buf_space_get(&ringbuf),
			// 		 sizeof(buffer));
			// recv_len = uart_fifo_read(uart_dev, buffer, len);
			// /*ECO PARA TESTES*/ uart_fifo_fill(uart_dev, buffer, recv_len);
			// flagMsgRx = 0;
		}
		k_sleep(K_MSEC(100));
	}
}
