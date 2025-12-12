#include "bmp280.h"
#include <math.h>

// Registros del BMP280
#define BMP280_REG_ID            0xD0
#define BMP280_REG_RESET         0xE0
#define BMP280_REG_CTRL_MEAS     0xF4
#define BMP280_REG_CONFIG        0xF5
#define BMP280_REG_PRESS_MSB     0xF7

// Calibración interna
static uint16_t dig_T1;
static int16_t  dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int32_t  t_fine;

static bool read_calibration_data(void) {
    uint8_t calib[24];
    if (HAL_I2C_Mem_Read(&hi2c1, BMP280_I2C_ADDR, 0x88, 1, calib, 24, HAL_MAX_DELAY) != HAL_OK)
        return false;

    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];
    dig_P1 = (calib[7] << 8) | calib[6];
    dig_P2 = (calib[9] << 8) | calib[8];
    dig_P3 = (calib[11] << 8) | calib[10];
    dig_P4 = (calib[13] << 8) | calib[12];
    dig_P5 = (calib[15] << 8) | calib[14];
    dig_P6 = (calib[17] << 8) | calib[16];
    dig_P7 = (calib[19] << 8) | calib[18];
    dig_P8 = (calib[21] << 8) | calib[20];
    dig_P9 = (calib[23] << 8) | calib[22];

    return true;
}

bool BMP280_Init(void) {
    uint8_t id;
    if (HAL_I2C_Mem_Read(&hi2c1, BMP280_I2C_ADDR, BMP280_REG_ID, 1, &id, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;

    if (id != 0x58) return false; // BMP280 ID

    if (!read_calibration_data())
        return false;

    uint8_t ctrl_meas = 0x27;  // Temp x1, Press x1, Normal mode
    uint8_t config = 0xA0;     // Standby 1000ms, filter off

    if (HAL_I2C_Mem_Write(&hi2c1, BMP280_I2C_ADDR, BMP280_REG_CTRL_MEAS, 1, &ctrl_meas, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Mem_Write(&hi2c1, BMP280_I2C_ADDR, BMP280_REG_CONFIG, 1, &config, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;

    return true;
}

static int32_t compensate_temperature(int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
                    ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static uint32_t compensate_pressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) +
           ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

    if (var1 == 0) return 0; // avoid exception

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (uint32_t)(p >> 8); // Pa
}

bool BMP280_ReadTemperatureAndPressure(float *temperature, float *pressure) {
    uint8_t data[6];
    if (HAL_I2C_Mem_Read(&hi2c1, BMP280_I2C_ADDR, BMP280_REG_PRESS_MSB, 1, data, 6, HAL_MAX_DELAY) != HAL_OK)
        return false;

    int32_t adc_P = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4));
    int32_t adc_T = (int32_t)(((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | (data[5] >> 4));

    int32_t temp = compensate_temperature(adc_T);
    uint32_t pres = compensate_pressure(adc_P);

    *temperature = temp / 100.0f;
    *pressure = (pres / 1.0f)/1000.0;

    return true;
}
