/*
 * IO.c
 *
 * Created: 2018/1/22 14:10:04
 * Author : A19671
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PREMBLE 0x00 // MAN CODE: 0x00, NRZ CODE: 0xAAAA 
#define SFID 0x01 // MAN CODE: 0x01, NRZ CODE: 0xAAA9
#define LENGTH 9

uint8_t lookUp[16] = {0xAA, 0xA9, 0xA6, 0xA5, 0x9A, 0x99, 0x96, 0x95, 0x6A, 0x69, 0x66, 0x65, 0x5A, 0x59, 0x56, 0x55};

volatile char flag = 1;
volatile uint8_t p_uint8 = 0;
uint8_t data[LENGTH] = {PREMBLE, PREMBLE, PREMBLE, PREMBLE, PREMBLE, SFID, 0xCC, 0xCC, 0x01};
uint8_t man_data[LENGTH*2];

void Timer_Init()
{
	// initialize Timer1
	cli();          // disable global interrupts
	TCCR1A = 0;     // set entire TCCR1A register to 0
	TCCR1B = 0;     // same for TCCR1B
	
	// set compare match register to desired timer count:
	OCR1A = 125; //symbol length 125us
	// turn on CTC mode:
	TCCR1B |= (1 << WGM12);
	// Set CS11 bits for 8 prescaler:
	TCCR1B |= (1 << CS11);
	// enable timer compare interrupt:
	TIMSK1 |= (1 << OCIE1A);
	// enable global interrupts:
	sei();
}

void Timer_Close()
{
	cli();
	TCCR1A = 0;     // set entire TCCR1A register to 0
	TCCR1B = 0;     // same for TCCR1B
}

ISR(TIMER1_COMPA_vect)
{
	uint8_t counter = p_uint8 >> 3;
	if ((counter) == 5) {
		Timer_Close();
		flag = 0;
		PORTA = 0x01;
		return;
	}


	PORTA =  man_data[counter] >> ((8 + ~p_uint8) & 0x07);
	//PORTA = ~PORTA;
	p_uint8 = p_uint8 + 1;
}

int main(void)
{
	DDRA |= 1 << DDA0;
	PORTA = 0x00;
	
	for (char ii = 0, jj = 0; ii < LENGTH; ii++) {
		man_data[jj++] = lookUp[data[ii] >> 4];
		man_data[jj++] = lookUp[data[ii] & 0x0F];
	}

	flag = 1;
	Timer_Init();
	//while(flag){};
	
	while(1);
}

