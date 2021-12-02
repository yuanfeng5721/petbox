#include <stdio.h>
#include <stdlib.h>
#include <os_msg.h>
#include <trace.h>
#include <app_msg.h>
#include <board.h>
#include <rtl876x.h>
#include <rtl876x_nvic.h>
#include <rtl876x_uart.h>
#include <rtl876x_rcc.h>
#include <rtl876x_pinmux.h>
#include "at_uart.h"


void (* at_uart_rcv_data)(uint8_t *buf, uint16_t size) = NULL;
bool uart_send_data(const uint8_t *data, uint16_t size)
{
	uint8_t i = 0;
	uint8_t count = size/UART_TX_FIFO_SIZE;
	
	for(i=0; i<count; i++)
	{
		UART_SendData(UART1, data+i*UART_TX_FIFO_SIZE, UART_TX_FIFO_SIZE);
		/* wait tx fifo empty */
		while (UART_GetFlagState(UART1, UART_FLAG_THR_TSR_EMPTY) != SET);
	}
	if(size%UART_TX_FIFO_SIZE)
	{
		UART_SendData(UART1, data+i*UART_TX_FIFO_SIZE, size%UART_TX_FIFO_SIZE);
		/* wait tx fifo empty */
		while (UART_GetFlagState(UART1, UART_FLAG_THR_TSR_EMPTY) != SET);
	}
	
	return true;
}

int at_uart_send_data(const uint8_t *data, uint16_t size)
{
//    UART_SendData(UART1, data, size);
//    /* wait tx fifo empty */
//    while (UART_GetFlagState(UART1, UART_FLAG_THR_TSR_EMPTY) != SET);

//    return true;
	return uart_send_data(data, size);
}

/****************************************************************************/
/* UART interrupt                                                           */
/****************************************************************************/
void UART1_Handler(void)
{
    uint8_t rx_char;

    uint32_t interrupt_id = 0;
    /* read interrupt id */
    interrupt_id = UART_GetIID(UART1);

    /* disable interrupt */
    UART_INTConfig(UART1, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);

    switch (interrupt_id)
    {
    /* tx fifo empty, not enable */
    case UART_INT_ID_TX_EMPTY:
        break;

    /* rx data valiable */
    case UART_INT_ID_RX_LEVEL_REACH:
        UART_ReceiveData(UART1, &rx_char, 1);
        if(at_uart_rcv_data != NULL)
			at_uart_rcv_data(&rx_char, 1);
        break;

    case UART_INT_ID_RX_TMEOUT:
        break;

    /* receive line status interrupt */
    case UART_INT_ID_LINE_STATUS:
        {
            DBG_DIRECT("Line status error!!!!\n");
        }
        break;

    case UART_INT_ID_MODEM_STATUS:
        break;

    default:
        break;
    }

    /* enable interrupt again */
    UART_INTConfig(UART1, UART_INT_RD_AVA, ENABLE);

    return;
}

/**
 * @brief  Initializes the Data UART.
 *
 * When data uart receives data, data uart will send an event IO_UART_MSG_TYPE to evt_queue_handle and send the data to io_queue_handle.
 * @param[in] event_queue_handle   Event queue handle which is created by APP.
 * @param[in] io_queue_handle      IO message queue handle which is created by APP.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_main_task(void *p_param)
    {
        char event;

        os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
        os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(unsigned char));

        gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

        data_uart_init(evt_queue_handle, io_queue_handle);
        ......
    }
    void app_handle_io_msg(T_IO_MSG io_msg)
    {
        uint16_t msg_type = io_msg.type;
        uint8_t rx_char;

        switch (msg_type)
        {
        case IO_MSG_TYPE_UART:
            // We handle user command informations from Data UART in this branch.
            rx_char = (uint8_t)io_msg.subtype;
            user_cmd_collect(&user_cmd_if, &rx_char, sizeof(rx_char), user_cmd_table);
            break;
        default:
            break;
        }
    }
 * \endcode
 */
void at_uart_init(void (*func)(uint8_t *, uint16_t))
{
	at_uart_rcv_data = func;
    RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);
    Pinmux_Config(AT_UART_TX_PIN, UART1_TX);
    Pinmux_Config(AT_UART_RX_PIN, UART1_RX);
    Pad_Config(AT_UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(AT_UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);

    /* uart init */
    UART_InitTypeDef uartInitStruct;
    UART_StructInit(&uartInitStruct);
	uartInitStruct.div = 271;
	uartInitStruct.ovsr = 10;
	uartInitStruct.ovsr_adj = 0x24a;
    uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_1BYTE;
    UART_Init(UART1, &uartInitStruct);
    UART_INTConfig(UART1, UART_INT_RD_AVA, ENABLE);

    /*  Enable UART IRQ  */
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = UART1_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 5;
    NVIC_Init(&nvic_init_struct);
}

void at_uart_deinit(void)
{
	at_uart_rcv_data = NULL;
    
	/*  Disable UART IRQ  */
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = UART1_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = DISABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 5;
    NVIC_Init(&nvic_init_struct);
	
	/* uart deinit */
	UART_INTConfig(UART1, UART_INT_RD_AVA, DISABLE);
	UART_DeInit(UART1);
	
	RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, DISABLE);
//    Pinmux_Config(AT_UART_TX_PIN, UART1_TX);
//    Pinmux_Config(AT_UART_RX_PIN, UART1_RX);
//    Pad_Config(AT_UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
//               PAD_OUT_HIGH);
//    Pad_Config(AT_UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
//               PAD_OUT_LOW);
}

void at_uart_set_buadrate(uint8_t buadrate)
{
	/* uart init */
    UART_InitTypeDef uartInitStruct;
    UART_StructInit(&uartInitStruct);
	
	switch(buadrate)
	{
		case UART_BAUDRATE_9600:
			uartInitStruct.div = 271;
			uartInitStruct.ovsr = 10;
			uartInitStruct.ovsr_adj = 0x24a;
		break;
		
		case UART_BAUDRATE_19200:
			uartInitStruct.div = 165;
			uartInitStruct.ovsr = 7;
			uartInitStruct.ovsr_adj = 0x5AD;
		break;
		
		case UART_BAUDRATE_38400:
			uartInitStruct.div = 85;
			uartInitStruct.ovsr = 7;
			uartInitStruct.ovsr_adj = 0x222;
		break;
		
		case UART_BAUDRATE_57600:
			uartInitStruct.div = 55;
			uartInitStruct.ovsr = 7;
			uartInitStruct.ovsr_adj = 0x5AD;
		break;
		
		case UART_BAUDRATE_115200:
			uartInitStruct.div = 20;
			uartInitStruct.ovsr = 12;
			uartInitStruct.ovsr_adj = 0x252;
		break;
		
		default:
		break;
	}
    uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_1BYTE;
    UART_Init(UART1, &uartInitStruct);
}
