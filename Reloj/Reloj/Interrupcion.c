/*
 * Interrupcion.c
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
/************************************************************************/
/* Configuracion de los registros del timer0                            */
/************************************************************************/
void setupTimer(){
	TCCR0B=(1<<CS02)|(1<<CS00); //configurar el registro del timer0 como temporizador con prescalador de 1024
	TCNT0=100;					//el registro empieza con valor 100
	TIMSK0|= (1<<TOIE0);	    //habilita la interrupcion por desbordamiento del timer0
	sei();				        //habilita interrupciones globales
}
/************************************************************************/
/* Funcion de la interrupción del timer0                                */
/************************************************************************/
ISR(TIMER0_OVF_vect)
{
	uint8_t static cont=0;
	cont++; 
	TCNT0=100;//reinicio contador del timer0
	if(cont==100){
		relojFuncion(); 
		cont=0;//reinicio contador
	}
	
}
