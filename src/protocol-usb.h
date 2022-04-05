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

static void interrupt_handler(const struct device *dev, void *user_data);
void configureUSB();