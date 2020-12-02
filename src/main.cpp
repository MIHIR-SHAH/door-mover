/** @file main.cpp
 *    This file contains the main program that automates door motion.
 * 
 *  @author Zoe Lam
 *  @author Andrew Noble
 *  @author Mihir Shah
 *  @author Various creaters of read files
 * 
 *  @date   2020-Nov-12 Original file
 *  @date   2020-Nov-19 MS Added motor task, emergency stop and distance measurement task, and encoder
 *  @date   2020-Nov-24 ZL Modified tasks
 *  @date   2020-Nov-25 AN Updated motor encoder tick values, small adjustments to task priorities
 *  @date   2020-Nov-26 MS Small updates to if statements and while loops to ensure motor will stop at certain distance. 
 *                         Also added some testing code for virtual testing using the wrong parts...
 *  @date   2020-Nov-28 ZL, MS Work on debugging code
 *  @date   2020-Nov-29 ZL, MS Completed debugging and commented code
 *  @date   2020-Nov-30 ZL, added  bool closed to check if door is open or closed
 *  @date   2020-Nov-30 MS, debugging bool closed to check if door is open or closed, 
 *                          updated emergency stop functionality, beautified the code
 * 
 */

//------------ Include statements ------------------//

#include <Arduino.h>
#include <PrintStream.h>
#if (defined STM32L4xx || defined STM32F4xx)
    #include <STM32FreeRTOS.h>
#endif

#include "taskshare.h"          // @author J. R. Ridgely
#include <Wire.h>           
#include "Adafruit_MCP9808.h"   // @author Adafruit
#include "motordriver.h"        // @author Mihir Shah


//------------- Creating objects/variables ---------//

// Create the inside and outside MCP9808 temperature sensor objects
Adafruit_MCP9808 tempOutside = Adafruit_MCP9808();      // chip on custom board
Adafruit_MCP9808 tempInside = Adafruit_MCP9808();       // breakout board

// Create motor driver object
MotorDriver moto = MotorDriver(D8, D9);

// Universal variable to count motor ticks
volatile int32_t ticks;

//------------- Inter-task shares and queues --------------------//

Share<bool> emergency ("Emergency Stop");                 // Create share for emergency stop
Share<float> temp_difference ("Temperature Difference");  // Create share for temperature difference
                                  

/** @brief    Read temperatures and calculate the difference
 *  @details  This task reads the inside and outside temperatures from the respective sensors. It
 *            then subtracts the outdoor temperature from the inside temperature and places the 
 *            difference in the shared variable.
 *  @param    p_params A pointer to function parameters which we don't use.
 */
void task_temp_difference(void* p_params)
{
  (void)p_params;            // Does nothing but shut up a compiler warning

  // set up variable to be put into shared temp_difference
  float tempDifference;

  // infinite loop for the RTOS
  for(;;)
  {
    // Take the difference between the inside and outside temperature
    float inside = tempInside.readTempF();
    float outside = tempOutside.readTempF();
    tempDifference = inside - outside;
    temp_difference.put(tempDifference);    // Put the temperature difference into the Share temp_difference

    Serial << "Temperature difference (F): " << tempDifference << endl;   // Print difference to serial port

    vTaskDelay(2000/portTICK_PERIOD_MS);    // Delay task for 2 seconds NEED TO CHANGE TO IRL TIME
  }
}

/** @brief    Task that operates motor.
 *  @details  This task will actualize the motor in a certain direction depending on the temperature
 *            difference. If the difference is above 10 degrees the motor will move forward to open.
 *            If the difference is less than 3 the motor will move in reverse to close the door. If 
 *            the difference is in between, the door will stay idle. Motion will stop after a set 
 *            number of ticks is reached.
 *  @param    p_params A pointer to function parameters which we don't use.
 */
void task_motor_movement(void* p_params)
{
  (void)p_params;               // Does nothing but shut up a compiler warning
  
  float delta;                  // To hold value retrieved from Share temp_difference
  float max_delta = 5.000;      // Max temperature difference (inside - outside)
                              
  float min_delta = 3.000;      // Min temperature difference (inside - outside)

  long dist_count = 2300;       // (not the right number rn) Number of revolutions to travel 24in * number of ticks to complete 1 revolution
                                // (24 in)*(16 thd/in)* 240 pulses per rev * 1 rev/thd
                                // note: ~240 ticks for 1 revolution by hand
                                // looking down leadscrew, cw rotation causes door to open
                                
  bool emergency_stop;          // create var to hold Share emergency
  bool closed = true;           // Is the door open or closed? Assuming system begins operation with closed door.
  for(;;)
  {
    temp_difference.get(delta);             // Retrieve value from shared variable

      if(delta > max_delta && closed)       // Compares temperature difference, checks if door is closed
      {

        while (ticks <= dist_count)         // Motor will move forward until ticks reaches dist_count
        {
          emergency.get(emergency_stop); 
          if(!emergency_stop)               // Continue if emergency_stop == false
          {
            moto.forward();                 // Commands motor to run in the forward direction
            Serial << "Decoder counts: " << ticks << endl;   // Test code
          }
          else                              // If emergency_stop == true, stop the motor
          {
            moto.brake();
          }                                 // BUT STAY INSIDE THE WHILE LOOP, WAITING FOR IT TO BE SAFE TO CONTINUE OPENING DOOR
        }                                   
        ticks = 0;                          // Resets ticks variable only after the door has completely opened;
                                            // in the event of an emergency stop, door will continue to finish opening once 
                                            // it is safe to do so.
        
        closed = false;                     // set bool closed to false, indicating that door is fully open.
        moto.brake();                       // stop the motor             
      }                         

      else if (min_delta >= delta && !closed)   // Compare temperature difference to minimum required to close the door
                                                // Also checks to ensure door is fully open
      {
        while (ticks <= dist_count)             // Motor will move in reverse until ticks reached dist_count
        {
          emergency.get(emergency_stop);        // Check for an emergency stop
          if(!emergency_stop)                   // Continue if emergency_stop == false
          {
            moto.reverse();                     // Tells motor to move in reverse direction
            Serial << "Decoder counts: " << ticks << endl;   // Test code
          }
          else                                  // emergency_stop == true
          {
            moto.brake();                       // stop the motor
          }                                     // BUT STAY INSIDE THE WHILE LOOP UNTIL IT IS SAFE TO CONTINUE CLOSING DOOR
        }                            

        ticks = 0;                              // Resets ticks variable only after the door has completely closed;
                                                // in the event of an emergency stop, door will continue to finish closing 
                                                // once it is safe to do so.

        closed = true;                          // set bool closed to true, indicating that the door is fully closed
        moto.brake();                           // and stop the motor
      }
      else                                      // When the temperature difference is in between set bounds
      {
        moto.brake();                           // Make sure the motor is stopped
      }
    
    vTaskDelay(10000/portTICK_PERIOD_MS);       // Delay the task from running for another [X] seconds/minutes
  } 
}

/** @brief    Emergency stop task.
 *  @details  Can be used to use any sort of input to convey an "emergency stop" signal to the motor movement task.
 * 
 *  @param    p_params Does nothing but shut up a compiler warning
 */
void task_e_stop(void* p_params)
{
  (void)p_params;            // Does nothing but shut up a compiler warning

  const TickType_t sim_period = 500;         // IRL 10 minutes. RTOS ticks (ms) between runs (500ms for testing purposes)

  // Initialise the xLastWakeTime variable with the current time.
  // It will be used to run the task at precise intervals
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // uint8_t distance_we_want = 5;
  pinMode(D3, INPUT);
  for(;;)
  {
    // uint8_t measured_distance = ulta.measure();

    // Serial << "Measured Distance: " << measured_distance << endl;
    
    if(digitalRead(D3) == HIGH /** measured_distance < distance_we_want */)
    {
      emergency.put(true);
      Serial << "--- EMERGENCY STOP ---" << endl;
      moto.brake();
    }
    else
    {
      emergency.put(false);
    }   
    vTaskDelayUntil (&xLastWakeTime, sim_period);   // Precision delay that accouts for the time it takes the task to run
  }
}

/** @brief    ISR that counts encoder ticks.
 *  @details  Is called whenever input is detected on interrupt pin.
 *            Increases tick count each time motor disk passes the encoder magnet.
 */
void magnet_detect()
{
    ticks++; 
}

/** @brief   Arduino setup function which runs once at program startup.
 *  @details This function sets up a serial port for communication and creates
 *           the tasks which will be run.
 */
void setup() 
{
  Serial.begin (115200);
  delay (3000);

  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example, also can be left in blank for default address use
  // Also there is a table with all addres possible for this sensor, you can connect multiple sensors
  // to the same i2c bus, just configure each sensor with a different address and define multiple objects for that
  //  A2 A1 A0 address
  //  0  0  0   0x18  this is the default address
  //  0  0  1   0x19
  //  0  1  0   0x1A
  //  0  1  1   0x1B
  //  1  0  0   0x1C
  //  1  0  1   0x1D
  //  1  1  0   0x1E
  //  1  1  1   0x1F

  Serial << "Lets automate a door!" << endl;

  // Connect to breakout and PCB temperature sensors
  if (!tempOutside.begin(0x18)) 
  {
    Serial.println("Couldn't find outside MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
  if (!tempInside.begin(0x19)) 
  {
    Serial.println("Couldn't find inside MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
  
  Serial.println("Found inside and outside MCP9808!");
  
  tempOutside.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
  tempInside.setResolution(3);

  tempOutside.wake();   // wake up, ready to read!
  tempInside.wake();


  temp_difference.put(4);    // Put the temperature difference into the shared variable

  // Encoder tick counter setup
  pinMode(D2, INPUT_PULLUP);  // Using this pin as one of the encoder input pins
  attachInterrupt(digitalPinToInterrupt(D2), magnet_detect, RISING);  // Setting up interrupt for encoder tick counting

  emergency.put(false);       // KEEP THIS IF/UNTIL WE ADD IN THE ULTRASONIC SENSOR MODULE
  // WE CAN ALSO USE THE EMERGENCY BOOLEAN WITH THE USER BUTTON OR ANOTHER PIN ON THE NUCLEO.

  // Creates a task that will measure distance
  // xTaskCreate (task_distance_measurement,
  //               "Temperature",                   // Name for printouts
  //               1536,                            // Stack size
  //               NULL,                            // Parameters for task fn.
  //               1,                               // Priority
  //               NULL);                           // Task handle
  // Creates a task that will measure temperature
  xTaskCreate (task_temp_difference,
                "Temperature",                   // Name for printouts
                1536,                            // Stack size
                NULL,                            // Parameters for task fn.
                3,                               // Priority
                NULL);                           // Task handle
  // Creates a task that will actuate a motor
  xTaskCreate (task_motor_movement,
                "Motor",                   // Name for printouts
                1536,                            // Stack size
                NULL,                            // Parameters for task fn.
                2,                               // Priority
                NULL);                           // Task handle
  // Creates a task that will do emergency stop
  xTaskCreate (task_e_stop,
                "Emergency",                   // Name for printouts
                1536,                            // Stack size
                NULL,                            // Parameters for task fn.
                5,                               // Priority
                NULL);                           // Task handle

    // If using an STM32, we need to call the scheduler startup function now;
    // if using an ESP32, it has already been called for us
    #if (defined STM32L4xx || defined STM32F4xx)
        vTaskStartScheduler ();
    #endif   
}

void loop() {
  // put your main code here, to run repeatedly:
}