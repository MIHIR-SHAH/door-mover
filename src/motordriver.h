/** @file motordriver.h
 *      This file contains the headers for a class the implements a DRV8871 Motor Driver.
 * 
 *  @author Mihir Shah
 *  @date 2020-Nov-19 Original file.
 */

#include <Arduino.h>

/** @brief    Class which controls a motor through the DRV8871 motor driver.
 *  @details  
 */
class MotorDriver
{
protected:
    uint8_t PWM;           // PWM Value
    uint8_t pin_M1;        // Motor power pins
    uint8_t pin_M2;
    uint8_t direction;     // 1 = forward, 0 = reverse
    
public:
    MotorDriver(uint8_t M1, uint8_t M2);
    void forward(void /** uint8_t pwm_value */);
    void brake(void);
    void reverse(void /** uint8_t pwm_value */);
    void coast(void);
    uint8_t getPWM(void);
    uint8_t getDirection(void);
};