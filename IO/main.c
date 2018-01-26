/*
 * IO.c
 *
 * Created: 2018/1/22 14:10:04
 * Author : A19671
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define PRE_CNT 10*8;
#define DATA_CNT (2*8*8 + 1)
uint8_t cnt = 0, d_flag = 0;
uint8_t pre[8] = {1,0,1,0,1,0,1,0};
uint8_t sfid[8] = {1,0,1,0,1,0,0,1};
uint8_t data[DATA_CNT] = {
							//ID
							1,0, 1,0, 1,0, 0,1, 1,0, 0,1, 1,0, 1,0,//0x14
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 0,1, 0,1,//0x13
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 0,1, 1,0,//0x12
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 1,0, 0,1,//0x11
							//data
							1,0, 1,0, 1,0, 0,1, 1,0, 0,1, 1,0, 1,0,//0x14
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 0,1, 0,1,//0x13
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 0,1, 1,0,//0x12
							1,0, 1,0, 1,0, 0,1, 1,0, 1,0, 1,0, 0,1,//0x11
							0
						};
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

ISR(TIMER1_COMPA_vect)
{
	static uint8_t i = 0;
	if (cnt > 0) {
		PORTA = pre[i];
		cnt--;
		i++;
		if (i > 7) i = 0;
	} else {
		if (d_flag) {
			PORTA = data[i];
			i++;
			if (i == DATA_CNT) {
				TCCR1A = 0;     // set entire TCCR1A register to 0
				TCCR1B = 0;     // same for TCCR1B
			}
		} else {
			PORTA = sfid[i];
			i++;
			if (i > 7) {
				i = 0;
				d_flag = 1;
			}
		}
	}
}

int main(void)
{
	DDRA |= 1 << DDA0;
	cnt = PRE_CNT;
	d_flag = 0;
	Timer_Init();
	
    while (1) 
    {
    }
}

