#include "R_Status.h"
#include "R_MP3.h"
#include <stdbool.h>
#include <stdio.h>

static bool playing;
// static bool stop;
static int index;
static int prev;
static Play_Mode play_mode;
static States state;
static Page_Mode page;
static int total;

void machine_state_init() {
  playing = false;
  index = 0;
  state = Welcome_STATUS;
  play_mode = List_Loop;
  page = Play_Page_Mode;
  total = song_list__get_item_count();
  prev = total - 1;
}

bool get_playing() { return playing; }
int get_index() { return index; }
int get_prev() { return prev; }
Play_Mode get_play_mode() { return play_mode; };
States get_states() { return state; };
Page_Mode get_page_mode() { return page; }
int get_total() { return total; };

void set_playing(bool val) { playing = val; }
void set_play_mode(Play_Mode val) { play_mode = val; };
void set_states(States val) { state = val; };
void set_page_mode(Page_Mode val) { page = val; };
// void set_total(int val) { total = val; };
// void set_prev(int val) { prev = val; }

void set_index(int val) {
  index = val;
  Save_index(index);
}

int next_song() {
  prev = index++;
  if (index == get_total()) {
    index = 0;
  }
  Save_index(index);
  return index;
}

int prev_song() {
  if (prev == 0) {
    prev = get_total() - 1;
    index = 0;
  } else {
    index = prev--;
  }
  Save_index(index);
  return index;
}

int random_next() {
  prev = index;
  srand(xTaskGetTickCount());
  index = rand() % total;
  Save_index(index);
  return index;
}
