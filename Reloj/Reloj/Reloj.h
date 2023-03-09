/*
 * Reloj.h
 */ 
#include <avr/io.h>
#include "lcd.h"

typedef struct{
	signed char second;
	signed char minute;
	signed char hour;
	signed char date;
	signed char month;
	signed char year;
}time;

volatile uint8_t FlagLcd=0;
volatile uint8_t FlagCambio=0;
volatile time t={01,01,01,01,01,01};
volatile time t_Parcial={01,01,01,01,01,01};
static char not_leap(time t);
#define DIA_HORA 4
#define MES_MINUTO 7
#define ANO_SEGUNDO 10
typedef struct{
	uint8_t x;
	uint8_t y;
} pos;
pos dia = {4,1};
pos mes = {7,1};
pos ano = {10,1};
pos hora = {4,0};
pos minuto = {7,0};
pos segundo = {10,0};
typedef enum{DIA,MES,ANO,MIN,SEG,HORA} identificadores;
	
typedef enum{S0,S1,S2,S3,S4,S5,S6} state;
state estado;
uint8_t leer=0;
volatile uint8_t posicion[2]={0,0};
volatile uint8_t FlagCursor=0;
void salidaA(pos posi);
void salidaD(pos posi);
void actualizarCampo(char campo,uint8_t estado);
void efecto_Apagado();
void actualizarTiempo();
void actualizarDia(int8_t dir);
uint8_t KEYPAD_scan (uint8_t *);
void actualizarCampo2(volatile signed char *asdf,int8_t dir,uint8_t maximo,int8_t minimo,int8_t is_month);
void relojFuncion();
uint8_t KEYPAD_Update(uint8_t *);