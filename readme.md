# Door Mover
## **Authors:** _Mihir Shah, Zoe Lam, Andrew Noble_  

## Overview
This source code actuates a motor to open a sliding door based on the temperature difference inside and outside the room.
- Uses FreeRTOS to control task delays
- Uses the ME 507 library's baseshare.h and taskshare.h
- Has its own built-in ISR for counting motor encoder ticks
- Uses the Adafruit library for the MCP9808 Temperature Sensor.
- We made our own class (motordriver.h) for the DRV8871 Motor Driver, using logic pin outputs to the driver chip.

## Usage
We used a custom board to hook up the motor driver and temperature sensors, but the same thing can be accomplished by using MCP9808 Temperature Sensor & DRV8871 Motor Driver breakout boards made by Adafruit.
1. Connect the MCP9808's to the Arduino SCL/SDA lines from the development board.
2. Connect the DRV8871's power inputs to a power supply and the motor input pins to Arduino pins D8 and D9.
3. Connect the encoder power and GND to either 3v3 or 5v and GND on the Arduino board, and connect one of the two encoder signal lines to pin D2 on the board. (D2 is the same as int0 on the Arduino which lets us count encoder ticks via interrupt).
4. Connect a line from 3v3 to pin D3 on the board. This is the "emergency stop" pin. Connecting D3 to GND will cause the system to enter an emergency stop. Reconnecting D3 to 3v3 will allow the system to continue operating.