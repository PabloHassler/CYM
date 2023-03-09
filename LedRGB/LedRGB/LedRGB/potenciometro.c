#define F_CPU 16000000
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
volatile int flagPote=0;
volatile int pote=0;
extern int RGB[3];

void setupADC(){
	ADCSRA = 0;
	ADCSRB = 0;
	ADMUX |= (1 << REFS0); //set reference voltage 5V
	ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only //
	ADMUX |= (1 << MUX0)|(1 << MUX1); //set reference voltage 5V
	ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); //prescalador ADC 128
	ADCSRA |= (1 << ADATE); //enabble auto trigger
	ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
	ADCSRA |= (1 << ADEN); //enable ADC
	ADCSRA |= (1 << ADSC); //start ADC measurements
}

void intensidad(){
	if(flagPote){							//Esperamos a que hay a nueva información en el ADC
		flagPote = 0;						//Desactivamos el flag de nueva información del pote
		uint8_t porcentajePote = (pote * 100)/(255);		//Calculo porcentual del alor del potenciomentro
		uint8_t finalR=(int)((RGB[0]*porcentajePote)/100);	//Calculo valor led Rojo con el valor de intensidad del potenciometro
		uint8_t finalG=(int)((RGB[1]*porcentajePote)/100);	//Calculo valor led Verde con el valor de intensidad del potenciometro
		uint8_t finalB=(int)((RGB[2]*porcentajePote)/100);	//Calculo valor led Azul con el valor de intensidad del potenciometro
		
		pwm('R',finalR);	//Actualizacion del pwm con el porcentaje ciclo de trabajo, Rojo
		pwm('G',finalG);	//Actualizacion del pwm con el porcentaje ciclo de trabajo, Verde
		pwm('B',finalB);	//Actualizacion del pwm con el porcentaje ciclo de trabajo, Azul
	}
}

ISR(ADC_vect) {//when new ADC value ready
	pote=ADCH;			//Captura de la parte alta del valor del potenciometro
	flagPote=1;			//Flag de nueva informacion del potenciometro
}