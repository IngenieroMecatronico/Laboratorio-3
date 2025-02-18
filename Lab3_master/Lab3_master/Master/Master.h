/*
 * Master.h
 *
 * Created: 11/02/2025 17:10:21
 *  Author: ferma
 */ 


#ifndef MASTER_H_
#define MASTER_H_

typedef enum{
	SPI_MASTER_OSC_DIV2   = 0b01010000,
	SPI_MASTER_OSC_DIV4	  = 0b01010001,
	SPI_MASTER_OSC_DIV8   = 0b01010010,
	SPI_MASTER_OSC_DIV16  = 0b01010011,
	SPI_MASTER_OSC_DIV32  = 0b01010100,
	SPI_MASTER_OSC_DIV64  = 0b01010101,
	SPI_MASTER_OSC_DIV128 = 0b01000000,
}Spi_Type;

typedef enum{
	SPI_DATA_ORDER_MSB = 0b00000000,
	SPI_DATA_ORDER_LSB = 0b00100000
}Spi_Data_Order;

typedef enum{
	SPI_CLOCK_IDLE_HIGH = 0b00001000,
	SPI_CLOCK_IDLE_LOW  = 0b00000000
}Spi_Clock_Polarity;

typedef enum{
	SPI_CLOCK_FIRST_EDGE  = 0b00000000,
	SPI_CLOCK_LAST_EDGE   = 0b00000100
}Spi_Clock_Phase;
//Configuración del Master
void spiInit(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase);
void spiWrite(uint8_t dat);
static void spiReceiveWait(void);
char spiRead();
unsigned spiDataReady(void);

//Inicialización del UART.
void initUart9600(void);

#endif /* MASTER_H_ */