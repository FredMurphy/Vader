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
// usci_a_uart.h - Driver for the USCI_A_UART Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_USCI_A_UART_H__
#define __MSP430WARE_USCI_A_UART_H__

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

#ifdef __MSP430_HAS_USCI_Ax__

//*****************************************************************************
//
// The following values are the sync characters possible
//
//*****************************************************************************
#define DEFAULT_SYNC                                                       0x00
#define USCI_A_UART_AUTOMATICBAUDRATE_SYNC                                 0x55

///////////////////////////////////////////////////////////////////////////////
//Deprecated
///////////////////////////////////////////////////////////////////////////////
#define UARTBREAK_DETECT                                                  UCBRK

//*****************************************************************************
//
// The following are values that can be passed to the parity parameter for
// functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_NO_PARITY                                              0x00
#define USCI_A_UART_ODD_PARITY                                             0x01
#define USCI_A_UART_EVEN_PARITY                                            0x02

//*****************************************************************************
//
// The following are values that can be passed to the msborLsbFirst parameter
// for functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_MSB_FIRST                                             UCMSB
#define USCI_A_UART_LSB_FIRST                                              0x00

//*****************************************************************************
//
// The following are values that can be passed to the uartMode parameter for
// functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_MODE                                               UCMODE_0
#define USCI_A_UART_IDLE_LINE_MULTI_PROCESSOR_MODE                     UCMODE_1
#define USCI_A_UART_ADDRESS_BIT_MULTI_PROCESSOR_MODE                   UCMODE_2
#define USCI_A_UART_AUTOMATIC_BAUDRATE_DETECTION_MODE                  UCMODE_3

//*****************************************************************************
//
// The following are values that can be passed to the selectClockSource
// parameter for functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_CLOCKSOURCE_SMCLK                             UCSSEL__SMCLK
#define USCI_A_UART_CLOCKSOURCE_ACLK                               UCSSEL__ACLK

//*****************************************************************************
//
// The following are values that can be passed to the numberofStopBits
// parameter for functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_ONE_STOP_BIT                                           0x00
#define USCI_A_UART_TWO_STOP_BITS                                         UCSPB

//*****************************************************************************
//
// The following are values that can be passed to the overSampling parameter
// for functions: USCI_A_UART_initAdvance().
//
//*****************************************************************************
#define USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION                       0x01
#define USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION                      0x00

//*****************************************************************************
//
// The following are values that can be passed to the mask parameter for
// functions: USCI_A_UART_enableInterrupt(), and
// USCI_A_UART_disableInterrupt().
//
//*****************************************************************************
#define USCI_A_UART_RECEIVE_INTERRUPT                                    UCRXIE
#define USCI_A_UART_TRANSMIT_INTERRUPT                                   UCTXIE
#define USCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT                     UCRXEIE
#define USCI_A_UART_BREAKCHAR_INTERRUPT                                 UCBRKIE

//*****************************************************************************
//
// The following are values that can be passed to the mask parameter for
// functions: USCI_A_UART_getInterruptStatus(), and
// USCI_A_UART_clearInterruptFlag() as well as returned by the
// USCI_A_UART_getInterruptStatus() function.
//
//*****************************************************************************
#define USCI_A_UART_RECEIVE_INTERRUPT_FLAG                              UCRXIFG
#define USCI_A_UART_TRANSMIT_INTERRUPT_FLAG                             UCTXIFG

//*****************************************************************************
//
// The following are values that can be passed to the mask parameter for
// functions: USCI_A_UART_queryStatusFlags() as well as returned by the
// USCI_A_UART_queryStatusFlags() function.
//
//*****************************************************************************
#define USCI_A_UART_LISTEN_ENABLE                                      UCLISTEN
#define USCI_A_UART_FRAMING_ERROR                                          UCFE
#define USCI_A_UART_OVERRUN_ERROR                                          UCOE
#define USCI_A_UART_PARITY_ERROR                                           UCPE
#define USCI_A_UART_BREAK_DETECT                                          UCBRK
#define USCI_A_UART_RECEIVE_ERROR                                       UCRXERR
#define USCI_A_UART_ADDRESS_RECEIVED                                     UCADDR
#define USCI_A_UART_IDLELINE                                             UCIDLE
#define USCI_A_UART_BUSY                                                 UCBUSY

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern bool USCI_A_UART_initAdvance(uint32_t baseAddress,
                                    uint8_t selectClockSource,
                                    uint16_t clockPrescalar,
                                    uint8_t firstModReg,
                                    uint8_t secondModReg,
                                    uint8_t parity,
                                    uint8_t msborLsbFirst,
                                    uint8_t numberofStopBits,
                                    uint8_t uartMode,
                                    uint8_t overSampling);

extern void USCI_A_UART_transmitData(uint32_t baseAddress,
                                     uint8_t transmitData);

extern uint8_t USCI_A_UART_receiveData(uint32_t baseAddress);

extern void USCI_A_UART_enableInterrupt(uint32_t baseAddress,
                                        uint8_t mask);

extern void USCI_A_UART_disableInterrupt(uint32_t baseAddress,
                                         uint8_t mask);

extern uint8_t USCI_A_UART_getInterruptStatus(uint32_t baseAddress,
                                              uint8_t mask);

extern void USCI_A_UART_clearInterruptFlag(uint32_t baseAddress,
                                           uint8_t mask);

extern void USCI_A_UART_enable(uint32_t baseAddress);

extern void USCI_A_UART_disable(uint32_t baseAddress);

extern uint8_t USCI_A_UART_queryStatusFlags(uint32_t baseAddress,
                                            uint8_t mask);

extern void USCI_A_UART_setDormant(uint32_t baseAddress);

extern void USCI_A_UART_resetDormant(uint32_t baseAddress);

extern void USCI_A_UART_transmitAddress(uint32_t baseAddress,
                                        uint8_t transmitAddress);

extern void USCI_A_UART_transmitBreak(uint32_t baseAddress);

extern uint32_t USCI_A_UART_getReceiveBufferAddressForDMA(uint32_t baseAddress);

extern uint32_t USCI_A_UART_getTransmitBufferAddressForDMA(uint32_t baseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_USCI_A_UART_H__
