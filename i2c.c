#include "i2c.h"

// P5.0 -> SDA
// P5.1 -> SCL
void configI2C(uint8_t ownAddress, uint16_t baudRate_kHz, uint8_t isMaster) {
    P5SEL0 |= (BIT0 | BIT1);
    P5SEL1 &= ~(BIT0 | BIT1);
    P5REN |= (BIT0 | BIT1);
    P5OUT |= (BIT0 | BIT1);

    UCB1CTLW0 = UCSWRST;
    UCB1CTLW0 |= UCMODE_3 | UCSYNC | UCSSEL__SMCLK;
    if(isMaster) {
        UCB1CTLW0 |= UCMST;
    }
    UCB1BRW = 1000 / baudRate_kHz; // divisor do clock de 1MHz
    UCB1I2COA0 = ownAddress;
    UCB1CTLW0 &= ~UCSWRST;
}

int i2cWrite(uint8_t slaveAddr, uint8_t data[], uint32_t nBytes) {
    uint32_t i;
    UCB1I2CSA = slaveAddr;

    // modo de transmissão com geração de start
    UCB1CTLW0 |= UCTR | UCTXSTT;
    // verificando se tem escravo na linha
    while(!(UCB1IFG & UCTXIFG0) && !(UCB1IFG & UCNACKIFG));
    if(UCB1IFG & UCNACKIFG) {
        UCB1CTLW0 |= UCTXSTP;
        while(UCB1CTLW0 & UCTXSTP);
        return -1;
    }

    for(i = 0; i < nBytes; i++) {
        UCB1TXBUF = data[i];
        while(!(UCB1IFG & UCTXIFG0) && !(UCB1IFG & UCNACKIFG));
        if(UCB1IFG & UCNACKIFG) {
            return 1;
        }
    }

    //Stop para encerrar a comunicação
    UCB1CTLW0 |= UCTXSTP;
    while(UCB1CTLW0 & UCTXSTP);
    return 0;
}
