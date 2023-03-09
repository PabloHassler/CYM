#define F_CPU 16000000
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "serialPort.h"
#include "main.h"
#include "USART.h"

volatile int ProcesarInstruccion;
state estado;

char BufferRX[32];
char BufferTX[32];
extern int RGB[3];


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
void iniciar_MEF(){
	estado=S0;
	//PWM manual entre 7 y 248 anda joya (simulador)
	pwm('R',0);
	pwm('G',0);
	pwm('B',0);
}

void actualizar_MEF(){
	switch (estado){
		case S0: 
			SerialPort_Send_String("Ingrese un valor entre 0 y 255 para R:\n\r"); 
			estado = S1;
		break;
		case S1:
		if(ProcesarInstruccion){	//Esperamos que el usuario ingrese un valor
			ProcesarInstruccion=0;  //Desactivamos el flag de nueva info
			if(procesarEntrada(0)){ //Vericamos que la entrada sea valida
				estado=S2;			//Pasamos a imprimir el proximo mensaje
			}
			else{
				estado=S0;			//Volvemos a imprimir el mensaje anterior
			}
		}
		break;
		case S2: 
			SerialPort_Send_String("Ingrese un valor entre 0 y 255 para G:\n\r"); 
			estado=S3;
		break;
		case S3:
		if(ProcesarInstruccion){		//Esperamos que el usuario ingrese un valor
			ProcesarInstruccion=0;		//Desactivamos el flag de nueva info
			if(procesarEntrada(1)){		//Vericamos que la entrada sea valida
				estado=S4;//Pasamos a imprimir el proximo mensaje
			}
			else{
				estado=S2;//Volvemos a imprimir el mensaje anterior
			}
		}
		break;
		case S4: SerialPort_Send_String("Ingrese un valor entre 0 y 255 para B:\n\r"); estado=S5;
		break;
		case S5:
		if(ProcesarInstruccion){	//Esperamos que el usuario ingrese un valor

			ProcesarInstruccion=0;		//Desactivamos el flag de nueva info
			if(procesarEntrada(2)){		//Vericamos que la entrada sea valida
				estado=S6;
			}
			else{
				estado=S4;			//Volvemos a imprimir el mensaje anterior
			}
		}
		break;
		case S6:
			pwm('R',RGB[0]); //Actualizamos la señal de salida para el terminal Rojo
			pwm('G',RGB[1]); //Actualizamos la señal de salida para el terminal Verde
			pwm('B',RGB[2]); //Actualizamos la señal de salida para el terminal Azul
			estado=S0;		 //Volvemos a imprimir el mensaje para que el usuario ingrese un nuevo valor
		break;
	}
}
int procesarEntrada(int cont){
	int num = atoi((char *) BufferRX);	//Transforma string en int
	SerialPort_Send_String(BufferRX);		//Eco del numero ingresado
	SerialPort_Send_String("\n\r");			
	if (num>=0 && num <= 255){				//Verifica el rango valido
		RGB[cont] = num;					//Actualiza el valor del led que corresponda
		return true;
	}
	else {						//En el caso de que no es ninguno de las opciones validas, devuelve 1
		SerialPort_Send_String("El numero debe estar entre 0 y 255\n\r"); //Imprime mensaje de error al usuario
		return false;
	}
}
