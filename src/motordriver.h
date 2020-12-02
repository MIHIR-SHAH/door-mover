/** @file motordriver.h
 *  @brief This file contains the headers for a class the implements a DRV8871 Motor Driver.
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
    
public:

/** @brief   Create a motor driver given 2 pins
 *  @details This constructor takes two Pins, M1 and M2.
 *  @param   M1 The Pin connected to M1 on the motor
 *  @param   M2 The Pin connected to M2 on the motor
 */
    MotorDriver(uint8_t M1, uint8_t M2);

    /** @brief   Tells the motor to move forward.
 *  @details Motor runs at full speed.
 *  
 *  @returns Nothing.
 */
    void forward(void /** uint8_t pwm_value */);

    /** @brief   Tells the motor to brake.
 *  @details Use this function to stop the motor.
 *  
 *  @returns Nothing.
 */
    void brake(void);


/** @brief   Tells the motor to move in reverse.
 *  @details Motor runs at full speed.
 *  
 *  @returns Nothing.
 */
    void reverse(void /** uint8_t pwm_value */);

    /** @brief   Tells the motor to coast.
 *  @details Use this function to allow the motor to freely rotate.
 *  
 *  @returns Nothing.
 */
    void coast(void);
    uint8_t getPWM(void);
    uint8_t getDirection(void);
};