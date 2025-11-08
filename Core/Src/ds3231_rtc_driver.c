//
// Created by Matthew Oduamafu on 08/11/2025.
//
/*
 * ds3231_rtc_driver.c
 * Implementation of DS3231 RTC driver
 */

#include "ds3231_rtc_driver.h"

#include <stdio.h>
#include <string.h>

static const char* DAY_NAMES[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

// Initialize RTC
HAL_StatusTypeDef RTC_Init(RTC_Handle *hrtc, I2C_HandleTypeDef *hi2c) {
    hrtc->hi2c = hi2c;
    hrtc->address = DS3231_I2C_ADDR;

    // Check if DS3231 is present
    if (HAL_I2C_IsDeviceReady(hrtc->hi2c, hrtc->address, 3, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    // Clear oscillator stop flag
    uint8_t status;
    HAL_I2C_Mem_Read(hrtc->hi2c, hrtc->address, DS3231_REG_STATUS, 1, &status, 1, 100);
    status &= ~0x80;
    HAL_I2C_Mem_Write(hrtc->hi2c, hrtc->address, DS3231_REG_STATUS, 1, &status, 1, 100);

    return HAL_OK;
}

// Set date and time
HAL_StatusTypeDef RTC_SetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime) {
    uint8_t data[7U];

    data[0] = Decimal_To_BCD(datetime->seconds);
    data[1] = Decimal_To_BCD(datetime->minutes);
    data[2] = Decimal_To_BCD(datetime->hours);
    data[3] = datetime->day;
    data[4] = Decimal_To_BCD(datetime->date);
    data[5] = Decimal_To_BCD(datetime->month);
    data[6] = Decimal_To_BCD(datetime->year - 2000);

    return HAL_I2C_Mem_Write(hrtc->hi2c, hrtc->address, DS3231_REG_SECONDS, 1, data, 7, 100);
}

// Get date and time
HAL_StatusTypeDef RTC_GetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime) {
    uint8_t data[7];

    if (HAL_I2C_Mem_Read(hrtc->hi2c, hrtc->address, DS3231_REG_SECONDS, 1, data, 7, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    datetime->seconds = BCD_To_Decimal(data[0] & 0x7FU);
    datetime->minutes = BCD_To_Decimal(data[1] & 0x7FU);
    datetime->hours = BCD_To_Decimal(data[2] & 0x3FU);
    datetime->day = data[3] & 0x07;
    datetime->date = BCD_To_Decimal(data[4] & 0x3FU);
    datetime->month = BCD_To_Decimal(data[5] & 0x1FU);
    datetime->year = BCD_To_Decimal(data[6]) + 2000;

    return HAL_OK;
}

// Get temperature from DS3231
HAL_StatusTypeDef RTC_GetTemperature(RTC_Handle *hrtc, float *temperature) {
    uint8_t data[2];

    if (HAL_I2C_Mem_Read(hrtc->hi2c, hrtc->address, DS3231_REG_TEMP_MSB, 1, data, 2, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    int16_t temp = (data[0U] << 8U) | data[1U];
    *temperature = temp / 256.0f;

    return HAL_OK;
}

// Format time as HH:MM:SS
void RTC_FormatTime(RTC_DateTime *datetime, char *buffer) {
    sprintf(buffer, "%02d:%02d:%02d",
            datetime->hours, datetime->minutes, datetime->seconds);
}

// Format date as DD/MM/YYYY
void RTC_FormatDate(RTC_DateTime *datetime, char *buffer) {
    sprintf(buffer, "%02d/%02d/%04d",
            datetime->date, datetime->month, datetime->year);
}

// Format date and time
void RTC_FormatDateTime(RTC_DateTime *datetime, char *buffer) {
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            datetime->date, datetime->month, datetime->year,
            datetime->hours, datetime->minutes, datetime->seconds);
}

// Get day name
const char* RTC_GetDayName(uint8_t day) {
    if (day >= 1U && day <= 7U) {
        return DAY_NAMES[day - 1U];
    }
    return "Unknown";
}

// Convert BCD to Decimal
uint8_t BCD_To_Decimal(uint8_t bcd) {
    return ((bcd >> 4U) * 10U) + (bcd & 0x0FU);
}

// Convert Decimal to BCD
uint8_t Decimal_To_BCD(uint8_t decimal) {
    return ((decimal / 10U) << 4U) | (decimal % 10U);
}