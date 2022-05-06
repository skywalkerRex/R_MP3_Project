#include "R_LCD.h"
#include "delay.h"
#include "i2c.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>
// Pre Config
#define backlight_en 1
#define i2c_line 2

uint8_t _Addr = 0x4E;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines = 4;
uint8_t _cols = 20;
uint8_t _rows = 4;
uint8_t _backlightval;

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04 // Enable bit
#define Rw 0x02 // Read/Write bit
#define Rs 0x01 // Register select bit

uint8_t Play_Symbol[8] = {0x00, 0x10, 0x1C, 0x1F, 0x1F, 0x1C, 0x10, 0x00};
uint8_t Pause_Symbol[8] = {0x00, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00};
uint8_t STOP_Symbol[8] = {0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00};
uint8_t Next_Symbol[8] = {0x00, 0x00, 0x12, 0x1B, 0x1B, 0x12, 0x00, 0x00};
uint8_t Prev_Symbol[8] = {0x00, 0x00, 0x09, 0x1B, 0x1B, 0x09, 0x00, 0x00};
uint8_t LLoop_Symbol[8] = {0x00, 0x0F, 0x10, 0x10, 0x10, 0x12, 0x0F, 0x02};
uint8_t RLoop_Symbol[8] = {0x08, 0x1E, 0x09, 0x01, 0x01, 0x01, 0x1E, 0x00};
uint8_t Select_Symbol[8] = {0x00, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00, 0x00};
uint8_t Selected_Symbol[8] = {0x00, 0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00};

void expanderWrite(uint8_t _data) { i2c__write_no_addr(i2c_line, _Addr, (_data) | _backlightval); }

void pulseEnable(uint8_t _data) {
  expanderWrite(_data | En);  // En high
  expanderWrite(_data & ~En); // En low
}

void write4bits(uint8_t data) {
  expanderWrite(data);
  pulseEnable(data);
}

void send(uint8_t data, uint8_t mode) {
  uint8_t highnib = data & 0xf0;
  uint8_t lownib = (data << 4) & 0xf0;
  write4bits((highnib) | mode);
  write4bits((lownib) | mode);
}

void command(uint8_t data) { send(data, 0); }
void test_command(uint8_t data) { i2c__write_no_addr(i2c_line, _Addr, data); }

void LCD_init(uint8_t addr) {
  _Addr = addr;
  _displayfunction |= LCD_2LINE;
  delay__ms(50);

  // put the LCD into 4 bit mode
  // this is according to the hitachi HD44780 datasheet
  // figure 24, pg 46

  // we start in 8bit mode, try to set 4 bit mode
  write4bits(0x03 << 4);
  delay__ms(5); // wait min 4.1ms

  // second try
  write4bits(0x03 << 4);
  delay__ms(5); // wait min 4.1ms

  // third go!
  write4bits(0x03 << 4);
  delay__ms(1);

  // finally, set to 4-bit interface
  write4bits(0x02 << 4);

  // set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for roman languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

  home();

  load_Custom_Ch();
#if backlight_en
  backlight();
#endif
}

void clear() {
  command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  delay__ms(2);              // this command takes a long time!
}

void home() {
  command(LCD_RETURNHOME); // set cursor position to zero
  delay__ms(2);            // this command takes a long time!
}

void setCursor(uint8_t row, uint8_t col) {
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > _numlines) {
    row = _numlines - 1; // we count rows starting w/0
  }
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft() { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT); }
void scrollDisplayRight() { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT); }

// This is for text that flows Left to Right
void leftToRight() {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft() {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll() {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll() {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Turn the (optional) backlight off/on
void noBacklight(void) {
  _backlightval = LCD_NOBACKLIGHT;
  expanderWrite(0);
}

void backlight(void) {
  _backlightval = LCD_BACKLIGHT;
  expanderWrite(0);
}

void LCD_print_ch(char ch) { send(ch, Rs); }

void print_ch_at(char ch, uint8_t row, uint8_t col) {
  setCursor(row, col);
  LCD_print_ch(ch);
}

void createChar(uint8_t location, uint8_t *charmap) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    LCD_print_ch(charmap[i]);
  }
  home();
}

void LCD_printf(char *ch_str, int size) {
  if (size > _cols) {
    size = _cols;
  }
  for (int i = 0; i < size; i++) {
    LCD_print_ch(ch_str[i]);
  }
}

void load_Custom_Ch() {
  createChar(STOP_SYM, STOP_Symbol);
  // createChar(Pause_SYM, Pause_Symbol);
  createChar(Next_SYM, Next_Symbol);
  createChar(Prev_SYM, Prev_Symbol);
  createChar(LLoop_SYM, LLoop_Symbol);
  createChar(RLoop_SYM, RLoop_Symbol);
  createChar(Select_SYM, Select_Symbol);
}

void SW_to_Play() { createChar(Play_SYM, Play_Symbol); }

void SW_to_Pause() { createChar(Pause_SYM, Pause_Symbol); }

void SW_to_STOP() { createChar(STOP_SYM, STOP_Symbol); }

void SW_to_Select() { createChar(Select_SYM, Select_Symbol); }

void SW_to_Selected() { createChar(Select_SYM, Selected_Symbol); }

void Space_Printf(int num) {
  for (int i = 0; i < num; i++) {
    LCD_print_ch(' ');
  }
}

void LCD_num_print(int num) {
  char str_int[12];
  sprintf(str_int, "%i", num);
  if (num < 10) {
    LCD_printf(str_int, 1);
  } else if (num < 100) {
    LCD_printf(str_int, 2);
  } else if (num < 1000) {
    LCD_printf(str_int, 3);
  } else if (num < 10000) {
    LCD_printf(str_int, 4);
  }
}

void LCD_num_R_print(int num, int fixed_space) {
  char str_int[12];
  int neg = 0;
  if (num < 0) {
    neg = 1;
  }
  sprintf(str_int, "%i", num);
  if (num < 10 && num > -10) {
    Space_Printf(fixed_space - 1 - neg);
    LCD_printf(str_int, 1 + neg);
  } else if (num < 100 && num > -100) {
    Space_Printf(fixed_space - 2 - neg);
    LCD_printf(str_int, 2 + neg);
  } else if (num < 1000 && num > -1000) {
    Space_Printf(fixed_space - 3 - neg);
    LCD_printf(str_int, 3 + neg);
  } else if (num < 10000 && num > -10000) {
    Space_Printf(fixed_space - 4 - neg);
    LCD_printf(str_int, 4 + neg);
  }
}

void LCD_S_name_printf(char *s_name) {
  int count = 0;
  for (int i = 0; i < 20; i++) {
    if (s_name[i] == '.') {
      break;
    }
    count++;
  }
  LCD_printf(s_name, count);
}