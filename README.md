# Snake
Snake game written in Embedded-C

Programmed on ATmega1284 microcontroller with ATmel Studio.

Working features:

- Snake can maneuver around the screen and collect fruit
- Score is kept and permanently saved using EEPROM on microcontroller

Known Bugs:

- No self-collision implemented
- Using graphics library draw_line() function causes program to draw diagonal line to incorrect corners
