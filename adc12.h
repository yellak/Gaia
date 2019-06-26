#ifndef _ADC12_LIB_
#define _ADC12_LIB_

#include <msp430.h>
#include <stdint.h>

// Configura a leitura nos canais
// A12 -> P3.0
// A13 -> P3.1
// 1 trigger -> 2 conversões
void adc12Config();

// Configura no canal
// A12 -> P3.0
// 1 trigger -> 16 conversões
// Deve ter uma PWM em TA0CCR1 para fazer o trigger
// Depois de terminada as conversões é só fazer a média delas
void adc12ConfChannelA12();

#endif
