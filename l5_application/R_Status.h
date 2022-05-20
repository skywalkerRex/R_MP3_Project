#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Page Mode enum
typedef enum Page_Mode { Menu_Mode, Song_list_Mode, Play_Page_Mode } Page_Mode;

// Play Mode Define
typedef enum Play_Mode { Single_Play, Single_Loop, List_Loop, Random_Play } Play_Mode;

// states Define
typedef enum States { Welcome_STATUS, Play_STATUS, Stop_STATUS, Pause_STATUS } States;

void machine_state_init();

bool get_playing();
int get_index();
int get_prev();
Page_Mode get_page_mode();
Play_Mode get_play_mode();
States get_states();
int get_total();

void set_playing(bool val);
void set_states(States val);
void set_play_mode(Play_Mode val);
void set_page_mode(Page_Mode val);

// void set_prev(int val);
// void set_total(int val);

int next_song();
int prev_song();
int random_next();
void set_index(int val);