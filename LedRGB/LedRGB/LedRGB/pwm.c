#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "main.h"
#define F_CPU 16000000
extern volatile int flagTimer;

/*
*Rutina de atención de interrupción Timer
*/
ISR(TIMER0_COMPA_vect)
{
	if(flagTimer){
		PORTB |= (1 << 5);
	}
}

ISR(TIMER0_OVF_vect)
{
	if(flagTimer){
		PORTB &= ~(1 << 5);
	}
}
void setupTimer(){
	TCCR0B |= (1<<CS01)|(1<<CS00);	//configurar el registro del timer0 como temporizador con prescalador de 1024
	TCCR0A = 0x03;				// modo PWM
	OCR0A = 0;					//el registro empieza con valor 255
	TIMSK0 |= (1<<OCIE0A)|(1<<TOIE0); //habilita la interrupcion por desbordamiento del timer0
}
void setupPines(){
	DDRB |= (1<<1)|(1<<2)|(1<<5);
}
void setupPWM(){
	TCCR1B |= (1<<WGM12)|(1<<CS11)|(1<<CS10); //Configuracion prescalador en 64 y modo
}
void pwm(int pin,int num){
	switch(pin){
		case 'R':		//Led rojo
		if(num<3){		//Cota inferior para apagar el led
			TCCR1A &= ~(1<<COM1A1);	//Apagamos el PWM
			TCCR1A &= ~(1<<COM1A0);	
			PORTB|= (1<<1);			//Seteamos la salida en alto
		}
		else{			//Led prendido
			TCCR1A |= (1<<WGM10)|(1<<COM1A1)|(1<<COM1A0); //Configuramos fast pwm, inverted
			OCR1A=num;									 // Asignamos el valor para el ciclo de trabajo
		}
		break;
		case 'G':	   //Led Verde
		if(num<3){		//Cota inferior para apagar el led
			TCCR1A &= ~(1<<COM1B1);	//Apagamos el PWM
			TCCR1A &= ~(1<<COM1B0);
			PORTB|= (1<<2);		//Seteamos la salida en alto
		}
		else{
			TCCR1A |= (1<<WGM10)|(1<<COM1B1)|(1<<COM1B0);//Configuramos fast pwm, inverted
			OCR1B=num;									//Asignamos el valor para el ciclo de trabajo
		}
		break;
		case 'B':				//Led azul
		if(num<3){				//Cota inferior para apagar el led
			flagTimer = false;	//Desactivamos el pwm
			PORTB|= (1<<5);		//Seteamos el led en alto
		}
		else{					//Led prendido
			flagTimer = true;	//Activamos el pwm
			TCCR0A=0x03;		//Configuramos fast pwm
			OCR0A=num;			//Registro de comparacion para el ciclo de trabajo
		}
		break;
	}
}