/*
 * interrupcion.c
 *
 * Created: 6/20/2022 8:23:36 PM
 *  Author: Hernan
 */ 
#include "interrupcion.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
extern volatile int checkSensorHumidity;
extern volatile int ProcesarInstruccion;
extern char BufferRX[32];
/*Configuración del timer 0*/
void setupTimer(){
	TCCR0B=(1<<CS02)|(1<<CS00); //configurar el registro del timer0 como temporizador con prescalador de 1024
	TCNT0=99;					//el registro empieza con valor 99
	TIMSK0|= (1<<TOIE0);	    //habilita la interrupcion por desbordamiento del timer0
	sei();				        //habilita interrupciones globales
}
/*
*Rutina de atención de interrupción Timer
*/
ISR(TIMER0_OVF_vect)
{
	uint8_t static cont=0;
	cont++;					//Contador para llegar a interrupciones cada 1 seg
	TCNT0=99;				//Reinicio contador del timer0
	if(cont==100){			//Condición de tiempo requerida (1 seg)
		checkSensorHumidity = true; //Flag 
		cont=0;				//reinicio contador
	}
}
/*
*Rutina de atención de interrupción Terminal serie
*/
ISR(USART_RX_vect){
	volatile char RX_Data = 0;  
	static short int Index=0;	
	
	RX_Data = UDR0;				//Obtengo la información del terminal
	if(RX_Data != '\r'){		//si no es un salto de linea entonces almaceno caracter en el buffer
		BufferRX[Index++] = RX_Data;
	}
	else{						//Si es un salto de linea entonces 
		BufferRX[Index]='\0';	//Determino el final del string
		ProcesarInstruccion = true; //Activo flag para atención de instrucción
		Index=0;				//Vuelvo el indice al principio del vector
	}
}