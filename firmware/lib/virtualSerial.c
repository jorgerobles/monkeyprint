#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/delay.h>

#include "virtualSerial.h"
#include "hardware.h"


/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

//****************************************************************************//
//******************* Sending and receiving functions. ***********************//
//****************************************************************************//

// Function: Send string via USB. **********************************************
uint8_t sendStringUSB(char* dataString)
{
	// Send string and return error variable.
	// See http://www.fourwalledcubicle.com/files/LUFA/Doc/120730/html/group___group___endpoint_stream_r_w.html#ga7f0d584afdd478f8fdf20b9daf58394b
	uint8_t errorCode = CDC_Device_SendString(&VirtualSerial_CDC_Interface, dataString);
	
	// Evaluate error code and signal LEDs.
	if (errorCode == ENDPOINT_RWSTREAM_Timeout)
	{
		// Blink if hardware exists.
		#if defined LEDPORT
		LEDPORT &= ~(1 << LEDPIN);
		_delay_us(10);
		LEDPORT |= (1 << LEDPIN);
		_delay_us(10);
		LEDPORT &= ~(1 << LEDPIN);
		_delay_us(10);
		LEDPORT |= (1 << LEDPIN);
		#endif
	}
	else if (errorCode == ENDPOINT_RWSTREAM_NoError)
	{
		// Blink if hardware exists.
		#if defined LEDPORT
		LEDPORT &= ~(1 << LEDPIN);
		_delay_us(10);
		LEDPORT |= (1 << LEDPIN);
		#endif
	}
	return errorCode;
}

void sendByteAsStringUSB(uint16_t dataByte)
{
	char dataString[10];
	itoa( dataByte, dataString, 10 );
	// Send string and return error variable.
	// See http://www.fourwalledcubicle.com/files/LUFA/Doc/120730/html/group___group___endpoint_stream_r_w.html#ga7f0d584afdd478f8fdf20b9daf58394b
	uint8_t errorCode = CDC_Device_SendString(&VirtualSerial_CDC_Interface, dataString);

	// Evaluate error code and signal LEDs.
	if (errorCode == ENDPOINT_READYWAIT_Timeout)
	{
		// Blink if hardware exists.
		#if defined LED1PORT
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		#endif
	}
	else if (errorCode == ENDPOINT_READYWAIT_NoError)
	{
		// Blink if hardware exists.
		#if defined LED2PORT
		LED2PORT ^= (1 << LED2PIN);
		_delay_us(10);
		LED2PORT ^= (1 << LED2PIN);
		#endif
	}
	//CDC_Device_Flush(&VirtualSerial_CDC_Interface);
}

// Function: Send byte via USB. ************************************************
void sendByteUSB(uint8_t dataByte)
{
	// Send byte and return error variable.
	// See http://www.fourwalledcubicle.com/files/LUFA/Doc/120730/html/group___group___endpoint_r_w___x_m_e_g_a.html#gaa42b7eb8d1be3afadb97097bf2605740
	uint8_t errorCode = CDC_Device_SendByte(&VirtualSerial_CDC_Interface, dataByte);

	// Evaluate error code and signal LEDs.
	if (errorCode == ENDPOINT_READYWAIT_Timeout)
	{
		// Blink if hardware exists.
		#if defined LED1PORT
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		_delay_us(10);
		LED1PORT ^= (1 << LED1PIN);
		#endif
	}
	else if (errorCode == ENDPOINT_READYWAIT_NoError)
	{
		// Blink if hardware exists.
		#if defined LED2PORT
		LED2PORT ^= (1 << LED2PIN);
		_delay_us(10);
		LED2PORT ^= (1 << LED2PIN);
		#endif
	}
	//CDC_Device_Flush(&VirtualSerial_CDC_Interface);
}


// Function: Check how many bytes are waiting at USB. **************************
uint16_t bytesWaitingUSB(void)
{
	uint16_t bytesWaiting = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
	return bytesWaiting;	
}


uint16_t receiveByteUSB(void)
{
	uint16_t byte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	return byte;	
}
char receiveCharUSB(void)
{
	char* byte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	return byte;	
}





//****************************************************************************//
//******************* USB management. Call in main loop. *********************//
//****************************************************************************//
		
void manageUSB(uint8_t receiving)
{
	if(!receiving)
	{
		// Must throw away unused bytes from the host, or it will lock up while waiting for the device.
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	}

	// Call CDC and USB management functions for proper operation.
	// Must be called every at least every 30 ms in device mode.
	// See http://www.fourwalledcubicle.com/files/LUFA/Doc/120730/html/group___group___u_s_b_management.html#gac4059f84a2fc0b926c31868c744f5853
	CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	USB_USBTask();
}

//****************************************************************************//
//******************* USB status event handler functions. ********************//
//****************************************************************************//

// Event handler for the library USB Connection event. *************************
void EVENT_USB_Device_Connect(void)
{
	// Do some blinking.
	for(uint8_t i=1;i<5;i++)
	{
		// Blink if hardware exists.
		#if defined LED1PORT
		LED1PORT &= ~(1 << LED1PIN);
		_delay_us(50);
		LED1PORT |= (1 << LED1PIN);
		#endif
		
	}

}

// Event handler for the library USB Disconnection event. **********************
void EVENT_USB_Device_Disconnect(void)
{
	// Flash LED...
}

// Event handler for the library USB Configuration Changed event. **************
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	// Flash LED...
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

// Event handler for the library USB Control Request reception event. **********
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}