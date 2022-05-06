/*
 * timer0_oscilador10hz.c
 *
 * Created: 18/4/2022 05:47:37
 * Author : Administrator
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned cont=0;
ISR(TIMER0_OVF_vect)
{
	cont++;
	if(cont==1){
		PORTB^=(1<<PORTB5);//invierto PB5		
		TCNT0=99;//reinicio contador del timer0
		cont=0;//reinicio contador
	}
	
}

int mainOscilador(void)
{
	DDRB=0b00100000;//PB5 como salida
	TCCR0B=(1<<CS02)|(1<<CS00);//configurar el registro del timer0 como temporizador con prescalador de 1024
	TCNT0=99;//el registro empieza con valor 99
	TIMSK0|= (1<<TOIE0);//habilita la interrupcion por desbordamiento del timer0
	sei();//habilita interrupciones globales
    /* Replace with your application code */
    while (1) 
    {
    }
}

