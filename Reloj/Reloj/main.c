/*
 * Reloj.c
 *
 * Created: 25/4/2022 14:54:59
 * Author : Barcala
 */  

/* Inclusión de cabeceras de bibliotecas de código */
//#include "main.h"
#include <avr/io.h>			// Definición de Registros del microcontrolador
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#define DIA_HORA 4
#define MES_MINUTO 7
#define ANO_SEGUNDO 10

void setupLCD();
void setupTimer();
uint8_t KEYPAD_scan (uint8_t *);
uint8_t KEYPAD_Update (uint8_t *);
void actualizar_MEF();
void iniciar_MEF();
static char not_leap(void);
static char not_leap_Parcial(void); 
void salida(uint8_t,uint8_t,uint8_t);
void actualizarTiempo(); 
void actualizarCampo(char,uint8_t);
void imprimir();
void efecto_Apagado();
void relojFuncion();

typedef enum{S0,S1,S2,S3,S4,S5,S6} state;
state estado;

typedef struct{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char date;
	unsigned char month;
	unsigned char year;
}time;

volatile time t={10,29,14,19,4,98};
volatile time t_Parcial={10,29,14,19,4,21};
volatile uint8_t FlagLcd=0;
volatile uint8_t FlagCambio=0;
volatile uint8_t FlagCursor=0;
volatile uint8_t actualizar=0;
volatile uint8_t posicion[2]={0,0};
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
	LCDcursorOFF();

}

void setupTimer(){
	DDRB=0b00100000;//PB5 como salida
	TCCR0B=(1<<CS02)|(1<<CS00);//configurar el registro del timer0 como temporizador con prescalador de 1024
	TCNT0=99;//el registro empieza con valor 99
	TIMSK0|= (1<<TOIE0);//habilita la interrupcion por desbordamiento del timer0
	sei();//habilita interrupciones globales
}

ISR(TIMER0_OVF_vect)
{
	uint8_t static cont=0;
	cont++;
	if(cont==50){
		relojFuncion();
		TCNT2=99;//reinicio contador del timer0
		cont=0;//reinicio contador
	}
	
}
void relojFuncion()
{
	FlagLcd=1;
	FlagCambio=!FlagCambio;
	
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

static char not_leap_Parcial(void)      //check for leap year
{
	if (!(t_Parcial.year%100))
	{
		return (char)(t_Parcial.year%400);
	}
	else
	{
		return (char)(t_Parcial.year%4);
	}
}

void iniciar_MEF(){
	estado=S0;t_Parcial=t;
}

void actualizar_MEF(){
	uint8_t key='#';
	uint8_t static nuevo=0;
	nuevo=KEYPAD_scan (&key);
	efecto_Apagado();
	switch (estado){
		case S0:
			switch (key){
				case 'A': t_Parcial=t; estado=S1; salida(1,1,ANO_SEGUNDO); 
				break;
				case 'D': estado=S0; salida(0,1,ANO_SEGUNDO);
				default: actualizarTiempo();
			} 
		break;
		case S1:
			switch (key){
				case 'A': estado=S2; salida(1,1,MES_MINUTO); 
				break;
				case 'D': estado=S0; salida(0,1,MES_MINUTO); 
				break;
				case 'B': actualizarCampo('A',1); imprimir(); LCDGotoXY(ANO_SEGUNDO,1);
				break;
				case 'C': actualizarCampo('A',0); imprimir(); LCDGotoXY(ANO_SEGUNDO,1);
			} 
		break;
		case S2:
			switch (key){
				case 'A': estado=S3; salida(1,1,DIA_HORA); 
				break;
				case 'D': estado=S0; salida(0,1,DIA_HORA);
				break;
				case 'B': actualizarCampo('M',1); imprimir(); LCDGotoXY(MES_MINUTO,1);
				break;
				case 'C': actualizarCampo('M',0); imprimir(); LCDGotoXY(MES_MINUTO,1);
			} 
		break;
		case S3:
			switch (key){
				case 'A': estado=S4; salida(1,0,DIA_HORA);
				break;
				case 'D': estado=S0; salida(0,0,DIA_HORA); 
				break;
				case 'B': actualizarCampo('D',1); imprimir(); LCDGotoXY(DIA_HORA,1);
				break;
				case 'C': actualizarCampo('D',0); imprimir(); LCDGotoXY(DIA_HORA,1);
			} 
		break;
		case S4:
			switch (key){
				case 'A': estado=S5; salida(1,0,MES_MINUTO); 
				break;
				case 'D': estado=S0; salida(0,0,MES_MINUTO); 
				break;
				case 'B': actualizarCampo('h',1); imprimir(); LCDGotoXY(DIA_HORA,0);
				break;
				case 'C': actualizarCampo('h',0); imprimir(); LCDGotoXY(DIA_HORA,0);
			} 
		break;
		case S5:
			switch (key){
				case 'A': estado=S6; salida(1,0,ANO_SEGUNDO); 
				break;
				case 'D': estado=S0; salida(0,0,ANO_SEGUNDO); 
				break;
				case 'B': actualizarCampo('n',1); imprimir(); LCDGotoXY(MES_MINUTO,0);
				break;
				case 'C': actualizarCampo('n',0); imprimir(); LCDGotoXY(MES_MINUTO,0);
			} 
		break;
		case S6:
			switch (key){
				case 'A': estado=S0; salida(1,0,ANO_SEGUNDO); salida(0,0,ANO_SEGUNDO); 
				break;
				case 'D': estado=S0; salida(0,0,ANO_SEGUNDO); 
				break;
				case 'B': actualizarCampo('s',1); imprimir(); LCDGotoXY(ANO_SEGUNDO,0);
				break;
				case 'C': actualizarCampo('s',0); imprimir(); LCDGotoXY(ANO_SEGUNDO,0);
			} 
		break;
	}
}

void efecto_Apagado(){
	if(FlagCursor){
		if(FlagCambio){
			switch (posicion[0]){
				case ANO_SEGUNDO: 
					if(!posicion[1])
						LCDescribeDato(t_Parcial.second,2);
					else LCDescribeDato(t_Parcial.year,2);
				break;
				case MES_MINUTO: 
					if(!posicion[1])
						LCDescribeDato(t_Parcial.minute,2);
					else LCDescribeDato(t_Parcial.month,2);
				break;
				case DIA_HORA: 
					if(!posicion[1])
						LCDescribeDato(t_Parcial.hour,2);
					else LCDescribeDato(t_Parcial.date,2);
				break;
			}
		}
		else LCDstring((uint8_t*)"  ",2);
		
		LCDGotoXY(posicion[0],posicion[1]);
	}
}

void actualizarTiempo(){
	if(FlagLcd){
		FlagLcd=0;
		LCDGotoXY(4,1);
		LCDescribeDato(t.date,2);
		LCDsendChar('/');
		LCDescribeDato(t.month,2);
		LCDsendChar('/');
		LCDescribeDato(t.year,2);
		LCDGotoXY(4,0);
		LCDescribeDato(t.hour,2);
		LCDsendChar(':');
		LCDescribeDato(t.minute,2);
		LCDsendChar(':');
		LCDescribeDato(t.second,2);
	}
}

void imprimir(){
	if(FlagLcd){
		FlagLcd=0;
		LCDGotoXY(4,1);
		LCDescribeDato(t_Parcial.date,2);
		LCDsendChar('/');
		LCDescribeDato(t_Parcial.month,2);
		LCDsendChar('/');
		LCDescribeDato(t_Parcial.year,2);
		LCDGotoXY(4,0);
		LCDescribeDato(t_Parcial.hour,2);
		LCDsendChar(':');
		LCDescribeDato(t_Parcial.minute,2);
		LCDsendChar(':');
		LCDescribeDato(t_Parcial.second,2);
	}
}

void salida(uint8_t z,uint8_t eje_Y,uint8_t eje_X){
	if(z){
		t=t_Parcial;
		imprimir();
		LCDGotoXY(eje_X,eje_Y);
		posicion[0]=eje_X;
		posicion[1]=eje_Y;
		FlagCursor=1;
	}
	else{
		t_Parcial=t;
		LCDGotoXY(eje_X,eje_Y);
		posicion[0]=eje_X;
		posicion[1]=eje_Y;
		FlagCursor=0;
	}
}

void actualizarCampo(char campo,uint8_t estado){
	switch (campo){
		case 'A':
			if(!estado)
				estado = -1;
			t_Parcial.year += 1*estado;
			t_Parcial.year = t_Parcial.year % 100;
			if(t_Parcial.year == 0 && !estado)
				t_Parcial.year = 99;
		break;
		case 'M':
			if(estado){
				if(t_Parcial.month==12)
					t_Parcial.month=1;
				else t_Parcial.month++;
			}
			else{
				if(t_Parcial.month==1)
					t_Parcial.month=12;
				else t_Parcial.month--;
			}
		break;
		case 'D':
			if(estado){
				if ((t_Parcial.month==4) || (t_Parcial.month==6) || (t_Parcial.month==9) || (t_Parcial.month==11)){
					if(t_Parcial.date==30)
						t_Parcial.date=1;
					else t_Parcial.date++;
				}
				else{
					if((t.month==2) && (not_leap_Parcial())){
						if(t_Parcial.date==28)
							t_Parcial.date=1;
						else t_Parcial.date++;
					}
					else{ 
						if((t.month==2) && (!not_leap_Parcial())){
							if(t_Parcial.date==29)
								t_Parcial.date=1;
							else t_Parcial.date++;
						}
						else{
							if(t_Parcial.date==31)
								t_Parcial.date=1;
							else t_Parcial.date++;
						}
					}
				}
			}
			else{
				if ((t_Parcial.month==4) || (t_Parcial.month==6) || (t_Parcial.month==9) || (t_Parcial.month==11)){
					if(t_Parcial.date==1)
						t_Parcial.date=30;
					else t_Parcial.date--;
				}
				else{
					if((t.month==2) && (not_leap_Parcial())){
						if(t_Parcial.date==1)
							t_Parcial.date=28;
						else t_Parcial.date--;
					}
					else{
						if((t.month==2) && (!not_leap_Parcial())){
							if(t_Parcial.date==1)
								t_Parcial.date=29;
							else t_Parcial.date--;
						}
						else{
							if(t_Parcial.date==1)
								t_Parcial.date=31;
							else t_Parcial.date--;
						}
					}
				}
			}
			break;
			case 'h':
				if(estado){
					if(t_Parcial.hour==23)
						t_Parcial.hour=0;
					else t_Parcial.hour++;
				}
				else{
					if(t_Parcial.hour==0)
						t_Parcial.hour=23;
					else t_Parcial.hour--;
				}
			break;
			case 'n':
				if(estado){
					if(t_Parcial.minute==59)
						t_Parcial.minute=0;
					else t_Parcial.minute++;
				}
				else{
					if(t_Parcial.minute==0)
						t_Parcial.minute=59;
					else t_Parcial.minute--;
				}
			break;
			case 's':
				if(estado){
					if(t_Parcial.second==59)
						t_Parcial.second=0;
					else t_Parcial.second++;
				}
				else{
					if(t_Parcial.second==0)
						t_Parcial.second=59;
					else t_Parcial.second--;
				}
			break;
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
				_delay_ms(500);
				return(1);
			}
		}
	}
	return(0);
}