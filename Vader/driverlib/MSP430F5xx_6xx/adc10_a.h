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
// adc10_a.h - Driver for the ADC10_A Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_ADC10_A_H__
#define __MSP430WARE_ADC10_A_H__

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

#ifdef __MSP430_HAS_ADC10_A__

//*****************************************************************************
//
// The following are values that can be passed to the clockSourceSelect
// parameter for functions: ADC10_A_init().
//
//*****************************************************************************
#define ADC10_A_CLOCKSOURCE_ADC10OSC                              (ADC10SSEL_0)
#define ADC10_A_CLOCKSOURCE_ACLK                                  (ADC10SSEL_1)
#define ADC10_A_CLOCKSOURCE_MCLK                                  (ADC10SSEL_2)
#define ADC10_A_CLOCKSOURCE_SMCLK                                 (ADC10SSEL_3)

//*****************************************************************************
//
// The following are values that can be passed to the clockSourceDivider
// parameter for functions: ADC10_A_init().
//
//*****************************************************************************
#define ADC10_A_CLOCKDIVIDER_1                       (ADC10DIV_0 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_2                       (ADC10DIV_1 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_3                       (ADC10DIV_2 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_4                       (ADC10DIV_3 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_5                       (ADC10DIV_4 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_6                       (ADC10DIV_5 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_7                       (ADC10DIV_6 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_8                       (ADC10DIV_7 + ADC10PDIV_0)
#define ADC10_A_CLOCKDIVIDER_12                      (ADC10DIV_2 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_16                      (ADC10DIV_3 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_20                      (ADC10DIV_4 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_24                      (ADC10DIV_5 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_28                      (ADC10DIV_6 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_32                      (ADC10DIV_7 + ADC10PDIV_1)
#define ADC10_A_CLOCKDIVIDER_64                      (ADC10DIV_0 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_128                     (ADC10DIV_1 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_192                     (ADC10DIV_2 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_256                     (ADC10DIV_3 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_320                     (ADC10DIV_4 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_384                     (ADC10DIV_5 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_448                     (ADC10DIV_6 + ADC10PDIV_2)
#define ADC10_A_CLOCKDIVIDER_512                     (ADC10DIV_7 + ADC10PDIV_2)

//*****************************************************************************
//
// The following are values that can be passed to the
// sampleHoldSignalSourceSelect parameter for functions: ADC10_A_init().
//
//*****************************************************************************
#define ADC10_A_SAMPLEHOLDSOURCE_SC                                (ADC10SHS_0)
#define ADC10_A_SAMPLEHOLDSOURCE_1                                 (ADC10SHS_1)
#define ADC10_A_SAMPLEHOLDSOURCE_2                                 (ADC10SHS_2)
#define ADC10_A_SAMPLEHOLDSOURCE_3                                 (ADC10SHS_3)

//*****************************************************************************
//
// The following are values that can be passed to the multipleSamplesEnabled
// parameter for functions: ADC10_A_setupSamplingTimer().
//
//*****************************************************************************
#define ADC10_A_MULTIPLESAMPLESDISABLE                            (!(ADC10MSC))
#define ADC10_A_MULTIPLESAMPLESENABLE                                (ADC10MSC)

//*****************************************************************************
//
// The following are values that can be passed to the clockCycleHoldCount
// parameter for functions: ADC10_A_setupSamplingTimer().
//
//*****************************************************************************
#define ADC10_A_CYCLEHOLD_4_CYCLES                                 (ADC10SHT_0)
#define ADC10_A_CYCLEHOLD_8_CYCLES                                 (ADC10SHT_1)
#define ADC10_A_CYCLEHOLD_16_CYCLES                                (ADC10SHT_2)
#define ADC10_A_CYCLEHOLD_32_CYCLES                                (ADC10SHT_3)
#define ADC10_A_CYCLEHOLD_64_CYCLES                                (ADC10SHT_4)
#define ADC10_A_CYCLEHOLD_96_CYCLES                                (ADC10SHT_5)
#define ADC10_A_CYCLEHOLD_128_CYCLES                               (ADC10SHT_6)
#define ADC10_A_CYCLEHOLD_192_CYCLES                               (ADC10SHT_7)
#define ADC10_A_CYCLEHOLD_256_CYCLES                               (ADC10SHT_8)
#define ADC10_A_CYCLEHOLD_384_CYCLES                               (ADC10SHT_9)
#define ADC10_A_CYCLEHOLD_512_CYCLES                              (ADC10SHT_10)
#define ADC10_A_CYCLEHOLD_768_CYCLES                              (ADC10SHT_11)
#define ADC10_A_CYCLEHOLD_1024_CYCLES                             (ADC10SHT_12)

//*****************************************************************************
//
// The following are values that can be passed to the
// positiveRefVoltageSourceSelect parameter for functions:
// ADC10_A_memoryConfigure().
//
//*****************************************************************************
#define ADC10_A_VREFPOS_AVCC                       (!(ADC10SREF0 + ADC10SREF1))
#define ADC10_A_VREFPOS_EXT                                        (ADC10SREF1)
#define ADC10_A_VREFPOS_INT                                        (ADC10SREF0)

//*****************************************************************************
//
// The following are values that can be passed to the inputSourceSelect
// parameter for functions: ADC10_A_memoryConfigure().
//
//*****************************************************************************
#define ADC10_A_INPUT_A0                                          (ADC10INCH_0)
#define ADC10_A_INPUT_A1                                          (ADC10INCH_1)
#define ADC10_A_INPUT_A2                                          (ADC10INCH_2)
#define ADC10_A_INPUT_A3                                          (ADC10INCH_3)
#define ADC10_A_INPUT_A4                                          (ADC10INCH_4)
#define ADC10_A_INPUT_A5                                          (ADC10INCH_5)
#define ADC10_A_INPUT_A6                                          (ADC10INCH_6)
#define ADC10_A_INPUT_A7                                          (ADC10INCH_7)
#define ADC10_A_INPUT_A8                                          (ADC10INCH_8)
#define ADC10_A_INPUT_A9                                          (ADC10INCH_9)
#define ADC10_A_INPUT_TEMPSENSOR                                 (ADC10INCH_10)
#define ADC10_A_INPUT_BATTERYMONITOR                             (ADC10INCH_11)
#define ADC10_A_INPUT_A12                                        (ADC10INCH_12)
#define ADC10_A_INPUT_A13                                        (ADC10INCH_13)
#define ADC10_A_INPUT_A14                                        (ADC10INCH_14)
#define ADC10_A_INPUT_A15                                        (ADC10INCH_15)

//*****************************************************************************
//
// The following are values that can be passed to the
// negativeRefVoltageSourceSelect parameter for functions:
// ADC10_A_memoryConfigure().
//
//*****************************************************************************
#define ADC10_A_VREFNEG_AVSS                                    (!(ADC10SREF2))
#define ADC10_A_VREFNEG_EXT                                        (ADC10SREF2)

//*****************************************************************************
//
// The following are values that can be passed to the interruptMask parameter
// for functions: ADC10_A_enableInterrupt(), and ADC10_A_disableInterrupt().
//
//*****************************************************************************
#define ADC10_A_TIMEOVERFLOW_INT                                   (ADC10TOVIE)
#define ADC10_A_OVERFLOW_INT                                        (ADC10OVIE)
#define ADC10_A_ABOVETHRESHOLD_INT                                  (ADC10HIIE)
#define ADC10_A_BELOWTHRESHOLD_INT                                  (ADC10LOIE)
#define ADC10_A_INSIDEWINDOW_INT                                    (ADC10INIE)
#define ADC10_A_COMPLETED_INT                                        (ADC10IE0)

//*****************************************************************************
//
// The following are values that can be passed to the interruptFlagMask
// parameter for functions: ADC10_A_clearInterrupt(), and
// ADC10_A_getInterruptStatus().
//
//*****************************************************************************
#define ADC10_A_TIMEOVERFLOW_INTFLAG                              (ADC10TOVIFG)
#define ADC10_A_OVERFLOW_INTFLAG                                   (ADC10OVIFG)
#define ADC10_A_ABOVETHRESHOLD_INTFLAG                             (ADC10HIIFG)
#define ADC10_A_BELOWTHRESHOLD_INTFLAG                             (ADC10LOIFG)
#define ADC10_A_INSIDEWINDOW_INTFLAG                               (ADC10INIFG)
#define ADC10_A_COMPLETED_INTFLAG                                   (ADC10IFG0)

//*****************************************************************************
//
// The following are values that can be passed to the
// conversionSequenceModeSelect parameter for functions:
// ADC10_A_startConversion().
//
//*****************************************************************************
#define ADC10_A_SINGLECHANNEL                                   (ADC10CONSEQ_0)
#define ADC10_A_SEQOFCHANNELS                                   (ADC10CONSEQ_1)
#define ADC10_A_REPEATED_SINGLECHANNEL                          (ADC10CONSEQ_2)
#define ADC10_A_REPEATED_SEQOFCHANNELS                          (ADC10CONSEQ_3)

//*****************************************************************************
//
// The following are values that can be passed to the preempt parameter for
// functions: ADC10_A_disableConversions().
//
//*****************************************************************************
#define ADC10_A_COMPLETECONVERSION                                        false
#define ADC10_A_PREEMPTCONVERSION                                          true

//*****************************************************************************
//
// The following are values that can be passed to the resolutionSelect
// parameter for functions: ADC10_A_setResolution().
//
//*****************************************************************************
#define ADC10_A_RESOLUTION_8BIT                                   (!(ADC10RES))
#define ADC10_A_RESOLUTION_10BIT                                     (ADC10RES)

//*****************************************************************************
//
// The following are values that can be passed to the invertedSignal parameter
// for functions: ADC10_A_setSampleHoldSignalInversion().
//
//*****************************************************************************
#define ADC10_A_NONINVERTEDSIGNAL                                (!(ADC10ISSH))
#define ADC10_A_INVERTEDSIGNAL                                      (ADC10ISSH)

//*****************************************************************************
//
// The following are values that can be passed to the readBackFormat parameter
// for functions: ADC10_A_setDataReadBackFormat().
//
//*****************************************************************************
#define ADC10_A_UNSIGNED_BINARY                                    (!(ADC10DF))
#define ADC10_A_SIGNED_2SCOMPLEMENT                                   (ADC10DF)

//*****************************************************************************
//
// The following are values that can be passed to the samplingRateSelect
// parameter for functions: ADC10_A_setReferenceBufferSamplingRate().
//
//*****************************************************************************
#define ADC10_A_MAXSAMPLINGRATE_200KSPS                            (!(ADC10SR))
#define ADC10_A_MAXSAMPLINGRATE_50KSPS                                (ADC10SR)

//*****************************************************************************
//
// The following are values that can be passed toThe following are values that
// can be returned by the ADC10_A_isBusy() function.
//
//*****************************************************************************
#define ADC10_A_BUSY                                                  ADC10BUSY
#define ADC10_A_NOTBUSY                                                    0x00

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern bool ADC10_A_init(uint32_t baseAddress,
                         uint16_t sampleHoldSignalSourceSelect,
                         uint8_t clockSourceSelect,
                         uint16_t clockSourceDivider);

extern void ADC10_A_enable(uint32_t baseAddress);

extern void ADC10_A_disable(uint32_t baseAddress);

extern void ADC10_A_setupSamplingTimer(uint32_t baseAddress,
                                       uint16_t clockCycleHoldCount,
                                       uint16_t multipleSamplesEnabled);

extern void ADC10_A_disableSamplingTimer(uint32_t baseAddress);

extern void ADC10_A_memoryConfigure(uint32_t baseAddress,
                                    uint8_t inputSourceSelect,
                                    uint8_t positiveRefVoltageSourceSelect,
                                    uint8_t negativeRefVoltageSourceSelect);

extern void ADC10_A_enableInterrupt(uint32_t baseAddress,
                                    uint8_t interruptMask);

extern void ADC10_A_disableInterrupt(uint32_t baseAddress,
                                     uint8_t interruptMask);

extern void ADC10_A_clearInterrupt(uint32_t baseAddress,
                                   uint8_t interruptFlagMask);

extern uint8_t ADC10_A_getInterruptStatus(uint32_t baseAddress,
                                          uint8_t interruptFlagMask);

extern void ADC10_A_startConversion(uint32_t baseAddress,
                                    uint8_t conversionSequenceModeSelect);

extern void ADC10_A_disableConversions(uint32_t baseAddress,
                                       bool preempt);

extern int16_t ADC10_A_getResults(uint32_t baseAddress);

extern void ADC10_A_setResolution(uint32_t baseAddress,
                                  uint8_t resolutionSelect);

extern void ADC10_A_setSampleHoldSignalInversion(uint32_t baseAddress,
                                                 uint16_t invertedSignal);

extern void ADC10_A_setDataReadBackFormat(uint32_t baseAddress,
                                          uint16_t readBackFormat);

extern void ADC10_A_enableReferenceBurst(uint32_t baseAddress);

extern void ADC10_A_disableReferenceBurst(uint32_t baseAddress);

extern void ADC10_A_setReferenceBufferSamplingRate(uint32_t baseAddress,
                                                   uint16_t samplingRateSelect);

extern void ADC10_A_setWindowComp(uint32_t baseAddress,
                                  uint16_t highThreshold,
                                  uint16_t lowThreshold);

extern uint32_t ADC10_A_getMemoryAddressForDMA(uint32_t baseAddress);

extern uint16_t ADC10_A_isBusy(uint32_t baseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_ADC10_A_H__
