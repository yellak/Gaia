#include "lcd.h"

void delay(long limite) {
    volatile long cont=0;
    while (cont++ < limite) ;
}

// Auxiliar inicialização do LCD (RS = RW = 0)
// envia um nibble
void LCD_aux(char dado) {
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | 0x08;          //PCF7:4 = dado;
    delay(50);
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | 0x08 | 0x04;  //E=1
    delay(50);
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | 0x08;          //E=0;
}

// Gerar START e STOP para colocar PCF em estado conhecido
void STT_STP(void) {
    int x = 0;
    UCB1I2CSA = LCD_ADDR;

    while (x < 5) {
        UCB1CTLW0 |= UCTR | UCTXSTT;
        while ( (UCB1IFG & UCTXIFG) == 0);
        UCB1CTLW0 |= UCTXSTP; //Gerar STOP
        delay(200);
        if ( (UCB1CTLW0 & UCTXSTP) == 0) {
            break;   //Esperar STOP
        }
        x++;
    }
    while (UCB1CTLW0 & UCTXSTP);   //I2C Travado (Desligar / Ligar)
}

// Incializar LCD modo 4 bits
void LCD_init(void){

    STT_STP(); //Colocar LCD em estado conhecido

    // Preparar I2C para operar
    UCB1I2CSA = LCD_ADDR;    //Endereço Escravo
    UCB1CTL1 |= UCTR | UCTXSTT;
    while (!(UCB1IFG & UCTXIFG));          //Esperar TXIFG=1
    UCB1TXBUF = 0;                              //Saída PCF = 0;
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //Esperar STT=0
    if ( (UCB1IFG & UCTXIFG0) == UCNACKIFG) {
        return;
    }

    // Começar inicialização
    LCD_aux(0);     //RS=RW=0, BL=1
    delay(20000);
    LCD_aux(3);     //3
    delay(10000);
    LCD_aux(3);     //3
    delay(10000);
    LCD_aux(3);     //3
    delay(10000);
    LCD_aux(2);     //2

    // Entrou em modo 4 bits
    LCD_aux(2);     LCD_aux(8);     // 0x28 - Def. 8 bits, N e F
    LCD_aux(0);     LCD_aux(8);     // 0x08 - desativar LCD
    LCD_aux(0);     LCD_aux(1);     // 0x01 - Limpar LCD
    LCD_aux(0);     LCD_aux(6);     // 0x06 - Modo de entrada I/D e S
    LCD_aux(0);     LCD_aux(0xF);   // 0x0F - Ativar LCD e def. C e B

    while ( (UCB1IFG & UCTXIFG) == 0)   ;          //Esperar TXIFG=1
    UCB1CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
    delay(50);
}

void backLightOn() {
    uint8_t data[1] = {0x08};
    i2cWrite(LCD_ADDR, data, 1);
}

void backLightOff() {
    uint8_t data[1] = {0x00};
    i2cWrite(LCD_ADDR, data, 1);
}

void lcdWriteNibble(uint8_t nibble, uint8_t rs) {
    uint8_t data[3];
    //legenda               | BL  | EN  |R/W|
    data[0] = (nibble << 4) | 0x8 | 0x0 | 0 | rs;
    data[1] = (nibble << 4) | 0x8 | 0x4 | 0 | rs;
    data[2] = (nibble << 4) | 0x8 | 0x0 | 0 | rs;
    i2cWrite(LCD_ADDR, data, 3);
}

void lcdWriteByte(uint8_t Byte, uint8_t rs) {
    lcdWriteNibble((Byte >> 4) & 0xF, rs);
    lcdWriteNibble(Byte & 0xF, rs);
}

void lcdMoveCursor(isRight) {
    uint8_t direction;
    if(isRight) {
        direction = 0x14;
    }
    else {
        direction = 0x10;
    }
    lcdWriteByte(direction, 0);
}

void lcdReturnHome() {
    uint8_t cmd = 0x02;
    lcdWriteByte(cmd, 0);
}

void lcdSetCursorPos(uint8_t line, uint8_t column) {
    uint8_t cmd = BIT7;
    if(line > 1 || column > 15) {
        return;
    }
    if(line == 0) {
        cmd |= column;
    }
    else {
        cmd |= (column + 64);
    }
    lcdWriteByte(cmd, 0);
}

void lcdHex8(uint8_t byte) {
    int i;
    uint8_t nibbles[2];
    for(i = 0; i < 2; i++) {
        nibbles[1 - i] = (byte >> i*4) & 0xF;
    }
    for(i = 0; i < 2; i++) {
        if(nibbles[i] >= 10) {
            lcdWriteByte('A' + nibbles[i] - 10, 1);
        }
        else {
            lcdWriteByte('0' + nibbles[i], 1);
        }
    }
}

void lcdHex16(uint16_t word) {
    lcdHex8(word >> 8);
    lcdHex8(word & 0xFF);
}

void lcdDec8(uint8_t value) {
    if(value <= 9) {
        lcdWriteByte('0' + value, 1);
    }
    else if(value > 9 && value <= 99) {
        lcdWriteByte('0' + (uint8_t)(value/10), 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
    else {
        lcdWriteByte('0' + value/100, 1);
        lcdWriteByte('0' + (uint8_t)(value/10) - ((uint8_t)(value/100))*10, 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
}

void lcdDec16(uint16_t value) {
    if(value <= 9) {
        lcdWriteByte('0' + value, 1);
    }
    else if(value > 9 && value <= 99) {
        lcdWriteByte('0' + (uint8_t)(value/10), 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
    else if(value > 99 && value <= 999) {
        lcdWriteByte('0' + value/100, 1);
        lcdWriteByte('0' + (uint8_t)(value/10) - ((uint8_t)(value/100))*10, 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
    else if(value > 999 && value <= 9999) {
        lcdWriteByte('0' + value/1000, 1);
        lcdWriteByte('0' + value/100 - ((uint8_t)(value/1000))*10, 1);
        lcdWriteByte('0' + value/10 - ((uint8_t)(value/100))*10, 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
    else {
        lcdWriteByte('0' + value/10000, 1);
        lcdWriteByte('0' + value/1000 - ((uint8_t)(value/10000))*10, 1);
        lcdWriteByte('0' + value/100 - ((uint8_t)(value/1000))*10, 1);
        lcdWriteByte('0' + value/10 - ((uint8_t)(value/100))*10, 1);
        lcdWriteByte('0' + value - ((uint8_t)(value/10))*10, 1);
    }
}

void lcdFloat1(float number, uint8_t nDec) {
    lcdWriteByte('0' + (uint8_t)number, 1);
    lcdWriteByte(',', 1);

    int i;
    uint8_t value;

    for(i = 0; i < nDec; i++) {
        value = (uint8_t)(number);
        number -= value;
        number *= 10;
        lcdWriteByte('0' + (uint8_t)(number), 1);
    }
}

void lcdDisplayCTL(uint8_t displayOn, uint8_t cursorOn, uint8_t curPosOn) {
    uint8_t cmd = BIT3 | ((displayOn > 0) << 2) | ((cursorOn > 0) << 1) |  (curPosOn > 0);
    lcdWriteByte(cmd, 0);
}

uint8_t lcdWhichAddr() {
    // Transmitindo o byte 0x3F
    UCB1I2CSA = 0x3F;
    UCB1CTLW0 |= UCTR | UCTXSTT; // modo transmissor e gerando start
    while(!(UCB1IFG & UCTXIFG0)); // esperando UCTXBUF ficar vazio
    UCB1TXBUF = 0;
    __delay_cycles(1000);
    if(UCB1IFG & UCTXIFG0) {
        return 0x3F;
    }

    // Transmitindo o byte 0x27
    UCB1I2CSA = 0x27;
    UCB1CTLW0 |= UCTR | UCTXSTT; // modo transmissor e gerando start
    while(!(UCB1IFG & UCTXIFG0)); // esperando UCTXBUF ficar vazio
    UCB1TXBUF = 0;
    __delay_cycles(1000);
    if(UCB1IFG & UCTXIFG0) {
        return 0x27;
    }

    return 0;
}

void lcdWriteStr(char string[]) {
    uint32_t c = 0;
    while(string[c] != '\0') {
        lcdWriteByte(string[c++], 1);
    }
}
