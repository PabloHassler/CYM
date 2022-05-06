/*
 * teclado4x4.c
 *
 * Created: 11/4/2022 19:09:11
 * Author : Administrator
 */ 

/* Inclusión de cabeceras de bibliotecas de código */
#include <avr/io.h> // Definición de Registros del microcontrolador
#define F_CPU 16000000UL // Especifico la frecuencia de reloj del MCU en 8MHz
#include <util/delay.h> // Retardos por software – Macros: depende de F_CPU
#include "lcd.h"
uint8_t KEYPAD_scan (uint8_t *);
uint8_t KEYPAD_Update (uint8_t *);

int mainLcd(void)
{
    /* Replace with your application code */
    uint8_t key=0x00;
	LCDinit();
	LCDclr();
	//setup();
	LCDhome();
	LCDsendChar('a');
	LCDcursorOnBlink();
	while (1) 
    {
    }
}
/*SETUP DE LA PANTALLA LCD*/
void setup(){
	LCDinit();
	LCDclr();
	LCDhome();
	LCDstring((uint8_t *)"CDyM PRUEBA",11);
	LCDGotoXY(0,1);
	LCDstring((uint8_t *)"Tecla:",6);
}
/********************************************************
FUNCION PARA ESCANEAR UN TECLADO MATRICIAL Y DEVOLVER LA
TECLA PRESIONADA UNA SOLA VEZ. TIENE DOBLE VERIFICACION Y
MEMORIZA LA ULTIMA TECLA PRESIONADA
DEVUELVE:
0 -> NO HAYNUEVA TECLA PRESIONADA
1 -> HAY NUEVA TECLA PRESIONADA Y ES *pkey
********************************************************/
uint8_t KEYPAD_Update (uint8_t *pkey)
{
	static uint8_t Old_key;
	uint8_t Key, Last_valid_key=0xFF; // no hay tecla presionada
	if(!KEYPAD_scan(&Key)) {
		Old_key=0xFF; // no hay tecla presionada
		Last_valid_key=0xFF;
		return 0;
	}
	if(Key==Old_key) { //2da verificación
		if(Key!=Last_valid_key){ //evita múltiple detección
			*pkey=Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key=Key; //1era verificación
	return 0;
}

const uint8_t filas[4] =   {0b00010000,0b00001000,0b00000001,0b10000000};
const uint8_t columna[4] = {0b00001000,0b00100000,0b00010000,0b00000100};
const char codChar[4][4] = {{'1','2','3','A'},
							{'4','5','6','B'},
							{'7','8','9','C'},
							{'0','*','#','D'}};

uint8_t KEYPAD_scan (uint8_t *key){
	PORTD=0b01111111;
	
	for(int c=0;c<4;c++){
		//poner en cero las filas
		DDRD &= ~(0b10000000);
		DDRB &= ~(0b00011001);
		
		if(c==3) DDRD |= filas[c];
		else     DDRB |= filas[c];
		
		for(int r=0; r<4; r++){
			if(!(PIND & columna[r])){
				*key = codChar[c][r];
				return(1);
			}
		}
	}
	return(0);
}