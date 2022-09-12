/*
 * UART.c
 *
 *  Created on: Dec 31, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/Timer.h>
#include <HAL/UART.h>

/**
 * Initializes the UART module except for the baudrate generation
 * Except for baudrate generation, all other uart configuration should match that of basic_example_UART
 *
 * @param moduleInstance:   The eUSCI module used to construct this UART
 * @param port:             The port the UART is connected to
 * @param pins:             The Rx and Tx pins UART is connected to
 *
 * @return the completed UART object
 */
UART UART_construct(uint32_t moduleInstance, uint32_t port, uint32_t pins)
{
    // Create the UART struct
    UART uart;

    // Initialize auxiliary members of the struct
    // -------------------------------------------------------------------------
    uart.moduleInstance = moduleInstance;
    uart.port = port;
    uart.pins = pins;

    GPIO_setAsPeripheralModuleFunctionInputPin(
            uart.port, uart.pins, GPIO_PRIMARY_MODULE_FUNCTION);


    // TODO: Determine what other variables need to be initialized in the config struct

    // DONE: all uart config parameters except those related to baudrate generation
    uart.config.parity           = EUSCI_A_UART_NO_PARITY;    // No Parity
    uart.config.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;           // LSB First
    uart.config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;     // One stop bit
    uart.config.dataLength = EUSCI_A_UART_8_BIT_LEN;              // Data length is 8 bits

    uart.config.uartMode = EUSCI_A_UART_MODE;                     // UART mode -- the most typical usage
    // Return the completed UART instance
    return uart;
}

/**
 * (Re)initializes and (re)enable the UART module to use a desired baudrate.
 *
 * @param uart_p        The pointer to the uart struct that needs a baudrate and should be enabled
 * @param baudChoice:   The new baud choice with which to update the module
 */
void UART_SetBaud_Enable(UART *uart_p, UART_Baudrate baudChoice)
{
    // We use the system clock for baudrate generation.
    // The processor clock runs at 48MHz in the project. In other words, system clock is 48MHz.
    uart_p->config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;


    // Fortunately for us, for all the baudrates we are working with with f = 48MHz, we use oversampling mode N>16
    uart_p->config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;


    // When using the baudChoice to index the array, we are effectively saying
    // that different baudrate choices correspond with different numbers
    // without requiring excessive amounts of if-statements.
    // DONE: fill these tables such that they work with the proper baudChoice. The first column is given for 9600 BPS
    uint32_t clockPrescalerMapping[NUM_BAUD_CHOICES] = { 312, 156, 78, 52}; //first col 9600, second col 19200, third col 38400, fourth col 57600
    uint32_t firstModRegMapping[NUM_BAUD_CHOICES] = { 8, 4, 2, 1};
    uint32_t secondModRegMapping[NUM_BAUD_CHOICES] = { 0, 0, 0, 0x49};

    // DONE: Replace 0s with the correct statement. Use line 71 as your guide
    uart_p->config.clockPrescalar = clockPrescalerMapping[baudChoice];
    uart_p->config.secondModReg = firstModRegMapping[baudChoice];
    uart_p->config.firstModReg = secondModRegMapping[baudChoice];

    // DONE: initialize and enable uart instance (refer to the basic_example_UART project for guidance)
    UART_initModule(uart_p->moduleInstance, &uart_p->config);
    UART_enableModule(uart_p->moduleInstance);


}


// Not-a-real TODO: read this function and its comment to learn how to implement the other functions
// Notice how we use uart_p's module instance instead of the name of the instance from the basic example where we used no HAL

/**
 * Determines if the user has sent a UART data packet to the board by checking
 * the interrupt status of the proper system UART module.
 *
 * @param uart_p: The pointer to the UART instance with which to handle our operations.
 *
 * @return true if the user has entered a character, and false otherwise
 */
bool UART_hasChar(UART* uart_p)
{
    uint8_t interruptStatus = UART_getInterruptStatus(
            uart_p->moduleInstance, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    return (interruptStatus == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}

/**
 * gets the character currently in the UART terminal.
 * this is done by checking to see if there is a character in the terminal
 * and receiving that data. The value is then returned. If there is not a character,
 * NULL is returned.
 *
 * @param uart_p: The pointer to the UART instance.
 *
 * @return char that was entered by the user into the terminal.
 */
char UART_getChar(UART* uart_p)
{
    char currentChar = '\0';

    //a safety feature in case the user doesn't already call hasChar(). I don't trust people to check lol
    //also it will act as a safety measure against blocking
   // if (UART_hasChar(uart_p))

    currentChar = UART_receiveData(uart_p->moduleInstance); //waits for terminal to have a character then returns it

    return currentChar;
}

/**
 * checks to see if the UART is able to receive a package by checking
 * the interrupt status with the transmit module
 *
 * @param uart_p the pointer to the UART instance
 *
 * @return true if can transmit, else return false
 */
bool UART_canSend(UART* uart_p)
{
    bool canSend =  UART_getInterruptStatus (uart_p->moduleInstance, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    return canSend;
}

/**
 * transmits the given character to the UART by calling a method that sends the character to the given uart module
 *
 * @param uart_p the pointer to the UART instance
 * @param c the character being transmitted to the UART
 *
 */
void UART_putChar(UART* uart_p, char c)
{
    //a fail-safe because I don't trust users to make sure that they can send before they do.
    //if (UART_canSend(uart_p)){
    UART_transmitData(uart_p->moduleInstance, c);

    return;
}

