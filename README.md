# Snake
Snake game written in Embedded-C

Programmed on ATmega1284 microcontroller with ATmel Studio.

Link to video demo: https://www.youtube.com/watch?v=wDqO1QHvHE4&t=1s

Working features:

- Snake can maneuver around the screen and collect fruit
- Score is kept and permanently saved using EEPROM on microcontroller

Known Bugs:

- No self-collision implemented
- Using graphics library draw_line() function causes program to draw diagonal line to incorrect corners
