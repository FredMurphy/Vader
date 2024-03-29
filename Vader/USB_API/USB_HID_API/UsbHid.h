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
 * ======== UsbHid.h ========
 */
#ifndef _UsbHid_H_
#define _UsbHid_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define kUSBHID_sendStarted         0x01
#define kUSBHID_sendComplete        0x02
#define kUSBHID_intfBusyError       0x03
#define kUSBHID_receiveStarted      0x04
#define kUSBHID_receiveCompleted    0x05
#define kUSBHID_receiveInProgress   0x06
#define kUSBHID_generalError        0x07
#define kUSBHID_busNotAvailable     0x08

#define HID_BOOT_PROTOCOL       0x00
#define HID_REPORT_PROTOCOL     0x01

#define USBHID_handleGetReport USBHID_handleEP0GetReport
#define USBHID_handleSetReport USBHID_handleEP0SetReport
#define USBHID_handleSetReportDataAvailable USBHID_handleEP0SetReportDataAvailable
#define USBHID_handleSetReportDataAvailable  USBHID_handleEP0SetReportDataAvailable

/*----------------------------------------------------------------------------
 * These functions can be used in application
 +----------------------------------------------------------------------------*/

/*
 * Sends a pre-built report reportData to the host.
 * Returns:  kUSBHID_sendComplete
 *          kUSBHID_intfBusyError
 *          kUSBHID_busSuspended
 */
BYTE USBHID_sendReport (const BYTE * reportData, BYTE intfNum);

/*
 * Receives report reportData from the host.
 * Return:     kUSBHID_receiveCompleted
 *          kUSBHID_generalError
 *          kUSBHID_busSuspended
 */
BYTE USBHID_receiveReport (BYTE * reportData, BYTE intfNum);

/*
 * Sends data over interface intfNum, of size size and starting at address data.
 * Returns:  kUSBHID_sendStarted
 *          kUSBHID_sendComplete
 *          kUSBHID_intfBusyError
 */
BYTE USBHID_sendData (const BYTE* data, WORD size, BYTE intfNum);

/*
 * Receives data over interface intfNum, of size size, into memory starting at address data.
 */
BYTE USBHID_receiveData (BYTE* data, WORD size, BYTE intfNum);

/*
 * Aborts an active receive operation on interface intfNum.
 * size: the number of bytes that were received and transferred
 * to the data location established for this receive operation.
 */
BYTE USBHID_abortReceive (WORD* size, BYTE intfNum);


#define kUSBHID_noDataWaiting 1 //returned by USBHID_rejectData() if no data pending

/*
 * This function rejects payload data that has been received from the host.
 */
BYTE USBHID_rejectData (BYTE intfNum);

/*
 * Aborts an active send operation on interface intfNum.  Returns the number of bytes that were sent prior to the abort, in size.
 */
BYTE USBHID_abortSend (WORD* size, BYTE intfNum);


#define kUSBHID_waitingForSend      0x01
#define kUSBHID_waitingForReceive   0x02
#define kUSBHID_dataWaiting         0x04
#define kUSBHID_busNotAvailable     0x08
#define kUSB_allHidEvents           0xFF
/*
 * This function indicates the status of the interface intfNum.
 * If a send operation is active for this interface,
 * the function also returns the number of bytes that have been transmitted to the host.
 * If a receiver operation is active for this interface, the function also returns
 * the number of bytes that have been received from the host and are waiting at the assigned address.
 *
 * returns kUSBHID_waitingForSend (indicates that a call to USBHID_SendData()
 * has been made, for which data transfer has not been completed)
 *
 * returns kUSBHID_waitingForReceive (indicates that a receive operation
 * has been initiated, but not all data has yet been received)
 *
 * returns kUSBHID_dataWaiting (indicates that data has been received
 * from the host, waiting in the USB receive buffers)
 */
BYTE USBHID_intfStatus (BYTE intfNum, WORD* bytesSent, WORD* bytesReceived);

/*
 * Returns how many bytes are in the buffer are received and ready to be read.
 */
BYTE USBHID_bytesInUSBBuffer (BYTE intfNum);

/*----------------------------------------------------------------------------
 * Event-Handling routines
 +----------------------------------------------------------------------------*/

/*
 * This event indicates that data has been received for port port, but no data receive operation is underway.
 * returns TRUE to keep CPU awake
 */
BYTE USBHID_handleDataReceived (BYTE intfNum);

/*
 * This event indicates that a send operation on port port has just been completed.
 * returns TRUE to keep CPU awake
 */
BYTE USBHID_handleSendCompleted (BYTE intfNum);

/*
 * This event indicates that a receive operation on port port has just been completed.
 * returns TRUE to keep CPU awake
 */
BYTE USBHID_handleReceiveCompleted (BYTE intfNum);

/*
 * This event indicates that a Set_Protocol request was received from the host
 * The application may maintain separate reports for boot and report protocols.
 * The protocol field is either HID_BOOT_PROTOCOL or
 * HID_REPORT_PROTOCOL
 */
BYTE USBHID_handleBootProtocol (BYTE protocol, BYTE intfnum);

/*
 * This event indicates that a Set_Report request was received from the host
 * The application needs to supply a buffer to retrieve the report data that will be sent
 * as part of this request. This handler is passed the reportType, reportId, the length of data
 * phase as well as the interface number.
 */
BYTE *USBHID_handleEP0SetReport (BYTE reportType, BYTE reportId,
    WORD requestedLength,
    BYTE intfnum);
/*
 * This event indicates that data as part of Set_Report request was received from the host
 * Tha application can return TRUE to wake up the CPU. If the application supplied a buffer
 * as part of USBHID_handleEP0SetReport, then this buffer will contain the Set Report data.
 */
BYTE USBHID_handleEP0SetReportDataAvailable (BYTE intfnum);
/*
 * This event indicates that a Get_Report request was received from the host
 * The application can supply a buffer of data that will be sent to the host.
 * This handler is passed the reportType, reportId, the requested length as
 * well as the interface number.
 */
BYTE *USBHID_handleEP0GetReport (BYTE reportType, BYTE reportId,
    WORD requestedLength,
    BYTE intfnum);

#ifdef __cplusplus
}
#endif
#endif  //_UsbHid_H_
//Released_Version_4_00_00
