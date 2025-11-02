#ifndef __I2C_LCD_H
#define __I2C_LCD_H

#include "stm32f1xx_hal.h"

typedef struct {
    I2C_HandleTypeDef *hi2c;   // I2C handler
    uint16_t address;          // LCD I2C address
} I2C_LCD_HandleTypeDef;

// Function prototypes
void lcd_init(I2C_LCD_HandleTypeDef *lcd);
void lcd_clear(I2C_LCD_HandleTypeDef *lcd);
void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, uint8_t col, uint8_t row);
void lcd_puts(I2C_LCD_HandleTypeDef *lcd, char *str);
void lcd_putchar(I2C_LCD_HandleTypeDef *lcd, char ch);
void lcd_shift_left(I2C_LCD_HandleTypeDef *lcd, uint8_t steps, uint32_t delay_ms);
void lcd_shift_right(I2C_LCD_HandleTypeDef *lcd, uint8_t steps, uint32_t delay_ms);
void lcd_create_char(I2C_LCD_HandleTypeDef *lcd, uint8_t location, uint8_t *pattern);
void lcd_put_custom(I2C_LCD_HandleTypeDef *lcd, uint8_t col, uint8_t row, uint8_t location);
void lcd_print_int(I2C_LCD_HandleTypeDef *lcd, int value, uint8_t row, uint8_t col);
void lcd_print_float(I2C_LCD_HandleTypeDef *lcd, float value, uint8_t row, uint8_t col, uint8_t precision);


#endif /* __I2C_LCD_H */
