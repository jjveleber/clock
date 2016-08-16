/*
 * Clock.c
 *
 * Created: 8/12/2016 7:52:25 AM
 * Author : jjveleber
 */ 

#define F_CPU 16000000UL 

#include <avr/io.h>
#include <avr/interrupt.h>

void setupTimers();
void setupIO();

ISR(TIMER5_COMPA_vect) {
	PORTD ^= (1 << PD7);
}

int main(void) {
	setupIO();
	setupTimers();
    /* Replace with your application code */
    while (1) 
    {
    }
}

void setupTimers() {
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
	OCR5A =  15; //lower number for simulator
	
	// Turn on CTC mode
	TCCR5B |= (1 << WGM52);
	
	// Set 1024 prescalar bits
	TCCR5B |= (1 << CS50);
	TCCR5B |= (1 << CS52);
	
	// Enable Timer compare interrupt
	TIMSK5 |= (1 << OCIE5A);
	// <<<<************************* TIMER 5 *****************************
	
	sei();
}

void setupIO() {
	DDRD  = 0b11111111;
}

