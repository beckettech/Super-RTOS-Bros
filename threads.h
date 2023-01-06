/**
 * thread.h
 * @author:
 * uP2 - Fall 2022
 */

#ifndef THREADS_H_
#define THREADS_H_

#include "G8RTOS_Lab4/G8RTOS_Semaphores.h"
#include "G8RTOS_Lab4/G8RTOS_Structures.h"

#define MAX_OBSTACLES 2

#define COOR_FIFO 0

// define semaphore
semaphore_t *LCDMutex;

// Struct object for a 'obstacle' thread
typedef struct player
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t old_x;
    int16_t old_y;
    int16_t xVelo;
    int16_t yVelo;
    uint16_t color;
    threadId_t ThreadID;
    bool alive;
    int sprite;
} player;

// Struct object for a 'obstacle' thread
typedef struct obstacle
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t old_x;
    int16_t xVelo;
    uint16_t color;
    threadId_t ThreadID;
    bool alive;
} obstacle;

void start_game(void);
void restart_game(void);
void IdleThread(void);
void buttons(void);
void jump(void);
void Player_Thread(void);

void Obstacle_Thread(void);
void run(void);
void Scoreboard_Thread(void);

#endif /* THREADS_H_ */
