#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Play_SYM 0
#define Pause_SYM 0
#define STOP_SYM 0
#define Next_SYM 1
#define Prev_SYM 2
#define LLoop_SYM 3
#define RLoop_SYM 4
#define Select_SYM 5

void test_command(uint8_t data);
void command(uint8_t data);
void write4bits(uint8_t data);
void send(uint8_t data, uint8_t mode);

void LCD_init(uint8_t addr);

void clear();
void home();
void noDisplay();
void display();
void noBlink();
void blink();
void noCursor();
void cursor();
void scrollDisplayLeft();
void scrollDisplayRight();
void printLeft();
void printRight();
void leftToRight();
void rightToLeft();
void shiftIncrement();
void shiftDecrement();
void noBacklight();
void backlight();
void autoscroll();
void noAutoscroll();
void setCursor(uint8_t row, uint8_t col);
void LCD_print_ch(char ch);
void print_ch_at(char ch, uint8_t col, uint8_t row);
void print_cus_ch_at(uint8_t cus[8], uint8_t row, uint8_t col);
void LCD_printf(char *ch_str, int size);
void createChar(uint8_t location, uint8_t charmap[]);
void Space_Printf(int num);
void LCD_num_print(int num);
void LCD_num_R_print(int num, int fixed_space);
void LCD_S_name_printf(char *s_name);
void load_Custom_Ch();
void SW_to_Play();
void SW_to_Pause();
void SW_to_STOP();
void SW_to_Select();
void SW_to_Selected();