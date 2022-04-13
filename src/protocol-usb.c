#include  "protocol-usb.h"

struct ring_buf ringbuf;
uint8_t ring_buffer[RING_BUF_SIZE];

static uint8_t idCMD[6]={0x75,0x76,0x77,0x78,0x79,0x7B};

static StatesCMD *FUNC[6]={CMD1,CMD2,CMD3,CMD4,CMD5,CMD6};
uint8_t flagMsgRx= 0;

char bufferACK[128];
uint8_t lenBufferACK = 0;

const struct device *uart_dev ;


 /**
  * @brief crc16calc
  * 		Esta função faz o cálculo do CRC16 /CCITT-FALSE de uma sequência de dados.
  *
  * @param pData, recebe um ponteiro para os dados que serão inseridos do calculo.
  *
  * @param length, recebe o tamanho dos dados em bytes.
  */

 uint16_t crc16calc(char* pData, int length){

     uint8_t i;
     uint16_t wCrc = 0xffff;
     while (length--) {
         wCrc ^= *(unsigned char *)pData++ << 8;
         for (i=0; i < 8; i++)
             wCrc = wCrc & 0x8000 ? (wCrc << 1) ^ 0x1021 : wCrc << 1;
     }
     return wCrc & 0xffff;

 }

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);
	
	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		// if (uart_irq_rx_ready(dev)) {
		// 	int recv_len, rb_len;
		// 	uint8_t buffer[64];

		// 	size_t len = MIN(ring_buf_space_get(&ringbuf),
		// 			 sizeof(buffer));

			// recv_len = uart_fifo_read(dev, buffer, len);
		// 	if (recv_len < 0/*Substituir pelo menor valor do protocolo*/) {
		// 		recv_len = 0;
		// 	};

			// rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
		// 	// if (rb_len < recv_len ) {
		// 	// }

		// 	if (rb_len) {
		// 		uart_irq_tx_enable(dev);
		// 	}

		// }

		// flagMsgRx = 1;
		// // if (uart_irq_tx_ready(dev)) {
		// 	// flagMsgRx = 1;
		// 	uint8_t buffer2[64];
		// 	int rb_len;//, send_len;
		// 	rb_len = ring_buf_get(&ringbuf, buffer2, sizeof(buffer2));
		// 	// uart_fifo_fill(dev, buffer2, rb_len);
		// 	// SendMsg(buffer2, rb_len);
		// // }
        // // //  ReadMsg();   
		// // }
	}
	flagMsgRx = 1;
}


/**
 * @brief ReadMsg
 * 		Esta função ler e processa protocolos recebidos via UART.
 *
 * @param huart, recebe um ponteiro contendo a informação da UART.

 */

void ReadMsg(){
	uint8_t buffer[64];
	uint8_t commandAPayload[16];
	uint8_t crc[2];
	int rb_len;

	size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

	rb_len = uart_fifo_read(uart_dev, buffer, len);
	
	// /*ECO PARA TESTES*/ uart_fifo_fill(uart_dev, buffer, rb_len);

	if(buffer[0]== 0x7E && buffer[rb_len-1] == 0xFF && rb_len >6){

		memcpy(commandAPayload,&buffer[1],rb_len -4);
		memcpy(crc,&buffer[rb_len-3],2);

		if( crc16calc(commandAPayload,rb_len -4 ) == (crc[0]<<8 | crc[1])){
			ProceduresMsg(commandAPayload);
			// /*ECO PARA TESTES*/ uart_fifo_fill(uart_dev, buffer, rb_len);
		}
		else{
			char NAKMSG[] = {NACK,commandAPayload[1],0x0A};
			SendMsg(NAKMSG,3);
			lenBufferACK = 0;
		}
    }
}


void configureUSB(){
    const struct device *dev;
	dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	uart_dev = dev;
	uint32_t baudrate, dtr = 0U;
	int ret;

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
	// uart_irq_callback_set(dev, interrupt_handler); // MOD
	/* Enable rx interrupts */
	uart_irq_rx_enable(dev);
	uart_irq_tx_enable(dev); // MOD
}


/**
 * @brief SendMsg
 * 		Esta função inicia o processo de transmissão de uma mensagem.
 *
 * @param msg1, recebe o ponteiro dos dados que serão enviados
 *
 * @param len, recebe o tamanho dos dados em bytes.
 */
void SendMsg(char*msg1,int len){
	  char msg3[128];
	  EncapsulationMsgs(msg1,msg3,len);
	  uart_fifo_fill(uart_dev, msg3, len + 4);
}


/**
 * @brief EncapsulationMsgs
 * 		Esta função encapsula os dados a serem enviado para um protocolo.
 *
 * @param data1, recebe o ponteiro dos dados que serão encapsulados.
 *
 * @param data2, recebe o ponteiro da saída dos dados no formato do protocolo.
 *
 * @param len, recebe o tamanho dos dados em bytes.
 */

 void EncapsulationMsgs(char *data1,char *data2,int len){
	 int i = 1;
	 uint16_t aux;
	 data2[0]=0x7E;
	 for(;i<=len;i++)data2[i]=data1[i-1];
	 aux=crc16calc(data1,len);
	 data2[i+1] = aux;
	 data2[i]= aux>>8;
	 data2[i+2] = 0xFF;

 }


 
/**
 * @brief ProceduresMsg
 * 		Esta função inicia a leitura de um comando externo e direciona para a função adequada.
 *
 * @param data, recebe um ponteiro para os dados da mensagem.
 */
void ProceduresMsg(char *data) {
	char NAKMSG[] = { 0x15, data[1], data[1] };
	switch (data[0]) {
	case 0x20:
		for (uint8_t f = 0; f < 5; f++) {
			if (data[1] == idCMD[f]) {
				FUNC[f](data);
				break;
			}
		}
		break;
	case 0x15:
		if (lenBufferACK != 0)
			ProceduresMsg(bufferACK);
		break;
	case 0x06:
		lenBufferACK = 0;
		break;
	case 0x40:
		if (lenBufferACK != 0)
			SendMsg(bufferACK,lenBufferACK);
		break; 
	default:
		SendMsg(NAKMSG, 3);
		lenBufferACK = 0;
	}
}

//testes
void CMD1(char *data){
	char MSG[] = {0x40,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

void CMD2(char *data){
	char MSG[] = {0x50,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

void CMD3(char *data){
	char MSG[] = {0x60,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

// Zerar os transdutores
void CMD4(char *data){
	char MSG[] = {0x65,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));

}


void CMD5(char *data){
	char MSG[] = {0x70,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

void CMD6(char *data){
	char MSG[] = {0x80,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}