/*
 * serialPort.c
 *
 * Created: 07/10/2020 03:02:18 p. m.
 *  Author: vfperri
 */ 
#define F_CPU 16000000UL
#include "SerialPort.h"


#define TX_BUFFER_LENGTH 32
#define RX_BUFFER_LENGTH 32

volatile static unsigned char TXindice_lectura=0, TXindice_escritura=0;
volatile static unsigned char RXindice_lectura=0, RXindice_escritura=0;

static char TX_Buffer [TX_BUFFER_LENGTH];
static char RX_Buffer [RX_BUFFER_LENGTH];

// Inicialización de Puerto Serie
void SerialPort_Init(uint8_t config){
	// config = 0x67 ==> Configuro UART 9600bps, 8 bit data, 1 stop @ F_CPU = 16MHz.
	// config = 0x33 ==> Configuro UART 9600bps, 8 bit data, 1 stop @ F_CPU = 8MHz.
	// config = 0x25 ==> Configuro UART 9600bps, 8 bit data, 1 stop @ F_CPU = 4MHz.
	UCSR0B = 0;
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	//UBRR0H = (unsigned char)(config>>8);
	UBRR0L = (unsigned char)config;
	

}


// Inicialización de Transmisor

void SerialPort_TX_Enable(void){
	UCSR0B |= (1<<TXEN0);
}

void SerialPort_TX_Interrupt_Enable(void){
	UCSR0B |= (1<<UDRIE0);
}

void SerialPort_TX_Interrupt_Disable(void)
{
	UCSR0B &=~(1<<UDRIE0);
}


// Inicialización de Receptor

void SerialPort_RX_Enable(void){
	UCSR0B |= (1<<RXEN0);
}

void SerialPort_RX_Interrupt_Enable(void){
	UCSR0B |= (1<<RXCIE0);
}


// Transmisión

// Espera hasta que el buffer de TX este libre.
void SerialPort_Wait_For_TX_Buffer_Free(void){
	// Pooling - Bloqueante hasta que termine de transmitir.
	while(!(UCSR0A & (1<<UDRE0)));
}

void SerialPort_Send_Data(char data){
	UDR0 = data;
}

void SerialPort_Send_String(char * msg){ //msg -> "Hola como andan hoy?" 20 ASCII+findecadena, tardo=20ms
	uint8_t i = 0;
	//'\0' = 0x00
	while(msg[i]){ // *(msg+i)
		SerialPort_Wait_For_TX_Buffer_Free(); //9600bps formato 8N1, 10bits, 10.Tbit=10/9600=1ms 
		SerialPort_Send_Data(msg[i]);
		i++;
	}
}
/*
*Configuración de la terminal serie
*/
void setupSerialPort(int cod){
	SerialPort_Init(cod);
	SerialPort_TX_Enable();
	SerialPort_RX_Enable();
	SerialPort_RX_Interrupt_Enable();
}

// Recepción

// Espera hasta que el buffer de RX este completo.
void SerialPort_Wait_Until_New_Data(void){
	// Pooling - Bloqueante, puede durar indefinidamente!
	while(!(UCSR0A & (1<<RXC0)));
}


char SerialPort_Recive_Data(void){
	return UDR0;
}


void SerialPort_Send_uint8_t(uint8_t num){
	
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_Data('0'+num/100);
	
	num-=100;
	
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_Data('0'+num/10);
	
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_Data('0'+ num%10);
}

/***************************************************************
	This function writes a integer type value to UART
	Arguments:
	1)int val	: Value to print
	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val
****************************************************************/
void SerialPort_send_int16_t(int val,unsigned int field_length)
{
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) {
		SerialPort_Wait_For_TX_Buffer_Free();
		SerialPort_Send_Data('-');
		}
	for(i=j;i<5;i++)
	{
	SerialPort_Wait_For_TX_Buffer_Free();
	SerialPort_Send_Data('0'+str[i]);
	}
}
//*********


void SerialPort_Write_Char_To_Buffer ( char Data )
{
	// Write to the buffer *only* if there is space
	if (TXindice_escritura < TX_BUFFER_LENGTH){
		TX_Buffer[TXindice_escritura] = Data;
		TXindice_escritura++;
	}
	else {
		// Write buffer is full
		//Error_code = ERROR_UART_FULL_BUFF;
	}
}

void SerialPort_Write_String_To_Buffer( char *  STR_PTR )
{
	unsigned char i = 0;
	while ( STR_PTR [ i ] != '\0')
	{
		SerialPort_Write_Char_To_Buffer ( STR_PTR [ i ] );
		i++;
	}
}

void SerialPort_Send_Char (char dato)
{
	SerialPort_Wait_For_TX_Buffer_Free(); // Espero a que el canal de transmisión este libre (bloqueante)
	SerialPort_Send_Data(dato);
}

void SerialPort_Update(void)
{
	static char key;
	     
	if ( UCSR0A & (1<<RXC0) ) { // Byte recibido. Escribir byte en buffer de entrada
		if (RXindice_escritura < RX_BUFFER_LENGTH) {
			RX_Buffer [RXindice_escritura] =UDR0; // Guardar dato en buffer
			RXindice_escritura++; // Inc sin desbordar buffer
		}
		//else
		//Error_code = ERROR_UART_FULL_BUFF;
	}	
	// Hay byte en el buffer Tx para transmitir?
	if (TXindice_lectura < TXindice_escritura){
		SerialPort_Send_Char ( TX_Buffer [TXindice_lectura] );
		TXindice_lectura++;
	}
	else {// No hay datos disponibles para enviar
		TXindice_lectura = 0;
		TXindice_escritura = 0;
	}		
}

char SerialPort_Get_Char_From_Buffer (char * ch)
{
	// Hay nuevo dato en el buffer?
	if (RXindice_lectura < RXindice_escritura){
		*ch = RX_Buffer [RXindice_lectura];
		RXindice_lectura++;
		return 1; // Hay nuevo dato
	}
	else {
		RXindice_lectura=0;
		RXindice_escritura=0;
		return 0; // No Hay
	}
}

char SerialPort_Get_String_From_Buffer (char * string)
{
	char rxchar=0;
	
	do{
		if(SerialPort_Get_Char_From_Buffer (&rxchar)){
			*string=rxchar;
			string++;
		}
		else{
			rxchar='\n'; //empty string
		}
	}while(rxchar!='\n');
	*string='\0'; //End of String
	return 1;
}

char SerialPort_Receive_data (char * dato)
{
	if ( (UCSR0A & (1<<RXC0))==1) {
		*dato=UDR0;
		return 1;
	}
	return 0; //no data
}

void displayMenu(){
		 SerialPort_Send_String("Menu \n\r");
		 SerialPort_Send_String("1. 'ON'<Enter>  - Encender registrador \n\r");
		 SerialPort_Send_String("2. 'OFF'<Enter> - Apagar registrador \n\r");
		 SerialPort_Send_String("3. 'RST'<Enter> - Volver al estado inicial \n\r");
}

void startSerialPort(){
		 //Inicializa USART
		 UCSR0B = (1<<TXEN0);               // Transmit Enable
		 UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	//Tamaño de la cantidad de información trasmitida: 110 -> Character Size = 8 
		 UBRR0L = 103;						//baud rate = 9600bps@16MHz
		 while (! (UCSR0A & (1<<UDRE0)));   //wait until UDR0 is empty
}


