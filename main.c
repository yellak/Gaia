#include "lcd.h"
#include "adc12.h"

uint8_t updateDisplay = 0;
uint8_t updateDisplay2 = 0;
uint32_t media = 0;
uint32_t results[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t count = 0;
uint8_t actual = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	P1DIR |= BIT0;
	
	float umidade;

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

    __enable_interrupt();

    lcdSetCursorPos(0, 0);
    lcdWriteStr("Media30: ");
    lcdSetCursorPos(1, 0);
    lcdWriteStr("Umidade: ");
    lcdDisplayCTL(1, 0, 0);

    while(1) {
        while((updateDisplay == 0) && (updateDisplay2 == 0));
        ADC12CTL0 &= ~ADC12ENC;
        if(updateDisplay != 0) {
            updateDisplay = 0;
            lcdSetCursorPos(0, 9);
            if(media < 0x7C4) {
                media = 0x7C4;
            }
            umidade = 100 - (media - 0x7C4)*0.0475;
            lcdDec16((uint16_t)(umidade*0.1));
            lcdFloat1(((float)((uint32_t)(umidade*1000)%1000))*0.01, 1);
            lcdWriteByte('%', 1);
            lcdWriteByte(' ', 1);
        }
        if(updateDisplay2 != 0) {
            updateDisplay2 = 0;
            lcdSetCursorPos(1, 9);
            if(results[count] < 0x7C4) {
                results[count] = 0x7C4;
            }
            umidade = 100 - (results[count] - 0x7C4)*0.0475;
            lcdDec16((uint16_t)(umidade*0.1));
            lcdFloat1(((float)((uint32_t)(umidade*1000)%1000))*0.01, 1);
            lcdWriteByte('%', 1);
            lcdWriteByte(' ', 1);
            count++;
        }
        ADC12CTL0 |= ADC12ENC;
    }
	return 0;
}

// Interrupção do conversor AD
#pragma vector = ADC12_B_VECTOR
__interrupt void ADC12_ISR() {
    ADC12CTL0 &= ~ADC12ENC;
    uint8_t i;
    media = 0;
    media += ADC12MEM0;
    media += ADC12MEM1;
    media += ADC12MEM2;
    media += ADC12MEM3;
    media += ADC12MEM4;
    media += ADC12MEM5;
    media += ADC12MEM6;
    media += ADC12MEM7;
    media += ADC12MEM8;
    media += ADC12MEM9;
    media += ADC12MEM10;
    media += ADC12MEM11;
    media += ADC12MEM12;
    media += ADC12MEM13;
    media += ADC12MEM14;
    media += ADC12MEM15;
    results[count] = (uint32_t)(media/16);
    if(count > 14) {
        media = 0;
        for(i = 0; i < 15; i++) {
            media += results[i];
        }
        media /= 15; // media das medias
        updateDisplay = 1;
        count = 0;
    }
    updateDisplay2 = 1;
    P1OUT ^= BIT0;
    ADC12CTL0 |= ADC12ENC;
}
