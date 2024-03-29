/* --COPYRIGHT--,BSD
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// sys.h - Driver for the SYS Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_SYS_H__
#define __MSP430WARE_SYS_H__

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

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SYS__

//*****************************************************************************
//
// The following are values that can be passed to the BSLRAMAssignment
// parameter for functions: SYS_setRAMAssignedToBSL().
//
//*****************************************************************************
#define SYS_BSLRAMASSIGN_NORAM                                     (!(SYSBSLR))
#define SYS_BSLRAMASSIGN_LOWEST16BYTES                                (SYSBSLR)

//*****************************************************************************
//
// The following are values that can be passed to the BSLSizeSelect parameter
// for functions: SYS_setBSLSize().
//
//*****************************************************************************
#define SYS_BSLSIZE_SEG3                         (~(SYSBSLSIZE0 + SYSBSLSIZE1))
#define SYS_BSLSIZE_SEGS23                                        (SYSBSLSIZE0)
#define SYS_BSLSIZE_SEGS123                                       (SYSBSLSIZE1)
#define SYS_BSLSIZE_SEGS1234                        (SYSBSLSIZE0 + SYSBSLSIZE1)

//*****************************************************************************
//
// The following are values that can be passed to the mailboxSizeSelect
// parameter for functions: SYS_JTAGMailboxInit().
//
//*****************************************************************************
#define SYS_JTAGMBSIZE_16BIT                                       (!(JMBMODE))
#define SYS_JTAGMBSIZE_32BIT                                          (JMBMODE)

//*****************************************************************************
//
// The following are values that can be passed to the autoClearInboxFlagSelect
// parameter for functions: SYS_JTAGMailboxInit().
//
//*****************************************************************************
#define SYS_JTAGINBOX0AUTO_JTAGINBOX1AUTO          (!(JMBCLR0OFF + JMBCLR1OFF))
#define SYS_JTAGINBOX0AUTO_JTAGINBOX1SW                            (JMBCLR1OFF)
#define SYS_JTAGINBOX0SW_JTAGINBOX1AUTO                            (JMBCLR0OFF)
#define SYS_JTAGINBOX0SW_JTAGINBOX1SW                 (JMBCLR0OFF + JMBCLR1OFF)

//*****************************************************************************
//
// The following are values that can be passed to the mailboxFlagMask parameter
// for functions: SYS_getJTAGMailboxFlagStatus(), and
// SYS_clearJTAGMailboxFlagStatus().
//
//*****************************************************************************
#define SYS_JTAGOUTBOX_FLAG0                                        (JMBOUT0FG)
#define SYS_JTAGOUTBOX_FLAG1                                        (JMBOUT1FG)
#define SYS_JTAGINBOX_FLAG0                                          (JMBIN0FG)
#define SYS_JTAGINBOX_FLAG1                                          (JMBIN1FG)

//*****************************************************************************
//
// The following are values that can be passed to the inboxSelect parameter for
// functions: SYS_getJTAGInboxMessage16Bit().
//
//*****************************************************************************
#define SYS_JTAGINBOX_0                                                   (0x0)
#define SYS_JTAGINBOX_1                                                   (0x2)

//*****************************************************************************
//
// The following are values that can be passed to the outboxSelect parameter
// for functions: SYS_setJTAGOutgoingMessage16Bit().
//
//*****************************************************************************
#define SYS_JTAGOUTBOX_0                                                  (0x0)
#define SYS_JTAGOUTBOX_1                                                  (0x2)

//*****************************************************************************
//
// The following are values that can be passed toThe following are values that
// can be returned by the SYS_getBSLEntryIndication() function.
//
//*****************************************************************************
#define SYS_BSLENTRY_INDICATED                                            (0x1)
#define SYS_BSLENTRY_NOTINDICATED                                         (0x0)

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void SYS_enableDedicatedJTAGPins(uint32_t baseAddress);

extern uint8_t SYS_getBSLEntryIndication(uint32_t baseAddress);

extern void SYS_enablePMMAccessProtect(uint32_t baseAddress);

extern void SYS_enableRAMBasedInterruptVectors(uint32_t baseAddress);

extern void SYS_disableRAMBasedInterruptVectors(uint32_t baseAddress);

extern void SYS_enableBSLProtect(uint32_t baseAddress);

extern void SYS_disableBSLProtect(uint32_t baseAddress);

extern void SYS_enableBSLMemory(uint32_t baseAddress);

extern void SYS_disableBSLMemory(uint32_t baseAddress);

extern void SYS_setRAMAssignedToBSL(uint32_t baseAddress,
                                    uint8_t BSLRAMAssignment);

extern void SYS_setBSLSize(uint32_t baseAddress,
                           uint8_t BSLSizeSelect);

extern void SYS_JTAGMailboxInit(uint32_t baseAddress,
                                uint8_t mailboxSizeSelect,
                                uint8_t autoClearInboxFlagSelect);

extern uint8_t SYS_getJTAGMailboxFlagStatus(uint32_t baseAddress,
                                            uint8_t mailboxFlagMask);

extern void SYS_clearJTAGMailboxFlagStatus(uint32_t baseAddress,
                                           uint8_t mailboxFlagMask);

extern uint16_t SYS_getJTAGInboxMessage16Bit(uint32_t baseAddress,
                                             uint8_t inboxSelect);

extern uint32_t SYS_getJTAGInboxMessage32Bit(uint32_t baseAddress);

extern void SYS_setJTAGOutgoingMessage16Bit(uint32_t baseAddress,
                                            uint8_t outboxSelect,
                                            uint16_t outgoingMessage);

extern void SYS_setJTAGOutgoingMessage32Bit(uint32_t baseAddress,
                                            uint32_t outgoingMessage);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_SYS_H__
