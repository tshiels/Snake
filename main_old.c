/*
 * LCD test 2.c
 *
 * Created: 3/14/2018 3:46:31 PM
 * Author : Tommy
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdio.h>
#include "glcd.h"
#include "fonts/font5x7.h"
#include <avr/eeprom.h>
#include <stdlib.h>


//Prototypes
unsigned short readController(void);
void restart(unsigned char *, unsigned char *, unsigned char *, unsigned char *);

//globals
uint8_t high_score;
uint8_t score = 0;
unsigned char fruit_x;
unsigned char fruit_y;

typedef struct
{
	unsigned char x;
	unsigned char y;
} point;

/*
void align_corners(unsigned char * x_h, unsigned char * y_h, unsigned char * x_t, unsigned char * y_t, point corner[], unsigned char * num_corners)
{
	if (abs(*x_h - corner[*num_corners - 1].x) > abs(*y_h - corner[*num_corners - 1].y)) //find direction of last corner relative to head
	{
		//then is horizontal
		if (*y_h != corner[*num_corners - 1].y)
		{
			//then is not perfectly horizontal
			//set equal to head's y position
			corner[*num_corners - 1].y = *y_h;
		}
	}
	else if (*x_h != corner[*num_corners - 1].x) //vertical and x-coord do not match
	{
		corner[*num_corners - 1].x = *x_h;
	}
	for (unsigned char i = *num_corners - 1; i > 0; i -=1)
	{
		if (abs(corner[i].x - corner[i - 1].x) > abs(corner[i].y - corner[i - 1].y)) //then horizontal
		{
			if (corner[i].y != corner[i - 1].y)
			{
				corner[i - 1].y = corner[i].y;
			}
		}
		else if (corner[i].x != corner[i - 1].x) //else if vertical and x's don't match
		{
			corner[i - 1].x = corner[i].x ;
		}
	}
	if (abs(*x_t - corner[0].x) > abs(*y_t - corner[0].y)) //find direction of last corner relative to tail
	{
		//then is horizontal
		if (*y_t != corner[0].y)
		{
			//then is not perfectly horizontal
			//set equal to head's y position
			corner[0].y = *y_t;
		}
	}
	else if (*x_t != corner[0].x) //vertical and x-coord do not match
	{
		corner[0].x = *x_t;
	}
}
*/
unsigned char move_point(unsigned short data, unsigned char *x, unsigned char *y, unsigned char *x_tail, unsigned char *y_tail,
	point corner[], unsigned char * num_corners)
{
	unsigned char edge = 0;

	if (data == 0x0080) //UP
	{
		if (*y > 9) //y < 48 before
		{
			*y -= 1;
			if (*x_tail < corner[0].x)
			{
				*x_tail +=1;
			}
			else if (*x_tail > corner[0].x)
			{
				*x_tail -= 1;
			}
			else if (*y_tail < corner[0].y)
			{
				*y_tail += 1;
			}
			else if (*y_tail > corner[0].y)
			{
				*y_tail -= 1;
			}
			else if ((*x_tail == corner[0].x) && (*y_tail == corner[0].y)) //delete first corner and shift all others down
			{
				*num_corners -= 1;
				for (unsigned char i = 0; i < *num_corners - 1; i += 1)
				{
					corner[i] = corner[i + 1];
				}
			}
		}
		else
		{
			//game over
			edge = 1;
		}
	}
	else if (data == 0x0100) //DOWN;
	{
		if (*y < 48)
		{
			*y += 1;
			if (*x_tail < corner[0].x)
			{
				*x_tail +=1;
			}
			else if (*x_tail > corner[0].x)
			{
				*x_tail -= 1;
			}
			else if (*y_tail < corner[0].y)
			{
				*y_tail += 1;
			}
			else if (*y_tail > corner[0].y)
			{
				*y_tail -= 1;
			}
			else if ((*x_tail == corner[0].x) && (*y_tail == corner[0].y)) //delete first corner and shift all others down
			{
				*num_corners -= 1;
				for (unsigned char i = 0; i < *num_corners - 1; i += 1)
				{
					corner[i] = corner[i + 1];
				}
			}
		}
		else
		{
			//game over
			edge = 1;
		}
	}
	else if (data == 0x0200) //LEFT
	{
		if (*x > 0)
		{
			*x -= 1;
			if (*x_tail < corner[0].x)
			{
				*x_tail +=1;
			}
			else if (*x_tail > corner[0].x)
			{
				*x_tail -= 1;
			}
			else if (*y_tail < corner[0].y)
			{
				*y_tail += 1;
			}
			else if (*y_tail > corner[0].y)
			{
				*y_tail -= 1;
			}
			else if ((*x_tail == corner[0].x) && (*y_tail == corner[0].y)) //delete first corner and shift all others down
			{
				*num_corners -= 1;
				for (unsigned char i = 0; i < *num_corners - 1; i += 1)
				{
					corner[i] = corner[i + 1];
				}
			}
		}
		else
		{
			//game over
			edge = 1;
		}
	}
	else if (data == 0x0400) //RIGHT
	{
		if (*x < 84)
		{
			*x += 1;
			if (*x_tail < corner[0].x)
			{
				*x_tail +=1;
			}
			else if (*x_tail > corner[0].x)
			{
				*x_tail -= 1;
			}
			else if (*y_tail < corner[0].y)
			{
				*y_tail += 1;
			}
			else if (*y_tail > corner[0].y)
			{
				*y_tail -= 1;
			}
			else if ((*x_tail == corner[0].x) && (*y_tail == corner[0].y)) //delete first corner and shift all others down
			{
				*num_corners -= 1;
				for (unsigned char i = 0; i < *num_corners - 1; i += 1)
				{
					corner[i] = corner[i + 1];
				}
			}
		}
		else
		{
			//game over
			edge = 1;
		}
	}

	return edge;
}

void restart(unsigned char * x_h, unsigned char * y_h, unsigned char * x_t, unsigned char * y_t)
{
	while (1)
	{
		unsigned short temp2 = 0;
		temp2 = readController();
		_delay_ms(50);
		if (temp2 == 64)
		{
			if (score > high_score)
			{
				eeprom_update_byte((uint8_t*)46, score);
			}
			score = 0;
			fruit_x = (rand() % 80) + 4;
			fruit_y = (rand() % 25) + 9;
			*x_h = 42;
			*y_h = 24;
			*x_t = 38;
			*y_t = 24;
			break;
		}
	}
}

void game_over(unsigned char * x_h, unsigned char * y_h, unsigned char * x_t, unsigned char * y_t)
{
	glcd_clear_buffer();
	glcd_tiny_draw_string(15,2,"GAME OVER");
	glcd_write();
	_delay_ms(1000);
	glcd_tiny_draw_string(15, 3, "Restart?");
	glcd_write();
	_delay_ms(15);
	restart(x_h,y_h,x_t,y_t);
}

unsigned short readController()
{
	unsigned short data = 0x0000;

	PORTA |= 0x01; // latch HIGH
	_delay_us(12); // delay 12 us
	PORTA &= 0xFE; // latch LOW
	data |= ~PINA & 0x04; //read data falling edge
	data = data << 1; //shift first bit
	_delay_us(6);

	//read bits
	for(int i = 1; i < 13; i++)
	{
		PORTA |= 0x02; //set clock HIGH
		PORTA &= 0xFD; //set clock LOW
		_delay_us(6);
		data |= ((~PINA & 0x04) << i);
		PORTA |= 0x02;
	}
	return data;
}

int main(void)
{
	//SNES controller initialization
	DDRA = 0x03; PORTA = 0x00;
	DDRC = 0x01; PORTC = 0x00;

	//load high score from EEPROM
	high_score = eeprom_read_byte((uint8_t*)46);
	if (high_score == 0xFF)
	{
		high_score = 0;
	}

	//game initialization
	point snake_head = {.x = 42, .y = 24};
	point snake_tail = {.x = 38, .y = 24};
	point snake_body[25] = {0};
	unsigned char num_corners = 0;
	unsigned char * num_c_ptr = &num_corners;
	unsigned short data = 0;
	unsigned short dir = 0x0400;
	char string[16] = "";
	char string2[16] = "";
	unsigned short menu_start = 0;
	fruit_x = (rand() % 80) + 4;
	fruit_y = (rand() % 25) + 9;
	unsigned char * x_ptr = &snake_head.x;
	unsigned char * y_ptr = &snake_head.y; 
	unsigned char * x_t_ptr = &snake_tail.x;
	unsigned char * y_t_ptr = &snake_tail.y;
	unsigned char edge = 0;
	uint8_t random_seed = eeprom_read_byte((uint8_t*)20);
	random_seed += 1;
	eeprom_update_byte((uint8_t*)20, random_seed);
	srand(random_seed);

	//lcd initialization
	glcd_init();
	glcd_set_contrast(50);
	glcd_tiny_set_font(Font5x7,5,7,32,127);
	glcd_clear_buffer();
	glcd_tiny_draw_string(30,2,"SNAKE");
	glcd_tiny_draw_string(10,3,"Press Start");
	glcd_write();
	_delay_ms(15);

	while (1)
	{
		menu_start = readController();
		_delay_ms(50);
		if (menu_start == 64)
		{
			break;
		}
	}

    while (1) 
    {	

		data = readController();
		if (data == 64) //pressed start
		{
			snake_head.x = 42;
			snake_head.y = 24;
			snake_tail.x = 38;
			snake_tail.y = 24;
			for (unsigned char i = 0; i < num_corners; i += 1)
			{
				snake_body[i].x = 0;
				snake_body[i].y = 0;
			}
			restart(x_ptr,y_ptr,x_t_ptr,y_t_ptr);
		}
		else if (data)
		{
			num_corners += 1;
			edge = move_point(data, x_ptr, y_ptr, x_t_ptr, y_t_ptr, snake_body, num_c_ptr); //if data != 0, move in direction of data and hold
			snake_body[num_corners - 1].x = snake_head.x; //add corner to array of corners
			snake_body[num_corners - 1].y = snake_head.y;
			dir = data;
		}
		else
		{
			edge = move_point(dir,x_ptr,y_ptr, x_t_ptr, y_t_ptr, snake_body, num_c_ptr); //if data == 0, use previous directions
		}

		if ((fruit_x == snake_head.x) && (fruit_y == snake_head.y)) //if fruit eaten
		{
			score += 1;
			fruit_x = (rand() % 80) + 4;
			fruit_y = rand() % 30 + 9;
		}

		if (edge)
		{
			snake_head.x = 42;
			snake_head.y = 24;
			snake_tail.x = 38;
			snake_tail.y = 24;
			for (unsigned char i = 0; i < num_corners; i += 1)
			{
				snake_body[i].x = 0;
				snake_body[i].y = 0;
			}
			game_over(x_ptr, y_ptr, x_t_ptr, y_t_ptr);
		}

		if (score > high_score)
		{
			high_score = score;
			eeprom_update_byte((uint8_t*)46, high_score);
		}

		//draw scores and bounding line
		glcd_clear_buffer();
		sprintf(string, "%d", score);
		sprintf(string2, " hi: %d", high_score);
		glcd_tiny_draw_string(0,0,string);
		glcd_tiny_draw_string(30, 0, string2);
		glcd_draw_line(0, 9, 84, 9, BLACK); 

		//draw fruit and snake
		glcd_set_pixel(fruit_x,fruit_y, BLACK);
		//check to make sure corners, head and tail align
		//align_corners(x_ptr,y_ptr,x_t_ptr,y_t_ptr,snake_body, num_c_ptr);
		glcd_draw_line(snake_tail.x, snake_tail.y, snake_body[0].x, snake_body[0].y, BLACK);
		for (unsigned char i = 0; i < num_corners; i += 1)
		{
			glcd_draw_line(snake_body[i].x, snake_body[i].y, snake_body[i + 1].x, snake_body[i + 1].y, BLACK);
		}
		glcd_draw_line(snake_body[num_corners - 1].x, snake_body[num_corners - 1].y, snake_head.x, snake_head.y, BLACK);

		glcd_write();
		_delay_ms(60);
    }
}