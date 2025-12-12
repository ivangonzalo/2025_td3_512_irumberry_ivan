<<<<<<<< HEAD:5_docs/unidades_tematicas/2_estrategias_control/511_consigna_2/workspace/lcd/include/lcd.h
#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Comandos
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags para tipo de entrada
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYLEFT 0x02

// Flags para control de display y cursor
#define LCD_BLINKON 0x01
#define LCD_CURSORON 0x02
#define LCD_DISPLAYON 0x04

// Flags para control de movimiento de cursor
#define LCD_MOVERIGHT 0x04
#define LCD_DISPLAYMOVE 0x08

// Flags para seteo de funcion
#define LCD_5x10DOTS 0x04
#define LCD_2LINE 0x08
#define LCD_8BITMODE 0x10

// Flah para control de backlight
#define LCD_BACKLIGHT 0x08

// Comando de habilitar enable
#define LCD_ENABLE_BIT 0x04

// Modos para el send byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

// Dimensiones del display
#define MAX_LINES      2
#define MAX_CHARS      16

// Prototipos de funciones
void lcd_clear(void);
void lcd_set_cursor(int line, int position);
void lcd_char(char val);
void lcd_string(const char *s);
void lcd_init(i2c_inst_t *i2c, uint8_t address);

#endif
========


#ifndef INC_LCD_H_
#define INC_LCD_H_

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"
#elif defined(__LPC17XX__)
#include "chip.h"
#endif

// comandos
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags para tipo de entrada
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYLEFT 0x02

// Flags para control de display y cursor
#define LCD_BLINKON 0x01
#define LCD_CURSORON 0x02
#define LCD_DISPLAYON 0x04

// Flags para control de movimiento de cursor
#define LCD_MOVERIGHT 0x04
#define LCD_DISPLAYMOVE 0x08

// Flags para seteo de funcion
#define LCD_5x10DOTS 0x04
#define LCD_2LINE 0x08
#define LCD_8BITMODE 0x10

// Flag para control de backlight
#define LCD_BACKLIGHT 0x08

//Comando de habilitar enable
#define LCD_ENABLE_BIT 0x04

// Modos para el send byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

// Dimensiones del display
#define MAX_LINES      2
#define MAX_CHARS      16

#ifdef STM32F103xB
#define i2c_transmit_byte(i2c, addr, val)	HAL_I2C_Master_Transmit(i2c, (addr << 1), &val, 1, 100)
// Prototipo para inicializar LCD
void lcd_init(I2C_HandleTypeDef *hi2c1, uint8_t address);
#elif defined(__LPC17XX__)
#define i2c_transmit_byte(i2c, addr, val)	Chip_I2C_MasterSend(i2c, addr, &val, 1)
// Prototipo para inicializar LCD
void lcd_init(I2C_ID_T i2c_id, uint8_t address);
#endif

// Prototipos de funciones
void i2c_write_byte(uint8_t val);
void lcd_toggle_enable(uint8_t val);
void lcd_send_byte(uint8_t val, int mode);
void lcd_clear(void);
void lcd_set_cursor(int line, int position);
void lcd_string(const char *s);

static void inline lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

#endif /* INC_LCD_H_ */
>>>>>>>> 8b232119fe7a989e30f61086c24e929401644bec:3_trabajos_practicos/tp4/firmware/Core/Inc/lcd.h
