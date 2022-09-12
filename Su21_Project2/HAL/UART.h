/*
 * UART.h
 *
 *  Created on: Dec 31, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Ever since the new API update in mid 2019, the UART configuration struct's
// name changed from [eUSCI_UART_Config] to [eUSCI_UART_ConfigV1]. If your code
// does not compile, uncomment the alternate #define and comment the original
// one.
// -----------------------------------------------------------------------------
// #define UART_Config eUSCI_UART_Config
#define UART_Config eUSCI_UART_ConfigV1


// TODO: define these macros based on the UART module that is connected to USB on Launchpad via XDS110

#define USB_UART_PORT 1
#define USB_UART_PINS (GPIO_PIN2 | GPIO_PIN3) // The pins are given to you for guidance. Also, because many students miss the parentheses
#define USB_UART_INSTANCE  EUSCI_A0_BASE


// An enum outlining what baud rates the UART_construct() function can use in
// its initialization.
enum _UART_Baudrate
{
    BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, NUM_BAUD_CHOICES
};
typedef enum _UART_Baudrate UART_Baudrate;

// TODO: Write an overview explanation of what this UART struct does, and how it
//       interacts with the functions below. Consult <HAL/Button.h> and
//       <HAL/LED.h> for examples on how to do this.

/**
 * this struct collects all of the relevant information about the UART into one
 * specific location. The location of the module on the board is recorded in port and pins
 * and the specific instance being used is also saved along with the configuration settings.
 *
 * there are multiple moduleInstances that can be used, but we need to specify one
 * to use many of the functions related to the UART.
 *
 * this interacts with the other functions by transmitting data received to the UART via the specified module
 *  and transmitting user inputs via the same module.
 *
 *  use UART_construct() to create a new UART instance.
 *  In order to receive data from the UART user must check that there is data to transmit via UART_hasChar()
 *  and in order to send data to the UART, user must check that the UART is able to receive data via UART_canSend()
 */
struct _UART
{
    UART_Config config;

    uint32_t moduleInstance;    //the specific instance of UART that is being used (example: EUSCI_A0_BASE)
    uint32_t port;  //the port that the UART is configured to
    uint32_t pins;  //the pins that the UART is configured to
};
typedef struct _UART UART;

// Constructs a uart using the moduleInstance at the given port and pin
UART UART_construct(uint32_t moduleInstance, uint32_t port, uint32_t pins);

// Sets the baudrate and enables a UART based on the given baudrate enum
void UART_SetBaud_Enable(UART *, UART_Baudrate baudrate);


//returns a character from the UART if one exists, otherwise returns NULL
char UART_getChar(UART* uart_p);

//returns true if the UART current has a keystroke saved as a data packet to transmit
bool UART_hasChar(UART* uart_p);

//returns true if the UART is currently able to receive data
bool UART_canSend(UART* uart_p);

//sends a character to the UART to display
void UART_putChar(UART* uart_p, char c);

// Updates the UART baudrate to use the new baud choice.
void UART_updateBaud(UART* uart_p, UART_Baudrate baudChoice);

#endif /* HAL_UART_H_ */
