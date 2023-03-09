/*
 * serialPort.h
 *
 * Created: 07/10/2020 03:02:42 p. m.
 *  Author: vfperri
 */ 
#ifndef SERIALPORT_H_
	#define SERIALPORT_H_

	// ------------------- Includes ----------------------------
	
	// Archivo de cabecera del Microcontrolador
	#include <avr/io.h>
	
	// Interrupciones del Microcontrolador
	#include <avr/interrupt.h>
	
	// -------- Prototipos de funciones Publicas ---------------
	
	// Inicializacion de Puerto Serie
	void SerialPort_Init(uint8_t);

	// Inicializacion de Transmisor
	void SerialPort_TX_Enable(void);
	void SerialPort_TX_Interrupt_Enable(void);
	void SerialPort_TX_Interrupt_Disable(void);
	
	// Inicializacion de Receptor
	void SerialPort_RX_Enable(void);
	void SerialPort_RX_Interrupt_Enable(void);
	
	// Transmision
	void SerialPort_Wait_For_TX_Buffer_Free(void);	// Pooling - Bloqueante hasta que termine de transmitir.
	void SerialPort_Send_Data(char);
	void SerialPort_Send_String(char *);
	void SerialPort_Send_uint8_t(uint8_t);
	void SerialPort_send_int16_t(int val,unsigned int field_length); //This function writes a integer type value to UART
							//-32768 y 32767

	// Recepcion
	void SerialPort_Wait_Until_New_Data(void);	// Pooling - Bloqueante, puede durar indefinidamente!
	char SerialPort_Recive_Data(void);

	//Driver P.C.
	void SerialPort_Write_Char_To_Buffer ( char Data );
	void SerialPort_Write_String_To_Buffer( char *  STR_PTR );
	void SerialPort_Send_Char (char dato);
	void SerialPort_Update(void);
	char SerialPort_Get_Char_From_Buffer (char * ch);
	char SerialPort_Get_String_From_Buffer (char * string);
	char SerialPort_Receive_data (char * dato);
	
	//TP
	void startSerialPort();
	void setupSerialPort(int cod);

#endif /* SERIALPORT_H_ */