/*
 * main.h
 */ 
#include <avr/io.h>			// Definición de Registros del microcontrolador
#include <avr/interrupt.h>
#define F_CPU 16000000UL // Especifico la frecuencia de reloj del MCU en 8MHz
#include <util/delay.h>
#include <avr/sleep.h>
#include "lcd.h"

void setupLCD();
void setupTimer();

void actualizar_MEF();
void iniciar_MEF();


#ifndef MAIN_H_
#define MAIN_H_




#endif /* MAIN_H_ */