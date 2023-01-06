/**
 * G8RTOS IPC - Inter-Process Communication
 * @author:
 * uP2 - Fall 2022
 */
#include <stdint.h>
#include "G8RTOS_IPC.h"
#include "G8RTOS_Semaphores.h"

#define FIFOSIZE 16
#define MAX_NUMBER_OF_FIFOS 4

/*
 * FIFO struct will hold
 *  - buffer
 *  - head
 *  - tail
 *  - lost data
 *  - current size
 *  - mutex
 */


typedef struct FIFO_t {
    int32_t buffer[16];
    int32_t *head;
    int32_t *tail;
    uint32_t lostData;
    semaphore_t currentSize;
    semaphore_t mutex;
} FIFO_t;

/* Array of FIFOS */
static FIFO_t FIFOs[4];


/*
 * Initializes FIFO Struct
 */
int G8RTOS_InitFIFO(uint32_t FIFOIndex)
{
    if(FIFOIndex >= MAX_NUMBER_OF_FIFOS){
        return -1;
    }
    //Initializes the buffer pointers as well as the semaphores
    FIFOs[FIFOIndex].head = &FIFOs[FIFOIndex].buffer[0];
    FIFOs[FIFOIndex].tail = &FIFOs[FIFOIndex].buffer[0];
    //Current size starts at 0 and mutex starts at 1
    FIFOs[FIFOIndex].mutex = 1;
    FIFOs[FIFOIndex].currentSize = 0;
    return 0;
}


/*
 * Reads FIFO
 *  - Waits until CurrentSize semaphore is greater than zero
 *  - Gets data and increments the head pointer (wraps if necessary)
 * Param: "FIFOChoice": chooses which buffer we want to read from
 * Returns: uint32_t Data from FIFO
 */
int32_t readFIFO(uint32_t FIFOChoice)
{
    G8RTOS_WaitSemaphore(&FIFOs[FIFOChoice].mutex);         //Checks for mutex
    G8RTOS_WaitSemaphore(&FIFOs[FIFOChoice].currentSize);   //Checks size
    int32_t data = *FIFOs[FIFOChoice].head;                 //Reads data
    FIFOs[FIFOChoice].head++;                               //Increments head pointer
    if(FIFOs[FIFOChoice].head == &FIFOs[FIFOChoice].buffer[15]){    //Resets pointer if out of bounds
        FIFOs[FIFOChoice].head = &FIFOs[FIFOChoice].buffer[0];
    }
    G8RTOS_SignalSemaphore(&FIFOs[FIFOChoice].mutex);       //Releases the mutex
    return data;

}

/*
 * Writes to FIFO
 *  Writes data to Tail of the buffer if the buffer is not full
 *  Increments tail (wraps if necessary)
 *  Param "FIFOChoice": chooses which buffer we want to read from
 *        "Data': Data being put into FIFO
 *  Returns: error code for full buffer if unable to write
 */
int writeFIFO(uint32_t FIFOChoice, uint32_t Data)
{
    if(FIFOs[FIFOChoice].currentSize > FIFOSIZE-1){         //Checks if the buffer is full
        FIFOs[FIFOChoice].lostData++;
        return -1;
    }
    *FIFOs[FIFOChoice].tail = Data;         //Writes data
    FIFOs[FIFOChoice].tail++;               //Increments tail pointer
    if(FIFOs[FIFOChoice].tail == &FIFOs[FIFOChoice].buffer[15]){    //Resets the pointer if out of bounds
        FIFOs[FIFOChoice].tail = &FIFOs[FIFOChoice].buffer[0];
    }
    G8RTOS_SignalSemaphore(&FIFOs[FIFOChoice].currentSize); //Increases the current size
    return 0;
}

