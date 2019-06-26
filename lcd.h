#ifndef _LIB_LCD_
#define _LIB_LCD_
#include "i2c.h"

#define LCD_ADDR 0x27

void delay(long limite);

// liga o backlight
void backLightOn();

// desliga o backlight
void backLightOff();

// escreve um nibble no LCD
void lcdWriteNibble(uint8_t nibble, uint8_t rs);

// Escreve um byte no LCD
// rs: 0 -> algum comando
// rs: 1 -> leitura ou escrita do que está na tela
void lcdWriteByte(uint8_t Byte, uint8_t rs);

// Move o cursor na tela
// isRight: '1' -> mover para a direita
//          '0' -> mover para a esquerda
void lcdMoveCursor(isRight);

// Retorna o crusor para o início e
// coloca o endereço da DDRAM em 0x00
void lcdReturnHome();

// Seta a posição do cursor
// 0 <= column <= 15
// 0 <= line <= 1
void lcdSetCursorPos(uint8_t line, uint8_t column);

// Escreve a forma hexadecimal de um byte no LCD
void lcdHex8(uint8_t byte);

// Escreve a forma hexadecimal de uma palavra de 16 bits
void lcdHex16(uint16_t word);

// Escreve no LCD um número em decimal de 8 bits
// 0 <= value <= 255
void lcdDec8(uint8_t value);

// Escreve no LCD um número em decimal de 16 bits
// 0 <= value <= 65535
void lcdDec16(uint16_t value);

// Mostra um valor float com 3 casas decimais no LCD
// a parte inteira do float deve ter apenas um dígito em decimal,
// ou será impesso um caractere inesperado
void lcdFloat1(float number, uint8_t nDec);

// Controle do display, parametros booleanos
// displayOn : '0' -> display ligado
//             '1' -> display desligado
// cursorOn  : '0' -> cursor invisível
//             '1' -> cursor visível
// curPosOn  : '0' -> cursor não pisca
//             '1' -> cursor fica piscando
void lcdDisplayCTL(uint8_t displayOn, uint8_t cursorOn, uint8_t curPosOn);

// Retorna qual o endereço de LCD usar:
// Retorno:
// 0x3F ou 0x27
// 0 se nenhum dos dois
uint8_t lcdWhichAddr();

// Escreve uma string no LCD
void lcdWriteStr(char string[]);

// Funções para inicialização do LCD
void LCD_init();
void LCD_aux(char dado);
void STT_STP(void);

#endif
