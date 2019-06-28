#include "lcd.h"
#include "adc12.h"
#include "uart.h"
#include "stdio.h"
#define THRESHOLD 0x700

uint8_t updateDisplay = 0;
uint32_t media = 0;
uint32_t results[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t count = 0;
uint8_t actual = 0;
uint8_t i;
char moisture[11];
float umidade;
volatile float factor;

void UpdateMedia() {
    count = 0;
    media = 0;
    for(i = 0; i < 15; i++) {
        media += results[i];
    }
    media /= 15; // media das medias
    lcdSetCursorPos(0, 9);
    if(media < THRESHOLD) {
        media = THRESHOLD;
    }
    umidade = 100 - (media - THRESHOLD)*factor;
    sprintf(moisture, "%.1f%%   ", umidade);
    lcdWriteStr(moisture);
}

void updateHumidity() {
    lcdSetCursorPos(1, 9);
    if(results[count] < THRESHOLD) {
        results[count] = THRESHOLD;
    }
    umidade = 100 - (results[count] - THRESHOLD)*factor;
    sprintf(moisture, "%.1f%%   ", umidade);
    lcdWriteStr(moisture);
    uartSendStr("Umidade: ");
    uartSendStr(moisture);
    uartSend('\n');
    count++;
    if(count > 14) {
        UpdateMedia();
    }
    updateDisplay = 0;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	P1DIR |= BIT0;
	
    // Configurando o canal A12
    adc12ConfChannelA12();

    // Configurando o LCD
    configI2C(0x42, 100, 1);
    LCD_init();

    // Timer para controlar o conversor AD
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR;
    TA0CCTL1 |= OUTMOD_7;
    TA0CCR0 = 65535;
    TA0CCR1 = 50;
    ADC12CTL0 |= ADC12ENC;

    // Inicializano modulo bluetooth
    uartInit(9600, 0, 0, 1);

    factor = (float)100 / (float)(0xFFF - THRESHOLD);

    __enable_interrupt();

    lcdSetCursorPos(0, 0);
    lcdWriteStr("Media30: ");
    lcdSetCursorPos(1, 0);
    lcdWriteStr("Umidade: ");
    lcdDisplayCTL(1, 0, 0);

    while(1) {
        while(updateDisplay == 0);
        ADC12CTL0 &= ~ADC12ENC;
        updateHumidity();
        ADC12CTL0 |= ADC12ENC;
    }
	return 0;
}

// Interrupção do conversor AD
#pragma vector = ADC12_B_VECTOR
__interrupt void ADC12_ISR() {
    ADC12CTL0 &= ~ADC12ENC;
    uint32_t med = 0;
    med += ADC12MEM0;
    med += ADC12MEM1;
    med += ADC12MEM2;
    med += ADC12MEM3;
    med += ADC12MEM4;
    med += ADC12MEM5;
    med += ADC12MEM6;
    med += ADC12MEM7;
    med += ADC12MEM8;
    med += ADC12MEM9;
    med += ADC12MEM10;
    med += ADC12MEM11;
    med += ADC12MEM12;
    med += ADC12MEM13;
    med += ADC12MEM14;
    med += ADC12MEM15;
    results[count] = (uint32_t)(med/16);
    updateDisplay = 1;
    P1OUT ^= BIT0;
    ADC12CTL0 |= ADC12ENC;
}
