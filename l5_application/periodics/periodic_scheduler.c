#include "periodic_callbacks.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "common_macros.h"

/// Task data structure of each periodic task
typedef struct {
  /// Task uses vTaskDelayUntil() to carry out its periodic callback
  const TickType_t task_delay_in_ticks;

  /// This callback is invoked from the task
  periodic_callbacks_f callback;

  /// Incremented until task_delay_in_ticks and then task_finished_flag is checked for deadline
  TickType_t ticks_elapsed;

  /// After the callback() is invoked, this flag is set which is later checked by periodic_scheduler__task_monitor()
  volatile bool task_finished_flag;
} periodic_scheduler_s;

/// Instances of the 4 periodic tasks and their callback function pointer
static periodic_scheduler_s periodic_scheduler__1Hz = {1000, periodic_callbacks__1Hz};
static periodic_scheduler_s periodic_scheduler__10Hz = {100, periodic_callbacks__10Hz};
static periodic_scheduler_s periodic_scheduler__100Hz = {10, periodic_callbacks__100Hz};
static periodic_scheduler_s periodic_scheduler__1000Hz = {1, periodic_callbacks__1000Hz};

static bool periodic_scheduler__run_1000hz;

/// Common task runner for each periodic task
static void periodic_scheduler__run(periodic_scheduler_s *periodic) {
  TickType_t previous_tick = 0;
  uint32_t callback_counter = 0;

  // Note: the order of the while loop's logic is critical; change with care
  while (true) {
    periodic->callback(callback_counter);
    ++callback_counter;
    periodic->task_finished_flag = true;

    vTaskDelayUntil(&previous_tick, periodic->task_delay_in_ticks);
  }
}

static void periodic_scheduler__1Hz_task(void *param) { periodic_scheduler__run(&periodic_scheduler__1Hz); }
static void periodic_scheduler__10Hz_task(void *param) { periodic_scheduler__run(&periodic_scheduler__10Hz); }
static void periodic_scheduler__100Hz_task(void *param) { periodic_scheduler__run(&periodic_scheduler__100Hz); }
static void periodic_scheduler__1000Hz_task(void *param) { periodic_scheduler__run(&periodic_scheduler__1000Hz); }

static void periodic_scheduler__check_flag(periodic_scheduler_s *periodic_task, TickType_t tick_delay) {
  periodic_task->ticks_elapsed += tick_delay;

  if (periodic_task->ticks_elapsed >= periodic_task->task_delay_in_ticks) {
    periodic_task->ticks_elapsed = 0;

    if (periodic_task->task_finished_flag) {
      periodic_task->task_finished_flag = false;
    } else {
      NVIC_SystemReset();
    }
  }
}
static void periodic_scheduler__task_monitor(void *param) {
  const TickType_t tick_delay = periodic_scheduler__run_1000hz ? 1 : 10;

  // We let all the other tasks run first, and then check their flags
  while (true) {
    vTaskDelay(tick_delay);

    if (periodic_scheduler__run_1000hz) {
      periodic_scheduler__check_flag(&periodic_scheduler__1000Hz, tick_delay);
    }

    periodic_scheduler__check_flag(&periodic_scheduler__100Hz, tick_delay);
    periodic_scheduler__check_flag(&periodic_scheduler__10Hz, tick_delay);
    periodic_scheduler__check_flag(&periodic_scheduler__1Hz, tick_delay);
  }
}

void periodic_scheduler__initialize(void) {
  /* You can modify these stack sizes to suit your needs
   * Generally, you should not need higher than 4k stack size as you should not allocate
   * large data objects on the stack memory
   */

  static StackType_t hz1_stack[4096 / sizeof(StackType_t)];
  static StackType_t hz10_stack[4096 / sizeof(StackType_t)];
  static StackType_t hz100_stack[4096 / sizeof(StackType_t)];
  static StaticTask_t hz1_struct, hz10_struct, hz100_struct;

  // clang-format off
  xTaskCreateStatic(periodic_scheduler__1Hz_task, "1Hz", ARRAY_SIZE(hz1_stack), NULL, PRIORITY_PERIODIC_1HZ, hz1_stack, &hz1_struct);
  xTaskCreateStatic(periodic_scheduler__10Hz_task, "10Hz", ARRAY_SIZE(hz10_stack), NULL, PRIORITY_PERIODIC_10HZ, hz10_stack, &hz10_struct);
  xTaskCreateStatic(periodic_scheduler__100Hz_task, "100Hz", ARRAY_SIZE(hz100_stack), NULL, PRIORITY_PERIODIC_100HZ, hz100_stack, &hz100_struct);
  // clang-format on

  static StackType_t wdt_stack[1024 / sizeof(StackType_t)];
  static StaticTask_t wdt_struct;
  xTaskCreateStatic(periodic_scheduler__task_monitor, "Hz_wdt", ARRAY_SIZE(wdt_stack), NULL, PRIORITY_PERIODIC_MONITOR,
                    wdt_stack, &wdt_struct);

  periodic_callbacks__initialize();
}

void periodic_scheduler__initialize_with_1khz(void) {
  static StackType_t hz1000_stack[4096 / sizeof(StackType_t)];
  static StaticTask_t hz1000_struct;
  xTaskCreateStatic(periodic_scheduler__1000Hz_task, "1000Hz", ARRAY_SIZE(hz1000_stack), NULL, PRIORITY_PERIODIC_1000HZ,
                    hz1000_stack, &hz1000_struct);

  periodic_scheduler__initialize();
}
