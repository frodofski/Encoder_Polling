/******************************************************************************************
 * Rotary-encoder decoder for Arduino
 ******************************************************************************************
 * Copyright (c) Robert Bakker 2013
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 ******************************************************************************************
 * This library handles polling and decoding a rotary encoder.
 * It is non-blocking code that executes in the background while your sketch
 * does it's thing.
 *****************************************************************************************/

#include "Encoder_Polling.h"
#include "Arduino.h"	// Has just about everything in it


/*Private #defines************************************************************************/

// Used by state machines
#define STANDBY 0
#define WAITING 2
#define IDLE 4


/*Declare Variables***********************************************************************/

struct encoderVars
{
	volatile uint8_t pin_A;
	volatile uint8_t pin_B;

	volatile uint8_t state;
	volatile int8_t data;

	volatile uint8_t pinState_A;
	volatile uint8_t pinStateOld_A;
	volatile uint8_t pinState_B;
};

// Initialize variables (start all state machines in STANDBY state)
encoderVars encoder = { 0, 0,  STANDBY, 0,  0, 0, 0 };


/*Start of Functions**********************************************************************/

// This function sets up timer2 to trigger an ISR every 300 us.
// It also sets up the input pins.
void encoder_begin(uint8_t pin_A, uint8_t pin_B)
{
	encoder.pin_A = pin_A;
	encoder.pin_B = pin_B;

	pinMode(encoder.pin_A, INPUT);
	pinMode(encoder.pin_B, INPUT);

	// Configure timer 2
	cli();					// Disable global interrupts

	TCCR2A = 0;				// Clear timer2's control registers
	TCCR2B = 0;
	TIMSK2 = 0;				// ...and interrupt mask register (just in case)
	TCNT2 = 0;				// Pre-load the timer to 0
	OCR2A = 149;			// Set output compare register to 149
	TCCR2A |= _BV(WGM21);	// Turn on CTC mode (Clear Timer on Compare match)
	TCCR2B |= 0b011;		// Set prescaler to 32 (starts timer) 
	TIMSK2 |= _BV(OCIE2A);	// Enable timer compare interrupt 

	sei();					// Re-enable global interrupts
}

int8_t encoder_data(void)
{
	if(encoder.state == IDLE)
	{
		int8_t encoderTemp = encoder.data;
		encoder.data = 0;

		// This must always be done last (if it isn't, the state machine could restart 
		// with the other vairable(s) not initialized (ie. data, counter, etc.)).
		encoder.state = STANDBY;

		return encoderTemp;
	}

	else
	{
		return 0;
	}
}


/*ISR*************************************************************************************/

// This is the interrupt itself
// It is only active if encoder_begin() is called
// This polls the input pins and decodes the incoming signals.
// "Decoding" is done using a state machine.
ISR(TIMER2_COMPA_vect)
{
	// State machine for encoder
	switch(encoder.state)
	{
		case STANDBY:
			// First read encoderPin_A's current state into pinStateOld_A.
			encoder.pinStateOld_A = digitalRead(encoder.pin_A);
			//Next step
			encoder.state = WAITING;
			break;

		case WAITING:
			// Read encoderPin_A into different variable
			encoder.pinState_A = digitalRead(encoder.pin_A);
			// If it has changed since the STANDBY state... (ie. encoder is turning)
			if(encoder.pinState_A != encoder.pinStateOld_A)
			{
				// ...read encoderPin_B's state...
				encoder.pinState_B = digitalRead(encoder.pin_B);
				// ...and use it to decide what direction the encoder's turning
				if(encoder.pinState_A == encoder.pinState_B)
				{
					// Set data
					encoder.data = 1;
				}

				else
				{
					// Set data
					encoder.data = -1;
				}

				// Next step (do nothing until further notice)
				encoder.state = IDLE;
			}
			break;
	}
}
