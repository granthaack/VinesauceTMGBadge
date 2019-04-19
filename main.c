/*
 * VineBadge.c
 *
 * Created: 1/31/2019 1:22:12 PM
 * Author : granthaack
 */  
#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define LED_COUNT 30

uint8_t wait = 1;

const uint16_t point_frames[] PROGMEM = {
	0b0001101100000001,0b0000000000000000,
	0b0011000110000000,0b0000000000000000,
	0b0110000011000000,0b0000010000000000,
	0b1100000001100000,0b0000111000000001,
	0b0000000000110000,0b0001101100000011,
	0b0000000000011000,0b0011000110000110,
	0b0000000000001100,0b0110000011001100,
	0b0000010000000110,0b1100000001111000,
	0b0000111000000011,0b1000000000110000};

const uint8_t LED_DDR[] PROGMEM =  {0b00001001, 0b00001001, 0b00100001, 0b00110000, 
									0b00110000, 0b00100010, 0b00100010, 0b00010001, 
									0b00010001, 0b00010010, 0b00010010, 0b00100100,
									0b00100100, 0b00010100, 0b00010100, 0b00000101,
									0b00000101, 0b00011000, 0b00100001, 0b00011000,
									0b00000011, 0b00000011, 0b00101000, 0b00101000,
									0b00001010, 0b00001010, 0b00000110, 0b00000110,
									0b00001100, 0b00001100};
const uint8_t LED_PORT[] PROGMEM = {0b00000001, 0b00001000, 0b00100000, 0b00010000, 
									0b00100000, 0b00000010, 0b00100000, 0b00000001, 
									0b00010000, 0b00000010, 0b00010000, 0b00000100,
									0b00100000, 0b00010000, 0b00000100, 0b00000100,
									0b00000001, 0b00010000, 0b00000001, 0b00001000,
									0b00000010, 0b00000001, 0b00001000, 0b00100000,
									0b00001000, 0b00000010, 0b00000010, 0b00000100,
									0b00001000, 0b00000100};

void set_led(uint8_t led){
	DDRA = LED_DDR[led % LED_COUNT];
	PORTA = LED_PORT[led % LED_COUNT];
}

void set_leds(uint16_t* fbuf, uint16_t time){
		do{
			for(uint8_t i = 0; i < LED_COUNT/2; i++){
				if(fbuf[0] & (1 << i)){
					set_led(i);
				}
			}
			for(uint8_t i = 0; i < LED_COUNT/2; i++){
				if(fbuf[1] & (1 << i)){
					set_led(i + LED_COUNT/2);
				}
			}		time--;
		}while(time);
}

void twinkle(){
	wait = 1;
	while(wait){
		uint16_t buf[] = {rand(), rand()};
		set_leds(buf, 40);
	}
}

void shimmer(){ 
	wait = 1;
	uint16_t all_on[] = {0xFFFF, 0xFFFF};
	uint16_t buf[] = {0xAAAA, 0xAAAA};
	while(wait){
		buf[0] ^= 0xFFFF;
		buf[1] ^= 0xFFFF;
		set_leds(buf, 50);
		set_leds(all_on, 50);
	}
}

void chase(){
	wait = 1;
	while(wait){
		for(uint8_t i = 0; i < LED_COUNT - 10; i++){
			uint16_t j = 200;
			do{
				set_led(i);
				set_led((i % 10) + LED_COUNT - 10);
				j--;
			}while(j);
			_delay_ms(10);
		}
	}
}

void point(){
	wait = 1;
	while(wait){
		for(uint8_t i = 0; i < 18; i += 2){
			set_leds(point_frames + i, 30);
		}
	}
}

void ringlet(){
	wait = 1;
	uint16_t leds[] = {0b0000000000000000, 0b1111111111100000};
	while(wait){
		set_leds(leds, 75);
		leds[0] = ~leds[0];
		leds[1] = ~leds[1];
	}
}

ISR(INT0_vect)
{
	wait = 0;
}

int main(void)
{
	//Enable pull up on PB2(INT0)
	PUEB = 0x04;
	//Set INT0 interrupt bit
	GIMSK = 0x01;
	//Set INT0 to activate on falling edge
	MCUCR = 0b11000000;
	//Enable global interrupt enable bit
	sei();
	while(1){
		ringlet();
		twinkle();
		chase();
		shimmer();
		point();
	}

}
