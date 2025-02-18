/*
 * Lab3_master.c
 *
 * Created: 11/02/2025 16:46:22
 * Author : Juanfer Maldonado - 22449.
 * Descripción: 
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Master/Master.h"
#include <stdio.h>

//*******************************
// Prototipos de función
//*******************************
uint8_t valorSPI = 0;
volatile uint8_t BufferTX;          // Variable donde se guarda el dato recibido.
volatile uint8_t datorecibido;       // Variable para mantener el dato recibido.
volatile uint8_t entero, primdecimal, segdecimal, pedirmodo, modo, valorascii, centesima; //Variables para guardar los valores de los potenciometros.
volatile uint16_t numero;			//Variable para verificar si el ASCII esta entre 0-255

//*******************************
// Variables globales
//*******************************
void spiWrite(uint8_t dat);
static void spiReceiveWait(void);
void WriteTextUART(char * Texto);
void writeUART(char Caracter);
void refreshPORT(uint8_t valor);
void LecturaAscii(void);
char UART_Read();

int main(void)
{
    initUart9600();   // Inicializo mi TX y RX (115200)
    sei();            // Habilito las interrupciones globales
    DDRD |= 0xFC;     // Puerto D: LEDs conectados menos en RX y TX
    PORTD = 0;
    DDRB |= (1<<PORTB0) | (1<<PORTB1); //Leds más significativos.
    PORTB = 0;
    BufferTX = 0;     // Limpio la variable donde recibo los datos UART.
	//Inicializo mis variables.
    numero = 0;
    primdecimal = 0;
    segdecimal = 0;
    entero = 0;
    pedirmodo = 0;
    spiInit(SPI_MASTER_OSC_DIV16, SPI_DATA_ORDER_MSB, SPI_CLOCK_IDLE_LOW, SPI_CLOCK_FIRST_EDGE);
	//Comunico que el SPI va a estar a una frecuencia de 16	Mhz/16, tiene que ser la misma que el slave. MSB es el más significativo. Low toma la parte inferior de los datos.
	//First Edge es que empieza en el flanco positivo.    
    while (1) 
    {
		//Mi menú desplegable en la hiperterminal.
        WriteTextUART("\n\r");
        WriteTextUART("Que opcion desea elegir? a. Leer los valores de los potenciometros. b. Mandar un dato para lectura ASCII. ");
		
        pedirmodo = 0;
        BufferTX = 0;
        // Espera hasta que se reciba 'a' (97) o 'b' (98)
        while (pedirmodo == 0)
        {
            modo = BufferTX; //BufferTX;
            switch (modo)
            {
                case 97: // 'a'
                    pedirmodo = 1;
                    break;
                case 98: // 'b'
                    pedirmodo = 1;
                    break;
				default:
					modo = 0;
				break;
            }
        }
        switch (modo)
        {
            case 97: // Modo potenciometro
				//Inicializo mis variables.
				numero = 0;
				primdecimal = 0;
				segdecimal = 0;
				entero = 0;
                PORTB &= ~(1<<PORTB2); // Slave Select activo
                spiWrite(0); //Mando un cero que el slave lo detecta para hacer la opción del primer potenciómetro.
                valorSPI = spiRead(); //Leo lo del slave.
                numero = 1.96 * valorSPI; //Mapeo de 0-255 a 500-0.
                entero = numero / 100; //Separo uno por uno los bits.
                primdecimal = (numero - (entero * 100)) / 10;
                segdecimal = numero - (entero * 100) - (primdecimal * 10);
                WriteTextUART("\n \r Valor_de_potenciometro_1: ");
                writeUART(entero + 48); //Le escribo +48 por el formato ASCII.
                WriteTextUART(".");
                writeUART(primdecimal + 48);
                writeUART(segdecimal + 48);
                WriteTextUART("V");
                WriteTextUART("\n\r\n\r");
                //refreshPORT(valorSPI);
                PORTB |= (1<<PORTB2);
                _delay_ms(250);
                
				//Repito lo mismo que de arriba.
                PORTB &= ~(1<<PORTB2); // Slave Select activo
				numero = 0;
				primdecimal = 0;
				segdecimal = 0;
				entero = 0;
                spiWrite(1);
                valorSPI = spiRead();
                numero = 1.96 * valorSPI;
                entero = numero / 100;
                primdecimal = (numero - (entero * 100)) / 10;
                segdecimal = numero - (entero * 100) - (primdecimal * 10);
                WriteTextUART("\n \r Valor_de_potenciometro_2: ");
                writeUART(entero + 48);
                WriteTextUART(".");
                writeUART(primdecimal + 48);
                writeUART(segdecimal + 48);
                WriteTextUART("V");
                WriteTextUART("\n\r\n\r");
                //refreshPORT(valorSPI);
                PORTB |= (1<<PORTB2);
                _delay_ms(250);
                break;
                
            case 98: // Modo formato ASCII
                PORTB &= ~(1<<PORTB2); // Slave Select activo
				spiWrite(2);		   // Escribo lo mismo para el slave.
				_delay_ms(100);
                LecturaAscii();		   //Llamo mi función para mi valor ASCII.
                PORTB |= (1<<PORTB2);
                _delay_ms(250);
                //_delay_ms(100);
                break;
        }
    }
}



//*******************************
// Subrutinas
//*******************************
void configmaster(void){
    // Configuración del pin MISO.
    DDRB &= ~(1<<PORTB3); // Se configura como INPUT.
}
//Esta función solo me sirve para refrescar los valores en los puertos.
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

void WriteTextUART(char * Texto){
    uint8_t i;
    for (i = 0; Texto[i] != '\0'; i++){
        while (!(UCSR0A & (1<<UDRE0)));
        UDR0 = Texto[i];
    }
}

void writeUART(char Caracter){
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = Caracter;
}

//***********************************************
// ISR de UART
//***********************************************
ISR(USART_RX_vect){
    uint8_t datoRecibido = UDR0;
    BufferTX = datoRecibido;
    // Eco no bloqueante: se envía el dato solo si el registro está libre.
    if (UCSR0A & (1 << UDRE0)) {
        UDR0 = datoRecibido;
    }
}

void LecturaAscii(void){
    uint8_t centesima, decima, unidad;
	uint16_t leds;
	uint8_t datos;
    _delay_ms(100);
	//Inicializo mis variables.
	leds = 0;
	centesima = 0;
	decima = 0;
	unidad = 0;
	WriteTextUART("\n\r");
	WriteTextUART("Recuerde ingresar un valor entre 0, siendo 000 a 255:  ");
    
    // Lectura de la centésima
    WriteTextUART("\n\r");
    WriteTextUART("Ingresa la centesima: ");
    BufferTX = 0;                  // Limpia BufferTX
    while (BufferTX == 0)
        ; // Espera a que se reciba un carácter
    centesima = BufferTX;
    WriteTextUART("\n\r");
    WriteTextUART("El valor escogido fue: ");
    writeUART(centesima);
    
    _delay_ms(100);
    
    // Lectura de la décima
    WriteTextUART("\n\r");
    WriteTextUART("Ingresa la decima: ");
    BufferTX = 0;
    while (BufferTX == 0)
        ; // Espera a que se reciba un carácter
    decima = BufferTX;
    WriteTextUART("\n\r");
    WriteTextUART("El valor escogido fue: ");
    writeUART(decima);
    
    _delay_ms(100);
    
    // Lectura de la unidad
    WriteTextUART("\n\r");
    WriteTextUART("Ingresa la unidad: ");
    BufferTX = 0;
    while (BufferTX == 0)
        ; // Espera a que se reciba un carácter
    unidad = BufferTX;
    WriteTextUART("\n\r");
    WriteTextUART("El valor escogido fue: ");
    writeUART(unidad);
    
    // Convertir de ASCII a número (asumiendo que se ingresaron dígitos '0'-'9')
	leds = 0;
    leds = (((centesima - 48) * 100) + ((decima - 48) * 10) + (unidad - 48));
	WriteTextUART("\n \r El valor escogido fue de: ");
	writeUART(centesima);
	writeUART(decima);
	writeUART(unidad);
	//Validación que el valor esta entre 0 y 255.
	if ((leds >= 0) && (leds <= 255 ))
	{
		//Si si, entonces mando el valor a SPI y refresco los leds.
		WriteTextUART("\r \n Ingreso a rango: ");
		datos = leds;
		refreshPORT(datos);
		spiWrite(datos);
		_delay_ms(1000);
	}
	else{
		WriteTextUART("\r \n No ingreso: ");
		WriteTextUART("\r \n El valor ingresado es incorrecto:");
	}
	//spiWrite(leds);
	//spiWrite(9);
	//return leds;
}

char UART_Read() {
	while (!(UCSR0A & (1 << RXC0))); // Esperar dato disponible
	return UDR0;
}