#include "FreeRTOS.h"
#include "R_MP3.h"
#include "cli_handlers.h"
#include "task.h"
#include "uart_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cli__task_list_print(sl_string_s user_input_minus_command_name, app_cli__print_string_function cli_output);

app_cli_status_e cli__uart3_transmit(app_cli__argument_t argument, sl_string_s user_input_minus_command_name,
                                     app_cli__print_string_function cli_output) {
  uart_puts(UART__3, sl_string__c_str(user_input_minus_command_name));

  cli_output(NULL, "Output to UART3: ");
  cli_output(NULL, sl_string__c_str(user_input_minus_command_name));
  cli_output(NULL, "\r\n");

  return APP_CLI_STATUS__SUCCESS;
}

app_cli_status_e cli__crash_me(app_cli__argument_t argument, sl_string_s user_input_minus_command_name,
                               app_cli__print_string_function cli_output) {
  uint32_t *bad_pointer = (uint32_t *)0x00000001;
  *bad_pointer = 0xDEADBEEF;
  return APP_CLI_STATUS__SUCCESS;
}

app_cli_status_e cli__task_list(app_cli__argument_t argument, sl_string_s user_input_minus_command_name,
                                app_cli__print_string_function cli_output) {
  const int sleep_time = sl_string__to_int(user_input_minus_command_name);
  if (sleep_time > 0) {
    vTaskResetRunTimeStats();
    vTaskDelay(sleep_time);
  }

  // re-use user_input_minus_command_name as 'output_string' to save memory:
  sl_string_s output_string = user_input_minus_command_name;
  cli__task_list_print(output_string, cli_output);

  return APP_CLI_STATUS__SUCCESS;
}

app_cli_status_e cli__task_ctl(app_cli__argument_t argument, sl_string_s user_input_minus_command_name,
                               app_cli__print_string_function cli_output) {
  sl_string_s s = user_input_minus_command_name;
  if (sl_string__begins_with_ignore_case(s, "suspend")) {
    sl_string__erase_first_word(s, ' ');
    TaskHandle_t task_handle = xTaskGetHandle(s.cstring);
    if (NULL == task_handle) {
      sl_string__insert_at(s, 0, "Could not find a task with name:");
      cli_output(NULL, s.cstring);
    } else {
      vTaskSuspend(task_handle);
    }

  } else if (sl_string__begins_with_ignore_case(s, "resume")) {
    sl_string__erase_first_word(s, ' ');
    TaskHandle_t task_handle = xTaskGetHandle(s.cstring);
    if (NULL == task_handle) {
      sl_string__insert_at(s, 0, "Could not find a task with name:");
      cli_output(NULL, s.cstring);
    } else {
      vTaskResume(task_handle);
    }
  } else {
    cli_output(NULL, "Did you mean to say suspend or resume?\n");
  }
  return APP_CLI_STATUS__SUCCESS;
}

app_cli_status_e cli__play(app_cli__argument_t argument, sl_string_s user_input_minus_command_name,
                           app_cli__print_string_function cli_output) {
  sl_string_s s = user_input_minus_command_name;
  song_memory_t Song_name;
  int number;
  if (sl_string__begins_with_ignore_case(s, "Name")) {

    sl_string__erase_first_word(s, ' ');

    if (song_list_find_by_name(s.cstring, Song_name)) {
      printf("Now Playing %s\n", Song_name);
      mp3_Song_to_Queue(Song_name);
    } else {
      printf("Song %s unfound please check the name\n", s.cstring);
    }

  } else if (sl_string__begins_with_ignore_case(s, "No")) {
    sl_string__erase_first_word(s, ' ');
    number = atoi(s.cstring) - 1;
    strcpy(Song_name, song_list__get_name_for_item(number));
    // Song_name = song_list__get_name_for_item(number);
    if (strcmp(Song_name, "") != 0) {
      printf("Now Playing %s\n", Song_name);
      mp3_Song_to_Queue(Song_name);
    } else {
      printf("Song Index out of range, only %i songs\n", song_list__get_item_count());
    }

  } else {
    cli_output(NULL, "Play a mp3 music from SD Card.\n"
                     "play Name <Song Name>.mp3' will play the music that match the name.\n"
                     "play No <Index of the Song>.mp3' will play the music that match the index.");
  }
  return APP_CLI_STATUS__SUCCESS;
}

static void cli__task_list_print(sl_string_s output_string, app_cli__print_string_function cli_output) {
  void *unused_cli_param = NULL;

#if (0 != configUSE_TRACE_FACILITY)
  // Enum to char : eRunning, eReady, eBlocked, eSuspended, eDeleted
  static const char *const task_status_table[] = {"running", " ready ", "blocked", "suspend", "deleted"};

  // Limit the tasks to avoid heap allocation.
  const unsigned portBASE_TYPE max_tasks = 10;
  TaskStatus_t status[max_tasks];
  uint32_t total_cpu_runtime = 0;
  uint32_t total_tasks_runtime = 0;

  const uint32_t total_run_time = portGET_RUN_TIME_COUNTER_VALUE();
  const unsigned portBASE_TYPE task_count = uxTaskGetSystemState(&status[0], max_tasks, &total_cpu_runtime);

  sl_string__printf(output_string, "%10s  Status Pr Stack CPU%%          Time\n", "Name");
  cli_output(unused_cli_param, sl_string__c_str(output_string));

  for (unsigned priority_number = 0; priority_number < configMAX_PRIORITIES; priority_number++) {
    /* Print in sorted priority order */
    for (unsigned i = 0; i < task_count; i++) {
      const TaskStatus_t *task = &status[i];
      if (task->uxBasePriority == priority_number) {
        total_tasks_runtime += task->ulRunTimeCounter;

        const unsigned cpu_percent = (0 == total_cpu_runtime) ? 0 : task->ulRunTimeCounter / (total_cpu_runtime / 100);
        const unsigned time_us = task->ulRunTimeCounter;
        const unsigned stack_in_bytes = (sizeof(void *) * task->usStackHighWaterMark);

        sl_string__printf(output_string, "%10s %s %2u %5u %4u %10u us\n", task->pcTaskName,
                          task_status_table[task->eCurrentState], (unsigned)task->uxBasePriority, stack_in_bytes,
                          cpu_percent, time_us);
        cli_output(unused_cli_param, sl_string__c_str(output_string));
      }
    }
  }

  sl_string__printf(output_string, "Overhead: %u uS\n", (unsigned)(total_run_time - total_tasks_runtime));
  cli_output(unused_cli_param, sl_string__c_str(output_string));
#else
  cli_output(unused_cli_param, "Unable to provide you the task information along with their CPU and stack usage.\n");
  cli_output(unused_cli_param, "configUSE_TRACE_FACILITY macro at FreeRTOSConfig.h must be non-zero\n");
#endif
}
