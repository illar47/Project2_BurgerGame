/*
 * Joystick.h
 *
 *  Created on: Jun 24, 2021
 *      Author: Illa Rochez
 */

#ifndef HAL_JOYSTICK_H_
#define HAL_JOYSTICK_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define LEFT_THRESHOLD 3000 //can be anything below 8000
#define RIGHT_THRESHOLD 12000
#define DOWN_THRESHOLD 3000
#define UP_THRESHOLD 12000

struct _Joystick{

    uint_fast16_t x;
    uint_fast16_t y;

    bool isTappedUp;
    bool isTappedDown;
    bool isTappedToLeft;
    bool isTappedToRight;
};

typedef struct _Joystick Joystick;

/** Constructs a new Joystick object.*/
Joystick Joystick_construct();

void initADC();
void startADC();
void initJoyStick();

/** Given a Joystick, determines if the joystick is currently pushed pressed from middle to respective direction */
bool Joystick_isPressedToLeft(Joystick* joystick);
bool Joystick_isPressedToRight(Joystick* joystick_p);
bool Joystick_isPressedUp(Joystick* joystick_p);
bool Joystick_isPressedDown(Joystick* joystick_p);

/** Given a Joystick, determines if it was "tapped" - pressed in respective direction and released */
bool Joystick_isTappedUp(Joystick* joystick_p);
bool Joystick_isTappedDown(Joystick* joystick_p);
bool Joystick_isTappedToLeft(Joystick* joystick_p);
bool Joystick_isTappedToRight(Joystick* joystick_p);

/** Refreshes this Joystick  */
void Joystick_refresh(Joystick* joystick);


#endif /* HAL_JOYSTICK_H_ */
