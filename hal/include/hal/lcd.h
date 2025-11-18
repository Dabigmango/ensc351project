#ifndef LCD_H
#define LCD_H

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <gpiod.h>

int spi_init(void);
void gpio_init(void);
void gpio_write(int pin, int value);
void lcd_cmd(int fd, uint8_t cmd);
void lcd_data(int fd, uint8_t data);
void lcd_reset(void);
void lcd_init(int fd);
void lcd_fill(int fd, uint16_t color);
void lcd_draw_pixel(int fd, int x, int y, uint16_t color);
void lcd_close(int fd);

#endif