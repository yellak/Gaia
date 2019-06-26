#include "uart.h"

// par -> '0' sem paridade
//        '1' paridade par
//        '2' paridade ímpar
void uartInit(uint32_t br, uint8_t isMSB, uint8_t par, uint8_t nStops) {
    UCA3CTLW0 = UCSWRST;
    if(par != 0) {
        UCA3CTLW0 |= UCPEN;
        if(par == 1) {
            UCA3CTLW0 |= UCPAR__EVEN; // par
        }
        else {
            UCA3CTLW0 |= UCPAR__ODD; // impar
        }
    }

    if(isMSB) {
        UCA3CTLW0 |= UCMSB;
    }

    if(nStops == 2) {
        UCA3CTLW0 |= UCSPB;
    }

    UCA3CTLW0 |= UCSSEL__SMCLK | UCSWRST;

    //uint32_t brDiv = (1000000 << 3) / br;
    //UCA3BRW = brDiv >> 3;
    //UCA3MCTLW = (brDiv & 0x07) << 8;
    UCA3BRW = 6;
    UCA3MCTLW = UCBRF_8 | UCOS16;
    P6SEL1 &= ~(BIT0 | BIT1);
    P6SEL0 |= (BIT0 | BIT1);
    UCA3CTLW0 &= ~UCSWRST;
}

void uartSend(uint8_t byte) {
    while(!(UCA3IFG & UCTXIFG));
    UCA3TXBUF = byte;
}

int uartGet(uint8_t *data) {
    TA2CTL = TASSEL__ACLK | MC__UP | ID__4;
    TA2CCR0 = 0xFFFF; // 8 segundos
    while(!(UCA3IFG & UCRXIFG) && !(TA2CTL & TAIFG));
    if(TA2CTL & TAIFG) {
        return -1;
    }
    *data = UCA3RXBUF;
    return 0;
}

int uartSendStr(uint8_t string[]) {
    uint32_t cont = 0;
    while(string[cont] != '\0') {
        uartSend(string[cont++]);
    }
}
