//
// Created by Matthew Oduamafu on 08/11/2025.
//
/*
 * ds3231_rtc_driver.h
 * Driver for DS3231 Real-Time Clock
 */

#ifndef DS3231_RTC_DRIVER_H
#define DS3231_RTC_DRIVER_H

#include "stm32f0xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// DS3231 I2C Address
#define DS3231_I2C_ADDR    0x68U << 1

// Register addresses
#define DS3231_REG_SECONDS      0x00U
#define DS3231_REG_MINUTES      0x01U
#define DS3231_REG_HOURS        0x02U
#define DS3231_REG_DAY          0x03U
#define DS3231_REG_DATE         0x04U
#define DS3231_REG_MONTH        0x05U
#define DS3231_REG_YEAR         0x06U
#define DS3231_REG_CONTROL      0x0EU
#define DS3231_REG_STATUS       0x0FU
#define DS3231_REG_TEMP_MSB     0x11U
#define DS3231_REG_TEMP_LSB     0x12U

// Date/Time structure
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;        // Day of week (1-7)
    uint8_t date;       // Day of month (1-31)
    uint8_t month;      // Month (1-12)
    uint16_t year;      // Year (2000-2099)
} RTC_DateTime;

// RTC Handle
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
} RTC_Handle;

// Function prototypes
HAL_StatusTypeDef RTC_Init(RTC_Handle *hrtc, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef RTC_SetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime);
HAL_StatusTypeDef RTC_GetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime);
HAL_StatusTypeDef RTC_GetTemperature(RTC_Handle *hrtc, float *temperature);
void RTC_FormatTime(RTC_DateTime *datetime, char *buffer);
void RTC_FormatDate(RTC_DateTime *datetime, char *buffer);
void RTC_FormatDateTime(RTC_DateTime *datetime, char *buffer);
const char* RTC_GetDayName(uint8_t day);

// Utility functions
uint8_t BCD_To_Decimal(uint8_t bcd);
uint8_t Decimal_To_BCD(uint8_t decimal);

#endif //DS3231_RTC_DRIVER_H