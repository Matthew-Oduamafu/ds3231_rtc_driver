# DS3231 RTC Driver for STM32F0

A comprehensive driver library for the DS3231 Real-Time Clock module for STM32F0 microcontrollers, with bonus LCD I2C display driver for 20x4 character displays.

## Features

### DS3231 RTC Driver
- **Time & Date Management**: Read and write time/date with BCD conversion
- **Temperature Sensor**: Built-in temperature sensor reading (±3°C accuracy)
- **I2C Communication**: Uses STM32 HAL I2C interface
- **Formatted Output**: Helper functions for time/date formatting
- **Day Names**: Automatic day-of-week name resolution

### LCD I2C Driver (20x4)
- **I2C Interface**: PCF8574-based LCD control
- **Text Display**: Print text at any position
- **Custom Characters**: Create and display custom characters
- **Animations**: Built-in support for arrows, spinners, and progress bars
- **Drawing Functions**: Boxes, separators, and progress bars
- **Backlight Control**: Turn backlight on/off

## Hardware Requirements

- **Microcontroller**: STM32F091RC (or compatible STM32F0 series)
- **RTC Module**: DS3231 Real-Time Clock module
- **Display** (optional): 20x4 LCD with I2C interface (PCF8574)
- **Communication**: I2C bus (I2C1 in this implementation)

## Pin Connections

### I2C Configuration (I2C1)
- **SDA**: Connect to PB7 (or your configured I2C1 SDA pin)
- **SCL**: Connect to PB6 (or your configured I2C1 SCL pin)
- **Pull-up resistors**: 4.7kΩ on both SDA and SCL lines (may be built into modules)

### DS3231 Module
```
DS3231         STM32F091RC
------         -----------
VCC     <-->   3.3V or 5V
GND     <-->   GND
SDA     <-->   PB7 (I2C1_SDA)
SCL     <-->   PB6 (I2C1_SCL)
```

### LCD I2C Module (Optional)
```
LCD I2C        STM32F091RC
-------        -----------
VCC     <-->   5V
GND     <-->   GND
SDA     <-->   PB7 (I2C1_SDA)
SCL     <-->   PB6 (I2C1_SCL)
```

**Note**: Both the DS3231 and LCD can share the same I2C bus, as they have different I2C addresses.

## Software Requirements

- **STM32CubeIDE** or compatible ARM toolchain
- **STM32 HAL Library** for STM32F0 series
- **CMake** 3.22 or higher (for CMake build)
- **ARM GCC Compiler**

## I2C Addresses

- **DS3231 RTC**: 0x68 (7-bit address)
- **LCD I2C**: 0x27 (7-bit address) - adjust in `lcd_i2c_driver.h` if your module uses 0x3F

## Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/Matthew-Oduamafu/ds3231_rtc_driver.git
cd ds3231_rtc_driver
```

### 2. Build with CMake
```bash
mkdir build
cd build
cmake ..
make
```

### 3. Flash to STM32
```bash
# Using STM32CubeProgrammer or your preferred flashing tool
st-flash write stm32f091rc_ds3231_rtc.bin 0x8000000
```

### 4. Set Initial Time (First Run)
On first run, you need to set the initial date and time:

1. Open `Core/Src/main.c`
2. Change `#define SET_RTC_TIME_ON_FIRST_RUN 0` to `1`
3. Modify the `init_time` structure with the current date/time:
```c
RTC_DateTime init_time = {
    .seconds = 0,
    .minutes = 30,
    .hours = 14,        // 2:30 PM
    .day = 1,           // Sunday (1-7)
    .date = 2,          // 2nd
    .month = 2,         // February
    .year = 2026
};
```
4. Rebuild and flash
5. After successful time set, change `SET_RTC_TIME_ON_FIRST_RUN` back to `0`
6. Rebuild and flash again

## Usage Examples

### Initialize RTC
```c
#include "ds3231_rtc_driver.h"

RTC_Handle rtc;
I2C_HandleTypeDef hi2c1;

// Initialize I2C (done by HAL)
MX_I2C1_Init();

// Initialize RTC
if (RTC_Init(&rtc, &hi2c1) != HAL_OK) {
    // Handle error
    Error_Handler();
}
```

### Set Date and Time
```c
RTC_DateTime datetime = {
    .seconds = 0,
    .minutes = 15,
    .hours = 10,
    .day = 5,           // Thursday
    .date = 8,
    .month = 11,
    .year = 2025
};

if (RTC_SetDateTime(&rtc, &datetime) != HAL_OK) {
    // Handle error
}
```

### Read Date and Time
```c
RTC_DateTime datetime;

if (RTC_GetDateTime(&rtc, &datetime) == HAL_OK) {
    // Use datetime structure
    printf("Time: %02d:%02d:%02d\n", 
           datetime.hours, datetime.minutes, datetime.seconds);
    printf("Date: %02d/%02d/%04d\n", 
           datetime.date, datetime.month, datetime.year);
    printf("Day: %s\n", RTC_GetDayName(datetime.day));
}
```

### Read Temperature
```c
float temperature;

if (RTC_GetTemperature(&rtc, &temperature) == HAL_OK) {
    printf("RTC Temperature: %.2f°C\n", temperature);
}
```

### Format Time/Date Strings
```c
RTC_DateTime datetime;
char buffer[32];

RTC_GetDateTime(&rtc, &datetime);

// Format time only (HH:MM:SS)
RTC_FormatTime(&datetime, buffer);
// buffer = "14:30:00"

// Format date only (DD/MM/YYYY)
RTC_FormatDate(&datetime, buffer);
// buffer = "08/11/2025"

// Format date and time (DD/MM/YYYY HH:MM:SS)
RTC_FormatDateTime(&datetime, buffer);
// buffer = "08/11/2025 14:30:00"
```

### Using the LCD Driver
```c
#include "lcd_i2c_driver.h"

LCD_Handle lcd;

// Initialize LCD
LCD_Init(&lcd, &hi2c1);

// Clear screen and print
LCD_Clear(&lcd);
LCD_PrintAt(&lcd, 0, 0, "Hello World!");

// Print formatted text
LCD_Printf(&lcd, 1, 0, "Time: %02d:%02d", hours, minutes);

// Draw a progress bar
LCD_DrawProgressBar(&lcd, 2, 0, 20, 75); // 75% progress

// Create separator line
LCD_DrawSeparator(&lcd, 1);
```

## API Reference

### DS3231 RTC Functions

#### `HAL_StatusTypeDef RTC_Init(RTC_Handle *hrtc, I2C_HandleTypeDef *hi2c)`
Initialize the RTC driver and verify DS3231 is present.

#### `HAL_StatusTypeDef RTC_SetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime)`
Set the current date and time.

#### `HAL_StatusTypeDef RTC_GetDateTime(RTC_Handle *hrtc, RTC_DateTime *datetime)`
Read the current date and time.

#### `HAL_StatusTypeDef RTC_GetTemperature(RTC_Handle *hrtc, float *temperature)`
Read the internal temperature sensor value.

#### `void RTC_FormatTime(RTC_DateTime *datetime, char *buffer)`
Format time as "HH:MM:SS" string.

#### `void RTC_FormatDate(RTC_DateTime *datetime, char *buffer)`
Format date as "DD/MM/YYYY" string.

#### `void RTC_FormatDateTime(RTC_DateTime *datetime, char *buffer)`
Format full date and time as "DD/MM/YYYY HH:MM:SS" string.

#### `const char* RTC_GetDayName(uint8_t day)`
Get day name string from day number (1-7).

### Data Structures

```c
typedef struct {
    uint8_t seconds;    // 0-59
    uint8_t minutes;    // 0-59
    uint8_t hours;      // 0-23
    uint8_t day;        // 1-7 (Sunday = 1)
    uint8_t date;       // 1-31
    uint8_t month;      // 1-12
    uint16_t year;      // 2000-2099
} RTC_DateTime;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
} RTC_Handle;
```

## Project Structure

```
ds3231_rtc_driver/
├── Core/
│   ├── Inc/
│   │   ├── ds3231_rtc_driver.h    # RTC driver header
│   │   ├── lcd_i2c_driver.h       # LCD driver header
│   │   ├── main.h
│   │   └── stm32f0xx_*.h          # HAL headers
│   └── Src/
│       ├── ds3231_rtc_driver.c    # RTC driver implementation
│       ├── lcd_i2c_driver.c       # LCD driver implementation
│       ├── main.c                 # Main application
│       └── stm32f0xx_*.c          # HAL sources
├── Drivers/                        # STM32 HAL drivers
├── CMakeLists.txt                  # CMake build configuration
├── STM32F091XX_FLASH.ld           # Linker script
└── README.md                       # This file
```

## Troubleshooting

### RTC Not Detected
- Check I2C connections (SDA, SCL, VCC, GND)
- Verify pull-up resistors on I2C lines (4.7kΩ typical)
- Check I2C address (should be 0x68 for DS3231)
- Verify DS3231 module has battery installed

### LCD Not Working
- Check LCD I2C address (0x27 or 0x3F) in `lcd_i2c_driver.h`
- Verify LCD module receives 5V power
- Adjust LCD contrast potentiometer if display is blank
- Check I2C connections

### Incorrect Time
- Ensure you've set the initial time using `SET_RTC_TIME_ON_FIRST_RUN`
- Verify DS3231 has a backup battery installed
- Check that BCD conversion is working correctly

### Build Errors
- Ensure all STM32 HAL drivers are properly configured
- Verify CMake version is 3.22 or higher
- Check that ARM GCC toolchain is in PATH

## About DS3231

The DS3231 is a highly accurate I2C real-time clock (RTC) with an integrated temperature-compensated crystal oscillator (TCXO) and crystal. The device incorporates:

- **Accuracy**: ±2ppm from 0°C to +40°C
- **Battery Backup**: Automatic switchover to battery when main power fails
- **Temperature Sensor**: Built-in sensor with ±3°C accuracy
- **Alarms**: Two programmable time-of-day alarms (not implemented in this driver yet)
- **32kHz Output**: Square-wave output (not used in this implementation)

## Future Enhancements

- [ ] Alarm functionality implementation
- [ ] Square wave output configuration
- [ ] Aging offset adjustment
- [ ] More robust error handling
- [ ] Support for 12-hour format
- [ ] Additional LCD animations and effects

## License

This project is provided as-is for educational and hobbyist purposes. Check individual file headers for specific licensing information.

## Author

**Matthew Oduamafu**
- GitHub: [@Matthew-Oduamafu](https://github.com/Matthew-Oduamafu)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgments

- STMicroelectronics for the STM32 HAL library
- Maxim Integrated for the DS3231 RTC datasheet
- The embedded systems community for inspiration and support

## Resources

- [DS3231 Datasheet](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf)
- [STM32F091 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031936.pdf)
- [PCF8574 I2C I/O Expander Datasheet](https://www.ti.com/lit/ds/symlink/pcf8574.pdf)
