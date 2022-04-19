#include  "protocol-usb.h"

struct ring_buf ringbuf;
uint8_t ring_buffer[RING_BUF_SIZE];

static uint8_t idCMD[6]={0x75,0x77,0x78,0x79,0x7B};

static StatesCMD *FUNC[6]={CMD1,CMD2,CMD3,CMD4,CMD5};
uint8_t flagMsgRx= 0;

char bufferACK[131];
uint8_t lenBufferACK = 0;

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

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

	if(buffer[0]== 0x7E && buffer[rb_len-1] == 0xFF && rb_len >6){

		memcpy(commandAPayload,&buffer[1],rb_len -4);
		memcpy(crc,&buffer[rb_len-3],2);

		if( crc16calc(commandAPayload,rb_len -4 ) == (crc[0]<<8 | crc[1])){
			ProceduresMsg(commandAPayload);
		}
		else{
			char NAKMSG[] = {NACK,commandAPayload[1],0x0A};
			SendMsg(NAKMSG,3);
			lenBufferACK = 0;
		}
    }
}

static int ReadSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len){
    return i2c_read(i2c_dev ,buf ,len ,addr);
}

static int WriteSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len){
    return i2c_write(i2c_dev ,buf ,len ,addr);
}

void ConfigureSDP(){
	
	    char temp[9] = {0};
		char buf[2]={0x36,0x1e};

		WriteSensorI2C(buf,ADDRESS_SDP31,2);

		k_msleep(20);

		ReadSensorI2C(temp,ADDRESS_SDP31,9);

		k_usleep(500);

        escalaBufferSDP = temp[6] << 8 | temp[7];

		k_usleep(500);
}

void ConfigureUSB(){
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
	  char msg3[135];
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
		for (uint8_t f = 0; f < 6; f++) {
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
	char MSG[] = {ACK,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

void CMD2(char *data){
	char MSG[] = {ACK,data[1],data[2]};
	SendMsg(MSG,3);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}


// Zerar os transdutores
void CMD3(char *data){
        char MSG[] = {ACK, data[1], data[2],0,0};
		// char MSG[] = {ACK, data[1], data[2],0,0,0,0};
        // uint8_t bufferLPS[3]={0};
		uint8_t bufferSDP[3]={0};
		uint8_t bufferHSC[2]={0};
		signed short calc_press;

		tabela_HSC[0]=0;
		tabela_SDP[0]=0;
		offset_LPS[0]=0;

		int k = 0;

        if(data[2] == 0x12){
            
            // while (K<20){

			// 	ReadSensorI2C(bufferLPS, ADDRESS_LPS27HHW ,3);
			// 	k_usleep(5000);
            //     offset_LPS[0] += (((bufferLPS[2] << 16) & 0xFF0000)| ((bufferLPS[1] << 8) & 0xFF00)| (bufferLPS[0] & 0xFF)) / 4096.0;
                
            //     K++;
            // }
			// offset_LPS[0] = offset_LPS[0]/20.0;

			// Savedata(0);

			// char aux2[2];
			// int aux4;
            for(k = 0;k<32;k++){

                ReadSensorI2C(bufferSDP, ADDRESS_SDP31 ,3);
				k_usleep(500);
				calc_press = ((bufferSDP[0]<<8 | bufferSDP[1]));
               	tabela_SDP[0]  += calc_press/((escalaBufferSDP) * 100.0);
				// aux4= (calc_press/((escalaBufferSDP) * 100.0))*1000/1;

				// aux4= abs(aux4);
				// aux2[0]= aux4>>8;
				// aux2[1]= aux4 & 0x00FF;
				// SendMsg(aux2,2);

            	ReadSensorI2C(bufferHSC, ADDRESS_HSC ,2);
				k_usleep(500);
                tabela_HSC[0] += ((((bufferHSC[0] & 0x3F)<<8 | bufferHSC[1])-0x0666)*(12.4541-(-12.4541))/(0x3999-0x0666) -12.4541);
				// aux4 =  (((((bufferHSC[0] & 0x3F)<<8 | bufferHSC[1])-0x0666)*(12.4541-(-12.4541))/(0x3999-0x0666) -12.4541))*1000/1;
				
				// aux4= abs(aux4);
				// aux2[0]= aux4>>8;
				// aux2[1]= aux4 & 0x00FF;
				// SendMsg(aux2,2);
			
			}

			tabela_SDP[0]=tabela_SDP[0]/32.0;
			Savedata(2);

			tabela_HSC[0]=tabela_HSC[0]/32.0;
			Savedata(1);

			//BEGIN::AREA RESERVADA PARA DEBUGAR PROTOCOLO, MANTER COMENTADO!
			
			//END::AREA RESERVADA

			SendMsg(MSG,5);
			lenBufferACK = sizeof(MSG);
			memcpy(bufferACK,MSG,sizeof(MSG));
        }
}


void CMD4(char *data){
    char MSG[] = {ACK, data[1], data[2],0,0};
	//  char MSG[] = {ACK, data[1], data[2],0,0,0,0};
	uint8_t bufferSDP[3]={0};
	uint8_t bufferHSC[2]={0};
	signed short calc_press;

	tabela_HSC[data[2]]=0;
	tabela_SDP[data[2]]=0;

	int k = 0;
	
	char aux3[4];
	char aux2[4];
	for(k = 0;k<32;k++){
		ReadSensorI2C(bufferSDP, ADDRESS_SDP31 ,3);
		k_usleep(500);
		calc_press = ((bufferSDP[0]<<8 | bufferSDP[1]));
		tabela_SDP[data[2]]  += calc_press/((escalaBufferSDP) * 100.0);

		ReadSensorI2C(bufferHSC, ADDRESS_HSC ,2);
		k_usleep(500);
		tabela_HSC[data[2]] += ((((bufferHSC[0] & 0x3F)<<8 | bufferHSC[1])-0x0666)*(12.4541-(-12.4541))/(0x3999-0x0666) -12.4541);
	}

	tabela_SDP[data[2]]=abs((tabela_SDP[data[2]]/32.0)-tabela_SDP[0]);
	Savedata(2);

	tabela_HSC[data[2]]=abs((tabela_HSC[data[2]]/32.0) - tabela_HSC[0]);
	Savedata(1);

	//BEGIN::AREA RESERVADA PARA DEBUGAR PROTOCOLO, MANTER COMENTADO!

	//END::AREA RESERVADA


	int aux = (tabela_HSC[data[2]]*1000)/1;
	MSG[3]= aux>>8;
	MSG[4]= aux & 0x00FF;

	SendMsg(MSG,5);
	lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}

void CMD5(char *data){
	char MSG[128] ={0};
	MSG[0] = ACK;
	MSG[1] = data[1];
	int aux = 0;
	
	for(int i=0;i<43;i++){
		MSG[i*3+2] = tabflow[i];
		if(i < 29){
			aux = (tabela_SDP[i]*1000)/1;
		}else{
			aux = (tabela_HSC[i]*1000)/1;
		}
		MSG[i*3+3] = aux>>8;
		MSG[i*3+4] = aux & 0x00FF;
	}

	SendMsg(MSG,128);
    lenBufferACK = sizeof(MSG);
	memcpy(bufferACK,MSG,sizeof(MSG));
}