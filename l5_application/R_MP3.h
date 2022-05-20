#pragma once

#include "ff.h"
#include "l3_drivers/gpio.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h> // size_t
#include <stdint.h>

// Function for Song list
typedef char song_memory_t[64];
#if 0
static song_memory_t list_of_songs[32];
static size_t number_of_songs;
#endif
void song_list__populate(void);
size_t song_list__get_item_count(void);
const char *song_list__get_name_for_item(size_t item_number);
bool song_list_find_by_name(char *s_name, char *Song);

// Function for playing the song
#define Size_Data_Block 512
typedef uint8_t mp3_data_blocks_s[Size_Data_Block]; // Align data size to the way data is read from the SD card
QueueHandle_t Butt_Queues[7];

// MP3 Decoder Instruction
#define VS_WRITE_COMMAND 0x02
#define VS_READ_COMMAND 0x03

// MP3 Decoder Address
#define SPI_MODE 0x0
#define SPI_STATUS 0x1
#define SPI_BASS 0x2
#define SPI_CLOCKF 0x3
#define SPI_DECODE_TIME 0x4
#define SPI_AUDATA 0x5
#define SPI_WRAM 0x6
#define SPI_WRAMADDR 0x7
#define SPI_HDAT0 0x8
#define SPI_HDAT1 0x9
#define SPI_AIADDR 0xa
#define SPI_VOL 0xb
#define SPI_AICTRL0 0xc
#define SPI_AICTRL1 0xd
#define SPI_AICTRL2 0xe
#define SPI_AICTRL3 0xf
#define SM_DIFF 0x01
#define SM_JUMP 0x02
#define SM_RESET 0x04
#define SM_OUTOFWAV 0x08
#define SM_PDOWN 0x10
#define SM_TESTS 0x20
#define SM_STREAM 0x40
#define SM_PLUSV 0x80
#define SM_DACT 0x100
#define SM_SDIORD 0x200
#define SM_SDISHARE 0x400
#define SM_SDINEW 0x800
#define SM_ADPCM 0x1000
#define SM_ADPCM_HP 0x2000

// Address
#define Save_Bass 0x00000000
#define Save_Treble 0x00000001
#define Save_Play_index 0x00000003
#define Save_Play_total 0x00000004

void mp3_init(void);                     // init all thing that mp3 related
void mp3_Song_to_Queue(char *s_name);    // Producer for Send the Song Name
void mp3_Song_From_Queue(void *p);       // Consumer for Kick off Data Transfer
void mp3_Play_Data_Send(FIL *file_p);    // Producer for Send the Data Block
void mp3_Play_Data_Get(void *p);         // Consumer for Send data block to MP3 Decoder
void mp3_Send_to_Decoder(uint8_t *buff); // Send to MP3 Decoder
void pin_cs(gpio_s pin_cs);
void pin_ds(gpio_s pin_ds);
void ssp0_mp3_write_single(uint8_t addr, uint8_t data1, uint8_t data2);
void ssp0_mp3_read_single(uint8_t addr, uint8_t *data1, uint8_t *data2);
void mp3_Reset();
void mp3_SoftReset();
void mp3_setStreamMode();
bool DREQ_Wait();
void mp3_set_Vol(uint8_t Vol);
void Flash_Write(uint32_t addr, uint8_t data);
uint8_t Flash_Read(uint32_t addr);
void Flash_Erase(uint32_t addr);
uint8_t Flash_Read_ID();
uint8_t Flash_Write_Status();
void Save_BT(uint8_t Treble_t, uint8_t Bass_t);
void Save_index(int s_index);
int Read_index();
