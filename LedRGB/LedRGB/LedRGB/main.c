/*
 * LedRGB.c
 *
 * Created: 25/7/2022 23:38:05
 * Author : Administrator
 */ 
#define F_CPU 16000000
#define RAND_MAX 255

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "SerialPort.h"
#include "USART.h"
#include "main.h"
#include "potenciometro.h"
#include "pwm.h"
void setupTimer();
void setupPines();
void setupADC();


volatile int ProcesarInstruccion=0;
char data[5];

int RGB[3] = {50,50,125};

volatile int flagTimer=0;
int main(void)
{
	setupPWM();
	setupTimer(); 
	setupPines(); //Setup pines de salida para PWM
	setupSerialPort(103); //Setup UART
	setupADC();   //Setup ADC potenciometro
	sei();
    /* Replace with your application code */
	iniciar_MEF();	
	while (1) 
    {
		actualizar_MEF(); //Muestra mensajes y captura de datos del usuario
		intensidad();	  //Captura el valor del potenciometro y regula la intencidad del LED
    }
}
