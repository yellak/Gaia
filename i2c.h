#ifndef _LIB_I2C_
#define _LIB_I2C_
#include <msp430.h>
#include <stdint.h>

// Função para configurar a interface I2C
// P5.0 -> SDA
// P5.1 -> SCL
// baudRate_kHz -> frequência em kiloHertz
void configI2C(uint8_t ownAddress, uint16_t baudRate_kHz, uint8_t isMaster);

// Função que manda mensagem pela interface I2C
int i2cWrite(uint8_t slaveAddr, uint8_t data[], uint32_t nBytes);

#endif
