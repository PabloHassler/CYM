/*
 * Reloj.c
 *
 * Created: 25/4/2022 14:54:59
 * Author : Barcala
 */  

/* Inclusión de cabeceras de bibliotecas de código */
#include <avr/io.h> // Definición de Registros del microcontrolador
#define F_CPU 16000000UL // Especifico la frecuencia de reloj del MCU en 8MHz
#include <util/delay.h> // Retardos por software – Macros: depende de F_CPU
#include <avr/interrupt.h>
#include "lcd.h"
uint8_t KEYPAD_scan (uint8_t *);
uint8_t KEYPAD_Update (uint8_t *);
void setup();

ISR(TIMER0_OVF_vect)
{
	static unsigned cont=0;
	static unsigned seg=0;
	if(++cont==50){
		//
		seg++;
		TCNT0=99;//reinicio contador del timer0
		cont=0;//reinicio contador
	}
	
}

int main(void)
{
    /* Replace with your application code */
   uint8_t key=0x00;
	TCCR0B=(1<<CS02)|(1<<CS00);//configurar el registro del timer0 como temporizador con prescalador de 1024
	TCNT0=99;//el registro empieza con valor 99
	TIMSK0|= (1<<TOIE0);//habilita la interrupcion por desbordamiento del timer0
	sei();//habilita interrupciones globales
	setup();

	while (1) 
    {
		if(KEYPAD_Update (&key)){
			LCDsendChar(key);//se manda key al lcd
			LCDGotoXY(6,1);
			_delay_ms(5);
		}
    }
}
/*SETUP DE LA PANTALLA LCD*/
void setup(){
	LCDinit();
	LCDclr();
	LCDGotoXY(0,0);
	LCDstring((uint8_t*)"31/12/21",8);	
	LCDGotoXY(0,1);
	LCDstring((uint8_t*)"23:59:59",8);
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