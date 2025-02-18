/*
 * Master.c
 *
 * Created: 11/02/2025 17:10:04
 *  Author: ferma
 */ 
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "Master.h"
void spiInit(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase){
	 //PB2 -> SS
	 //PB3 -> MOSI
	 //PB4 -> MISO
	 //PB5 -> SCK
	 if (sType & 0b00010000) //MasterMode
	 {
			DDRB |= (1<<PORTB3)|(1<<PORTB5)|(1<<PORTB2);
			DDRB &= ~(1<<PORTB4);
			SPCR |= (1<<MSTR);
			
			uint8_t temp = sType & 0b00000111;
			switch(temp){
				case 0: //DIV2
					SPCR &= ~((1<<SPR1)|(1<<SPR0));
					SPSR |= (1<<SPI2X);
				break;
				case 1: //DIV4
					SPCR &= ~((1<<SPR1)|(1<<SPR0));
					SPSR &= ~(1<<SPI2X);
				break;
				case 2: //DIV8
					SPCR |= (1<<SPR0);
					SPCR &= ~(1<<SPR1);
					SPSR |= (1<<SPI2X);
				break;
				case 3: //DIV16
					SPCR |= (1<<SPR0);
					SPCR &= ~(1<<SPR1);
					SPSR &= ~(1<<SPI2X);
				break;
				case 4://32
					SPCR |= (1<<SPR1);
					SPCR &= ~(1<<SPR0);
					SPSR |= (1<<SPI2X);	
				break;
				case 5: //64
					SPCR |= (1<<SPR0);
					SPCR |= (1<<SPR1);
					SPSR |= (1<<SPI2X);
				break;
				case 6: //128
					SPCR |= (1<<SPR0);
					SPCR |= (1<<SPR1);
					SPSR &= ~(1<<SPI2X);
				break;
			}
					
	 }
	 SPCR |= (1<<SPE)|sDataOrder|sClockPolarity|sClockPhase;
}

static void spiReceiveWait(void){
	while (!(SPSR & (1<<SPIF)));
}

void spiWrite(uint8_t dat){
	SPDR = dat;
}

unsigned spiDataReady(void)
{
	if(SPSR & (1<<SPIF))
	return 1;
	else
	return 0;
}

char spiRead()
{
	while (!(SPSR & (1<<SPIF)));
	return SPDR;
}


void initUart9600(void){
	//Paso 1: RX como entrada y TX como salida.
	DDRD |= (1<<DDD1);
	DDRD &=	~(1<<DDD0);
	//Paso 2: Configurar UCSR0A
	UCSR0A = 0;
	//Paso 3: Configurar UCSR0B: habilitamos ISR de recepción y RX Y TX.
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0); //Estoy activando la bandera que me indica que se recibió el mensaje.
	//Paso 4: COnfigurar UCSR0C: Primero asíncrono, sin pariedad, bit 1 de stop, Data bits/ 8 bits,
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//Paso 5: Configurar velocidad de Baudrate: 115200
	UBRR0 = 8;
}
