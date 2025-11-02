#include "i2c_lcd.h"

// Gửi lệnh tới LCD
static void lcd_send_cmd(I2C_LCD_HandleTypeDef *lcd, char cmd) {
    char upper = (cmd & 0xF0);
    char lower = ((cmd << 4) & 0xF0);
    uint8_t data[4];

    data[0] = upper | 0x0C;  // en=1, rs=0
    data[1] = upper | 0x08;  // en=0, rs=0
    data[2] = lower | 0x0C;  // en=1, rs=0
    data[3] = lower | 0x08;  // en=0, rs=0

    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, data, 4, 100);
}

// Gửi data (ký tự) tới LCD
static void lcd_send_data(I2C_LCD_HandleTypeDef *lcd, char data) {
    char upper = (data & 0xF0);
    char lower = ((data << 4) & 0xF0);
    uint8_t data_t[4];

    data_t[0] = upper | 0x0D;  // en=1, rs=1
    data_t[1] = upper | 0x09;  // en=0, rs=1
    data_t[2] = lower | 0x0D;  // en=1, rs=1
    data_t[3] = lower | 0x09;  // en=0, rs=1

    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, data_t, 4, 100);
}

// Xóa LCD
void lcd_clear(I2C_LCD_HandleTypeDef *lcd) {
    lcd_send_cmd(lcd, 0x01); // Clear display
    HAL_Delay(2);
}

// Di chuyển con trỏ
void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, uint8_t col, uint8_t row) {
    uint8_t address;
    switch (row) {
        case 0: address = 0x80 + col; break;
        case 1: address = 0xC0 + col; break;
        case 2: address = 0x94 + col; break;
        case 3: address = 0xD4 + col; break;
        default: return;
    }
    lcd_send_cmd(lcd, address);
}

// Khởi tạo LCD
void lcd_init(I2C_LCD_HandleTypeDef *lcd) {
    HAL_Delay(50);
    lcd_send_cmd(lcd, 0x30);
    HAL_Delay(5);
    lcd_send_cmd(lcd, 0x30);
    HAL_Delay(1);
    lcd_send_cmd(lcd, 0x30);
    HAL_Delay(10);
    lcd_send_cmd(lcd, 0x20); // 4-bit mode
    HAL_Delay(10);

    lcd_send_cmd(lcd, 0x28); // 4-bit, 2 lines
    lcd_send_cmd(lcd, 0x08); // Display off
    lcd_send_cmd(lcd, 0x01); // Clear
    HAL_Delay(2);
    lcd_send_cmd(lcd, 0x06); // Entry mode
    lcd_send_cmd(lcd, 0x0C); // Display on
}

// Gửi chuỗi
void lcd_puts(I2C_LCD_HandleTypeDef *lcd, char *str) {
    while (*str) {
        lcd_send_data(lcd, *str++);
    }
}

// Gửi 1 ký tự
void lcd_putchar(I2C_LCD_HandleTypeDef *lcd, char ch) {
    lcd_send_data(lcd, ch);
}

// Dịch trái
void lcd_shift_left(I2C_LCD_HandleTypeDef *lcd, uint8_t steps, uint32_t delay_ms) {
    for (uint8_t i = 0; i < steps; i++) {
        lcd_send_cmd(lcd, 0x18); // dịch trái toàn màn hình
        HAL_Delay(delay_ms);
    }
}

// Dịch phải
void lcd_shift_right(I2C_LCD_HandleTypeDef *lcd, uint8_t steps, uint32_t delay_ms) {
    for (uint8_t i = 0; i < steps; i++) {
        lcd_send_cmd(lcd, 0x1C); // dịch phải toàn màn hình
        HAL_Delay(delay_ms);
    }
}

/**
 * @brief  Create a custom character in CGRAM
 * @param  lcd: Pointer to LCD handle
 * @param  location: CGRAM location (0–7)
 * @param  pattern: 8-byte array (5x8 pixel map)
 */
void lcd_create_char(I2C_LCD_HandleTypeDef *lcd, uint8_t location, uint8_t *pattern)
{
    location &= 0x07; // chỉ có 8 slot (0–7)
    lcd_send_cmd(lcd, 0x40 | (location << 3)); // set CGRAM address

    for (int i = 0; i < 8; i++) {
        lcd_send_data(lcd, pattern[i]);
    }
}

/**
 * @brief  Print custom char at given position
 * @param  lcd: Pointer to LCD handle
 * @param  col: Column
 * @param  row: Row
 * @param  location: CGRAM slot (0–7)
 */
void lcd_put_custom(I2C_LCD_HandleTypeDef *lcd, uint8_t col, uint8_t row, uint8_t location)
{
    lcd_gotoxy(lcd, col, row);
    lcd_send_data(lcd, location);
}
#include <stdio.h>   // để dùng sprintf

void lcd_print_int(I2C_LCD_HandleTypeDef *lcd, int value, uint8_t row, uint8_t col) {
    char buffer[16];
    sprintf(buffer, "%d", value);
    lcd_gotoxy(lcd, row, col);
    lcd_puts(lcd, buffer);
}

void lcd_print_float(I2C_LCD_HandleTypeDef *lcd, float value, uint8_t row, uint8_t col, uint8_t precision) {
    char buffer[20];
    char format[10];

    // Tạo format động, ví dụ "%.2f"
    sprintf(format, "%%.%df", precision);
    sprintf(buffer, format, value);

    lcd_gotoxy(lcd, row, col);
    lcd_puts(lcd, buffer);
}

