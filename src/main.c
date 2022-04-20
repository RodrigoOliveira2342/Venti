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
#include  "sensor_controller.h"


void main(void)
{	
	// NRF_USBD_Type 
	// DT_N_S_soc_S_uart_40002000_P_rx_pull_up
	// NRF_USBD->USBPULLUP =  USBD_USBPULLUP_CONNECT_Enabled << USBD_USBPULLUP_CONNECT_Pos;
	ConfigureUSB(); // COMUNICAÇÃO UART VIA USB
	ConfigureLFS(); // SISTEMA DE ARQUIVOS
	ConfigureSDP(); // SENSOR DE PRESÃO DIFERENCIAL SDP
	ConfigurePin(); // PINO DO LED
	ConfigureTimer(); // timer


	Readdata(0); // offset LPS
	Readdata(1); // Tabela de fluxo HSC
	Readdata(2); // Tabela de fluxo SDP
	Readdata(3); // pontos FIO2

	while(1){
		if(uart_irq_rx_ready(uart_dev)){
			ReadMsg();
		}
		if(flagCE){
			Telemetria();
		}
		k_sleep(K_MSEC(100));
	}
}
