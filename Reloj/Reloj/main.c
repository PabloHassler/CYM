/*
 * Reloj.c
 *
 * Created: 25/4/2022 14:54:59
 * Author : Barcala
 */  

/* Inclusión de cabeceras de bibliotecas de código */

#include <avr/io.h>			// Definición de Registros del microcontrolador
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#define DIA 4
#define MES 7
#define ANO 10
#define HORA 4
#define MINUTO 7
#define SEGUNDO 10

uint8_t KEYPAD_scan (uint8_t *);
uint8_t KEYPAD_Update (uint8_t *);
void setupLCD();
void setupTimer();
void actualizar_MEF();
void iniciar_MEF();
static char not_leap(void);
void salida(uint8_t,uint8_t,uint8_t);
void actualizarTiempo(); 
void actualizarCampo(char,uint8_t);
void imprimir();

typedef enum{S0,S1,S2,S3,S4,S5,S6} state;
state estado;state estadoAuxiliar;

typedef struct{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char date;
	unsigned char month;
	unsigned char year;
}time;

volatile time t={10,29,14,19,4,21};
volatile time t_Parcial={10,29,14,19,4,21};
volatile uint8_t FlagCursor=0;
const uint8_t filas[4] =   {0b00010000,0b00001000,0b00000001,0b10000000};
const uint8_t columna[4] = {0b00001000,0b00100000,0b00010000,0b00000100};
const char codChar[4][4] = {{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'0','*','#','D'}};

int main(void)
{
    /* Replace with your application code */
	setupTimer();
	setupLCD();
	
	iniciar_MEF();
	while (1) 
    {
		actualizar_MEF();	
    }
}

/*SETUP DE LA PANTALLA LCD*/
void setupLCD(){
	LCDinit();
	LCDclr();
	LCDGotoXY(0,0);
	LCDstring((uint8_t*)"31/12/21",8);	
	LCDGotoXY(0,1);
	LCDstring((uint8_t*)"23:59:59",8);
}

void setupTimer(){
	DDRC = 0xFF;											//Configure all eight pins of port B as outputs
	TIMSK2 &= ~((1<<TOIE2)|(1<<OCIE2A));					//Make sure all TC0 interrupts are disabled
	ASSR |= (1<<AS2);										//set Timer/counter0 to be asynchronous from the CPU clock
															//with a second external clock (32,768kHz)driving it.
	TCNT2 =0;												//Reset timer
	TCCR2B =(1<<CS20)|(1<<CS22);							//Prescale the timer to be clock source/128 to make it
															//exactly 1 second for every overflow to occur
	while (ASSR & ((1<<TCN2UB)|(1<<OCR2AUB)|(1<<TCR2AUB)))	//Wait until TC0 is updated
	{}
	TIMSK2 |= (1<<TOIE2);									//Set 8-bit Timer/Counter0 Overflow Interrupt Enable
	sei();
}

ISR(TIMER2_OVF_vect)
{
	static uint8_t cambio=0;
	if(FlagCursor){
		if(cambio){
			LCDcursorOn();
		}
		else{
			LCDcursorOFF();
		}
		cambio=!cambio;
	}
	if (++t.second==60)        //keep track of time, date, month, and year
	{
		t.second=0;
		if (++t.minute==60)
		{
			t.minute=0;
			if (++t.hour==24)
			{
				t.hour=0;
				if (++t.date==32)
				{
					t.month++;
					t.date=1;
				}
				else if (t.date==31)
				{
					if ((t.month==4) || (t.month==6) || (t.month==9) || (t.month==11))
					{
						t.month++;
						t.date=1;
					}
				}
				else if (t.date==30)
				{
					if(t.month==2)
					{
						t.month++;
						t.date=1;
					}
				}
				else if (t.date==29)
				{
					if((t.month==2) && (not_leap()))
					{
						t.month++;
						t.date=1;
					}
				}
				if (t.month==13)
				{
					t.month=1;
					t.year++;
				}
			}
		}
	}
}

static char not_leap(void)      //check for leap year
{
	if (!(t.year%100))
	{
		return (char)(t.year%400);
	}
	else
	{
		return (char)(t.year%4);
	}
}

void iniciar_MEF(){
	estado=S0;
	
}

void actualizar_MEF(){
	uint8_t key=0x00;
	static uint8_t nuevo=0;
	nuevo=KEYPAD_Update (&key);
	switch (estado){
		case S0:
			switch (key){
				case 'A': estado=S1; salida(1,0,ANO);
				case 'D': estado=S0; salida(0,0,ANO);
				default: actualizarTiempo();
			}
		case S1:
			switch (key){
				case 'A': estado=S2; salida(1,0,MES);
				case 'D': estado=S0; salida(0,0,MES);
				case 'B': actualizarCampo('A',1); imprimir();
				case 'C': actualizarCampo('A',0); imprimir();
			}
		case S2:
			switch (key){
				case 'A': estado=S3; salida(1,0,DIA);
				case 'D': estado=S0; salida(0,0,DIA);
				case 'B': actualizarCampo('M',1); imprimir();
				case 'C': actualizarCampo('M',0); imprimir();
		}
		case S3:
			switch (key){
				case 'A': estado=S4; salida(1,0,HORA);
				case 'D': estado=S0; salida(0,0,HORA);
				case 'B': actualizarCampo('D',1); imprimir();
				case 'C': actualizarCampo('D',0); imprimir();
			}
		case S4:
			switch (key){
				case 'A': estado=S5; salida(1,1,MINUTO);
				case 'D': estado=S0; salida(0,1,MINUTO);
				case 'B': actualizarCampo('h',1); imprimir();
				case 'C': actualizarCampo('h',0); imprimir();
			}
		case S5:
			switch (key){
				case 'A': estado=S6; salida(1,1,SEGUNDO);
				case 'D': estado=S0; salida(0,1,SEGUNDO);
				case 'B': actualizarCampo('m',1); imprimir();
				case 'C': actualizarCampo('m',0); imprimir();
			}
		case S6:
			switch (key){
				case 'A': estado=S0; salida(1,1,SEGUNDO); salida(0,1,SEGUNDO);
				case 'D': estado=S0; salida(0,1,SEGUNDO);
				case 'B': actualizarCampo('s',1); imprimir();
				case 'C': actualizarCampo('s',0); imprimir();
			}
	}
}

void actualizarTiempo(){
	LCDGotoXY(4,0);
	LCDescribeDato(t.date,2);
	LCDsendChar('/');
	LCDescribeDato(t.month,2);
	LCDsendChar('/');
	LCDescribeDato(t.year,2);
	LCDGotoXY(4,1);
	LCDescribeDato(t.hour,2);
	LCDsendChar(':');
	LCDescribeDato(t.minute,2);
	LCDsendChar(':');
	LCDescribeDato(t.second,2);
}

void imprimir(){
	LCDGotoXY(4,0);
	LCDescribeDato(t_Parcial.date,2);
	LCDsendChar('/');
	LCDescribeDato(t_Parcial.month,2);
	LCDsendChar('/');
	LCDescribeDato(t_Parcial.year,2);
	LCDGotoXY(4,1);
	LCDescribeDato(t_Parcial.hour,2);
	LCDsendChar(':');
	LCDescribeDato(t_Parcial.minute,2);
	LCDsendChar(':');
	LCDescribeDato(t_Parcial.second,2);
}

void salida(uint8_t z,uint8_t pos,uint8_t campo){
	if(z==1){
		t=t_Parcial;
		LCDGotoXY(campo,pos);
		FlagCursor=1;
	}
	else{
		t_Parcial=t;
		LCDGotoXY(campo,pos);
		FlagCursor=0;
	}
}


/********************************************************
FUNCION PARA ESCANEAR UN TECLADO MATRICIAL Y DEVOLVER LA
TECLA PRESIONADA UNA SOLA VEZ. TIENE DOBLE VERIFICACION Y
MEMORIZA LA ULTIMA TECLA PRESIONADA
DEVUELVE:
0 -> NO HAYNUEVA TECLA PRESIONADA
1 -> HAY NUEVA TECLA PRESIONADA Y ES *pkey
********************************************************/
uint8_t KEYPAD_Update (uint8_t *pkey){
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