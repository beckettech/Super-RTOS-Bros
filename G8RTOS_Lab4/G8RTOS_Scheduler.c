/**
 * G8RTOS_Scheduler.c
 * uP2 - Fall 2022
 */
#include <stdint.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "BoardSupport/inc/RGBLedDriver.h"
#include "G8RTOS_Scheduler.h"
#include "G8RTOS_Structures.h"
#include "G8RTOS_CriticalSection.h"

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;

/*
 * Pointer to the currently running Thread Control Block
 */
extern tcb_t * CurrentlyRunningThread;

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Defines ******************************************************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

/*********************************************** Defines ******************************************************************************/


/*********************************************** Data Structures Used *****************************************************************/

/* Thread Control Blocks
 *	- An array of thread control blocks to hold pertinent information for each thread
 */
static tcb_t threadControlBlocks[MAX_THREADS];

/* Thread Stacks
 *	- An array of arrays that will act as individual stacks for each thread
 */
static int32_t threadStacks[MAX_THREADS][STACKSIZE];

/* Periodic Event Threads
 * - An array of periodic events to hold pertinent information for each thread
 */
static ptcb_t Pthread[MAXPTHREADS];

/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;

/*
 * Current Number of Periodic Threads currently in the scheduler
 */
static uint32_t NumberOfPthreads;

/*********************************************** Private Variables ********************************************************************/


/*********************************************** Private Functions ********************************************************************/

/*
 * Initializes the Systick and Systick Interrupt
 * The Systick interrupt will be responsible for starting a context switch between threads
 * Param "numCycles": Number of cycles for each systick interrupt
 */
static void InitSysTick(uint32_t numCycles)
{
    SysTickPeriodSet(numCycles);
    SysTickEnable();
}

/*
 * Chooses the next thread to run.
 * Lab 2 Scheduling Algorithm:
 * 	- Simple Round Robin: Choose the next running thread by selecting the currently running thread's next pointer
 * 	- Check for sleeping and blocked threads
 */
void G8RTOS_Scheduler()
{
	int currentMaxPriority = 256;
    tcb_t *threadPtr = CurrentlyRunningThread->nextTCB;

    // find highest priority thread that is ready
    int i;
    for(i = 0; i < NumberOfThreads; i++)
    {
    	if(threadPtr->asleep == 0 && threadPtr->blocked == 0)
    	{
    		if(threadPtr->priority < currentMaxPriority)
    		{
    			CurrentlyRunningThread = threadPtr;
    			currentMaxPriority = threadPtr->priority;
    		}
    	}
    	threadPtr = threadPtr->nextTCB;
    }
}


/*
 * SysTick Handler
 * The Systick Handler now will increment the system time,
 * set the PendSV flag to start the scheduler,
 * and be responsible for handling sleeping and periodic threads
 */
void SysTick_Handler()
{
    SystemTime++;
    tcb_t *ptr = CurrentlyRunningThread;
    ptcb_t *Pptr = &Pthread[0];

    //Checks for periodic threads, and executes them appropriately
    uint8_t i;
    for(i = 0; i < NumberOfPthreads; i++)
    {
        if(Pptr->executeTime == SystemTime)
        {
            Pptr->executeTime = Pptr->period + SystemTime;
            Pptr->handler();
        }
        Pptr = Pptr->nextPTCB;
    }

    //Checks sleeping threads and wakes them up appropriately
    for(i = 0;i < NumberOfThreads;i++)
    {
        if(ptr->asleep == 1)
        {
            if(ptr->sleepCount == SystemTime)
            {
                ptr->asleep = 0;
                ptr->sleepCount = 0;
            }
        }
        ptr = ptr->nextTCB;
    }

    // now lets do the context switch
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

/*********************************************** Private Functions ********************************************************************/


/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Sets variables to an initial state (system time and number of threads)
 * Enables board for highest speed clock and disables watchdog
 */
void G8RTOS_Init()
{
    SystemTime = 0;
    NumberOfThreads = 0;
    NumberOfPthreads = 0;
    CurrentlyRunningThread = &threadControlBlocks[0];

    // relocate ISR vector table
    uint32_t newVTORTable = 0x20000000;
    uint32_t * newTable = (uint32_t *)newVTORTable;
    uint32_t * oldTable = (uint32_t *)0;
    int i;
    for (i = 0; i < 155; i++)
    {
    newTable[i] = oldTable[i];
    }
    HWREG(NVIC_VTABLE) = newVTORTable;
}

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes the Systick
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int G8RTOS_Launch()
{
    InitSysTick(SysCtlClockGet() / 1000); // 1 ms tick (1Hz / 1000)
    int currentMaxPriority = 256;
    tcb_t *threadPtr = &threadControlBlocks[0];

    // find highest priority thread that is ready
    int i;
    for(i = 0; i < NumberOfThreads; i++)
    {
    	if(threadPtr->asleep == 0 && threadPtr->blocked == 0)
    	{
    		if(threadPtr->priority < currentMaxPriority)
    		{
    			CurrentlyRunningThread = threadPtr;
    			currentMaxPriority = threadPtr->priority;
    		}
    	}
    	threadPtr = threadPtr->nextTCB;
    }

    IntPrioritySet(FAULT_PENDSV, 0xE0);
    IntPrioritySet(FAULT_SYSTICK, 0xE0);
    SysTickIntEnable();
    IntMasterEnable();
    G8RTOS_Start();
    return 0;
}


/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are stil available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread to hold a "fake context"
 * 	- Sets stack tcb stack pointer to top of thread stack
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *name)
{
	IBit_State = StartCriticalSection();
    if (NumberOfThreads >= MAX_THREADS)
    {
    	EndCriticalSection(IBit_State);
        return THREAD_LIMIT_REACHED;
    }
    else
    {
    	int index = 0;
        // Create the head of the linked list (there is only one thread, so both the next and previous threads are itself
        if (NumberOfThreads == 0)
        {
            threadControlBlocks[0].nextTCB = &threadControlBlocks[0];
            threadControlBlocks[0].previousTCB = &threadControlBlocks[0];
        }

        /* Append the new thread to the end of the linked list
         * 1. Number of threads will refer to the newest thread to be added since the current index would be NumberOfThreads - 1
         * 2. Set the next thread for the new thread to be the first in the list, so that round-robin will be maintained
         * 3. Set the current thread's nextTCB to be the new thread
         * 3. Set the first thread's previous thread to be the new thread, so that it goes in the right spot in the list
         * 4. Point the previousTCB of the new thread to the current thread so that it moves in the correct order
         */

        else
        {
        	// find first available thread
        	int i = 0;
        	for(i = 0; i < MAX_THREADS; i++)
        	{
        		if(threadControlBlocks[i].isAlive == 0)
        		{
        			index = i;
        			break;
        		}
        	}
            threadControlBlocks[index].nextTCB = CurrentlyRunningThread->nextTCB;
            threadControlBlocks[index].previousTCB = CurrentlyRunningThread;
            threadControlBlocks[index].nextTCB->previousTCB = &threadControlBlocks[index];
            CurrentlyRunningThread->nextTCB = &threadControlBlocks[index];
        }
        // set unique ID and name
        threadControlBlocks[index].ThreadID = index;
        strcpy(threadControlBlocks[index].Threadname, name);

        // set priority
        threadControlBlocks[index].priority = priority;
        threadControlBlocks[index].isAlive = 1;

        // Set up the stack pointer
        threadControlBlocks[index].stackPointer = &threadStacks[index][STACKSIZE - 16];
        threadStacks[index][STACKSIZE - 1] = THUMBBIT;
        threadStacks[index][STACKSIZE - 2] = (uint32_t)threadToAdd;

        // Increment number of threads present in the scheduler
        NumberOfThreads++;
    }
    EndCriticalSection(IBit_State);
    return NO_ERROR;
}


/*
 * Adds periodic threads to G8RTOS Scheduler
 * Function will initialize a periodic event struct to represent event.
 * The struct will be added to a linked list of periodic events
 * Param Pthread To Add: void-void function for P thread handler
 * Param period: period of P thread to add
 * Returns: Error code for adding threads
 */
int G8RTOS_AddPeriodicEvent(void (*PthreadToAdd)(void), uint32_t period, uint32_t execution)
{
    //Maximum amount of P threads
    if(NumberOfPthreads > MAXPTHREADS)
    {
        return -1;  //Return -1 if at max
    }
    else
    {
        //Sets the first thread
        if(NumberOfPthreads == 0)
        {
            Pthread[NumberOfPthreads].nextPTCB = &Pthread[0];
            Pthread[NumberOfPthreads].previousPTCB = &Pthread[0];
        }
        else    //Inserts the new thread into the linked list
        {
            Pthread[NumberOfPthreads].nextPTCB = &Pthread[0];
            Pthread[NumberOfPthreads-1].nextPTCB = &Pthread[NumberOfPthreads];
            Pthread[0].previousPTCB = &Pthread[NumberOfPthreads];
            Pthread[NumberOfPthreads].previousPTCB = &Pthread[NumberOfPthreads-1];
        }
        Pthread[NumberOfPthreads].period = period;          //Stores period
        Pthread[NumberOfPthreads].executeTime = execution;  //Stores execution
        Pthread[NumberOfPthreads].handler = PthreadToAdd;   //Stores handler
        NumberOfPthreads++; //Increases thread count
    }
    return 1;
}

sched_ErrCode_t G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, int32_t IRQn)
{
	IBit_State = StartCriticalSection();
	if(IRQn > 154 || IRQn < 1)
	{
		return IRQn_INVALID;
		EndCriticalSection(IBit_State);
	}
	if(priority >= OSINT_PRIORITY)
	{
		return HWI_PRIORITY_INVALID;
		EndCriticalSection(IBit_State);
	}

	IntRegister(IRQn, AthreadToAdd);
	IntPrioritySet(IRQn, priority);
	IntEnable(IRQn);
	IntMasterEnable();
	EndCriticalSection(IBit_State);
	return NO_ERROR;
}


/*
 * Puts the current thread into a sleep state.
 *  param durationMS: Duration of sleep time in ms
 */
void sleep(uint32_t durationMS)
{
    CurrentlyRunningThread->sleepCount = durationMS + SystemTime;   //Sets sleep count
    CurrentlyRunningThread->asleep = 1;                             //Puts the thread to sleep
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;                  //Start context switch
}

threadId_t G8RTOS_GetThreadId()
{
    return CurrentlyRunningThread->ThreadID;        //Returns the thread ID
}

sched_ErrCode_t G8RTOS_KillThread(threadId_t threadID)
{
	IBit_State = StartCriticalSection();

	if(NumberOfThreads == 0)
	{
		return NO_THREADS_SCHEDULED;
	}
	if(NumberOfThreads == 1)
	{
		EndCriticalSection(IBit_State);
		return CANNOT_KILL_LAST_THREAD;
	}
	// find thread with matching ID
	int index = -1;
	int i;
	for(i = 0; i < MAX_THREADS; i++)
	{
		if(threadControlBlocks[i].ThreadID == threadID)
		{
			index = i;
			break;
		}
	}
	// not found
	if(index == -1)
	{
		EndCriticalSection(IBit_State);
		return THREAD_DOES_NOT_EXIST;
	}

	NumberOfThreads--;
	threadControlBlocks[index].isAlive = 0;
	threadControlBlocks[index].nextTCB->previousTCB = threadControlBlocks[index].previousTCB;
	threadControlBlocks[index].previousTCB->nextTCB = threadControlBlocks[index].nextTCB;

	// context switch if killed currently running thread
	if(CurrentlyRunningThread->isAlive == 0)
		 HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;

	EndCriticalSection(IBit_State);
	return NO_ERROR;
}

//Thread kills itself
sched_ErrCode_t G8RTOS_KillSelf()
{
    return G8RTOS_KillThread(CurrentlyRunningThread->ThreadID);
}

uint32_t GetNumberOfThreads(void)
{
    return NumberOfThreads;         //Returns the number of threads
}

void G8RTOS_KillAllThreads()
{
	int i;
	for(i = 0; i < MAX_THREADS; i++)
	{
		if(threadControlBlocks[i].ThreadID != CurrentlyRunningThread->ThreadID)
			G8RTOS_KillThread(threadControlBlocks[i].ThreadID);
	}
}






/*********************************************** Public Functions *********************************************************************/
