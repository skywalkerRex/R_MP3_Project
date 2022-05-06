#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Creates all of the periodic tasks (1Hz, 10Hz, 100Hz) and the periodic task monitor task.
 * If 1000Hz task is required, use periodic_scheduler__initialize_with_1khz()
 *
 * After the RTOS starts, it will start to invoke the functions defined at periodic_callbacks.h
 *
 * Stack sizes are defaulted to reasonable numbers, but you can modify them inside of
 * periodic_scheduler__initialize() if desired.
 */
void periodic_scheduler__initialize(void);

/**
 * Initialize the periodic scheduler, but also with 1Khz task
 * Note that 1Khz is a recurring task every 1ms, and may not be highly desirable unless you need
 * fast recurring function: @see periodic_callbacks__1000Hz()
 */
void periodic_scheduler__initialize_with_1khz(void);