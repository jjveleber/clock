/*
 * Clock.c
 *
 * Created: 8/12/2016 7:52:25 AM
 * Author : jjveleber
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL                    // set the CPU clock
#endif

#define BAUD 9600                           // define baud
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)    // set baudrate value for UBRR


#include <avr/io.h>
#include <avr/interrupt.h>
#include "RingBuffer.h"

// Time Data
volatile uint8_t seconds = 0;
volatile uint8_t minutes = 0;
volatile uint8_t hours = 0;
volatile uint8_t* curtime[3];
volatile uint8_t txTime = 0;
volatile uint8_t timeInputPos = 0;

// Wifi Data
RingBuffer wifiBuffer;

void setupTimers(void);
void setupIO(void);
void setupUSARTs(void);
void usart0SendByte(uint8_t u8Data);
void usart1SendByte(uint8_t u8Data);
void computeTime(void);
void transmitTime(void);

ISR(TIMER5_COMPA_vect) {
	cli();
	PORTD ^= (1 << PD7);
	seconds++;
	txTime = 1;
	sei();
}

ISR(USART0_RX_vect) {
	uint8_t value = UDR0;
	if(value == 0xFF || timeInputPos >= 3) {
		timeInputPos = 0;
	} else  {
		*curtime[timeInputPos++] = value;
	}
}

ISR(USART1_RX_vect) {
	ringBufferPut(&wifiBuffer, UDR1);
}

int main(void) {
	
	curtime[0] = &seconds;
	curtime[1] = &minutes;
	curtime[2] = &hours;
	
	setupIO();
	setupTimers();
	setupUSARTs();
    /* Replace with your application code */
    while (1){ 
		computeTime();
		if(txTime) {
			txTime = 0;
			transmitTime();
		}
	}
}

void setupTimers(void) {
	cli();
	
	// ***************************** TIMER 5 *************************>>>>
	// Initialize Timer
	// Clear TCCR registers
	TCCR5A = 0; 
	TCCR5B = 0;
	
	// Set Compare match register to timer count for 1 second
	// ((1 s) / (6.4e-5 s)) -1
	// 15 = about 1 millisecond
	// 15624 = 1 second
	//OCR5A =  15; //lower number for simulator
	OCR5A = 15624;

	
	// Turn on CTC mode
	TCCR5B |= (1 << WGM52);
	
	// Set 1024 prescaler bits
	TCCR5B |= (1 << CS50);
	TCCR5B |= (1 << CS52);

	
	// Enable Timer compare interrupt
	TIMSK5 |= (1 << OCIE5A);
	// <<<<************************* TIMER 5 *****************************
	
	sei();
}

void setupIO(void) {
	DDRD  = 0b11111111;
	PORTD = 0xFF;
	
	DDRB = 0xFF;
	PORTB = 0xFF;
}

void setupUSARTs(void){
	// USART 0 -- Clock debug
    UBRR0H = (BAUDRATE>>8);
    UBRR0L = BAUDRATE;                        //set baud rate
    UCSR0B |= (1<<TXEN0)|(1<<RXEN0);          //enable receiver and transmitter
	UCSR0B |=  (1<<RXCIE0);					// enable receiver interrupt 
    UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);		// 8bit data format
	
	// USART 1 -- Wifi Comms
	UBRR1H = (BAUDRATE>>8);
	UBRR1L = BAUDRATE;                        //set baud rate
	UCSR1B |= (1<<TXEN1)|(1<<RXEN1);          //enable receiver and transmitter
	UCSR1B |=  (1<<RXCIE1);					// enable receiver interrupt
	UCSR1C |= (1<<UCSZ10)|(1<<UCSZ11);		// 8bit data format
}

void usart0SendByte(uint8_t u8Data) {
	// Wait until last byte has been transmitted
	while (!( UCSR0A & (1<<UDRE0))); 
	// Transmit data
	UDR0 = u8Data;
}

void usart1SendByte(uint8_t u8Data) {
	// Wait until last byte has been transmitted
	while (!( UCSR0A & (1<<UDRE0)));
	// Transmit data
	UDR0 = u8Data;
}

void computeTime(void){
	if(seconds >= 60) {
		minutes++;
		seconds -= 60;
	}
	
	if(minutes >= 60) {
		hours++;
		minutes -= 60;
	}
	
	if(hours >= 24) {
		hours -= 24;
	}
	
}

void transmitTime(void) {
	usart0SendByte(hours);
	usart0SendByte(minutes);
	usart0SendByte(seconds);
	usart0SendByte(0xFF);
}