/*
 * LCD test 2.c
 *
 * Created: 3/14/2018 3:46:31 PM
 * Author : Tommy
 */ 

#include <avr/io.h>
//#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "glcd.h"
#include "fonts/font5x7.h"
#include "fonts/Liberation_Sans17x17_Alpha.h"

typedef struct  
{
	unsigned char x;
	unsigned char y; 
} point;

unsigned char move_point(unsigned short direction, point *snake, unsigned char snake_sz)
{
	unsigned char edge = 0;
	point old_head = snake[0];
	
	//shift all points down one, starting with the head at snake[0]
	for (unsigned char i = snake_sz - 1; i > 0; i--) 
	{
		snake[i] = snake[i - 1];
	}
	
	if (direction == 64) //up
	{
		old_head.y > 0 ? old_head.y -= 2 : (edge = 1);
	}
	else if (direction == 128) //down
	{
		old_head.y < 46 ? old_head.y += 2 : (edge = 1);
	}
	else if (direction == 256) //left
	{
		old_head.x > 0 ? old_head.x -= 2 : (edge = 1);
	}
	else if (direction == 512) //right
	{
		old_head.x < 82 ? old_head.x += 2 : (edge = 1);
	}
	
	snake[0] = old_head;
	return edge;
}

unsigned short readController()
{
	unsigned short data = 0x0000;

	//pull latch to read new data
	PORTA |= 0x01; // latch HIGH
	_delay_us(12); // delay 12 us
	PORTA &= 0xFE; // latch LOW
	data |= ~PINA & 0x04; //read data falling edge
	//data = data << 1; //shift bit
	_delay_us(6);

	//read bits
	for(int i = 0; i < 13; i++)
	{
		PORTA |= 0x02; //set clock HIGH
		PORTA &= 0xFD; //set clock LOW
		_delay_us(6);
		data |= ((~PINA & 0x04) << i);
		PORTA |= 0x02;
	}
	return data;
}

unsigned short game_over(unsigned short *dir, point *snake, unsigned char snake_sz, unsigned short *score)
{
	glcd_clear_buffer();
	glcd_tiny_draw_string(15,1,"GAME OVER");
	char score_string[13];
	sprintf(score_string, "SCORE: %d", *score);
	glcd_tiny_draw_string(15, 3, score_string);
	glcd_write();
	_delay_ms(500);
	glcd_tiny_draw_string(10, 5, "Play Again?");
	glcd_write();
	
	//char string[15] = "";
	//glcd_clear_buffer();
	//sprintf(string, "%d %d %d %d %d", snake[0].x, snake[1].x, snake[2].x, snake[3].x, snake[4].x);
	//glcd_tiny_draw_string(10, 0, string);
	//glcd_write();
	
	unsigned short input = 0;
	unsigned short count = 0;
	//char cnt_string[4] = "";
	while (1)
	{
		input = readController();
		_delay_us(500);
		if (input == 32)
		{
			*dir = 512;
			*score = 0;
			for (unsigned char i = 0; i < snake_sz; ++i) {
				snake[i].x = 42 - (2 * i);
				snake[i].y = 24;
			}
			break;
		}
		//sprintf(cnt_string, "%d", count);
		//glcd_tiny_draw_string(10, 5, cnt_string);
		//glcd_write();
		++count;
	}
	
	return count;
}

unsigned char valid_dir(unsigned short data, unsigned short dir)
{
	// Cannot go in opposite direction of current direction
	if ((data == 64  && dir != 128) || 
		(data == 128 && dir != 64)  || 
		(data == 256 && dir != 512) || 
		(data == 512 && dir != 256))
	{
		return 1;		 
	}
	else
	{
		return 0;
	}
}

void reset_fruit(point *fruit)
{
	fruit->x = (rand() % 82) - 1;
	fruit->y = (rand() % 46) - 1;
	if (fruit->x % 2) fruit->x++;
	if (fruit->y % 2) fruit->y++;
}

unsigned short start_menu()
{
	unsigned char menu_start = 0;
	unsigned short count = 0;
	while (1)
	{
		menu_start = readController();
		_delay_us(500);
		
		if (count < 250)
		{
			glcd_tiny_draw_string(10, 3,"Press Start");

		}
		else if (count < 500)
		{
			glcd_tiny_draw_string(10, 3,"           ");
		}
		else
		{
			count = 0;
		}
		
		glcd_write();
		++count;
		if (menu_start == 32)
		{
			break;
		}
	}
	return count;
}

unsigned char self_collision(point *snake, unsigned short size)
{
	for (unsigned short i = 1; i < size; ++i)
	{
		if ((snake[0].x == snake[i].x) && (snake[0].y == snake[i].y))
		{
			return 1;
		}
	}
	return 0;
}

int main(void)
{
	_delay_ms(100);
	//SNES controller initialization
	DDRA = 0x03; PORTA = 0x00;
	DDRC = 0x01; PORTC = 0x00;
	//char string[8] = "";
	
	// LCD Init
	glcd_init();
	glcd_set_contrast(65);
	glcd_clear_buffer();
	glcd_set_font(Liberation_Sans17x17_Alpha, 17, 17, 65, 91);
	glcd_draw_string_xy(7, 7, "SNAKE");
	glcd_write();
	_delay_ms(500);
	glcd_tiny_set_font(Font5x7,5,7,32,127);
	glcd_tiny_draw_string(10, 3,"Press Start");
	glcd_write();
	_delay_ms(15);

	// Game Initialization
	TimerSet(1000);
	TimerOn();
	unsigned short data = 0;
	unsigned short dir = 512;
	unsigned short *dir_ptr = &dir;
	unsigned char edge = 0;
	unsigned short score = 0;
	unsigned short *score_ptr = &score;
	unsigned short snake_sz = 15;
	unsigned short count = 0;
	_delay_ms(10);
	

	point snake[snake_sz];
	memset(snake, 0, snake_sz * sizeof(point));
	for (unsigned short i = 0; i < snake_sz; ++i) {
		//snake body elements will be 2px long, offset by 2px
		snake[i].x = 42 - (2 * i);
		snake[i].y = 24;
	}

	count = start_menu();
	
	srand(count);
	point fruit;
	point *fruitptr = &fruit;
	reset_fruit(fruitptr);
	
    while (1) 
    {
		data = readController();
		if (valid_dir(data, dir))
		{
			edge = move_point(data, snake, snake_sz);
			_delay_ms(20);
			dir = data;
		}
		else
		{
			edge = move_point(dir, snake, snake_sz);
			_delay_ms(20);
		}
		
		if (edge || self_collision(snake, snake_sz))
		{
			count = game_over(dir_ptr, snake, snake_sz, score_ptr);
			srand(count);
			reset_fruit(fruitptr);
			continue;
		}
		
		
		if ((snake[0].x == fruit.x) && (snake[0].y == fruit.y))
		{
			score++;
			reset_fruit(fruitptr);
		}
		
		glcd_fill_rect(fruit.x, fruit.y, 2, 2, BLACK);
		
		for (unsigned char i = 0; i < snake_sz; ++i)
		{
			glcd_fill_rect(snake[i].x, snake[i].y, 2, 2, BLACK);
		}
		
		glcd_write();
		_delay_ms(60);
		glcd_clear_buffer();
    }
	
}

