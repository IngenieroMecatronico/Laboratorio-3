/*
 * main.c
 *
 * Created: 2/11/2025 11:08:05 AM
 *  Author: Adrian Pascual
 * Lab03 - Digital 2
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SPI/SPI.h"

uint8_t contador = 0b11111111;
uint8_t valorSPI;
uint8_t valor_ADC = 0;
volatile uint8_t voltaje1;
volatile uint8_t voltaje2;
volatile uint8_t spiValor;
volatile uint8_t dato = 3;
volatile uint8_t leds;
char numero;
uint8_t cont;

// Leer ADC del canal seleccionado
uint16_t Read_ADC(uint8_t canal) {
	ADCSRA |= (1<<ADSC);   //Habilito la interrupcion de ADC.
	ADMUX = (ADMUX & 0xF8) | (canal & 0x07); // Seleccionar canal
	//ADCSRA |= (1 << ADSC); // Iniciar conversión
	while (ADCSRA & (1 << ADSC)); // Esperar que termine la conversión
	
	return ADC;
}

void initADC(void);
void refreshPORT(uint8_t valor);

int main(void)
{
	cli();
	DDRB |= (1 << PORTB0)|(1 << PORTB1);  // LEDS
	DDRD |= (1 << DDD2)|(1 << DDD3)|(1 << DDD4)|(1 << DDD5)|(1 << DDD6)|(1 << DDD7); //LEDS
	PORTC &= ~((1 << PORTC1)|(1 << PORTC3)); // Potenciometros
	PORTD &= ~((1 << PORTD2)|(1 << PORTD3)|(1 << PORTD4)|(1 << PORTD5)|(1 << PORTD6)|(1 << PORTD7)); //Limpiar puertos
	PORTC = 0;
	PORTD = 0;
	
	spiInit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE); // SLAVE
	initADC();
	
	SPCR |= (1 << SPIE); // Interrupcion
	sei(); 
	ADCSRA |= (1<<ADSC);   //Habilito la interrupcion de ADC.

    while(1)
    {
		// Leer voltajes de los potenciómetros
		
		if (dato == 0)
		{
			uint16_t valor1 = Read_ADC(1); // Potenciómetro en ADC1	
			voltaje1 = valor1/256;
			spiWrite(voltaje1);
			dato = 3;
		}
		if (dato == 1)
		{
			uint16_t valor2 = Read_ADC(3); // Potenciómetro en ADC3
			voltaje2 = valor2/256;
			spiWrite(voltaje2);
			dato = 3;
		
		}
		if (dato == 2)
		{
			refreshPORT(leds);
// 			while(cont == 0){
// 			numero = spiRead();
// 			//_delay_ms(250);
// 			cont = numero;
// 			refreshPORT(cont);
// 			_delay_ms(100);
// 			}
// 			cont = 0;
		}
		
    }
}

void refreshPORT(uint8_t valor){
	if(valor & 0b10000000){
		PORTD |= (1<<PORTD2);
	}
	else{
		PORTD &= ~(1<<PORTD2);
	}
	if(valor & 0b01000000){
		PORTD |= (1<<PORTD3);
	}
	else{
		PORTD &= ~(1<<PORTD3);
	}
	if(valor & 0b00100000){
		PORTD |= (1<<PORTD4);
	}
	else{
		PORTD &= ~(1<<PORTD4);
	}
	if(valor & 0b00010000){
		PORTD |= (1<<PORTD5);
	}
	else{
		PORTD &= ~(1<<PORTD5);
	}
	if(valor & 0b00001000){
		PORTD |= (1<<PORTD6);
	}
	else{
		PORTD &= ~(1<<PORTD6);
	}
	if(valor & 0b00000100){
		PORTD |= (1<<PORTD7);
	}
	else{
		PORTD &= ~(1<<PORTD7);
	}
	if(valor & 0b00000010){
		PORTB |= (1<<PORTB0);
	}
	else{
		PORTB &= ~(1<<PORTB0);
	}
	if(valor & 0b00000001){
		PORTB |= (1<<PORTB1);
	}
	else{
		PORTB &= ~(1<<PORTB1);
	}
}

void initADC(void){
	ADMUX = 0;
	
	// Referencia AVCC = 5V
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	// Justificación a la izquierda
	ADMUX |= (1 << ADLAR);
	
	ADCSRA = 0;
	
	// Habilitamos ISR de ADC
	ADCSRA |= (1 << ADIE);
	
	// Habilitar prescaler de 128, F_ADC = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// Habilitando el ADC
	ADCSRA |= (1 << ADEN);
}

ISR(ADC_vect){
	valor_ADC = ADCH;
	
	// Apagar la bandera
	ADCSRA |= (1 << ADIF);
	
}

ISR(SPI_STC_vect){
	uint8_t spiValor = SPDR;
	
	if(spiValor == 0){ // Si el Master envía 1, leer ADC1
		spiWrite(voltaje1);
		dato = 0;
	}if(spiValor == 1){ // Si envía 2, enviar ADC2
		spiWrite(voltaje2);
		dato = 1;
	}if (spiValor == 2){
		dato = 2;
	}
	else{
		if (dato == 2)
		{
			leds = spiValor;
		}
		}
}


