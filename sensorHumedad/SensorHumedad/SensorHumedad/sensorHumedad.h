/*
 * sensorHumedad.h
 *
 * Created: 7/3/2022 4:55:49 PM
 *  Author: Hernan
 */ 
#include <stdint.h>
#define DHT11_PIN 0
void Request();
void Response();
uint8_t Receive_data();
void updateHumidity();
void procesarEntrada();