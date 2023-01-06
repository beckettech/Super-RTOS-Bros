//*****************************************************************************
//
// watchdog.h - Prototypes for the Watchdog Timer API
//
// Copyright (c) 2005-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 2.2.0.295 of the Tiva Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __DRIVERLIB_WATCHDOG_H__
#define __DRIVERLIB_WATCHDOG_H__

#include <stdbool.h>
#include <stdint.h>

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The type of interrupt that can be generated by the watchdog.
//
//*****************************************************************************
#define WATCHDOG_INT_TYPE_INT   0x00000000
#define WATCHDOG_INT_TYPE_NMI   0x00000004

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern bool WatchdogRunning(uint32_t ui32Base);
extern void WatchdogEnable(uint32_t ui32Base);
extern void WatchdogResetEnable(uint32_t ui32Base);
extern void WatchdogResetDisable(uint32_t ui32Base);
extern void WatchdogLock(uint32_t ui32Base);
extern void WatchdogUnlock(uint32_t ui32Base);
extern bool WatchdogLockState(uint32_t ui32Base);
extern void WatchdogReloadSet(uint32_t ui32Base, uint32_t ui32LoadVal);
extern uint32_t WatchdogReloadGet(uint32_t ui32Base);
extern uint32_t WatchdogValueGet(uint32_t ui32Base);
extern void WatchdogIntRegister(uint32_t ui32Base, void (*pfnHandler)(void));
extern void WatchdogIntUnregister(uint32_t ui32Base);
extern void WatchdogIntEnable(uint32_t ui32Base);
extern uint32_t WatchdogIntStatus(uint32_t ui32Base, bool bMasked);
extern void WatchdogIntClear(uint32_t ui32Base);
extern void WatchdogIntTypeSet(uint32_t ui32Base, uint32_t ui32Type);
extern void WatchdogStallEnable(uint32_t ui32Base);
extern void WatchdogStallDisable(uint32_t ui32Base);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_WATCHDOG_H__
