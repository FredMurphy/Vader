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
/*  
 * ======== main.c ========
 * LED Control Demo:
 *
 * This example implements a simple command-line interface, where the command 
 * is ended by pressing ‘return’.  It accepts four "commands":
 * "LED ON"
 * "LED OFF"
 * "LED TOGGLE - SLOW"
 * "LED TOGGLE – FAST"
 +----------------------------------------------------------------------------+
 * Please refer to the Examples Guide for more details.
 * ---------------------------------------------------------------------------*/
#include <string.h>
#include "inc/hw_memmap.h"
#include "gpio.h"
#include "wdt_a.h"
#include "ucs.h"
#include "pmm.h"
#include "sfr.h"
#include "timer_a.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"               // Basic Type declarations
#include "USB_API/USB_Common/usb.h"                 // USB-specific functions
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"

/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"

// Function declarations
BYTE retInString (char* string);


// Global flags set by events
volatile BYTE bCDCDataReceived_event = FALSE; // Indicates data has been rx'ed
                                              // without an open rx operation

#define MAX_STR_LENGTH 64
char wholeString[MAX_STR_LENGTH] = ""; // Entire input str from last 'return'

BYTE wildcardMatch (char* string, char* match);
void send(const char* message);

void initServo(void);
void stopServo(void);
void setServo(unsigned char duty);
void parseAndSetServoPosition(char positionText);

void setServoOffTimer(void);

void initRgbLed(void);
void setRgbLed(unsigned char red, unsigned char green, unsigned char blue);
void parseAndSetColour(char* colourText);
void vaderTest(void);

unsigned char getHexDigit(char text);

const unsigned int ServoPeriod = 655; // = 32768 / 50 = 20ms;
const unsigned int positions[] = {30,34,38,42,46,50,54,58,62,66}; // 0.9 to 2.1ms

/*  
 * ======== main ========
 */
VOID main (VOID)
{
    WDT_A_hold(WDT_A_BASE); // Stop watchdog timer

    // Minumum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
    PMM_setVCore(PMM_BASE, PMM_CORE_LEVEL_2);

    initPorts();           // Config GPIOS for low-power (output low)
    initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect

    initRgbLed();
    initServo();           // Setup servo PWM

    __enable_interrupt();  // Enable interrupts globally
    

    while (1)
    {
        BYTE i;
        
        // Check the USB state and directly main loop accordingly
        switch (USB_connectionState())
        {
            // This case is executed while your device is enumerated on the
            // USB host
            case ST_ENUM_ACTIVE:
            
                // Enter LPM0 (can't do LPM3 when active)
                __bis_SR_register(LPM0_bits + GIE);
                _NOP(); 
                // Exit LPM on USB receive and perform a receive operation
                
                // If true, some data is in the buffer; begin receiving a cmd
                if (bCDCDataReceived_event){
 
                    // Holds the new addition to the string
                    char pieceOfString[MAX_STR_LENGTH] = "";
                    
                    // Holds the outgoing string
                    char outString[MAX_STR_LENGTH] = "";

                    // Add bytes in USB buffer to the string
                    cdcReceiveDataInBuffer((BYTE*)pieceOfString,
                        MAX_STR_LENGTH,
                        CDC0_INTFNUM); // Get the next piece of the string

                    // Append new piece to the whole
                    strcat(wholeString,pieceOfString);

                    // Echo back the characters received
                    cdcSendDataInBackground((BYTE*)pieceOfString,
                        strlen(pieceOfString),CDC0_INTFNUM,0);

                    // Has the user pressed return yet?
                    if (retInString(wholeString)){
                    
                        // Compare to string #1, and respond
                        if (!(strcmp(wholeString, "LED ON"))){

                        	                            // Turn on LED P1.0
                            GPIO_setOutputHighOnPin(LED_PORT, LED_PIN);
                            
                            setRgbLed(0xFF, 0x00, 0x00);

                            // Prepare the outgoing string
                            send("\r\nLED is ON\r\n\r\n");
                            // Prepare the outgoing string
                            strcpy(outString,"\r\nLED is ON\r\n\r\n");

                            // Send the response over USB
                            cdcSendDataInBackground((BYTE*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

                        // Compare to string #2, and respond
                        } else if (!(strcmp(wholeString, "LED OFF"))){
                        

                            // Turn off LED P1.0
                            GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);

                            setRgbLed(0x00, 0xFF, 0x00);

                            send("\r\nLED is OFF\r\n\r\n");
                            // Prepare the outgoing string
                            strcpy(outString,"\r\nLED is OFF\r\n\r\n");

                            // Send the response over USB
                            cdcSendDataInBackground((BYTE*)outString,
                                strlen(outString),CDC0_INTFNUM,0);

                        } else if (!(strcmp(wholeString, "TEST"))) {

                        	vaderTest();

                        } else if(wildcardMatch(wholeString,"POSITION ?")) {

							parseAndSetServoPosition(wholeString[9]);
							

                        } else if(wildcardMatch(wholeString,"COLOUR #???")) {

                        	parseAndSetColour(wholeString+8);

                        } else if(wildcardMatch(wholeString,"ALL ?#???")) {

                        	parseAndSetServoPosition(wholeString[4]);
                        	parseAndSetColour(wholeString+6);

                        // Handle other
                        } else {
                        
                            // Prepare the outgoing string
                            strcpy(outString,"\r\nNo such command!\r\n\r\n");
                            
                            // Send the response over USB
                            cdcSendDataInBackground((BYTE*)outString,
                                strlen(outString),CDC0_INTFNUM,0);
                        }
                        
                        // Clear the string in preparation for the next one
                        for (i = 0; i < MAX_STR_LENGTH; i++){
                            wholeString[i] = 0x00;
                        }
                    }
                    bCDCDataReceived_event = FALSE;
                }
                break;
                
            // These cases are executed while your device is disconnected from
            // the host (meaning, not enumerated); enumerated but suspended
            // by the host, or connected to a powered hub without a USB host
            // present.
            case ST_PHYS_DISCONNECTED:
            case ST_ENUM_SUSPENDED:
            case ST_PHYS_CONNECTED_NOENUM_SUSP:
            
                //Turn off LED P1.0
                GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);
                __bis_SR_register(LPM3_bits + GIE);
                _NOP();
                break;

            // The default is executed for the momentary state
            // ST_ENUM_IN_PROGRESS.  Usually, this state only last a few
            // seconds.  Be sure not to enter LPM3 in this state; USB
            // communication is taking place here, and therefore the mode must
            // be LPM0 or active-CPU.
            case ST_ENUM_IN_PROGRESS:
            default:;
        }

    }  // while(1)
} // main()

/*  
 * ======== UNMI_ISR ========
 */
#pragma vector = UNMI_VECTOR
__interrupt VOID UNMI_ISR (VOID)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCS_clearFaultFlag(UCS_BASE, UCS_XT2OFFG);
            UCS_clearFaultFlag(UCS_BASE, UCS_DCOFFG);
            SFR_clearInterrupt(SFR_BASE, SFR_OSCILLATOR_FAULT_INTERRUPT);
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            // If the CPU accesses USB memory while the USB module is
            // suspended, a "bus error" can occur.  This generates an NMI.  If
            // USB is automatically disconnecting in your software, set a
            // breakpoint here and see if execution hits it.  See the
            // Programmer's Guide for more information.
            SYSBERRIV = 0; //clear bus error flag
            USB_disable(); //Disable
    }
}


BYTE wildcardMatch (char* string, char* match) {

	char s;
	char m;
	BYTE i;
	for (i=0; i < MAX_STR_LENGTH; i++) {
		s = string[i];
		m = match[i];

		// No match
		if (s != m && m != '?')
			return(FALSE);

		// Reached the end
		if (s == 0 || m == 0) {
			// of both?
			return (s == m);
		}
	}

	// Catch any overrun
	return (FALSE);
}

/*  
 * ======== retInString ========
 */
// This function returns true if there's an 0x0D character in the string; and if
// so, it trims the 0x0D and anything that had followed it.
BYTE retInString (char* string)
{
    BYTE retPos = 0,i,len;
    char tempStr[MAX_STR_LENGTH] = "";

    strncpy(tempStr,string,strlen(string));  // Make a copy of the string
    len = strlen(tempStr);
    
    // Find 0x0D; if not found, retPos ends up at len
    while ((tempStr[retPos] != 0x0A) && (tempStr[retPos] != 0x0D) &&
           (retPos++ < len)) ;

    // If 0x0D was actually found...
    if ((retPos < len) && (tempStr[retPos] == 0x0D)){
        for (i = 0; i < MAX_STR_LENGTH; i++){ // Empty the buffer
            string[i] = 0x00;
        }
        
        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);
        
        //...and tell the calling function that we did so
        return ( TRUE) ;
        
    // If 0x0D was actually found...
    } else if ((retPos < len) && (tempStr[retPos] == 0x0A)){
        // Empty the buffer
        for (i = 0; i < MAX_STR_LENGTH; i++){
            string[i] = 0x00;
        }
        
        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);
        
        //...and tell the calling function that we did so
        return ( TRUE) ;
    } else if (tempStr[retPos] == 0x0D){
        for (i = 0; i < MAX_STR_LENGTH; i++){  // Empty the buffer
            string[i] = 0x00;
        }
        // ...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);
        // ...and tell the calling function that we did so
        return ( TRUE) ;
    } else if (retPos < len){
        for (i = 0; i < MAX_STR_LENGTH; i++){  // Empty the buffer
            string[i] = 0x00;
        }
        
        //...trim the input string to just before 0x0D
        strncpy(string,tempStr,retPos);
        
        //...and tell the calling function that we did so
        return ( TRUE) ;
    }

    return ( FALSE) ; // Otherwise, it wasn't found
}


/*
 * Switch the servo off after it's settled to stop the hum
 */
void setServoOffTimer(void) {
	// Use Timer A1 for a one-shot interrupt
	  TA1CCTL0 = CCIE;                          // CCR1 interrupt enabled
	  TA1CCR0 = 32768;							// About 1s
	  TA1CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, up mode, clear TAR
}

// Timer0 A1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  	// Stop servo
	TA2CCR1 = 0;
  	// Stop timer
  	TA1CTL = TASSEL_0 + MC_1 + TACLR;
}

void parseAndSetServoPosition(char positionText) {

	if (positionText < '0' || positionText >'9') {
		stopServo();
		send("\r\nArm servo off\r\n\r\n");
		return;
	}
	
	setServo(positionText - '0');
	send("\r\nArm servo position set\r\n\r\n");
	
}

void initServo(void)
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4);

    TIMER_A_clearTimerInterruptFlag(TIMER_A2_BASE);

    //Generate PWM - Timer runs in Up-Down mode
    TIMER_A_generatePWM(TIMER_A2_BASE,
                        TIMER_A_CLOCKSOURCE_ACLK, TIMER_A_CLOCKSOURCE_DIVIDER_1,
                        ServoPeriod,
                        TIMER_A_CAPTURECOMPARE_REGISTER_1, TIMER_A_OUTPUTMODE_RESET_SET,
                        0);

}

void stopServo(void)
{
	TA2CCR1 = 0;
}

void setServo(unsigned char position)
{
	if (position > 9)
	{
		stopServo();
	} else {
		TA2CCR1 = positions[position];
		setServoOffTimer();
	}
}

/*
 * Uses Timer A0 for 3PWM output
 * Chosen as these are accessible on LaunchPad
 * 		TA0.2 on P1.3
 * 		TA0.3 on P1.4
 * 		TA0.4 on P1.5
 */
void initRgbLed(void)
{
	  P1DIR |= BIT3 + BIT4 + BIT5;              // P1.3, P1.4 and P1.5 output
	  P1SEL |= BIT3 + BIT4 + BIT5;              // P1.3, P1.4 and P1.5 options select
	  TA0CCR0 = 0xFE;                           // PWM Period
	  TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set
	  TA0CCR2 = 0;                           	// CCR2 PWM duty cycle initially 0
	  TA0CCTL3 = OUTMOD_7;
	  TA0CCR3 = 0;
	  TA0CCTL4 = OUTMOD_7;
	  TA0CCR4 = 0;
	  TA0CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, up mode, clear TAR

}

void setRgbLed(unsigned char red, unsigned char green, unsigned char blue)
{
	// Set PWM registers
	TA0CCR2 = red;
	TA0CCR3 = green;
	TA0CCR4 = blue;


}

void parseAndSetColour(char* colourText) {

	unsigned char red = getHexDigit(colourText[0]);
	unsigned char green = getHexDigit(colourText[1]);
	unsigned char blue = getHexDigit(colourText[2]);

	if (red == 255 || green == 255 || blue == 255) {
		send("\r\nUnrecognised colour");
		return;
	}

	red *= 0x11;
	green *= 0x11;
	blue *= 0x11;

	// Set PWM registers
	setRgbLed(red, green, blue);

	send ("\r\nColour set\r\n");
}

void vaderTest() {

	setServo(0);

	unsigned char x;
	// red
	for (x=0; x < 255; x++) {
		setRgbLed(x, 0, 0);
		__delay_cycles(50000);
	}
	setServo(1);
	// --> yellow
	for (x=0; x < 255; x++) {
		setRgbLed(255, x, 0);
		__delay_cycles(50000);
	}
	setServo(2);
	// --> green
	for (x=255; x; x--) {
		setRgbLed(x, 255, 0);
		__delay_cycles(50000);
	}
	setServo(3);
	// --> green/blue
	for (x=0; x < 255; x++) {
		setRgbLed(0, 255, x);
		__delay_cycles(50000);
	}
	setServo(4);
	// --> blue
	for (x=255; x; x--) {
		setRgbLed(0, x, 255);
		__delay_cycles(50000);
	}
	setServo(5);
	// --> purple
	for (x=0; x < 255; x++) {
		setRgbLed(x, 0, 255);
		__delay_cycles(50000);
	}
	setServo(6);
	// --> red
	for (x=255; x; x--) {
		setRgbLed(255, 0, x);
		__delay_cycles(50000);
	}
	setServo(7);
	// --> off
	for (x=255; x; x--) {
		setRgbLed(x, 0, 0);
		__delay_cycles(50000);
	}
	setServo(0);
	setRgbLed(0,0xFF,0);
	__delay_cycles(5000000);
	setServo(9);
	setRgbLed(0xFF,0,0);
	__delay_cycles(5000000);
	setServo(0);
	setRgbLed(0,0,0);
}

unsigned char getHexDigit(char text) {

	if ((text >= '0') && (text <= '9'))
		return (text-'0');


	if ((text >= 'A') && (text <= 'F'))
		return (10 + text-'A');

	if ((text >= 'a') && (text <= 'f'))
		return (10 + text-'a');

	return 255;
}

void send(const char* message) {

/*    // Holds the outgoing string
    char out[MAX_STR_LENGTH] = "";

    // Prepare the outgoing string
    strcpy(out, message);

    // Send the response over USB
    cdcSendDataInBackground((BYTE*)out,
        strlen(out),CDC0_INTFNUM,0);
*/
}
//Released_Version_4_00_00
