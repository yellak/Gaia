#include "adc12.h"

void adc12Config() {
    P3SEL0 |= (BIT0 | BIT1);
    P3SEL1 |= (BIT0 | BIT1);

    ADC12CTL0 &= ~ADC12ENC;
    // SHT0_3 -> alonga o ciclo do timer de 32 batidas
    // ON     -> liga o conversor
    // MSC    -> faz com que trigger possa gerar N conversões
    ADC12CTL0 |= ADC12SHT0_3 | ADC12ON | ADC12MSC;

    // SHS_0    -> clock original do ADC12
    // SHP      -> utilizar um contador para alongar o ciclo do clock
    // CONSEQ_1 -> modo de operação 1, com MSC ativo, um trigger faz N conversões
    ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12CONSEQ_1;
    ADC12CTL2 = ADC12RES_2; // resolução de 12 bits
    ADC12CTL3 = ADC12CSTARTADD_0; // começa em ADC12MEM0

    ADC12MCTL0 = ADC12INCH_12;
    ADC12MCTL1 = ADC12INCH_13 | ADC12EOS; // as conversões param por aqui
    ADC12IER0 = ADC12IE1; // habilita a interrupção do conversor
}

void adc12ConfChannelA12() {
    P3SEL0 |= BIT0;
    P3SEL1 |= BIT0;

    ADC12CTL0 &= ~ADC12ENC;
    // SHT0_3 -> alonga o ciclo do timer de 32 batidas
    // ON     -> liga o conversor
    // MSC    -> faz com que trigger possa gerar N conversões
    ADC12CTL0 |= ADC12SHT0_3 | ADC12SHT1_3 | ADC12ON | ADC12MSC;

    // SHS_1    -> ACLOCK CCR1
    // SHP      -> utilizar um contador para alongar o ciclo do clock original do conversor
    // CONSEQ_3 -> modo de operação 3, com MSC ativo, um trigger faz N conversões
    ADC12CTL1 = ADC12SHS_1 | ADC12CONSEQ_3 | ADC12SHP;
    ADC12CTL2 = ADC12RES_2; // resolução de 12 bits
    ADC12CTL3 = ADC12CSTARTADD_0; // começa em ADC12MEM0

    // Fazendo 16 medições para tirar a média delas depois
    ADC12MCTL0 = ADC12INCH_12;
    ADC12MCTL1 = ADC12INCH_12;
    ADC12MCTL2 = ADC12INCH_12;
    ADC12MCTL3 = ADC12INCH_12;
    ADC12MCTL4 = ADC12INCH_12;
    ADC12MCTL5 = ADC12INCH_12;
    ADC12MCTL6 = ADC12INCH_12;
    ADC12MCTL7 = ADC12INCH_12;
    ADC12MCTL8 = ADC12INCH_12;
    ADC12MCTL9 = ADC12INCH_12;
    ADC12MCTL10 = ADC12INCH_12;
    ADC12MCTL11 = ADC12INCH_12;
    ADC12MCTL12 = ADC12INCH_12;
    ADC12MCTL13 = ADC12INCH_12;
    ADC12MCTL14 = ADC12INCH_12;
    ADC12MCTL15 = ADC12INCH_12 | ADC12EOS;

    ADC12IER0 |= ADC12IE15;
}
