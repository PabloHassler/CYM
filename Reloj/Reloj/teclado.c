/*
 * Teclado.c
 */ 

#include "Teclado.h"
/********************************************************
FUNCION PARA ESCANEAR UN TECLADO MATRICIAL Y DEVOLVER LA
TECLA PRESIONADA UNA SOLA VEZ. TIENE DOBLE VERIFICACION Y
MEMORIZA LA ULTIMA TECLA PRESIONADA
DEVUELVE:
0 -> NO HAY NUEVA TECLA PRESIONADA
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

uint8_t KEYPAD_scan (uint8_t *key){
	PORTD=0b00111100;
	
	for(int c=0;c<4;c++){
		//poner en cero las filas
		DDRD &= ~(0b10000000);
		DDRB &= ~(0b00011001);
		
		if(c==3) DDRD |= filas[c];
		else     DDRB |= filas[c];
		
		for(int r=0; r<4; r++){
			if(!(PIND & columna[r])){
				*key = codChar[c][r];
				_delay_ms(8);
				return(1);
			}
		}
	}
	return(0);
}