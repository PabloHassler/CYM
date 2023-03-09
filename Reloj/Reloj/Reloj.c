/*
 * Reloj.c
 */ 
#include "Reloj.h"
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
					if((t.month==2) && (not_leap(t)))
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

static char not_leap(time t)      //check for leap year
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
	estado=S0;t_Parcial=t;
}
/*
	Actualiza la maquina de estados dependiendo 
	de la tecla precionada
*/
void actualizar_MEF(){
	uint8_t key='#';
	KEYPAD_Update (&key); //leer la entrada
	efecto_Apagado();   //Efecto de encendido y apagado del campo modificandose
	switch (estado){	//Maquina de estados
		case S0:		//Estado 0, default
			switch (key){ 
				case 'A': t_Parcial=t; estado=S1; salidaA(ano);
				break;
				default: actualizarTiempo(t);
			}
		break;
		case S1:
			switch (key){
				case 'A': estado=S2; salidaA(mes); 
				break;
				case 'D': estado=S0; salidaD(mes); 
				break;
				case 'B': actualizarCampo2(&t_Parcial.year,1,100,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(ANO_SEGUNDO,1);
				break;
				case 'C': actualizarCampo2(&t_Parcial.year,-1,100,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(ANO_SEGUNDO,1);
			}
		break;
		case S2:
			switch (key){
				case 'A': estado=S3; salidaA(dia); actualizarDia(0);
				break;
				case 'D': estado=S0; salidaD(dia);
				break;
				case 'B': actualizarCampo2(&t_Parcial.month,1,12,0,0); actualizarTiempo(t_Parcial); LCDGotoXY(MES_MINUTO,1);
				break;
				case 'C': actualizarCampo2(&t_Parcial.month,-1,12,0,0);  actualizarTiempo(t_Parcial); LCDGotoXY(MES_MINUTO,1);
			}
			break;
		case S3:
			switch (key){
				case 'A': estado=S4; salidaA(hora);
				break;
				case 'D': estado=S0; salidaD(hora);
				break;
				case 'B': actualizarDia(1); actualizarTiempo(t_Parcial); LCDGotoXY(DIA_HORA,1);
				break;
				case 'C': actualizarDia(-1); actualizarTiempo(t_Parcial); LCDGotoXY(DIA_HORA,1);
			}
			break;
		case S4:
			switch (key){
				case 'A': estado=S5; salidaA(minuto);
				break;
				case 'D': estado=S0; salidaD(minuto);
				break;
				case 'B': actualizarCampo2(&t_Parcial.hour,1,24,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(DIA_HORA,0);
				break;
				case 'C': actualizarCampo2(&t_Parcial.hour,-1,24,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(DIA_HORA,0);
			}
			break;
		case S5:
			switch (key){
				case 'A': estado=S6; salidaA(segundo);
				break;
				case 'D': estado=S0; salidaD(segundo);
				break;
				case 'B': actualizarCampo2(&t_Parcial.minute,1,60,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(MES_MINUTO,0);
				break;
				case 'C': actualizarCampo2(&t_Parcial.minute,-1,60,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(MES_MINUTO,0);
			}
			break;
		case S6:
			switch (key){
				case 'A': estado=S0; salidaA(segundo); salidaD(segundo);
				break;
				case 'D': estado=S0; salidaD(segundo);
				break;
				case 'B': actualizarCampo2(&t_Parcial.second,1,60,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(ANO_SEGUNDO,0);
				break;
				case 'C': actualizarCampo2(&t_Parcial.second,-1,60,-1,1); actualizarTiempo(t_Parcial); LCDGotoXY(ANO_SEGUNDO,0);
			}
			break;
	}
}
/*
Actualiza el valor impreso en la pantalla LCD
Parametros:
	t: valor de tiempo a imprimir en pantalla
*/
void actualizarTiempo(time t){
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
/*
	Se encarga de pasar el siguiente estado de modificación
	Parametro:
		posi: posicion donde se imprime el siguiente campo
*/
void salidaA(pos posi){
	t=t_Parcial;
	actualizarTiempo(t_Parcial);
	LCDGotoXY(posi.x,posi.y);
	posicion[0]=posi.x;
	posicion[1]=posi.y;
	FlagCursor=1;
}
/*
	Resulver la logica asociada a volver 
	al estado default luego de la modificación
	Parametro:
		posi: posicion donde se imprime el siguiente campo
*/
void salidaD(pos posi){
	t_Parcial=t;
	LCDGotoXY(posi.x,posi.y);
	posicion[0]=posi.x;
	posicion[1]=posi.y;
	FlagCursor=0;
}
/*
	Actualiza el valor del día, contemplando año biciestos
	Parámetro:
		dir: 1 si se quiere incrementar, -1 si se quiere decrementar.
*/
void actualizarDia(int8_t dir){
	switch (t_Parcial.month)
	{
		case 2:
			if(not_leap(t_Parcial)){
				if(t_Parcial.date < 29)
					t_Parcial.date = (t_Parcial.date+dir) % 28;
					if(t_Parcial.date == 0)
						t_Parcial.date = 28;
				else{
					t_Parcial.date = 28;
				}
				
			}
			else{
				if(t_Parcial.date < 30){
					t_Parcial.date = (t_Parcial.date+dir) % 29;
					if(t_Parcial.date == 0)
					t_Parcial.date = 29;
				}
				else{
					t_Parcial.date = 29;
				}
			}
		break;
		case 4:
		case 6:
		case 9:
		case 11:
			t_Parcial.date = (t_Parcial.date+dir) % 30;
			if(t_Parcial.date == 0)
				t_Parcial.date = 30;
		break;
		default:
			t_Parcial.date = (t_Parcial.date+dir) % 31;
			if(t_Parcial.date == 0)
				t_Parcial.date = 31;
		break;
		
	}
}
/************************************************************************/
/*   Verificación de máximo día cuando se actualiza el mes              */
/************************************************************************/
void verificacion(){
	switch (t_Parcial.month)
	{
		case 2:
		if(not_leap(t_Parcial)){
			if(t_Parcial.date < 29)
				t_Parcial.date = 28;
		}
		else{
			if(t_Parcial.date < 30)
				t_Parcial.date = 29;

		}
		break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(t_Parcial.date < 31)
				t_Parcial.date = 30;
		break;
	}
}
/************************************************************************/
/* Actualiza el valor del campo del reloj
	Parámetros:
		dir: 1 para aumentar, -1 para decrementar
		actual: valor actual que tiene la variable a modificar  
		máximo: valor máximo que puede tomar el campo
		minimo: valor minimo que puede tomar el campo
		is_month: diferencia los meses                                       */
/************************************************************************/
void actualizarCampo2(volatile signed char *actual,int8_t dir,uint8_t maximo,int8_t minimo,int8_t is_month){
	*actual = (*actual+dir) % maximo;
	if(*actual == minimo)
		*actual = maximo-is_month;
		return;
}
/************************************************************************/
/* Efecto de parpadeo del campo en la modificación                     */
/************************************************************************/
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