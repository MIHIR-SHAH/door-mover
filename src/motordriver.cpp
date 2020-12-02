/** @file motordriver.cpp
 *  @brief  This file contains source code for a class that implements a motor driver for the DRV8871 chip.
 * 
 *  @author Mihir Shah
 *  @date  2020-Nov-19 Original File
 */

#include <Arduino.h>
#include "motordriver.h"

uint8_t PWM;            // PWM Variable (0->255)
uint8_t pin_M1;         // Motor power pins
uint8_t pin_M2;             

/** @brief   Create a motor driver given 2 pins
 *  @details This constructor takes two Pins, M1 and M2.
 *  @param   M1 The Pin connected to M1 on the motor
 *  @param   M2 The Pin connected to M2 on the motor
 */
MotorDriver::MotorDriver (uint8_t M1, uint8_t M2)
{
    // Save the M1 Pin, which will evaporate when the constructor exits
    pin_M1 = M1;

    // Save the M2 Pin, which will evaporate when the constructor exits
    pin_M2 = M2;

    pinMode(pin_M1, OUTPUT); // Sets the M1 as an OUTPUT
    pinMode(pin_M2, OUTPUT); // Sets the M2 as an OUTPUT

    digitalWrite(pin_M1, 0);
    digitalWrite(pin_M2, 0);
}

/** @brief   Tells the motor to move forward.
 *  @details Motor runs at full speed.
 *  
 *  @returns Nothing.
 */
void MotorDriver::forward(void)
{
    digitalWrite(pin_M1, 0);        // Reset pins to low
    digitalWrite(pin_M2, 0);        // Reset pins to low

    // analogWrite(pin_M1, pwm_value); // Write PWM to pin_M1
    digitalWrite(pin_M1, 1);
}

/** @brief   Tells the motor to brake.
 *  @details Use this function to stop the motor.
 *  
 *  @returns Nothing.
 */
void MotorDriver::brake(void)
{
    digitalWrite(pin_M1, 1);        // Reset pins to high
    digitalWrite(pin_M2, 1);        // Reset pins to high
}


/** @brief   Tells the motor to move in reverse.
 *  @details Motor runs at full speed.
 *  
 *  @returns Nothing.
 */
void MotorDriver::reverse(void /** uint8_t pwm_value */)
{
    digitalWrite(pin_M1, 0);        // Reset pins to low
    digitalWrite(pin_M2, 0);        // Reset pins to low

    // analogWrite(pin_M2, pwm_value); // Write PWM to pin_M2 (reverses direction)
    digitalWrite(pin_M2, 1);
}

/** @brief   Tells the motor to coast.
 *  @details Use this function to allow the motor to freely rotate.
 *  
 *  @returns Nothing.
 */
void MotorDriver::coast(void)
{
    digitalWrite(pin_M1, 0);        // Reset pins to low
    digitalWrite(pin_M2, 0);        // Reset pins to low
}


