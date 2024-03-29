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
/** @file usb.c
 *  @brief Contains APIs related to handling of Control Endpoint
 */
/* 
 * ======== usb.c ========
 */
/*----------------------------------------------------------------------------+
 | Include files                                                               |
 +----------------------------------------------------------------------------*/

//
//! \cond
//
#include <string.h>
#include <stdint.h>

#include <string.h>
#include "inc/hw_memmap.h"
#include "ucs.h"
#include "tlv.h"
#include "gpio.h"

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"    //Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"      //USB-specific Data Structures
#include "../USB_CDC_API/UsbCdc.h"
#include "../USB_PHDC_API/UsbPHDC.h"
#include "../USB_HID_API/UsbHidReq.h"
#include "../USB_MSC_API/UsbMscScsi.h"
#include <descriptors.h>

#include <string.h>

/*----------------------------------------------------------------------------+
 | Internal Constant Definition                                               |
 +----------------------------------------------------------------------------*/
#define NO_MORE_DATA    0xFFFF
#define EPBCT_NAK       0x80
#define EPCNF_TOGLE     0x20

#define DIRECTION_IN    0x80
#define DIRECTION_OUT   0x00

/*----------------------------------------------------------------------------+
 | Internal Variables                                                          |
 +----------------------------------------------------------------------------*/

static BYTE bConfigurationNumber;   //Set to 1 when USB device has been
//configured, set to 0 when unconfigured

static BYTE bInterfaceNumber;       //interface number

WORD wBytesRemainingOnIEP0;         //For endpoint zero transmitter only
                                    //Holds count of bytes remaining to be
                                    //transmitted by endpoint 0.  A value
                                    //of 0 means that a 0-length data packet
                                    //A value of 0xFFFF means that transfer
                                    //is complete.

WORD wBytesRemainingOnOEP0;         //For endpoint zero transmitter only
                                    //Holds count of bytes remaining to be
                                    //received by endpoint 0.  A value
                                    //of 0 means that a 0-length data packet
                                    //A value of 0xFFFF means that transfer
                                    //is complete.

static PBYTE pbIEP0Buffer;          //A buffer pointer to input end point 0
                                    //Data sent back to host is copied from
                                    //this pointed memory location

static PBYTE pbOEP0Buffer;          //A buffer pointer to output end point 0
                                    //Data sent from host is copied to
                                    //this pointed memory location

static BYTE bHostAskMoreDataThanAvailable = 0;

BYTE abUsbRequestReturnData[USB_RETURN_DATA_LENGTH];
BYTE abUsbRequestIncomingData[USB_RETURN_DATA_LENGTH];

__no_init BYTE abramSerialStringDescriptor[34];

BYTE bStatusAction;
BYTE bFunctionSuspended = FALSE;    //TRUE if function is suspended
BYTE bEnumerationStatus = 0;        //is 0 if not enumerated

static BYTE bRemoteWakeup;

WORD wUsbEventMask;                 //used by USB_getEnabledEvents() and USB_setEnabledEvents()

#ifdef _MSC_
extern BYTE USBMSC_reset (void);
void MscResetData ();
extern struct _MscState MscState;
#endif

#ifdef NON_COMPOSITE_MULTIPLE_INTERFACES

extern const void *usbConfigurationDescriptors[];
extern const void *usbDeviceDescriptors[];
extern const BYTE usbConfigurationsSizes[];
BYTE activeInterfaceIndex = 0;

#endif

/*----------------------------------------------------------------------------+
 | Global Variables                                                            |
 +----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
 | Hardware Related Structure Definition                                       |
 +----------------------------------------------------------------------------*/

#ifdef __IAR_SYSTEMS_ICC__

#pragma location = 0x2380
__no_init tDEVICE_REQUEST __data16 tSetupPacket;

#pragma location = 0x0920
__no_init tEDB0 __data16 tEndPoint0DescriptorBlock;

#pragma location = 0x23C8
__no_init tEDB __data16 tInputEndPointDescriptorBlock[7];

#pragma location = 0x2388
__no_init tEDB __data16 tOutputEndPointDescriptorBlock[7];

#pragma location = 0x2378
__no_init BYTE __data16 abIEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = 0x2370
__no_init BYTE __data16 abOEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = OEP1_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = OEP1_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = OEP3_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];

#pragma location = OEP3_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp3[EP_MAX_PACKET_SIZE];

#pragma location = IEP3_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp83[EP_MAX_PACKET_SIZE];

#pragma location = IEP3_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp83[EP_MAX_PACKET_SIZE];

#pragma location = OEP4_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp4[EP_MAX_PACKET_SIZE];

#pragma location = OEP4_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp4[EP_MAX_PACKET_SIZE];

#pragma location = IEP4_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp84[EP_MAX_PACKET_SIZE];

#pragma location = IEP4_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp84[EP_MAX_PACKET_SIZE];

#pragma location = OEP5_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp5[EP_MAX_PACKET_SIZE];

#pragma location = OEP5_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp5[EP_MAX_PACKET_SIZE];

#pragma location = IEP5_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp85[EP_MAX_PACKET_SIZE];

#pragma location = IEP5_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp85[EP_MAX_PACKET_SIZE];

#pragma location = OEP6_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp6[EP_MAX_PACKET_SIZE];

#pragma location = OEP6_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp6[EP_MAX_PACKET_SIZE];

#pragma location = IEP6_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp86[EP_MAX_PACKET_SIZE];

#pragma location = IEP6_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp86[EP_MAX_PACKET_SIZE];

#pragma location = OEP7_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp7[EP_MAX_PACKET_SIZE];

#pragma location = OEP7_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp7[EP_MAX_PACKET_SIZE];

#pragma location = IEP7_X_BUFFER_ADDRESS
__no_init BYTE __data16 pbXBufferAddressEp87[EP_MAX_PACKET_SIZE];

#pragma location = IEP7_Y_BUFFER_ADDRESS
__no_init BYTE __data16 pbYBufferAddressEp87[EP_MAX_PACKET_SIZE];



#endif

#ifdef __TI_COMPILER_VERSION__
extern __no_init tDEVICE_REQUEST tSetupPacket;
extern __no_init tEDB0 tEndPoint0DescriptorBlock;
extern __no_init tEDB tInputEndPointDescriptorBlock[7];
extern __no_init tEDB tOutputEndPointDescriptorBlock[7];
extern __no_init BYTE abIEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE abOEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp3[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp83[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp83[EP_MAX_PACKET_SIZE];

extern __no_init BYTE pbXBufferAddressEp4[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp4[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp84[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp84[EP_MAX_PACKET_SIZE];

extern __no_init BYTE pbXBufferAddressEp5[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp5[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp85[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp85[EP_MAX_PACKET_SIZE];

#endif

VOID CdcResetData ();
VOID HidResetData ();
VOID PHDCResetData();

VOID USB_InitSerialStringDescriptor (VOID);
VOID USB_initMemcpy (VOID);
WORD USB_determineFreq(VOID);

/* Version string to embed in executable. May need to change for ELF compiler */
const char *VERSION = "USB_DEVELOPERS_PACKAGE_4_00_00";
char *USB_getVersion(void)
{
	return ((char *)&VERSION);
}

//
//! \endcond
//

//*****************************************************************************
//
//! Initializes the USB Module.
//!
//! 
//! Initializes the USB module by configuring power and clocks, and configures
//! pins that are critical for USB. This should be called very soon after the 
//! beginning of program execution. 
//! 
//! Note that this does not enable the USB module (that is, does not set 
//! USB_EN bit). Rather, it prepares the USB module to detect the application of
//! power to VBUS, after which the application may choose to enable the module
//! and connect to USB. Calling this function is necessary to achieve expected 
//! LPM3 current consumption into DVCC.
//!
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_init (VOID)
{
    WORD bGIE  = __get_SR_register() & GIE;                                 //save interrupt status
    WORD MCLKFreq = USB_determineFreq();
    WORD DelayConstant_250us = ((MCLKFreq >> 6) + (MCLKFreq >> 7) + (MCLKFreq >> 9));
    volatile unsigned int i, j;

    char *(*fp)(void);

	/* force version string into executable */
    fp = &USB_getVersion;
    fp();

    //atomic operation - disable interrupts
    __disable_interrupt();                                                      //Disable global interrupts

    //configuration of USB module
    USBKEYPID   =     0x9628;                                                   //set KEY and PID to 0x9628 -> access to
                                                                                //configuration registers enabled
    /* To fix USB9 enumeration issue */
    USBPWRCTL = 0;
	
    USBPHYCTL   =     PUSEL;                                                    //use DP and DM as USB terminals (not needed
                                                                                //because an external PHY is connected to port
                                                                                //9)
    if(USB_USE_INTERNAL_3V3LDO == TRUE)
        USBPWRCTL   =     VUSBEN + SLDOAON;                                     //enable primary and secondary LDO (3.3 and 1.8V)
    else
        USBPWRCTL   =     SLDOAON;                                         		//enable primary LDO (3.3V)

    for (j = 0; j < 20; j++) {
        for (i = 0; i < (DelayConstant_250us); i++) {//wait some time for LDOs (5ms delay)
            _NOP();
        }
    }

    USBPWRCTL   |=   VBONIE;                                  					//enable interrupt VBUSon
    USBKEYPID   =    0x9600;                                                    //access to configuration registers disabled

    //reset events mask
    wUsbEventMask = 0;

    //init Serial Number
#if (USB_STR_INDEX_SERNUM != 0)
    USB_InitSerialStringDescriptor();
#endif

    //init memcpy() function: DMA or non-DMA
    USB_initMemcpy();
#ifdef _MSC_
    MscResetCtrlLun();
#endif

#ifdef BRIDGE_CDC_PRESENT
    USBCDC_setupDMA_Bridge();
#endif
    
    __bis_SR_register(bGIE);                                                    //restore interrupt status
    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Initializes the USB Module. Also enables events and connects.
//!
//! 
//! Initializes the USB module by configuring power and clocks, and configures
//! pins that are critical for USB. This should be called very soon after the 
//! beginning of program execution. 
//!
//! If connectEnable is TRUE, then this API then enables the USB module, which 
//! includes activating the PLL and setting the USB_EN bit. AFter enabling the
//! USB module, this API will connect to the host if VBUS is present.
//!
//! If eventsEnable is set to TRUE then all USB events are enabled by this API.
//!
//! \param	connectEnable	If TRUE, Connect to host if VBUS is present by 
//!							pulling the D+ signal high using the PUR pin.
//! \param  eventsEnable	If TRUE, all USB events handlers are enabled
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_setup(BYTE connectEnable, BYTE eventsEnable)
{
	uint8_t status;

	status = USB_init();

	if (eventsEnable) {
		USB_setEnabledEvents(kUSB_allUsbEvents);
	}
	if (connectEnable) {
		if (USB_connectionInfo() & kUSB_vbusPresent){
			if (USB_enable() == kUSB_succeed){
				USB_reset();
				USB_connect();
			}
		}
	}

    return (status);
}

//
//! \cond
//

//----------------------------------------------------------------------------
//This function will be compiled only if
#if (USB_STR_INDEX_SERNUM != 0)
VOID USB_InitSerialStringDescriptor (VOID)
{
    BYTE i,j,hexValue;
    PBYTE pbSerNum;
    BYTE bBytes;

    j = 1;                                                                      //we start with second byte, first byte (lenght)
                                                                                //will be filled later
    pbSerNum = 0;
    abramSerialStringDescriptor[j++] = DESC_TYPE_STRING;

    //TLV access Function Call
    TLV_getInfo(TLV_TAG_DIERECORD, 0,
        (uint8_t *)&bBytes, (uint16_t **)&pbSerNum);
    if (bBytes == 0){                                                           //no serial number available
        //use 00 as serial number = no serial number available
        abramSerialStringDescriptor[0] = 4;                                     //length
        abramSerialStringDescriptor[j++] = 0;                                   //no serial number available
        abramSerialStringDescriptor[j++] = 0;                                   //no serial number available
    } else {
        for (i = 0; (i < bBytes) && (i < 8); i++,pbSerNum++)
        {
            hexValue = (*pbSerNum & 0xF0) >> 4;
            if (hexValue < 10){
                abramSerialStringDescriptor[j++] = (hexValue + '0');
            } else {          abramSerialStringDescriptor[j++] = (hexValue + 55);}
            abramSerialStringDescriptor[j++] = 0x00;                            //needed for UNI-Code

            hexValue = (*pbSerNum & 0x0F);
            if (hexValue < 10){
                abramSerialStringDescriptor[j++] = (hexValue + '0');
            } else {          abramSerialStringDescriptor[j++] = (hexValue + 55);}
            abramSerialStringDescriptor[j++] = 0x00;                            //needed for UNI-Code
        }
        abramSerialStringDescriptor[0] = i * 4 + 2;                             //calculate the length
    }
}

#endif

//
//! \endcond
//

//*****************************************************************************
//
//! Enables the USB Module.
//!
//! Enables the USB module, which includes activating the PLL and setting the 
//! USB_EN bit. Power consumption increases as a result of this operation (see 
//! device datasheet for specifics). This call should only be made after an 
//! earlier call to USB_init(), and prior to any other call except than 
//! USB_setEnabledEvents(), or USB_getEnabledEvents(). It is usually called just
//! prior to attempting to connect with a host after a bus connection has 
//! already been detected.
//! 
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_enable ()
{
    volatile unsigned int i, k;
    volatile unsigned int j = 0;
    uint16_t status;
    WORD MCLKFreq = USB_determineFreq();
    WORD DelayConstant_250us = ((MCLKFreq >> 6) + (MCLKFreq >> 7) + (MCLKFreq >> 9));

    if (!(USBPWRCTL & USBBGVBV)){                                               //check USB Bandgap and VBUS valid
        return (kUSB_generalError);
    }

    if ((USBCNF & USB_EN) &&
        (USBPLLCTL & UPLLEN)){
        return (kUSB_succeed);                                                  //exit if PLL is already enalbed
    }

#if defined (__MSP430F552x) || defined (__MSP430F550x)
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN3);
#elif defined (__MSP430F563x_F663x)
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN2);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN3);
#endif
    USBKEYPID = 0x9628;                                                         //set KEY and PID to 0x9628 -> access to
                                                                                //configuration registers enabled
    if (USB_XT_FREQ_VALUE >= 24) {
    	status = UCS_XT2StartWithTimeout(UCS_BASE,
    			XT2DRIVE_3, 50000);
    }
    else if(USB_XT_FREQ_VALUE >= 16) {
    	status = UCS_XT2StartWithTimeout(UCS_BASE,
    			XT2DRIVE_2, 50000);
    }
    else if(USB_XT_FREQ_VALUE >= 8) {
    	status = UCS_XT2StartWithTimeout(UCS_BASE,
    			XT2DRIVE_1, 50000);
    }
    else {
    	status = UCS_XT2StartWithTimeout(UCS_BASE,
    			XT2DRIVE_0, 50000);
    }

	if (status == STATUS_FAIL) {
		return (kUSB_generalError);
	}
	
    USBPLLDIVB = USB_XT_FREQ;                                                   //Settings desired frequency

    if (USB_PLL_XT == 2){
        USBPLLCTL = UPFDEN + UPLLEN;                                    		//Select XT2 as Ref / Select PLL for USB / Discrim.
                                                                                //on, enable PLL
    } else {
        USBPLLCTL = UPFDEN + UPLLEN;                                            //Select XT1 as Ref / Select PLL for USB /
                                                                                //Discrim. on, enable PLL
    }

    //Wait some time till PLL is settled
    do
    {
        USBPLLIR    =     0x0000;                                               //make sure no interrupts can occur on
                                                                                //PLL-module

#ifdef __MSP430F6638
        //wait 1 ms till enable USB
        for(k = 0; k < 4; k++)
        {
            for (i = 0; i < (DelayConstant_250us); i++){
                _NOP();
            }
        }
#else
        //wait 1/2 ms till enable USB
        for(k = 0; k < 2; k++)
        {
            for (i = 0; i < (DelayConstant_250us); i++){
                _NOP();
            }
        }        
#endif

        if (j++ > 10){
            USBKEYPID   =    0x9600;                                            //access to configuration registers disabled
            return (kUSB_generalError);
        }
    } while (USBPLLIR != 0);

    USBCNF     |=    USB_EN;                                                    //enable USB module
    USBKEYPID   =    0x9600;                                                    //access to configuration registers disabled
    return (kUSB_succeed);
}

#ifdef USE_TIMER_FOR_RESUME

//*****************************************************************************
//
//! First phase of enabling the USB Module when USE_TIMER_FOR_RESUME is defined
//!
//! This functions is only used by USB_resume to reduce the interrupt latency
//! of the resume interrupt.
//! This function starts the XT2 crystal and then calls an event handler
//! USB_handleCrystalStartedEvent() to allow the application to get control. The
//! application can use a timer or other peripheral to "wait" for the XT2
//! crystal to stabilize. See the crystal datasheet for typical wait times.
//! The application then informs the stack of XT2
//! stabilization by calling USB_enable_PLL().
//!
//! \return \b kUSB_succeed or kUSB_generalError
//
//*****************************************************************************

BYTE USB_enable_crystal (void)
{
    volatile unsigned int i, k;
    volatile unsigned int j = 0;

    if (!(USBPWRCTL & USBBGVBV)){                                               //check USB Bandgap and VBUS valid
        return (kUSB_generalError);
    }

    if ((USBCNF & USB_EN) &&
        (USBPLLCTL & UPLLEN)){
        return (kUSB_succeed);                                                  //exit if PLL is already enalbed
    }

#if defined (__MSP430F552x) || defined (__MSP430F550x)
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN3);
#elif defined (__MSP430F563x_F663x)
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN2);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN3);
#endif
    USBKEYPID = 0x9628;                                                         //set KEY and PID to 0x9628 -> access to
                                                                                //configuration registers enabled
    if (USB_XT_FREQ_VALUE >= 24) {
    	UCS_XT2StartWithTimeout(UCS_BASE, XT2DRIVE_3, 1);
    }
    else if(USB_XT_FREQ_VALUE >= 16) {
    	UCS_XT2StartWithTimeout(UCS_BASE, XT2DRIVE_2, 1);
    }
    else if(USB_XT_FREQ_VALUE >= 8) {
    	UCS_XT2StartWithTimeout(UCS_BASE, XT2DRIVE_1, 1);
    }
    else {
    	UCS_XT2StartWithTimeout(UCS_BASE, XT2DRIVE_0, 1);
    }

    USB_handleCrystalStartedEvent();

    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Second phase of enabling the USB Module when USE_TIMER_FOR_RESUME is defined
//!
//! This functions is only used by USB_resume to reduce the interrupt latency
//! of the resume interrupt.
//! This function starts the PLL and then calls an event handler
//! USB_handlePLLStartedEvent() to allow the application to get control. The
//! application can use a timer or other peripheral to "wait" for the USB PLL
//! to stabilize. See the datasheet for typical PLL wait times.
//! The application then informs the stack of XT2
//! stabilization by calling USB_enable_final().
//!
//! \return \b kUSB_succeed or kUSB_generalError
//
//*****************************************************************************
void USB_enable_PLL(void)
{
    USBPLLDIVB = USB_XT_FREQ;                                                   //Settings desired frequency

    if (USB_PLL_XT == 2){
        USBPLLCTL = UPFDEN + UPLLEN;                                    		//Select XT2 as Ref / Select PLL for USB / Discrim.
                                                                                //on, enable PLL
    } else {
        USBPLLCTL = UPFDEN + UPLLEN;                                            //Select XT1 as Ref / Select PLL for USB /
                                                                                //Discrim. on, enable PLL
    }

    USB_handlePLLStartedEvent();
}

//*****************************************************************************
//
//! Final phase of enabling the USB Module when USE_TIMER_FOR_RESUME is defined
//!
//! This function is only used by USB_resume to reduce the interrupt latency
//! of the resume interrupt.
//! This function gets called by the application when thye USB PLL has stabilized
//! to allow the resume process to finish.
//!
//
//*****************************************************************************
void USB_enable_final(void)
{
    USBCNF     |=    USB_EN;                                                    //enable USB module
    USBKEYPID   =    0x9600;                                                    //access to configuration registers disabled
    USBIFG &= ~(RESRIFG | SUSRIFG);     //clear interrupt flags
    USBIE = SETUPIE | RSTRIE | SUSRIE;  //enable USB specific interrupts (setup, reset, suspend)

    bFunctionSuspended  = FALSE;
}

#endif

//*****************************************************************************
//
//! Disables the USB Module and PLL.
//!
//!
//! Disables the USB module and PLL. If USB is not enabled when this call is 
//! made, no error is returned - the call simply exits with success.
//! 
//! If a handleVbusOffEvent() occurs, or if USB_connectionState() begins 
//! returning ST_USB_DISCONNECTED, this function should be called (following a 
//! call to USB_disconnect()), in order to avoid unnecessary current draw.
//!
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_disable (VOID)
{
    USBKEYPID = 0x9628;                                                         //set KEY and PID to 0x9628 -> access to
                                                                                //configuration registers enabled
    USBCNF    = 0;                                                              //disable USB module
    USBPLLCTL &= ~UPLLEN;                                                       //disable PLL
    USBKEYPID = 0x9600;                                                         //access to configuration registers disabled
    bEnumerationStatus = 0x00;                                                  //device is not enumerated
    bFunctionSuspended = FALSE;                                                 //device is not suspended
    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Enables/Disables the Various USB Events.
//!
//! \param events is the mask for what is to be enabled/disabled.
//!       - Valid values are:
//!        		- \b kUSB_clockFaultEvent
//!        		- \b kUSB_VbusOnEvent
//!        		- \b kUSB_VbusOffEvent
//!        		- \b kUSB_UsbResetEvent
//!        		- \b kUSB_UsbSuspendEvent
//!        		- \b kUSB_UsbResumeEvent
//!        		- \b kUSBCDC_dataReceivedEvent
//!        		- \b kUSBCDC_sendCompletedEvent
//!        		- \b kUSBCDC_receiveCompletedEvent
//!        		- \b kUSBHID_dataReceivedEvent
//!        		- \b kUSBHID_sendCompletedEvent
//!        		- \b kUSBHID_receiveCompletedEvent
//!        		- \b kUSB_allUsbEvents
//!
//! Enables/disables various USB events. Within the events byte, all bits with
//! '1' values will be enabled, and all bits with '0' values will be disabled.
//! (There are no bit-wise operations). By default (that is, prior to any call 
//! to this function), all events are disabled.
//! 
//! The status of event enabling can be read with the USB_getEnabledEvents() 
//! function. This call can be made at any time after a call to USB_init().
//! 
//! USB_setEnabledEvents() can be thought of in a similar fashion to 
//! setting/clearing interrupt enable bits. The only benefit in keeping an event 
//! disabled is to save the unnecessary execution cycles incurred from running 
//! an "empty" event handler.
//! 
//! The mask constant \b kUSB_allUsbEvents is used to enable/disable all events 
//! pertaining to core USB functions; in other words, it enables all those with 
//! a \b kUSB_ prefix. 
//! 
//! See Sec. 10 of \e "Programmer's Guide: MSP430 USB API Stack for CDC/PHDC/HID/MSC" for more information about
//! events.
//! 
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_setEnabledEvents (WORD events)
{
    wUsbEventMask = events;
    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Returns Which Events are Enabled/Disabled.
//!
//! Returns which events are enabled and which are disabled. The definition of 
//! events is the same as for USB_enableEvents() above.
//! 
//! If the bit is set, the event is enabled. If cleared, the event is disabled. 
//! By default (that is, prior to calling USB_setEnabledEvents() ), all events 
//! are disabled. This call can be made at any time after a call to USB_init().
//! 
//! \return \b Events
//
//*****************************************************************************

WORD USB_getEnabledEvents ()
{
    return (wUsbEventMask);
}

//*****************************************************************************
//
//! Resets the USB Module and the Internal State of the API.
//!
//! Resets the USB module and also the internal state of the API. The interrupt 
//! register is cleared to make sure no interrupts are pending. If the device 
//! had been enumerated, the enumeration is now lost. All open send/receive 
//! operations are aborted. 
//! 
//! This function is most often called immediately before a call to 
//! USB_connect(). It should not be called prior to USB_enable().
//!
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_reset ()
{
    int i;

    USBKEYPID = 0x9628;                                                             //set KEY and PID to 0x9628 -> access to
                                                                                    //configuration registers enabled

    //reset should be on the bus after this!
    bEnumerationStatus = 0x00;                                                      //Device not enumerated yet
    bFunctionSuspended = FALSE;                                                     //Device is not in suspend mode

    bRemoteWakeup = DISABLE;

    bConfigurationNumber    = 0x00;                                                 //device unconfigured
    bInterfaceNumber        = 0x00;

    //FRSTE handling:
    //Clear FRSTE in the RESRIFG interrupt service routine before re-configuring USB control registers.
    //Set FRSTE at the beginning of SUSRIFG, SETUP, IEPIFG.EP0 and OEPIFG.EP0 interrupt service routines.
    USBCTL = 0;                                                                     //Function Reset Connection disable (FRSTE)

    wBytesRemainingOnIEP0   = NO_MORE_DATA;
    wBytesRemainingOnOEP0   = NO_MORE_DATA;
    bStatusAction           = STATUS_ACTION_NOTHING;

    //The address reset normally will be done automatically during bus function reset
    USBFUNADR   =     0x00;                                                         //reset address of USB device (unconfigured)

    /* Set settings for EP0 */
    //NAK both 0 endpoints and enable endpoint 0 interrupt
    tEndPoint0DescriptorBlock.bIEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bOEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bIEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    //8 byte data packet
    tEndPoint0DescriptorBlock.bOEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    //8 byte data packet

    USBOEPIE = USB_OUTEP_INT_EN;
    USBIEPIE = USB_INEP_INT_EN;

    //loop for initialization all of used enpoints
    for (i = 0;
         i < (CDC_NUM_INTERFACES + HID_NUM_INTERFACES + MSC_NUM_INTERFACES + PHDC_NUM_INTERFACES);
         i++)
    {
        BYTE edbIndex = stUsbHandle[i].edb_Index;

        /* Set settings for IEPx */
        tInputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE |
                                                           EPCNF_UBME |
                                                           EPCNF_DBUF;              //double buffering
        tInputEndPointDescriptorBlock[edbIndex].bEPBBAX  =
            (BYTE)(((stUsbHandle[i].iep_X_Buffer -
                     START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tInputEndPointDescriptorBlock[edbIndex].bEPBBAY  =
            (BYTE)(((stUsbHandle[i].iep_Y_Buffer -
                     START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tInputEndPointDescriptorBlock[edbIndex].bEPBCTX  = EPBCNT_NAK;
        tInputEndPointDescriptorBlock[edbIndex].bEPBCTY  = EPBCNT_NAK;
        tInputEndPointDescriptorBlock[edbIndex].bEPSIZXY = MAX_PACKET_SIZE;

        /* Set settings for OEPx */
#ifdef BRIDGE_CDC_PRESENT
        if (i == BRIDGE_CDC_INTFNUM) {
            tOutputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE |
                                                                EPCNF_UBME;            
        }
        else
        {
            tOutputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE |
                                                                EPCNF_UBME |
                                                                EPCNF_DBUF ;            //double buffering
        }
#else        
        tOutputEndPointDescriptorBlock[edbIndex].bEPCNF   = EPCNF_USBIE |
                                                            EPCNF_UBME |
                                                            EPCNF_DBUF ;            //double buffering
#endif
        
        tOutputEndPointDescriptorBlock[edbIndex].bEPBBAX  =
            (BYTE)(((stUsbHandle[i].oep_X_Buffer -
                     START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tOutputEndPointDescriptorBlock[edbIndex].bEPBBAY  =
            (BYTE)(((stUsbHandle[i].oep_Y_Buffer -
                     START_OF_USB_BUFFER) >> 3) & 0x00ff);
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTX  = 0x00;
        tOutputEndPointDescriptorBlock[edbIndex].bEPBCTY  = 0x00;
        tOutputEndPointDescriptorBlock[edbIndex].bEPSIZXY = MAX_PACKET_SIZE;

#       ifdef _CDC_
        /* Additional interrupt end point for CDC */
        if (stUsbHandle[i].dev_Class == CDC_CLASS){
            //The decriptor tool always generates the managemnet endpoint before the data endpoint
            tInputEndPointDescriptorBlock[edbIndex -
                                          1].bEPCNF   = EPCNF_USBIE |
                                                        EPCNF_UBME | EPCNF_DBUF;    //double buffering
            tInputEndPointDescriptorBlock[edbIndex -
                                          1].bEPBBAX  =
                (BYTE)(((stUsbHandle[i].intepEP_X_Buffer -
                         START_OF_USB_BUFFER) >> 3) & 0x00ff);
            tInputEndPointDescriptorBlock[edbIndex -
                                          1].bEPBBAY  =
                (BYTE)(((stUsbHandle[i].intepEP_Y_Buffer -
                         START_OF_USB_BUFFER) >> 3) & 0x00ff);
            tInputEndPointDescriptorBlock[edbIndex - 1].bEPBCTX  = EPBCNT_NAK;
            tInputEndPointDescriptorBlock[edbIndex - 1].bEPBCTY  = EPBCNT_NAK;
            tInputEndPointDescriptorBlock[edbIndex -
                                          1].bEPSIZXY = MAX_PACKET_SIZE;
        }
#       endif
    }

#   ifdef _HID_
    HidResetData();                                                                 //reset HID specific data structures
#   endif //_HID_

#   ifdef _MSC_
    MscState.isMSCConfigured = FALSE;
    USBMSC_reset();
    MscResetData();
#   endif

#   ifdef _CDC_
    CdcResetData();                                                                 //reset CDC specific data structures
#   endif //_CDC_

#   ifdef _PHDC_
        PHDCResetData();                     // reset CDC specific data structures
#   endif // _PHDC_

    USBCTL = FEN;                                                                   //enable function
    USBIFG = 0;                                                                     //make sure no interrupts are pending

    USBIE = SETUPIE | RSTRIE | SUSRIE;                                              //enable USB specific interrupts (setup, reset,
                                                                                    //suspend)
    USBKEYPID = 0x9600;                                                             //access to configuration registers disabled
    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Makes USB Module Available to Host for Connection.
//!
//! Instructs the USB module to make itself available to the host for 
//! connection, by pulling the D+ signal high using the PUR pin. This call 
//! should only be made after a call to USB_enable().
//!
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_connect ()
{
    USBKEYPID = 0x9628;                                                             //set KEY and PID to 0x9628 -> access to
                                                                                    //configuration registers enabled
    USBCNF |= PUR_EN;                                                               //generate rising edge on DP -> the host
                                                                                    //enumerates our device as full speed device
    USBPWRCTL |= VBOFFIE;                                                           //enable interrupt VUSBoff
    USBKEYPID = 0x9600;                                                             //access to configuration registers disabled

    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Forces a Disconnect From the USB Host.
//!
//! Forces a logical disconnect from the USB host by pulling the PUR pin low, 
//! removing the pullup on the D+ signal. The USB module and PLL remain enabled.
//! If the USB is not connected when this call is made, no error is returned -
//! the call simply exits with success after ensuring PUR is low.
//!
//! \return \b kUSB_succeed
//
//*****************************************************************************

BYTE USB_disconnect ()
{
    USBKEYPID = 0x9628;                                                             //set KEY and PID to 0x9628 -> access to
                                                                                    //configuration registers enabled
    USBCNF &= ~PUR_EN;                                                              //disconnect pull up resistor - logical
                                                                                    //disconnect from HOST
    USBPWRCTL &= ~VBOFFIE;                                                          //disable interrupt VUSBoff
    USBKEYPID = 0x9600;                                                             //access to configuration registers disabled
    bEnumerationStatus = 0;                                                         //not enumerated
    bFunctionSuspended = FALSE;                                                     //device is not suspended
    return (kUSB_succeed);
}

//*****************************************************************************
//
//! Remote Wakeup of USB Host.
//!
//! Prompts a remote wakeup of the USB host. The user must ensure that the USB 
//! descriptors had indicated remote wakeup capability (using the Descriptor 
//! Tool); otherwise the host will ignore the request.
//! 
//! If the function returns \b kUSB_generalError, it means that the host did not 
//! grant the device the ability to perform a remote wakeup, when it enumerated 
//! the device.
//!
//! \return \b kUSB_succeed, \b kUSBgeneralError or \b kUSB_notSuspended.
//
//*****************************************************************************

BYTE USB_forceRemoteWakeup ()
{
    if (bFunctionSuspended == FALSE){                                               //device is not suspended
        return (kUSB_NotSuspended);
    }
    if (bRemoteWakeup == ENABLE){
        volatile unsigned int i;
        USBCTL |= RWUP;                                                             //USB - Device Remote Wakeup Request - this bit
                                                                                    //is self-cleaned
        return (kUSB_succeed);
    }
    return (kUSB_generalError);
}

//*****************************************************************************
//
//! Gets Connection Info.
//! 
//! Returns low-level status information about the USB connection.
//! 
//! Because multiple flags can be returned, the possible values can be masked 
//! together - for example, \b kUSB_vbusPresent + \b kUSB_suspended.
//!
//! \return A single mask that is the all the statuses together and may
//! 		consist of the following:
//! 				- \b kUSB_purHigh
//! 				- \b kUSB_suspended
//! 				- \b kUSB_NotSuspended
//! 				- \b kUSB_Enumerated
//! 				- \b kUSB_vbusPresent
//
//*****************************************************************************

BYTE USB_connectionInfo ()
{
    BYTE retVal = 0;

    if (USBPWRCTL & USBBGVBV){
        retVal |= kUSB_vbusPresent;
    }

    if (bEnumerationStatus == ENUMERATION_COMPLETE){
        retVal |= kUSB_Enumerated;
    }

    if (USBCNF & PUR_EN){
        retVal |= kUSB_purHigh;
    }

    if (bFunctionSuspended == TRUE){
        retVal |= kUSB_suspended;
    } else {
        retVal |= kUSB_NotSuspended;
    }
    return (retVal);
}

//*****************************************************************************
//
//! Gets State of the USB Connection.
//!
//! Returns the state of the USB connection, according to the state diagram 
//! in Sec. 6 of \e "Programmer's Guide: MSP430 USB API Stack for CDC/PHDC/HID/MSC".
//! 
//! \return Any of the following: 
//! 			- \b ST_USB_DISCONNECTED
//! 			- \b ST_USB_CONNECTED_NO_ENUM
//! 			- \b ST_ENUM_IN_PROGRESS
//! 			- \b ST_ENUM_ACTIVE
//! 			- \b ST_ENUM_SUSPENDED
//! 			- \b ST_NOENUM_SUSPENDED,
//! 			- \b ST_ERROR.
//
//*****************************************************************************

BYTE USB_connectionState ()
{
    //If no VBUS present
    if (!(USBPWRCTL & USBBGVBV)){
        return (ST_USB_DISCONNECTED);
    }

    //If VBUS present, but PUR is low
    if ((USBPWRCTL & USBBGVBV) && (!(USBCNF & PUR_EN))){
        return (ST_USB_CONNECTED_NO_ENUM);
    }

    //If VBUS present, PUR is high, and enumeration is complete, and not suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (!(bFunctionSuspended == TRUE))){
        return (ST_ENUM_ACTIVE);
    }

    //If VBUS present, PUR is high, and enumeration is NOT complete, and  suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (!(bEnumerationStatus == ENUMERATION_COMPLETE))
        && (bFunctionSuspended == TRUE)){
        return (ST_NOENUM_SUSPENDED);
    }

    //If VBUS present, PUR is high, and enumeration is complete, and  suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (bFunctionSuspended == TRUE)){
        return (ST_ENUM_SUSPENDED);
    }

    //If VBUS present, PUR is high, but no enumeration yet
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (!(bEnumerationStatus == ENUMERATION_COMPLETE))){
        return (ST_ENUM_IN_PROGRESS);
    }

    return (ST_ERROR);
}

//
//! \cond
//

//----------------------------------------------------------------------------

BYTE USB_suspend (VOID)
{
    bFunctionSuspended  = TRUE;
    USBKEYPID = 0x9628;                 //set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCTL |= FRSTE;                    //Function Reset Connection Enable
    USBIFG &= ~SUSRIFG;                 //clear interrupt flag

    USBPLLCTL &= ~UPLLEN;

    if (USB_DISABLE_XT_SUSPEND){
        if (USB_PLL_XT == 2){
            UCS_XT2Off(UCS_BASE);         //disable XT2
        } else {
            UCS_XT1Off(UCS_BASE);         //disable XT1
        }
    }

    USBIE = RESRIE;                     //disable USB specific interrupts (setup, suspend, reset), enable resume.
                                        //If the reset occured during device in suspend, the resume-interrupt will come, after -
                                        //reset interrupt
    USBKEYPID = 0x9600;                 //access to configuration registers disabled

    return (kUSB_succeed);
}

//----------------------------------------------------------------------------

BYTE USB_resume (VOID)
{
#ifdef USE_TIMER_FOR_RESUME
	USB_enable_crystal();
	return (kUSB_succeed);
#else
    USB_enable();                       //enable PLL

    USBIFG &= ~(RESRIFG | SUSRIFG);     //clear interrupt flags
    USBIE = SETUPIE | RSTRIE | SUSRIE;  //enable USB specific interrupts (setup, reset, suspend)

    bFunctionSuspended  = FALSE;
    return (kUSB_succeed);
#endif
}

//----------------------------------------------------------------------------

VOID usbStallEndpoint0 (VOID)
{
    tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL;
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbClearOEP0ByteCount (VOID)
{
    tEndPoint0DescriptorBlock.bOEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

VOID usbStallOEP0 (VOID)
{
    //in standard USB request, there is not control write request with data stage
    //control write, stall output endpoint 0
    //wLength should be 0 in all cases
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbSendNextPacketOnIEP0 (VOID)
{
    BYTE bPacketSize,bIndex;

    //First check if there are bytes remaining to be transferred
    if (wBytesRemainingOnIEP0 != NO_MORE_DATA){
        if (wBytesRemainingOnIEP0 > EP0_PACKET_SIZE){
            //More bytes are remaining than will fit in one packet
            //there will be More IN Stage
            bPacketSize = EP0_PACKET_SIZE;
            wBytesRemainingOnIEP0 -= EP0_PACKET_SIZE;
            bStatusAction = STATUS_ACTION_DATA_IN;
        } else if (wBytesRemainingOnIEP0 < EP0_PACKET_SIZE){
            //The remaining data will fit in one packet.
            //This case will properly handle wBytesRemainingOnIEP0 == 0
            bPacketSize = (BYTE)wBytesRemainingOnIEP0;
            wBytesRemainingOnIEP0 = NO_MORE_DATA;   //No more data need to be Txed
            bStatusAction = STATUS_ACTION_NOTHING;
        } else {
            bPacketSize = EP0_PACKET_SIZE;
            if (bHostAskMoreDataThanAvailable == TRUE){
                wBytesRemainingOnIEP0 = 0;
                bStatusAction = STATUS_ACTION_DATA_IN;
            } else {
                wBytesRemainingOnIEP0 = NO_MORE_DATA;
                bStatusAction = STATUS_ACTION_NOTHING;
            }
        }

        for (bIndex = 0; bIndex < bPacketSize; bIndex++)
        {
            abIEP0Buffer[bIndex] = *pbIEP0Buffer;
            pbIEP0Buffer++;
        }
        tEndPoint0DescriptorBlock.bIEPBCNT = bPacketSize;
    } else {
        bStatusAction = STATUS_ACTION_NOTHING;
    }
}

//----------------------------------------------------------------------------

VOID usbSendDataPacketOnEP0 (PBYTE pbBuffer)
{
    WORD wTemp;

    pbIEP0Buffer = pbBuffer;
    wTemp = tSetupPacket.wLength;

    //Limit transfer size to wLength if needed
    //this prevent USB device sending 'more than require' data back to host
    if (wBytesRemainingOnIEP0 >= wTemp){
        wBytesRemainingOnIEP0 = wTemp;
        bHostAskMoreDataThanAvailable = FALSE;
    } else {
        bHostAskMoreDataThanAvailable = TRUE;
    }
    usbSendNextPacketOnIEP0();
}

//----------------------------------------------------------------------------
VOID usbReceiveNextPacketOnOEP0 (VOID)
{
    BYTE bIndex,bByte;

    bByte = tEndPoint0DescriptorBlock.bOEPBCNT & EPBCNT_BYTECNT_MASK;

    if (wBytesRemainingOnOEP0 >= (WORD)bByte){
        for (bIndex = 0; bIndex < bByte; bIndex++)
        {
            *pbOEP0Buffer = abOEP0Buffer[bIndex];
            pbOEP0Buffer++;
        }
        wBytesRemainingOnOEP0 -= (WORD)bByte;

        //clear the NAK bit for next packet
        if (wBytesRemainingOnOEP0 > 0){
            usbClearOEP0ByteCount();
            bStatusAction = STATUS_ACTION_DATA_OUT;
        } else {
            usbStallOEP0();
            bStatusAction = STATUS_ACTION_NOTHING;
        }
    } else {
        usbStallOEP0();
        bStatusAction = STATUS_ACTION_NOTHING;
    }
}

//----------------------------------------------------------------------------

VOID usbReceiveDataPacketOnEP0 (PBYTE pbBuffer)
{
    pbOEP0Buffer = pbBuffer;

    wBytesRemainingOnOEP0 = tSetupPacket.wLength;
    bStatusAction = STATUS_ACTION_DATA_OUT;

    usbClearOEP0ByteCount();
}

//----------------------------------------------------------------------------

VOID usbSendZeroLengthPacketOnIEP0 (VOID)
{
    wBytesRemainingOnIEP0 = NO_MORE_DATA;
    bStatusAction = STATUS_ACTION_NOTHING;
    tEndPoint0DescriptorBlock.bIEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

BYTE usbClearEndpointFeature (VOID)
{
    BYTE bEndpointNumber;

    //EP is from EP1 to EP7 while C language start from 0
    bEndpointNumber = (tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM);
    if (bEndpointNumber == 0x00){
        usbSendZeroLengthPacketOnIEP0();
    } else {
        bEndpointNumber--;
        if (bEndpointNumber < MAX_ENDPOINT_NUMBER){
            if ((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) ==
                EP_DESC_ADDR_DIR_IN){
#ifdef _MSC_
                if (!MscState.bMscResetRequired){
#endif
                tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &=
                    ~(EPCNF_STALL | EPCNF_TOGGLE );
#ifdef _MSC_
            }
#endif
#               ifdef _MSC_
                if (stUsbHandle[MSC0_INTFNUM].edb_Index == bEndpointNumber){
                    MscReadControl.bCurrentBufferXY = 0;    //Set current buffer to X
                    MscState.bMcsCommandSupported = TRUE;
                }
#               endif
            } else {
#ifdef _MSC_
                if (!MscState.bMscResetRequired){
#endif
					tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &=
						~(EPCNF_STALL | EPCNF_TOGGLE );
#ifdef _MSC_						
					tOutputEndPointDescriptorBlock[bEndpointNumber].bEPBCTX = 0;
                    tOutputEndPointDescriptorBlock[bEndpointNumber].bEPBCTY = 0;
                    MscState.stallEndpoint = FALSE;
#endif					
#ifdef _MSC_
            }
#endif
#               ifdef _MSC_
                if (stUsbHandle[MSC0_INTFNUM].edb_Index == bEndpointNumber){
                    MscWriteControl.bCurrentBufferXY = 0;   //Set current buffer to X
                    MscState.bMcsCommandSupported = TRUE;
                }
#               endif
            }
            usbSendZeroLengthPacketOnIEP0();
        }
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetConfiguration (VOID)
{
    usbClearOEP0ByteCount();                                //for status stage
    wBytesRemainingOnIEP0 = 1;
    usbSendDataPacketOnEP0((PBYTE)&bConfigurationNumber);

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetDeviceDescriptor (VOID)
{
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = SIZEOF_DEVICE_DESCRIPTOR;
#ifdef NON_COMPOSITE_MULTIPLE_INTERFACES
    usbSendDataPacketOnEP0((PBYTE)usbDeviceDescriptors[activeInterfaceIndex]);
#else
    usbSendDataPacketOnEP0((PBYTE)&abromDeviceDescriptor);
#endif
    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetConfigurationDescriptor (VOID)
{
    usbClearOEP0ByteCount();
#ifdef NON_COMPOSITE_MULTIPLE_INTERFACES
    wBytesRemainingOnIEP0 = usbConfigurationsSizes[activeInterfaceIndex];
    usbSendDataPacketOnEP0((PBYTE)usbConfigurationDescriptors[activeInterfaceIndex]);
#else
    wBytesRemainingOnIEP0 = sizeof(abromConfigurationDescriptorGroup);
    usbSendDataPacketOnEP0((PBYTE)&abromConfigurationDescriptorGroup);
#endif	

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetStringDescriptor (VOID)
{
    WORD bIndex;
    BYTE bVal = (BYTE)tSetupPacket.wValue;

    usbClearOEP0ByteCount();    //for status stage
    if (bVal <= MAX_STRING_DESCRIPTOR_INDEX) {
#if (USB_STR_INDEX_SERNUM != 0)
        if (bVal == 0x03){
            wBytesRemainingOnIEP0 = abramSerialStringDescriptor[0];
            usbSendDataPacketOnEP0((PBYTE)&abramSerialStringDescriptor);
        } else
#endif
        {
            bIndex = 0x00;
            while (bVal-- >  0x00) bIndex += abromStringDescriptor[bIndex];
            wBytesRemainingOnIEP0 = abromStringDescriptor[bIndex];
            usbSendDataPacketOnEP0((PBYTE)&abromStringDescriptor[bIndex]);
        }
    }
    else {
        usbStallEndpoint0();
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetInterface (VOID)
{
    //not fully supported, return one byte, zero
    usbClearOEP0ByteCount();            //for status stage
    wBytesRemainingOnIEP0 = 0x02;
    abUsbRequestReturnData[0] = 0x00;   //changed to report alternative setting byte
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetDeviceStatus (VOID)
{
#ifdef NON_COMPOSITE_MULTIPLE_INTERFACES
	if ((((struct abromConfigurationDescriptorGroup *)
		usbConfigurationDescriptors[activeInterfaceIndex])->
		abromConfigurationDescriptorGenric.mattributes  &
		CFG_DESC_ATTR_SELF_POWERED) == CFG_DESC_ATTR_SELF_POWERED){
        abUsbRequestReturnData[0] = DEVICE_STATUS_SELF_POWER;
    }
#else
    if ((abromConfigurationDescriptorGroup.abromConfigurationDescriptorGenric.
         mattributes &
         CFG_DESC_ATTR_SELF_POWERED) == CFG_DESC_ATTR_SELF_POWERED){
        abUsbRequestReturnData[0] = DEVICE_STATUS_SELF_POWER;
    }
#endif	
    if (bRemoteWakeup == ENABLE){
        abUsbRequestReturnData[0] |= DEVICE_STATUS_REMOTE_WAKEUP;
    }
    usbClearOEP0ByteCount();            //for status stage

    //Return self power status and remote wakeup status
    wBytesRemainingOnIEP0 = 2;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetInterfaceStatus (VOID)
{
    //check bIndexL for index number (not supported)
    usbClearOEP0ByteCount();            //for status stage

    //Return two zero bytes
    wBytesRemainingOnIEP0 = 2;
    abUsbRequestReturnData[0] = 0x00;   //changed to support multiple interfaces
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbGetEndpointStatus (VOID)
{
    BYTE bEndpointNumber;

    //Endpoint number is bIndexL
    bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
    if (bEndpointNumber == 0x00){
        if ((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) ==
            EP_DESC_ADDR_DIR_IN){
            //input endpoint 0
            abUsbRequestReturnData[0] =
                (BYTE)(tEndPoint0DescriptorBlock.bIEPCNFG & EPCNF_STALL);
        } else {
            //output endpoint 0
            abUsbRequestReturnData[0] =
                (BYTE)(tEndPoint0DescriptorBlock.bOEPCNFG & EPCNF_STALL);
        }
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; //STALL is on bit 3
        usbClearOEP0ByteCount();                                    //for status stage
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    } else {
        bEndpointNumber--;
        //EP is from EP1 to EP7 while C language start from 0
        //Firmware should NOT response if specified endpoint is not supported. (charpter 8)
        if (bEndpointNumber < MAX_ENDPOINT_NUMBER){
            if (tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN){
                //input endpoint
                abUsbRequestReturnData[0] =
                    (BYTE)(tInputEndPointDescriptorBlock[bEndpointNumber].
                           bEPCNF &
                           EPCNF_STALL);
            } else {
                //output endpoint
                abUsbRequestReturnData[0] =
                    (BYTE)(tOutputEndPointDescriptorBlock[bEndpointNumber].
                           bEPCNF &
                           EPCNF_STALL);
            }
        }                                                           //no response if endpoint is not supported.
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; //STALL is on bit 3
        usbClearOEP0ByteCount();
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    }

    return (FALSE);
}

//----------------------------------------------------------------------------
BYTE usbSetAddress (VOID)
{
    usbStallOEP0();                                                 //control write without data stage

    //bValueL contains device address
    if (tSetupPacket.wValue < 128){
        //hardware will update the address after status stage
        //therefore, firmware can set the address now.
        USBFUNADR = tSetupPacket.wValue;
        usbSendZeroLengthPacketOnIEP0();
    } else {
        usbStallEndpoint0();
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbSetConfiguration (VOID)
{
	BYTE bWakeUp = FALSE;

    usbStallOEP0();                                                 //control write without data stage

    //configuration number is in bValueL
    //change the code if more than one configuration is supported
    bConfigurationNumber = tSetupPacket.wValue;
    usbSendZeroLengthPacketOnIEP0();

    if (bConfigurationNumber == 1){
        bEnumerationStatus = ENUMERATION_COMPLETE;                  //set device as enumerated
        //perform enumeration complete event:
        bWakeUp = USB_handleEnumCompleteEvent();
    } else {
        bEnumerationStatus = 0;                                     //device is not configured == config # is zero
    }

    return (bWakeUp);
}

//----------------------------------------------------------------------------

BYTE usbClearDeviceFeature (VOID)
{
    //bValueL contains feature selector
    if (tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP){
        bRemoteWakeup = DISABLE;
        usbSendZeroLengthPacketOnIEP0();
    } else {
        usbStallEndpoint0();
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbSetDeviceFeature (VOID)
{
    //bValueL contains feature selector
    if (tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP){
        bRemoteWakeup = ENABLE;
        usbSendZeroLengthPacketOnIEP0();
    } else {
        usbStallEndpoint0();
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbSetEndpointFeature (VOID)
{
    BYTE bEndpointNumber;

    //wValue contains feature selector
    //bIndexL contains endpoint number
    //Endpoint number is in low byte of wIndex
    if (tSetupPacket.wValue == FEATURE_ENDPOINT_STALL){
        bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
        if (bEndpointNumber == 0x00){
            usbSendZeroLengthPacketOnIEP0();    //do nothing for endpoint 0
        } else {
            bEndpointNumber--;
            //Firmware should NOT response if specified endpoint is not supported. (charpter 8)
            if (bEndpointNumber < MAX_ENDPOINT_NUMBER){
                if (tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN){
                    //input endpoint
                    tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |=
                        EPCNF_STALL;
                } else {
                    //output endpoint
                    tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |=
                        EPCNF_STALL;
                }
                usbSendZeroLengthPacketOnIEP0();
            } //no response if endpoint is not supported.
        }
    } else {
        usbStallEndpoint0();
    }

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbSetInterface (VOID)
{
    //bValueL contains alternative setting
    //bIndexL contains interface number
    //change code if more than one interface is supported
    usbStallOEP0();                         //control write without data stage
    bInterfaceNumber = tSetupPacket.wIndex;
#ifdef _MSC_
    tInputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF
        &= ~(EPCNF_TOGGLE);
    tOutputEndPointDescriptorBlock[stUsbHandle[MSC0_INTFNUM].edb_Index].bEPCNF
        &= ~(EPCNF_TOGGLE);
    MscReadControl.bCurrentBufferXY = 0;    //Set current buffer to X
    MscWriteControl.bCurrentBufferXY = 0;   //Set current buffer to X
#endif
    usbSendZeroLengthPacketOnIEP0();

    return (FALSE);
}

//----------------------------------------------------------------------------

BYTE usbInvalidRequest (VOID)
{
    //check if setup overwrite is set
    //if set, do nothing since we might decode it wrong
    //setup packet buffer could be modified by hardware if another setup packet
    //was sent while we are deocding setup packet
    if ((USBIFG & STPOWIFG) == 0x00){
        usbStallEndpoint0();
    }

    return (FALSE);
}

typedef BYTE (*tpF)(VOID);

BYTE usbDecodeAndProcessUsbRequest (VOID)
{
    BYTE bMask,bResult,bTemp;
    const BYTE* pbUsbRequestList;
    BYTE bWakeUp = FALSE;
    ptDEVICE_REQUEST ptSetupPacket = &tSetupPacket;
    BYTE bRequestType,bRequest;
    tpF lAddrOfFunction;

    //point to beginning of the matrix
    pbUsbRequestList = (PBYTE)&tUsbRequestList[0];

    while (1)
    {
        bRequestType = *pbUsbRequestList++;
        bRequest     = *pbUsbRequestList++;

        if (((bRequestType == 0xff) && (bRequest == 0xff)) ||
            (tSetupPacket.bmRequestType ==
             (USB_REQ_TYPE_INPUT | USB_REQ_TYPE_VENDOR |
              USB_REQ_TYPE_DEVICE)) ||
            (tSetupPacket.bmRequestType ==
             (USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_VENDOR |
              USB_REQ_TYPE_DEVICE))){
            pbUsbRequestList -= 2;
            break;
        }

        if ((bRequestType == tSetupPacket.bmRequestType) &&
            (bRequest == tSetupPacket.bRequest)){
            //compare the first two
            bResult = 0xc0;
            bMask   = 0x20;
            //first two bytes matched, compare the rest
            for (bTemp = 2; bTemp < 8; bTemp++)
            {
                if (*((BYTE*)ptSetupPacket + bTemp) == *pbUsbRequestList){
                    bResult |= bMask;
                }
                pbUsbRequestList++;
                bMask = bMask >> 1;
            }
            //now we have the result
            if ((*pbUsbRequestList & bResult) == *pbUsbRequestList){
                pbUsbRequestList -= 8;
                break;
            } else {
                pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE) - 8);
            }
        } else {
            pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE) - 2);
        }
    }

    //if another setup packet comes before we have the chance to process current
    //setup request, we return here without processing the request
    //this check is not necessary but still kept here to reduce response(or simulation) time

    if ((USBIFG & STPOWIFG) != 0x00){
        return (bWakeUp);
    }

    //now we found the match and jump to the function accordingly.
    lAddrOfFunction =
        ((tDEVICE_REQUEST_COMPARE*)pbUsbRequestList)->pUsbFunction;

    //call function
    bWakeUp = (*lAddrOfFunction)();

    return (bWakeUp);
}

WORD usbDisableInEndpointInterrupt(BYTE edbIndex)
{
	WORD state;
	state = USBIEPIE & (1 << (edbIndex + 1));
	USBIEPIE &= ~(1 << (edbIndex + 1));
	return (state);
}
VOID usbRestoreInEndpointInterrupt(WORD state)
{
	USBIEPIE |= state;
}
WORD usbDisableOutEndpointInterrupt(BYTE edbIndex)
{
	WORD state;
	state = USBOEPIE & (1 << (edbIndex + 1));
	USBOEPIE &= ~(1 << (edbIndex + 1));
	return (state);
}
VOID usbRestoreOutEndpointInterrupt(WORD state)
{
	USBOEPIE |= state;
}
#ifdef NON_COMPOSITE_MULTIPLE_INTERFACES

BYTE USB_switchInterface(BYTE interfaceIndex)
{
	if (interfaceIndex < NONCOMP_NUM_USB_INTERFACES) {
		activeInterfaceIndex = interfaceIndex;
		return (TRUE);
	}
	else {
		return (FALSE);
	}
}

#endif

WORD USB_determineFreq(void){
    WORD freq;                  // calculated MCLK freq in kHz
    WORD currentFLLN;           // value of divider N taken from UCS registers
    BYTE currentSELM;           // MCLK reference taken from UCS registers
    BYTE currentFLLREFDIV;      // value of divider n taken from UCS registers
    WORD currentFLLD;           // value of prescalar D taken from UCS registers
    WORD FLLRefFreq;

    currentSELM = (UCSCTL4_L & SELM_7);   // get clock selection control register

    if(currentSELM<=4) // MCLK = DCO, DCOCLKDIV, XT1, VLO, or REFO.  The last three aren't supported by the API.
    {
        FLLRefFreq = 33;                    // The reference is usually 32.768 kHz.
        if((UCSCTL3_L & SELREF_7) > 0x50){  // Unless it's XT2 frequency
            FLLRefFreq = USB_XT_FREQ_VALUE * 1000;
        }

        // determine factors N and n
        currentFLLN = UCSCTL2 & 0x03FF;          // get FLL multiplier register
        currentFLLN++;
        if(currentSELM == SELM_3)            // if MCLK is sourced by DCOCLK
        {
            // determine D
            currentFLLD = UCSCTL2 & FLLD_7;  // get FLLD register
            currentFLLD >>= 12;
            currentFLLN <<= currentFLLD;
        }

        currentFLLREFDIV = UCSCTL3_L & FLLREFDIV_7; // get FLL reference divider register
        if(currentFLLREFDIV>=4)
        {
            currentFLLREFDIV = FLLREFDIV_5;
        }
        freq = currentFLLN * (FLLRefFreq >> currentFLLREFDIV);
    }
    else
    {
        freq = USB_XT_FREQ_VALUE * 1000;
    }
    return freq >> (UCSCTL5_L & DIVM_7);  // Divide by any divider present in DIVM
}


//
//! \endcond
//

/*----------------------------------------------------------------------------+
 | End of source file                                                          |
 +----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
//Released_Version_4_00_00
