#include "R_CTRL.h"
#include "FreeRTOS.h"
#include "R_LCD.h"
#include "R_MP3.h"
#include "R_Status.h"
#include "l3_drivers/adc.h"
#include "l3_drivers/gpio.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl_debug_prt 0

static void welcome_page();
static void play_page();
static void stop_page();
static void pause_page();
static void Line2_Play_line();
static void Line3_Vol_line(int vol);
static void Play_Page_Parent();

static gpio_s Butt_list[7] = {{1, 29}, {0, 9}, {1, 23}, {0, 25}, {1, 30}, {0, 7}, {1, 14}};

static void butt_construct(gpio_s butt) {
  gpio__set_as_input(butt);
  gpio__enable_pull_down_resistors(butt);
}

static void pin_config_adc() {
  LPC_IOCON->P1_31 &= ~0b111;
  LPC_IOCON->P1_31 |= 0b011;
  LPC_IOCON->P1_31 &= ~(1 << 7);
}

static void Line2_Play_line() {
  // Line 2
  print_ch_at(Play_SYM, 2, 9);
  print_ch_at(Next_SYM, 2, 12);
  print_ch_at(Prev_SYM, 2, 6);
  print_ch_at(LLoop_SYM, 2, 0);

  if (get_play_mode() == Random_Play) {
    SW_to_Rand();
    print_ch_at('X', 2, 1);
  } else {
    SW_to_Loop();
    if (get_play_mode() == List_Loop) {
      print_ch_at(' ', 2, 1);
    } else if (get_play_mode() == Single_Loop) {
      print_ch_at('1', 2, 1);
    } else {
      print_ch_at('/', 2, 1);
    }
  }

  print_ch_at(RLoop_SYM, 2, 2);
}

static void welcome_page() {
  // Line 0
  clear();
  setCursor(0, 5);
  LCD_printf("Rex's MP3", 9);
#if ctrl_debug_prt
  Save_BT(0x01, 0x08);
  setCursor(1, 0);
  LCD_hex_print(Flash_Read(Save_Bass));
  setCursor(1, 5);
  LCD_hex_print(Flash_Read(Save_Treble));
  setCursor(1, 10);
  LCD_hex_print(Flash_Write_Status());
  setCursor(1, 15);
  LCD_hex_print(Flash_Read_ID());
#endif
  Line2_Play_line();
}

static void Line3_Vol_line(int vol) {
  // Song Total Handel
  setCursor(3, 0);
  LCD_printf("Song:", 5);
  if (get_states() != Welcome_STATUS) {
    LCD_num_R_print(get_index() + 1, 3);
    LCD_print_ch('/');
  }
  LCD_num_print(get_total());

  setCursor(3, 13);
  LCD_printf("Vol:", 4);
  LCD_num_R_print(vol, 3);
}

static int vol_scan() {
  int adc_value, vol;
  adc_value = adc__get_adc_value(ADC__CHANNEL_5);
  vol = ((double)(4095 - adc_value) / 4095.0) * 254;
  mp3_set_Vol(vol);
  return (int)(((double)(254 - vol) / 254.0) * 100.0);
}

static void Play_Page_Parent() {
  int count = 0;
  int center = 0;
  char *name = (char *)song_list__get_name_for_item(get_index());
  clear();
  //
  // Line 0 missing
  //
  for (int i = 0; i < 20; i++) {
    if (name[i] == '.') {
      break;
    }
    count++;
  }
  center = (20 - count) / 2;
  setCursor(1, center);
  LCD_printf(name, count);
  Line2_Play_line();
  // Line3
}

static void play_page() {
  SW_to_Pause();
  Play_Page_Parent();
  setCursor(0, 6);
  LCD_printf("Playing", 7);
}

static void stop_page() {
  SW_to_STOP();
  Play_Page_Parent();
  setCursor(0, 8);
  LCD_printf("STOP", 4);
}

static void pause_page() {
  SW_to_Play();
  Play_Page_Parent();
  setCursor(0, 7);
  LCD_printf("Paused", 6);
}

typedef struct Menu_Option {
  char name[18];
  int length;
  int value;
} Menu_Option;

#define Menu_len 4
static Menu_Option Options[Menu_len];

static void Menu_init() {
  strcpy(Options[0].name, "Bass Amp");
  Options[0].length = 8;
  strcpy(Options[1].name, "Bass Freq");
  Options[1].length = 9;
  strcpy(Options[2].name, "Treble Amp");
  Options[2].length = 10;
  strcpy(Options[3].name, "Treble Freq");
  Options[3].length = 11;
  // strcpy(Options[4].name, "Reset");
  // Options[3].length = 5;
}

static int Menu_page(int select) {
  clear();
  setCursor(0, 8);
  LCD_printf("Menu", 4);
  uint8_t Bass, Treble;
  ssp0_mp3_read_single(SPI_BASS, &Treble, &Bass);
  Options[0].value = (int)((Treble >> 4) & 0x0F) - 8;
  Options[1].value = Treble & 0x0F;
  Options[2].value = (Bass >> 4) & 0x0F;
  Options[3].value = Bass & 0x0F;

  if (select >= Menu_len) {
    select = 0;
  } else if (select < 0) {
    select = Menu_len - 1;
  }

  if (select == 0) {
    print_ch_at(Select_SYM, 1, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_printf(Options[select - 1 + i].name, Options[select - 1 + i].length);
      setCursor(i, 16);
      LCD_num_R_print(Options[select - 1 + i].value, 3);
    }
  } else if (select == Menu_len - 1) {
    print_ch_at(Select_SYM, 3, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_printf(Options[select - 3 + i].name, Options[select - 3 + i].length);
      setCursor(i, 16);
      LCD_num_R_print(Options[select - 3 + i].value, 3);
    }
  } else {
    print_ch_at(Select_SYM, 2, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_printf(Options[select - 2 + i].name, Options[select - 2 + i].length);
      setCursor(i, 16);
      LCD_num_R_print(Options[select - 2 + i].value, 3);
    }
  }

  return select;
}

static void menu_mod(int select, bool add) {
  uint8_t Bass, Treble, Temp;
  if (select >= 0 && select < 4) {
    ssp0_mp3_read_single(SPI_BASS, &Treble, &Bass);
    if (select == 0) {
      Temp = (Treble >> 4) & 0x0F;
      if (add && Temp < 0x0F) {
        Temp++;
      } else if (!add && Temp > 0) {
        Temp--;
      }
      Treble = (Treble & 0x0F) | (Temp << 4);
      setCursor(1, 16);
      LCD_num_R_print(Temp - 8, 3);
    } else if (select == 1) {
      Temp = (Treble)&0x0F;
      if (add && Temp < 0x0F) {
        Temp++;
      } else if (!add && Temp > 1) {
        Temp--;
      }
      Treble = (Treble & 0xF0) | (Temp);
      setCursor(2, 16);
      LCD_num_R_print(Temp, 3);
    } else if (select == 2) {
      Temp = (Bass >> 4) & 0x0F;
      if (add && Temp < 0x0F) {
        Temp++;
      } else if (!add && Temp > 0) {
        Temp--;
      }
      Bass = (Bass & 0x0F) | (Temp << 4);
      setCursor(2, 16);
      LCD_num_R_print(Temp, 3);
    } else if (select == 3) {
      Temp = (Bass)&0x0F;
      if (add && Temp < 0x0F) {
        Temp++;
      } else if (!add && Temp > 2) {
        Temp--;
      }
      Bass = (Bass & 0xF0) | (Temp);
      setCursor(3, 16);
      LCD_num_R_print(Temp, 3);
    }
    ssp0_mp3_write_single(SPI_BASS, Treble, Bass);
    Save_BT(Treble, Bass);
    // Flash_Write(Save_Bass, Bass);
    // Flash_Write(Save_Treble, Treble);
  }
}

static int Song_list_page(int select) {
  if (select >= get_total()) {
    select = 0;
  } else if (select < 0) {
    select = get_total() - 1;
  }
  clear();
  SW_to_Select();
  setCursor(0, 5);
  LCD_printf("Song List", 9);
  if (select == 0) {
    print_ch_at(Select_SYM, 1, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_S_name_printf((char *)song_list__get_name_for_item(select - 1 + i));
    }
  } else if (select == get_total() - 1) {
    print_ch_at(Select_SYM, 3, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_S_name_printf((char *)song_list__get_name_for_item(select - 3 + i));
    }
  } else {
    print_ch_at(Select_SYM, 2, 0);
    for (int i = 1; i <= 3; i++) {
      setCursor(i, 1);
      LCD_S_name_printf((char *)song_list__get_name_for_item(select - 2 + i));
    }
  }

  return select;
}

static void Play_Current() { mp3_Song_to_Queue((char *)song_list__get_name_for_item(get_index())); }

static void Play_Next() {
  if (get_play_mode() == Random_Play) {
    mp3_Song_to_Queue((char *)song_list__get_name_for_item(random_next()));
  } else {
    mp3_Song_to_Queue((char *)song_list__get_name_for_item(next_song()));
  }
  vTaskDelay(100);
}

static void Play_Prev() {
  mp3_Song_to_Queue((char *)song_list__get_name_for_item(prev_song()));
  vTaskDelay(100);
}

static void Play_STOP() { set_states(Stop_STATUS); }

// Public Function

void key_board_init() {
  adc__initialize();
  Menu_init();
  // adc__enable_burst_mode();
  pin_config_adc();
  for (int i = 0; i < 7; i++) {
    butt_construct(Butt_list[i]);
    Butt_Queues[i] = xQueueCreate(1, sizeof(bool));
  }
}

void key_board(void *p) {
  bool pressed = true;
  int vol_int;
  int prev_vol = -1;
  int prev_page = -1;
  int prev_index = -1;
  int select = 0;
  bool selected = false;
  while (1) {
    vTaskDelay(200);
    // Volume Check
    vol_int = vol_scan();
    if (get_in_Menu()) {
      prev_page = -1;
      if (gpio__get(Butt_list[Butt_UP]) && !selected) {
        // UP
        select = Menu_page(--select);
#if ctrl_debug_prt
        fprintf(stderr, "Select: %i \n", select);
#endif
      }
      if (gpio__get(Butt_list[Butt_DOWN]) && !selected) {
        // DOWN
        select = Menu_page(++select);
#if ctrl_debug_prt
        fprintf(stderr, "Select: %i \n", select);
#endif
      }
      if (gpio__get(Butt_list[Butt_LEFT]) && selected) {
        // LEFT
        menu_mod(select, false);
#if ctrl_debug_prt
        fprintf(stderr, "Select: %i \n", select);
#endif
      }
      if (gpio__get(Butt_list[Butt_RIGHT]) && selected) {
        // RIGHT
        menu_mod(select, true);
#if ctrl_debug_prt
        fprintf(stderr, "Select: %i \n", select);
#endif
      }
      if (gpio__get(Butt_list[Butt_PLAY])) {
        // Confirm
        if (selected) {
          SW_to_Select();
          selected = false;
        } else {
          SW_to_Selected();
          selected = true;
        }
      }
      if (gpio__get(Butt_list[Butt_MEUN])) {
        SW_to_Play_Page();
        selected = false;
        SW_to_Select();
      }
      if (gpio__get(Butt_list[Butt_SONG])) {
        // xQueueSend(Butt_Queues[Butt_SONG], &pressed, 0);
        SW_to_Select();
        selected = false;
        SW_to_Song_List();
        select = Song_list_page(get_index());
      }
    } else if (get_in_Song_list()) {
      prev_page = -1;
      if (gpio__get(Butt_list[Butt_UP])) {
        // UP
        select = Song_list_page(--select);
      }
      if (gpio__get(Butt_list[Butt_DOWN])) {
        // DOWN
        select = Song_list_page(++select);
      }
      /* not used
      if (gpio__get(Butt_list[Butt_LEFT])) {
        // LEFT
      }
      if (gpio__get(Butt_list[Butt_RIGHT])) {
        // RIGHT
      }*/
      if (gpio__get(Butt_list[Butt_PLAY])) {
        // Confirm
        SW_to_Play_Page();
        set_index(select);
        Play_Current();
      }
      if (gpio__get(Butt_list[Butt_MEUN])) {
        SW_to_Menu();
        selected = false;
        select = Menu_page(0);
      }
      if (gpio__get(Butt_list[Butt_SONG])) {
        SW_to_Play_Page();
      }
    } else {
      if (gpio__get(Butt_list[Butt_UP])) {
        switch (get_play_mode()) {
        case 0:
          set_play_mode(1);
          Line2_Play_line();
          break;

        case 1:
          set_play_mode(2);
          Line2_Play_line();
          break;

        case 2:
          set_play_mode(3);
          Line2_Play_line();
          break;

        case 3:
          set_play_mode(0);
          Line2_Play_line();
          break;

        default:
          break;
        }
        // xQueueSend(Butt_Queues[Butt_UP], &pressed, 0);
      }
      if (gpio__get(Butt_list[Butt_DOWN])) {
        Play_STOP();
        xQueueSend(Butt_Queues[Butt_DOWN], &pressed, 0);
      }
      if (gpio__get(Butt_list[Butt_LEFT])) {
        // xQueueSend(Butt_Queues[Butt_LEFT], &pressed, 0);
        Play_Prev();
      }
      if (gpio__get(Butt_list[Butt_RIGHT])) {
        // xQueueSend(Butt_Queues[Butt_RIGHT], &pressed, 0);
        Play_Next();
      }
      if (gpio__get(Butt_list[Butt_PLAY])) {
        if (get_playing()) {
          xQueueSend(Butt_Queues[Butt_PLAY], &pressed, 0);
        } else {
          Play_Current();
        }
      }
      if (gpio__get(Butt_list[Butt_MEUN])) {
        // xQueueSend(Butt_Queues[Butt_MEUN], &pressed, 0);
        SW_to_Menu();
        selected = false;
        select = Menu_page(0);
      }
      if (gpio__get(Butt_list[Butt_SONG])) {
        // xQueueSend(Butt_Queues[Butt_SONG], &pressed, 0);
        SW_to_Song_List();
        select = Song_list_page(get_index());
      }

      // Volume Handle 0~4096 for adc 0x0 ~ 0xFE for Vol
      if (prev_vol != vol_int) {
        prev_vol = vol_int;
        Line3_Vol_line(vol_int);
      }

      // Page Handle
      if ((!get_in_Menu()) && ((prev_page != get_states()) || (prev_index != get_index()))) {
        prev_page = get_states();
        prev_index = get_index();
        switch (get_states()) {
        case Welcome_STATUS:
          welcome_page();
          break;

        case Play_STATUS:
          play_page();
          break;

        case Pause_STATUS:
          pause_page();
          break;

        case Stop_STATUS:
          stop_page();
          break;

        default:
          break;
        }
        Line3_Vol_line(vol_int);
      }
    }

    // PlayMode Handle
    if (get_states() == Play_STATUS && (!get_playing())) {
      if (get_play_mode() == Single_Loop) {
        Play_Current();
      } else if (get_play_mode() == List_Loop) {
        Play_Next();
      } else if (get_play_mode() == Random_Play) {
        Play_Next();
      } else {
        SW_to_STOP();
        Play_STOP();
      }
    }
  }
}