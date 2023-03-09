/*
 * Teclado.h
 */ 
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
	#define F_CPU 16000000UL

#include <util/delay.h>
const uint8_t filas[4] =   {0b00010000,0b00001000,0b00000001,0b10000000};
const uint8_t columna[4] = {0b00001000,0b00100000,0b00010000,0b00000100};
const char codChar[4][4] =  {{'1','2','3','A'},
							{'4','5','6','B'},
							{'7','8','9','C'},
							{'0','*','#','D'}};

uint8_t KEYPAD_scan (uint8_t *);
uint8_t KEYPAD_Update (uint8_t *);