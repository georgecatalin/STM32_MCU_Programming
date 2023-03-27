/*
 * uart.c
 *
 *  Created on: 16 Mar 2023
 *  Author: George Calin
 *  Code for: Nucleo 144 family
 */

/* ******************************
 * FOR THE NUCLEO 144 :
  * you need to configure the PD8 and PD9 with UART3.
 * ***************************** */


#include "uart.h"

#define GPIODEN (1UL<<3)
#define UART3EN (1UL<<18)

#define SYSTEM_FREQ (16000000)
#define APB1_CLOCK	SYSTEM_FREQ

#define UART_BAUDRATE (115200)

#define CR1_TE (1UL<<3)
#define CR1_RE	(1UL<<2)

#define CR1_UE (1UL<<13)

#define SR_TXE (1UL<<7)
#define SR_RXNE (1UL<<5)

static uint16_t compute_uart_bd(uint32_t PeriphClock, uint32_t BaudRate);
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate );


void uart3_rxtx_init(void)
{
	/* *** CONFIGURE UART GPIO PIN *** */
	/* Enable clock access to gpioD */
	RCC->AHB1ENR |= GPIODEN;

	/* Set PD8 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<17); // '1'
	GPIOD->MODER &=~(1UL<<16); // '0'

	/* Set PD8 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<3); //'0'
	GPIOD->AFR[1] |=(1UL<<2); //'1'
	GPIOD->AFR[1] |=(1UL<<1); //'1'
	GPIOD->AFR[1] |=(1UL<<0);//'1'

	/* Set PD9 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<19); // '1'
	GPIOD->MODER &=~(1UL<<18); // '0'

	/* Set PD9 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<7); //'0'
	GPIOD->AFR[1] |=(1UL<<6); //'1'
	GPIOD->AFR[1] |=(1UL<<5); //'1'
	GPIOD->AFR[1] |=(1UL<<4);//'1'



	/* **** Configure UART Module *** */
	/* Enable clock access to UART2 */
	RCC->APB1ENR |= UART3EN;

	/* Configure baudrate */
	uart_set_baudrate(USART3, APB1_CLOCK, UART_BAUDRATE);


	/* Configure the transfer direction */
	USART3->CR1 = (CR1_TE | CR1_RE); // Set for both TX and RX

	/* Enable the UART module */
	USART3->CR1 |= CR1_UE; // |= so to say, write only that particular bit, and leave the others unchanged cause we already set the bit 3 at the previous line of code
}

void uart3_tx_init(void)
{
	/* *** CONFIGURE UART GPIO PIN *** */
	/* Enable clock access to gpioD */
	RCC->AHB1ENR |= GPIODEN;

	/* Set PD8 mode to alternate function mode */
	GPIOD->MODER |=(1UL<<17); // '1'
	GPIOD->MODER &=~(1UL<<16); // '0'

	/* Set PD8 alternate function type to UART_TX(AF7) */
	GPIOD->AFR[1] &=~(1UL<<3); //'0'
	GPIOD->AFR[1] |=(1UL<<2); //'1'
	GPIOD->AFR[1] |=(1UL<<1); //'1'
	GPIOD->AFR[1] |=(1UL<<0);//'1'


	/* **** Configure UART Module *** */
	/* Enable clock access to UART2 */
	RCC->APB1ENR |= UART3EN;

	/* Configure baudrate */
	uart_set_baudrate(USART3, APB1_CLOCK, UART_BAUDRATE);


	/* Configure the transfer direction */
	USART3->CR1 = CR1_TE; // this overwrites all bits to 0, except the one in the desired position (3) => thus all the parameters of the communication are set as per CR1 bits values

	/* Enable the UART module */
	USART3->CR1 |= CR1_UE; // |= so to say, write only that particular bit, and leave the others unchanged cause we already set the bit 3 at the previous line of code
}



char uart3_rx_read(void)
{
	/* Make sure the receive data register is not empty */
	while(!(USART3->SR & SR_RXNE));

	/* Read data */
	return USART3->DR;
}

void uart3_write(int charYouWantToWrite)
{
	/* Make sure the transmit data register is empty */
	while(!(USART3->SR & SR_TXE ));  //execute this until data is transmitted , then go to the next step

	/* Write to transmit data register */
	USART3->DR = (charYouWantToWrite & 0xFF);
}


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClock, uint32_t BaudRate )
{
	USARTx->BRR = compute_uart_bd(PeriphClock, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClock, uint32_t BaudRate)
{
	return ((PeriphClock + (BaudRate/2UL))/BaudRate);
}

int __io_putchar(int myCharacter)
{
	uart3_write(myCharacter);
	return myCharacter;
}

