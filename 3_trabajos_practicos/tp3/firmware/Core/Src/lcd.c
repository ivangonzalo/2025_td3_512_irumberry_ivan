

#include <lcd.h>

// I2C handler
#ifdef STM32F103xB

// Puntero a I2C usado
I2C_HandleTypeDef *i2c;

#elif defined(__LPC17XX__)
I2C_ID_T i2c;
#endif

//Direccion de 7 bits del I2C tipicamente 0x27 o 0x3f
static uint8_t pc8574_7bit_addr;

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
	// Envio byte por I2C
	i2c_transmit_byte(i2c, pc8574_7bit_addr, val);
}

void lcd_toggle_enable(uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
    for(uint32_t i = 0; i < 5000; i++);
	i2c_write_byte(val | LCD_ENABLE_BIT);
	for(uint32_t i = 0; i < 5000; i++);
	i2c_write_byte((uint8_t)(val & ~LCD_ENABLE_BIT));
	for(uint32_t i = 0; i < 5000; i++);
}

//  Envia el byte por I2C en dos nibbles

void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}
//Envia un comando de limpiar y resetear cursor

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// Pone al cursor del LCD en la posicion indicada
//line es el numero de linea (0 o 1)
//es el numero de caracter (0 a 15)

void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

#ifdef STM32F103xB
void lcd_init(I2C_HandleTypeDef *hi2c1, uint8_t address) {
	// Guardo el I2C usado
	i2c = hi2c1;
#elif defined(__LPC17XX__)
void lcd_init(I2C_ID_T i2c_id) {
	// Guardo el I2C usado
	i2c = i2c_id;
#endif
	// Guardo la direccion del display
	pc8574_7bit_addr = address;
	// Inicializo para que funcione con 4 bits de datos
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}
