#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// Dirección I2C por defecto (puede ser 0x76 o 0x77)
#define BMP280_I2C_ADDR  0x76 << 1  // Asegurate de ponerla correctamente

// Registrar este I2C externamente
extern I2C_HandleTypeDef hi2c1;

// Funciones públicas
bool BMP280_Init(void);
bool BMP280_ReadTemperatureAndPressure(float *temperature, float *pressure);

#endif /* INC_BMP280_H_ */
