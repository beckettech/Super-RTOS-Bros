/**
 * G8RTOS_Semaphores.c
 * uP2 - Fall 2022
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "G8RTOS_Semaphores.h"
#include "G8RTOS_CriticalSection.h"
#include "G8RTOS_Scheduler.h"


/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes a semaphore to a given value
 * Param "s": Pointer to semaphore
 * Param "value": Value to initialize semaphore to
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
    IBit_State = StartCriticalSection();    //Disables interrupts
    *s = value;                             //Sets the semaphore
    EndCriticalSection(IBit_State);         //Enables interrupts
}


/*
 * No longer waits for semaphore
 *  - Decrements semaphore
 *  - Blocks thread if semaphore is unavailable
 * Param "s": Pointer to semaphore to wait on
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{

    IBit_State = StartCriticalSection();    //Disables interrupts
    (*s)--;                                 //decrements the semaphore
    EndCriticalSection(IBit_State);         //Enables interrupts
    if((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s; //Blocks thread if semaphore unavailable
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV; //Start context switch
    }
}


/*
 * Signals the completion of the usage of a semaphore
 *  - Increments the semaphore value by 1
 *  - Unblocks any threads waiting on that semaphore
 * Param "s": Pointer to semaphore to be signaled
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
    IBit_State = StartCriticalSection();    //Disables interrupts
    (*s)++;                                 //increments the semaphore
    EndCriticalSection(IBit_State);         //Enables interrupts

    //Checks if any threads were waiting on the semaphore, and unblocks them
    if((*s) <= 0)
    {
        tcb_t *pt = (tcb_t *)&CurrentlyRunningThread->nextTCB;
        while(pt->blocked != s)
        {
            pt = pt->nextTCB;
        }
        pt->blocked = 0;
    }
}

/*********************************************** Public Functions *********************************************************************/


