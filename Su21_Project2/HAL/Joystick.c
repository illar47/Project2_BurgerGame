/*
 * Joystick.c
 *
 *  Created on: Jun 24, 2021
 *      Author: Illa Rochez
 */

#include<HAL/Joystick.h>

enum _DebounceState
{
    ACTIVE, NOT_ACTIVE
};
typedef enum _DebounceState DebounceState;
/**
 * Constructs a Joystick as a GPIO pushJoystick, given a proper port and pin.
 * Initializes the debouncing and output FSMs.
 *
 * @param port:     The GPIO port used to initialize this Joystick
 * @param pin:      The GPIO pin  used to initialize this Joystick
 *
 * @return a constructed Joystick with debouncing and output FSMs initialized
 */
Joystick Joystick_construct()
{
    // The Joystick object which will be returned at the end of construction
    Joystick Joystick;

    initADC();
    initJoyStick();
    startADC();
    // Return the constructed Joystick object to the user
    //(dont worry about warning, just necessary to return for HAL)
    return Joystick;
}

// Initializing the ADC which resides on SoC
void initADC()
{
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
    ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1, 0);

    // This configures the ADC to store output results
    // in ADC_MEM0 for joystick X.
    // MEM0 is for x and MEM1 is for Y directions of joystick
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}

void startADC()
{
    // Starts the ADC with the first conversion
    // in repeat-mode, subsequent conversions run automatically
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick()
{

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A15,                 // joystick X
                                    ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input for Joystick X
    // A15 is multiplexed on GPIO port P6 pin PIN0

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
    GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    // This configures ADC_MEM1 to store the result from
    // input channel A9 (Joystick Y), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM1,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A9,                 // joystick Y
                                    ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input for Joystick Y
    // A9 is multiplexed on GPIO port P pin PIN

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
    GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

}

/**
 *
 */
void Joystick_refresh(Joystick *joystick_p)
{
    // ADC runs in continuous mode, we just read the conversion buffers

    joystick_p->x = ADC14_getResult(ADC_MEM0);

    //read the Y channel
    joystick_p->y = ADC14_getResult(ADC_MEM1);

    //checks if the joystick is tapped Up
    static DebounceState stateUp = NOT_ACTIVE;
    joystick_p->isTappedUp = false;

    switch (stateUp)
    {
    case ACTIVE:

        if (joystick_p->y < UP_THRESHOLD)
        {
            stateUp = NOT_ACTIVE;
            joystick_p->isTappedUp = true;
        }
        break;

    case NOT_ACTIVE:

        if (joystick_p->y > UP_THRESHOLD)
        {
            stateUp = ACTIVE;
        }
        break;
    }

    //checks if the joystick is tapped Down
    static DebounceState stateDown = NOT_ACTIVE;
    joystick_p->isTappedDown = false;

    switch (stateDown)
    {
    case ACTIVE:

        if (joystick_p->y > DOWN_THRESHOLD)
        {
            stateDown = NOT_ACTIVE;
            joystick_p->isTappedDown = true;
        }
        break;

    case NOT_ACTIVE:

        if (joystick_p->y < DOWN_THRESHOLD)
        {
            stateDown = ACTIVE;
        }
        break;
    }

    //checks if the joystick is tapped to the Right
        static DebounceState stateRight = NOT_ACTIVE;
        joystick_p->isTappedToRight = false;

        switch (stateRight)
        {
        case ACTIVE:

            if (joystick_p->x < RIGHT_THRESHOLD)
            {
                stateRight = NOT_ACTIVE;
                joystick_p->isTappedToRight = true;
            }
            break;

        case NOT_ACTIVE:

            if (joystick_p->x > RIGHT_THRESHOLD)
            {
                stateRight = ACTIVE;
            }
            break;
        }

    //checks if the joystick is tapped to the Left
        static DebounceState stateLeft = NOT_ACTIVE;
        joystick_p->isTappedToLeft = false;

        switch (stateLeft)
        {
        case ACTIVE:

            if (joystick_p->x > LEFT_THRESHOLD)
            {
                stateLeft = NOT_ACTIVE;
                joystick_p->isTappedToLeft = true;
            }
            break;

        case NOT_ACTIVE:

            if (joystick_p->x < LEFT_THRESHOLD)
            {
                stateLeft = ACTIVE;
            }
            break;
        }
}

/**
 * returns if the x value of the joystick is less than its threshold
 *
 * @param joystick_p is the pointer to the joystick peripheral.
 */
bool Joystick_isPressedToLeft(Joystick *joystick_p)
{

    return (joystick_p->x < LEFT_THRESHOLD);
}

/**
 * returns if the x value of the joystick is greater than its threshold
 *
 * @param joystick_p is the pointer to the joystick peripheral.
 */
bool Joystick_isPressedToRight(Joystick *joystick_p)
{
    return (joystick_p->x > RIGHT_THRESHOLD);
}

/**
 * returns if the y value of the joystick is greater than its threshold
 *
 * @param joystick_p is the pointer to the joystick peripheral.
 */
bool Joystick_isPressedUp(Joystick *joystick_p)
{
    return (joystick_p->y > UP_THRESHOLD);
}

/**
 * returns if the y value of the joystick is greater than its threshold
 *
 * @param joystick_p is the pointer to the joystick peripheral.
 */
bool Joystick_isPressedDown(Joystick *joystick_p)
{
    return (joystick_p->y < DOWN_THRESHOLD);
}

/**
 *
 */
bool Joystick_isTappedUp(Joystick *joystick_p)
{
    return (joystick_p->isTappedUp);
}

/**
 *
 */
bool Joystick_isTappedDown(Joystick *joystick_p)
{
    return (joystick_p->isTappedDown);
}

/**
 *
 */
bool Joystick_isTappedToLeft(Joystick *joystick_p)
{
    return (joystick_p->isTappedToLeft);
}

/**
 *
 */
bool Joystick_isTappedToRight(Joystick *joystick_p)
{
    return (joystick_p->isTappedToRight);
}
