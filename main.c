/**
 * main.c
 * @author:
 * uP2 - Fall 2022
 */

// Required headers and definitions
#include <G8RTOS_Lab4/G8RTOS_Semaphores.h>
#include <G8RTOS_Lab4/G8RTOS_Structures.h>
#include <G8RTOS_Lab4/G8RTOS_Scheduler.h>
#include <G8RTOS_Lab4/G8RTOS_IPC.h>
#include "threads.h"
#include "driverlib/watchdog.h"
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "BoardSupport/inc/BoardInitialization.h"
#include "BoardSupport/inc/bme280_support.h"
#include "BoardSupport/inc/ILI9341_Lib.h"
#include <stdbool.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern int G8RTOS_InitFIFO(uint32_t FIFOIndex);


char* itoa(int num, char* buffer, int base)
{
	int current = 0;
	if (num == 0) {
		buffer[current++] = '0';
		buffer[current] = '\0';
		return buffer;
	}
	int num_digits = 0;
	if (num < 0) {
		if (base == 10) {
			num_digits ++;
			buffer[current] = '-';
			current ++;
			num *= -1;
		}
		else
			return NULL;
	}
	num_digits += (int)floor(log(num) / log(base)) + 1;
	while (current < num_digits)
	{
		int base_val = (int) pow(base, num_digits-1-current);
		int num_val = num / base_val;
		char value = num_val + '0';
		buffer[current] = value;
		current ++;
		num -= base_val * num_val;
	}
	buffer[current] = '\0';
	return buffer;
}

void main(void)
{
	G8RTOS_Init(); //Initializes the G8RTOS

	InitializeBoard(); //Initializes the BSP

	// Initializes mutexes
	G8RTOS_InitSemaphore(&LCDMutex, 1);

	// Initialize coordinate FIFO
	G8RTOS_InitFIFO(COOR_FIFO);

	G8RTOS_AddThread(start_game, 1, "START");

//	int num = 6969;
//	char snum[5];
//	// convert 123 to string [buf]
//	itoa(num, snum, 10);
//    LCD_Text(200, 20, "SCORE: ", BLACK);
//    LCD_Text(150, 20, snum, BLACK);

	G8RTOS_Launch(); //Launches the G8RTOS
}

