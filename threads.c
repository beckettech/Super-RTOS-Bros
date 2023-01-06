/*
 * threads.c
 * @author:
 * uP2 - Fall 2022
 */
#include <time.h>
#include "G8RTOS_Lab4/G8RTOS.h"
#include "G8RTOS_Lab4/G8RTOS_IPC.h"
#include "BoardSupport/inc/bme280_support.h"
#include "BoardSupport/inc/opt3001.h"
#include "BoardSupport/inc/RGBLedDriver.h"
#include "BoardSupport/inc/Joystick.h"
#include "BoardSupport/inc/ILI9341_Lib.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "threads.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/interrupt.h"
#include "BoardSupport/inc/demo_sysctl.h"
#include "stdio.h"
#include "sprites.h"


#define MAXPLAYERS	1
#define BackgroundColor 0x543B

//obstacle obstacles[MAX_OBSTACLES];
obstacle obstacle1;

semaphore_t *sensorMutex;
semaphore_t *LCDMutex;

bool volatile button_pressed = false;
uint8_t volatile score = 0;
uint8_t volatile hiscore = 0;

player player1;


void start_game(void)
{
	G8RTOS_KillAllThreads();

	// Initialize button0 PC4 as input
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);
	GPIO_PORTC_PUR_R |= 0x10;

	// Initialize PC4 interrupt on falling edge
	GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
	GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
	IntEnable(INT_GPIOC);

	// Add button 1 aperiodic thread
	G8RTOS_AddAPeriodicEvent(buttons, 0, INT_GPIOC);

	G8RTOS_WaitSemaphore(&LCDMutex);
	LCD_Init(false); //Initializes LCD

	LCD_Clear(BackgroundColor); // set blue background
	drawSprite(80, 40, 160, 80, LOGO);
	drawSprite(0, 220, MAX_SCREEN_X, 20, GROUND);
	LCD_Text(225, 150, "PRESS UP TO JUMP", WHITE);

	// draw scoreboards
	LCD_Text(85, 15, "SCORE:", WHITE);
	LCD_Text(30, 15, "0", WHITE);
	LCD_Text(310, 15, "HI:", WHITE);
	LCD_Text(280, 15, "0", WHITE);

	// Add background threads
	G8RTOS_AddThread(IdleThread, 5, "IDLE");
	G8RTOS_AddThread(Player_Thread, 1, "Player 1");
	G8RTOS_AddThread(Obstacle_Thread, 1, "Obstacle 1");

	G8RTOS_SignalSemaphore(&LCDMutex);
	while(!button_pressed); //wait on user to press start
	button_pressed = false;

	G8RTOS_WaitSemaphore(&LCDMutex);
	// erase text and logo
	//LCD_Text(225, 150, "PRESS UP TO JUMP", BackgroundColor);
	LCD_DrawRectangle(80, 40, 160, 110, BackgroundColor);
	G8RTOS_AddPeriodicEvent(Scoreboard_Thread, 1000, SystemTime+1000);

	G8RTOS_SignalSemaphore(&LCDMutex);
	sleep(600); // sight delay before start

	// Initialize PC4 interrupt
	GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
	GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

	// add motion threads
	G8RTOS_AddThread(jump, 2, "JUMP");
	G8RTOS_AddThread(run, 2, "RUN");

	G8RTOS_KillSelf();
}

void restart_game(void)
{
	if(!player1.alive)
	{
		// erase player
		G8RTOS_WaitSemaphore(&LCDMutex);
		LCD_DrawRectangle(player1.x, player1.old_y, player1.width, player1.height, BackgroundColor);
		drawSprite(obstacle1.old_x, obstacle1.y, obstacle1.width, obstacle1.height, OBSTACLE1);
		G8RTOS_SignalSemaphore(&LCDMutex);

		player1.yVelo = 0;
		player1.old_y = player1.y;
		obstacle1.xVelo = 0;
		obstacle1.old_x = obstacle1.x;

		sleep(500); // slight delay

		G8RTOS_WaitSemaphore(&LCDMutex);
		LCD_Text(200, 130, "GAME OVER", WHITE);
		G8RTOS_SignalSemaphore(&LCDMutex);

		while(!button_pressed); //wait on user to press start
		button_pressed = false;

		score = 0;
		uint8_t score_string[3];
		sprintf(score_string, "%u", score);
		uint8_t hiscore_string[2];
		sprintf(hiscore_string, "%u", hiscore);

		// erase text and obstacles
		G8RTOS_WaitSemaphore(&LCDMutex);
		LCD_Clear(BackgroundColor); // set blue background
		drawSprite(0, 220, MAX_SCREEN_X, 20, GROUND);

		// draw scoreboards
		LCD_Text(85, 15, "SCORE:", WHITE);
		LCD_Text(30, 15, score_string, WHITE);
		LCD_Text(310, 15, "HI:", WHITE);
		LCD_Text(280, 15, hiscore_string, WHITE);
		G8RTOS_SignalSemaphore(&LCDMutex);

		sleep(1000); // slight delay before start

		// Initialize PC4 interrupt
		GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
		GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);


		player1.x = 250; // init (x,y) position
		player1.y = 170;
		player1.old_y = player1.y;
		player1.sprite = PLAYER1;
		player1.alive = true; // wake up

		obstacle1.x = 0; // init (x,y) position
		obstacle1.y = 190;
		obstacle1.width = 30;
		obstacle1.old_x = obstacle1.x;
		obstacle1.alive = true; // wake up
		score = 0;

		// draw starting positions
		G8RTOS_WaitSemaphore(&LCDMutex);
		//LCD_DrawRectangle(player1.x, player1.y, player1.width, player1.height, RED);
		//LCD_DrawRectangle(obstacle1.x, obstacle1.y, obstacle1.width, obstacle1.height, BLUE);
		drawSprite(player1.x, player1.y, player1.width, player1.height, player1.sprite);
		drawSprite(obstacle1.x, obstacle1.y, obstacle1.width, obstacle1.height, OBSTACLE1);
		G8RTOS_SignalSemaphore(&LCDMutex);

		drawSprite(80, 40, 160, 80, LOGO);
		LCD_Text(225, 150, "PRESS UP TO JUMP", WHITE);

		while(!button_pressed); //wait on user to press start
		button_pressed = false;

		// erase text and logo
		LCD_Text(225, 150, "PRESS UP TO JUMP", BackgroundColor);
		LCD_DrawRectangle(80, 40, 160, 80, BackgroundColor);

		sleep(1000); // sight delay before start

		// Initialize PC4 interrupt
		GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
		GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

		obstacle1.xVelo = 12;
		G8RTOS_KillSelf();
	}
}

void IdleThread(void)
{
    while(1); //Empty default thread
}

// handles button interrupts
void buttons(void)
{
	GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
	GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
	button_pressed = 1;
}

// creates motion for player to jump
void jump(void)
{
    while(1)
    {
    	if(player1.alive)
    	{
    		// update y position
    		player1.old_y = player1.y;
    		player1.y += player1.yVelo;

    		// check boundaries
    		if(player1.y+player1.height> MAX_SCREEN_Y -20) // ground
    		{
    			player1.y = MAX_SCREEN_Y-player1.height-20;
    			player1.yVelo = 0;
    			player1.sprite = PLAYER1;
    		}
//    		else if(player1.y < MIN_SCREEN_Y) // sky
//    			player1.y = MIN_SCREEN_Y;

    		// obstacle collision detection
    		if(player1.y+player1.height >= obstacle1.y && player1.x+20 <= obstacle1.x+obstacle1.width && player1.x+player1.width >= obstacle1.x)
    		{
    			player1.alive = false;
    			obstacle1.alive = false;
    			G8RTOS_AddThread(restart_game, 0, "RESTART");
    		}

    		// gravity
    		if(player1.yVelo < 50)
    			player1.yVelo += 5;
    		if(player1.yVelo < -30)
    			player1.yVelo = -30;

    		// jump if on the ground
    		if(button_pressed && player1.y == MAX_SCREEN_Y-70)
    		{
    			player1.xVelo = 0;
    			player1.yVelo = -50; //start jump
        		button_pressed = 0;
        		player1.sprite = JUMP;
        		sleep(200);
         		// Initialize PC4 interrupt
            	GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
            	GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
    		}
     	}
    	sleep(30);
    }
}

// creates running motion for obstacles
void run(void)
{
    while(1)
    {
    	if(obstacle1.alive)
    	{
    		// increase difficulty
    		if(score > 30)
    			obstacle1.xVelo = 17;
    		else if(score > 25)
    			obstacle1.xVelo = 16;
    		else if(score > 20)
    			obstacle1.xVelo = 15;
    		else if(score > 15)
    			obstacle1.xVelo = 14;
    		else if(score > 10)
    			obstacle1.xVelo = 14;
    		else if(score > 5)
    			obstacle1.xVelo = 13;

    		// update x position
    		obstacle1.old_x = obstacle1.x;
    		obstacle1.x += obstacle1.xVelo;

    		// check boundaries
    		if(obstacle1.x > MAX_SCREEN_X) // left
    		{
    			// create new obstacle of random size (1-3)
    			obstacle1.width = 30*((rand() % 3) + 1); // 1*width < width < 3*width;
    			obstacle1.x = MIN_SCREEN_X-obstacle1.width;
    		}

    		// obstacle collision detection
    		if(player1.y+player1.height >= obstacle1.y && player1.x +20 <= obstacle1.x+obstacle1.width && player1.x+player1.width >= obstacle1.x +5)
    	    {
    			player1.alive = false;
    			obstacle1.alive = false;
    			G8RTOS_AddThread(restart_game, 0, "RESTART");
    		}
    	}
    	sleep(30);
    }
}

void Player_Thread(void)
{
	player1.alive = true; // wake up
	player1.ThreadID = G8RTOS_GetThreadId(); // update ID
	player1.x = 250; // init (x,y) position
	player1.y = 170;
	player1.width = 30;
	player1.height = 50;
	player1.old_y = player1.y;
	player1.yVelo = 0;
	player1.sprite = PLAYER1;

	// draw starting positions
	G8RTOS_WaitSemaphore(&LCDMutex);

	//LCD_DrawRectangle(player1.x, player1.y, player1.width, player1.height, RED);
	drawSprite(player1.x, player1.y, player1.width, player1.height, player1.sprite);
	G8RTOS_SignalSemaphore(&LCDMutex);

	while(1)
	{
		// update player position only if not still
		if(player1.old_y != player1.y || player1.xVelo != 0)
		{
			int diff = player1.y - player1.old_y;

			// erase player's old position
			G8RTOS_WaitSemaphore(&LCDMutex);
			if(diff > 0) // player is falling
				LCD_DrawRectangle(player1.x, player1.old_y, player1.width, diff, BackgroundColor);
			if(diff < 0) // player is rising
				LCD_DrawRectangle(player1.x, player1.y + player1.height, player1.width, abs(diff), BackgroundColor);

			// draw new position
			drawSprite(player1.x, player1.y, player1.width, player1.height, player1.sprite);
			//LCD_DrawRectangle(player1.x, player1.y, player1.width, player1.height, BLUE);
			G8RTOS_SignalSemaphore(&LCDMutex);
		}

		sleep(30);
	}
}

void Obstacle_Thread(void)
{
	obstacle1.alive = true; // wake up
	obstacle1.ThreadID = G8RTOS_GetThreadId(); // update ID
	obstacle1.x = 0; // init (x,y) position
	obstacle1.y = 190;
	obstacle1.width = 30;
	obstacle1.height = 30;
	obstacle1.old_x = obstacle1.x;
	obstacle1.xVelo = 12;

	// draw starting positions
	G8RTOS_WaitSemaphore(&LCDMutex);
	drawSprite(obstacle1.x, obstacle1.y, obstacle1.width, obstacle1.height, OBSTACLE1);
	G8RTOS_SignalSemaphore(&LCDMutex);

	while(1)
	{
		// update player position only if not still
		if(obstacle1.old_x != obstacle1.x || obstacle1.xVelo != 0)
		{
			int diff = obstacle1.x - obstacle1.old_x;

			G8RTOS_WaitSemaphore(&LCDMutex);
			// obstacle on right boundary
			if(obstacle1.x < 0)
			{
				drawSprite(obstacle1.x, obstacle1.y, obstacle1.width + obstacle1.x, obstacle1.height, OBSTACLE1);
				LCD_DrawRectangle(310, 190, 10, obstacle1.height, BackgroundColor);
			}
			// obstacle in middle of screen
			else if(obstacle1.x+obstacle1.width <= MAX_SCREEN_X + 5)
			{
				LCD_DrawRectangle(obstacle1.old_x, obstacle1.y, diff, obstacle1.height, BackgroundColor);
				drawSprite(obstacle1.x, obstacle1.y, obstacle1.width, obstacle1.height, OBSTACLE1);
			}
			// obstacle on left boundary
			else if(obstacle1.x+obstacle1.width > MAX_SCREEN_X + 5)
			{
				LCD_DrawRectangle(obstacle1.old_x, obstacle1.y, MAX_SCREEN_X-obstacle1.old_x, obstacle1.height, BackgroundColor);
				drawSprite(obstacle1.x, obstacle1.y, obstacle1.width, obstacle1.height, OBSTACLE1);
			}
			G8RTOS_SignalSemaphore(&LCDMutex);
		}
		sleep(30);
	}
}

void Scoreboard_Thread(void)
{
	if(obstacle1.xVelo != 0)
	{
		if(score > hiscore)
			hiscore++;

		uint8_t score_string[3];
		sprintf(score_string, "%u", score++);
		uint8_t hiscore_string[2];
		sprintf(hiscore_string, "%u", hiscore);

		G8RTOS_WaitSemaphore(&LCDMutex);
		LCD_DrawRectangle(0, 0, 30, 15, BackgroundColor); // erase old score
		LCD_Text(30, 15, score_string, WHITE); // draw new score
		LCD_DrawRectangle(260, 0, 30, 15, BackgroundColor); //erase old hi
		LCD_Text(280, 15, hiscore_string, WHITE); // draw hi score
		G8RTOS_SignalSemaphore(&LCDMutex);
	}
}
