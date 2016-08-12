/*
 * Clock.c
 *
 * Created: 8/12/2016 7:52:25 AM
 * Author : jjveleber
 */ 

#define F_CPU 16000000UL 
#define TIMER_RESOLUTION (1/(F_CPU/64))

#include <avr/io.h>
#include <avr/interrupt.h>

void setupTimers();
void setupIO();

ISR(TIMER1_COMPA_vect) {
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
	
	// Initialize Timer
	// Clear TCCR registers
	TCCR1A = 0; 
	TCCR1B = 0;
	
	// Set Compare match register to timer count for 1 second
	// ((1 s) / (6.4e-5 s)) -1
	OCR1A =  1/TIMER_RESOLUTION; //(1/6.4e-5f) - 1; //15624; 
	
	// Turn on CTC mode
	TCCR1B |= (1 << WGM12);
	
	// Set 64 prescalar bits
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS11);
	
	// Enable Timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	sei();
}

void setupIO() {
	DDRD  = 0b11111111;
}

