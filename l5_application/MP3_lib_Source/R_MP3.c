#include "R_MP3.h"
#include "R_CTRL.h"
#include "R_LCD.h"
// Standard Lib
#include <string.h>

// Custom Lib
#include "delay.h"
#include "ff.h"
#include "l3_drivers/gpio.h"
#include "lpc40xx.h"
#include "semphr.h"
#include "ssp0_mp3.h"
#include "stdio.h"

#define mp3_debugging_mode 1

static song_memory_t list_of_songs[99];
static size_t number_of_songs;

#if 1
static gpio_s XCS = {2, 0};
static gpio_s XDCS = {2, 1};
static gpio_s DREQ = {2, 2};
static gpio_s XRESET = {2, 4};
#endif

#if 0
static gpio_s XCS = {2, 0};
static gpio_s XDCS = {2, 1};
static gpio_s DREQ = {2, 5};
static gpio_s XRESET = {2, 2};
#endif

void pin_cs(gpio_s pin_cs) {
  gpio__set_as_output(pin_cs);
  gpio__reset(pin_cs);
}

void pin_ds(gpio_s pin_ds) {
  gpio__set_as_output(pin_ds);
  gpio__set(pin_ds);
}

void ssp0_mp3_write_single(uint8_t addr, uint8_t data1, uint8_t data2) {
  pin_ds(XDCS);
  pin_cs(XCS);
  ssp0__exchange_byte(VS_WRITE_COMMAND);
  ssp0__exchange_byte(addr);
  ssp0__exchange_byte(data1);
  ssp0__exchange_byte(data2);
  pin_ds(XCS);
}

void ssp0_mp3_read_single(uint8_t addr, uint8_t *data1, uint8_t *data2) {
  pin_ds(XDCS);
  pin_cs(XCS);
  ssp0__exchange_byte(VS_READ_COMMAND);
  ssp0__exchange_byte(addr);
  *data1 = ssp0__exchange_byte(0xFF);
  *data2 = ssp0__exchange_byte(0xFF);
  pin_ds(XCS);
}

static void song_list__handle_filename(const char *filename) {
  // This will not work for cases like "file.mp3.zip"
  if (NULL != strstr(filename, ".mp3")) {
    // printf("Filename: %s\n", filename);

    // Dangerous function: If filename is > 128 chars, then it will copy extra bytes leading to memory corruption
    // strcpy(list_of_songs[number_of_songs], filename);

    // Better: But strncpy() does not guarantee to copy null char if max length encountered
    // So we can manually subtract 1 to reserve as NULL char
    strncpy(list_of_songs[number_of_songs], filename, sizeof(song_memory_t) - 1);

    // Best: Compensates for the null, so if 128 char filename, then it copies 127 chars, AND the NULL char
    // snprintf(list_of_songs[number_of_songs], sizeof(song_memory_t), "%.149s", filename);

    ++number_of_songs;
    // or
    // number_of_songs++;
  }
}

void song_list__populate(void) {
  FRESULT res;
  static FILINFO file_info;
  const char *root_path = "/";

  DIR dir;
  res = f_opendir(&dir, root_path);

  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &file_info); /* Read a directory item */
      if (res != FR_OK || file_info.fname[0] == 0) {
        break; /* Break on error or end of dir */
      }

      if (file_info.fattrib & AM_DIR) {
        /* Skip nested directories, only focus on MP3 songs at the root */
      } else { /* It is a file. */
        song_list__handle_filename(file_info.fname);
      }
    }
    f_closedir(&dir);
  }
}

size_t song_list__get_item_count(void) { return number_of_songs; }

const char *song_list__get_name_for_item(size_t item_number) {
  const char *return_pointer = "";

  if (item_number >= number_of_songs) {
    return_pointer = "";
  } else {
    return_pointer = list_of_songs[item_number];
  }

  return return_pointer;
}

bool song_list_find_by_name(char *s_name, char *Song) {
  for (size_t song_number = 0; song_number < song_list__get_item_count(); song_number++) {
    if ((strcmp(song_list__get_name_for_item(song_number), s_name) == 0) |
        (strstr(song_list__get_name_for_item(song_number), s_name) != NULL)) {
      strcpy(Song, song_list__get_name_for_item(song_number));
      return true;
    }
  }
  return false;
}

static QueueHandle_t Song_Quene;
static QueueHandle_t MP3_Data_Queue;

void mp3_SoftReset() {
  ssp0_mp3_write_single(SPI_MODE, 0x08, 0x04);
  delay__ms(1);
  while (!DREQ_Wait()) {
  };
  ssp0_mp3_write_single(SPI_MODE, 0x48, 0x00);
  ssp0_mp3_write_single(SPI_CLOCKF, 0x60, 0x00);
  ssp0_mp3_write_single(SPI_AUDATA, 0xBB, 0x81);
  ssp0_mp3_write_single(SPI_BASS, 0x00, 0x55);
  mp3_set_Vol(0x20);
  delay__ms(1);
  pin_cs(XDCS);
  ssp0__exchange_byte(0x00);
  ssp0__exchange_byte(0x00);
  ssp0__exchange_byte(0x00);
  ssp0__exchange_byte(0x00);
  pin_ds(XDCS);
}

void mp3_Reset() {
  pin_cs(XRESET);
  delay__ms(200);
  ssp0__exchange_byte(0xff);
  pin_ds(XCS);
  pin_ds(XDCS);
  pin_ds(XRESET);
  delay__ms(200);
  while (!DREQ_Wait()) {
  };
  delay__ms(200);
  mp3_SoftReset();
}

void mp3_setStreamMode() {
  uint8_t data[2];
  uint8_t Stream_Mode_Mask = (1 << 6);
  uint8_t Test_Mode_Mask = (1 << 5);
  ssp0_mp3_read_single(0x00, &data[0], &data[1]);
  data[1] &= ~Test_Mode_Mask;
  data[1] |= Stream_Mode_Mask;
  ssp0_mp3_write_single(0x00, data[0], data[1]);
}

void mp3_init(void) {
  song_list__populate();

  current_machine_state.playing = false;
  current_machine_state.index = 0;
  current_machine_state.states = Welcome_STATUS;
  current_machine_state.in_Menu = false;
  current_machine_state.in_Song_list = false;
  current_machine_state.total = song_list__get_item_count();

  printf("\nSong list in SD Card root:\n");
  for (size_t song_number = 0; song_number < current_machine_state.total; song_number++) {
    printf("Song %2d: %s\n", (1 + song_number), song_list__get_name_for_item(song_number));
  }
  printf("\n");
  ssp0__initialize(1);
  ssp0__pin_init();
  mp3_Reset();
  ssp0__exchange_byte(0x00);
  mp3_set_Vol(0x20);
  ssp0_mp3_write_single(SPI_MODE, 0x48, 0x00);
  ssp0_mp3_write_single(SPI_CLOCKF, 0x60, 0x00); // Set Clock
  ssp0_mp3_write_single(SPI_AUDATA, 0xBB, 0x81); // Set Frequcy
  ssp0_mp3_write_single(SPI_BASS, 0x01, 0x08);   // Set Bass and Treble

#if mp3_debugging_mode
  uint8_t data[2];
  printf("MP3 Decoder Reg: \n");
  for (int i = 0; i < 16; i++) {
    ssp0_mp3_read_single(i, &data[0], &data[1]);
    printf("MP3 Reg %02X: %02X%02X\n", i, data[0], data[1]);
    delay__ms(10);
  }
#endif
  // VsSineTest();
  // ssp0_mp3_write_single(SPI_MODE, 0x00, 0x40);
  Song_Quene = xQueueCreate(1, sizeof(song_memory_t));
  MP3_Data_Queue = xQueueCreate(2, sizeof(mp3_data_blocks_s));
}

void mp3_Song_to_Queue(char *s_name) { xQueueSend(Song_Quene, s_name, portMAX_DELAY); }

void mp3_Song_From_Queue(void *p) {
  song_memory_t Song_Name;
  FIL file_handle;
  FRESULT Result;
  while (1) {
    xQueueReceive(Song_Quene, Song_Name, portMAX_DELAY);
    Result = f_open(&file_handle, Song_Name, (FA_READ));
    // Result = f_open(&file_handle, "test.txt", (FA_READ));
    if (Result == FR_OK) {
      // play_file(file_handle);
      // printf("File %s found\n", Song_Name);
      current_machine_state.playing = true;
      current_machine_state.states = Play_STATUS;
      mp3_Play_Data_Send(&file_handle);
      f_close(&file_handle);
      current_machine_state.playing = false;
      printf("\nA Song has end\n");
    } else {
      printf("Unable to open and read file: %s\n", Song_Name);
      vTaskDelay(1000);
    }
  }
}

void mp3_Play_Data_Send(FIL *file_p) {
  mp3_data_blocks_s buff;
  UINT U_size;
  bool pressed;
  bool skiped = false;
  printf("\nA new Song Start Playing\n");
  while (f_read(file_p, &buff, 512, &U_size) == FR_OK) {
    xQueueSend(MP3_Data_Queue, &buff, portMAX_DELAY);

    if ((uxQueueMessagesWaiting(Song_Quene) > 0) || (xQueueReceive(Butt_Queues[Butt_DOWN], &pressed, 0))) {
      printf("\nA Song has been skip\n");
      skiped = true;
    }
    if (xQueueReceive(Butt_Queues[Butt_PLAY], &pressed, 0)) {
      current_machine_state.states = Pause_STATUS;
      fprintf(stderr, "Song Has Paused\n");
      while (!xQueueReceive(Butt_Queues[Butt_PLAY], &pressed, 100)) {
        if ((uxQueueMessagesWaiting(Song_Quene) > 0) || (xQueueReceive(Butt_Queues[Butt_DOWN], &pressed, 0))) {
          printf("\nA Song has been skip\n");
          skiped = true;
          break;
        }
      }
      current_machine_state.states = Play_STATUS;
      fprintf(stderr, "Song Has Resumed\n");
    }
    if (skiped || f_eof(file_p)) {
      break;
    }
  }
}

void mp3_Play_Data_Get(void *p) {
  mp3_data_blocks_s buff;
  while (1) {
    if (xQueueReceive(MP3_Data_Queue, &buff, portMAX_DELAY)) {
      mp3_Send_to_Decoder(buff);
    }
  }
}

/*
This is for MP3 Decoder
*/

void mp3_Send_to_Decoder(uint8_t *buff) {

  pin_ds(XCS);
  for (int i = 0; i < 512; i++) {
    while (!DREQ_Wait()) {
    }
    pin_cs(XDCS);
    ssp0__exchange_byte(buff);
    pin_ds(XDCS);
  }
}

bool DREQ_Wait() {
  gpio__set_as_input(DREQ);
  return gpio__get(DREQ);
}

void mp3_set_Vol(uint8_t Vol) { ssp0_mp3_write_single(SPI_VOL, Vol, Vol); }