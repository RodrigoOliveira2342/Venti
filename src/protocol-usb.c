#include  "protocol-usb.h"

struct ring_buf ringbuf;
uint8_t ring_buffer[RING_BUF_SIZE];
uint8_t flagDATA = 0;


static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

			recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len < 0/*Substituir pelo menor valor do protocolo*/) {
				recv_len = 0;
			};

            flagDATA = 0;

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len ) {
                flagDATA = flagDATA|1;
			}

			if (rb_len) {
				uart_irq_tx_enable(dev);
			}
		}


		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len, send_len;

			rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
			if (!rb_len) {
				/*uart_irq_tx_disable(dev);*/
                flagDATA = flagDATA|1;
				continue;
			}

            if(buffer[0] != HEADER || buffer[rb_len-1]!= FOOTER || flagDATA != 0){
                //SEND NACK
                return;
            }
            
            /*verificar CRC aqui*/
            
			/*ECO PARA TESTES*/ send_len = uart_fifo_fill(dev, buffer, rb_len);
            /*tratar a resposta e encamiar para os estados*/
		}
	}
}



void configureUSB(){
    const struct device *dev;
	uint32_t baudrate, dtr = 0U;
	int ret;
	dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev)) {
		return;
	}
	ret = usb_enable(NULL);
	if (ret != 0) {
		return;
	}
	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);
	while (true) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}
	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	uart_irq_callback_set(dev, interrupt_handler);
	/* Enable rx interrupts */
	uart_irq_rx_enable(dev);
}