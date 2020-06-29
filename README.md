# Snake
Snake game written in Embedded-C

<h3><strong>UPDATED</strong> June 2020:</h3>
<p align="center"><strong>Current Working Version</strong></p>
<p align="center">
  <img src="snake_demo.gif" >
</p>

Programmed on ATmega1284 microcontroller with ATmel Studio.

Link to video demo: https://youtu.be/07_hWhRFD4I

<strong>Working features:</strong>
- Snake can maneuver around the screen and collect fruit
- Fruit location is randomized by using user input as seed value.
- Score is kept and displayed at end of each game. 
- Self-collision
- Snake grows as it collects fruit, with size of array dynamically increasing

<strong>Recent Fixes:</strong>
- Fixed border issue where if direction changed right as snake reached border, the game would only check against the borders in the new direction, allowing the snake to ride on the original border 
- Fruits no longer appear on the border

<strong>Known Bugs:</strong>
- Fruits can appear on the snake's body.

<strong>Features In-Progress:</strong>

- High-score saved permanently in EEPROM. 
