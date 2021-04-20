/*
 * finalCustom.c
 *
 * Created: 6/4/2019 9:50:44 PM
 * Author : matt_
 */ 

#include <avr/io.h>
#include "nokia5110.h"
//#include <util/delay.h>
#include "SNES.h"
#include <avr/eeprom.h>
#include "timer.h"

typedef struct Player 
{
	uint8_t x;
	uint8_t y;
}player;

typedef struct Enemy{
	uint8_t x;
	uint8_t y;
}enemy;

typedef struct Task{
	int	state;
	int period;
	int elapsedTime;
	int (*TickFct)(int);
}task;

player player1;
unsigned short input = 0x0000;
unsigned char gameFlag = 0;
enum PLAYER_STATES {PLAYER_INIT, PLAYER_MOVE} PLAYER_STATE;
int TickFct_Player(int PLAYER_STATE){
	int next_state;
	switch(PLAYER_STATE){
		case PLAYER_INIT:
		if (gameFlag == 1)
		{
			next_state = PLAYER_MOVE;
		}
		else{
			next_state = PLAYER_INIT;	
		}
			break;
		case PLAYER_MOVE:
			if (gameFlag == 1)
			{
				next_state = PLAYER_MOVE;
			}
			if (gameFlag == 0)
			{
				next_state = PLAYER_INIT;
			}
			break;
		default:
			next_state = PLAYER_INIT;
			break;
	}
	switch(PLAYER_STATE){
		case PLAYER_INIT:
			if (gameFlag == 1)
			{
				player1.x = 42;
				player1.y = 42;
			}
			break;
		case PLAYER_MOVE:
		if (gameFlag == 1)
		{
			input = SNES_Read();
			nokia_lcd_set_cursor(player1.x,player1.y);
			nokia_lcd_write_char(' ',1);
			if (input == SNES_A && (player1.x < 80))//SNES_A, move right
			{
				player1.x = player1.x + 3;
			}
			else if (input == SNES_Y && (player1.x > 4))//SNES_Y, move left
			{
				player1.x -= 3;
			}
			else if (input == SNES_B && (player1.y < 40))//SNES_B, move down
			{
				player1.y += 3;
			}
			else if (input == SNES_X)//SNES_X, move up
			{
				player1.y -= 3;

			}
			nokia_lcd_set_cursor(player1.x,player1.y);
			nokia_lcd_write_char('*',1);
			nokia_lcd_render();
		}
			break;
		default:
			break;
	}
	PLAYER_STATE = next_state;
	return PLAYER_STATE;
};

enum ENEMY_STATES {ENEMY_INIT, ENEMY_MOVE}ENEMY_STATE;
enemy arr[30];
unsigned char speed = 1;
int TickFct_Enemy(int ENEMY_STATE){
	int next_state;
	switch(ENEMY_STATE){
		case ENEMY_INIT:
			if (gameFlag == 1)
			{
				next_state = ENEMY_MOVE;
			}
			else{
				next_state = ENEMY_INIT;
			}
			break;
		case ENEMY_MOVE:
		if (gameFlag == 1)
		{
			if (arr[10].x < 0)
			{
				next_state = ENEMY_INIT;
			}
			else{
				next_state = ENEMY_MOVE;
			}
		}
			break;
		default:
			break;
	}
	switch(ENEMY_STATE){
		case ENEMY_INIT:
		if (gameFlag == 1)
		{
			for (unsigned char i = 0; i < 10; i++)
			{
				if (i%2 == 0)
				{
					arr[i].x = (rand() % 80);
					arr[i].y = (rand() % 30) + 8;
					nokia_lcd_set_cursor(arr[i].x,arr[i].y);
					nokia_lcd_write_char('=',1);
					nokia_lcd_render();
				}
				else{
					arr[i].x = (rand() % 80);
					arr[i].y = (rand() % 30) + 8;
					nokia_lcd_set_cursor(arr[i].x,arr[i].y);
					nokia_lcd_write_char('=',1);
					nokia_lcd_render();
				}
			}
		}
		break;
		case ENEMY_MOVE:
		if (gameFlag == 1)
		{
			for (unsigned char j = 0; j < 10; j++)
			{
				if (j%2 == 0)
				{
					nokia_lcd_set_cursor(arr[j].x,arr[j].y);
					nokia_lcd_write_char(' ',1);
					if (arr[j].x < 80)
					{
						arr[j].x = arr[j].x + speed;
					}
					else{
						arr[j].x = 0;
						arr[j].y = (rand() % 30) + 8;
					}
					nokia_lcd_set_cursor(arr[j].x,arr[j].y);
					nokia_lcd_write_char('=',1);
					nokia_lcd_render();
				}
				else{
					nokia_lcd_set_cursor(arr[j].x,arr[j].y);
					nokia_lcd_write_char(' ',1);
					if (arr[j].x > 4)
					{
						arr[j].x = arr[j].x - speed;
					}
					else{
						arr[j].x = 80;
						arr[j].y = (rand() % 30) + 8;
					}
					nokia_lcd_set_cursor(arr[j].x,arr[j].y);
					nokia_lcd_write_char('=',1);
					nokia_lcd_render();
				}
				
			}
		}	
		break;
	}
	ENEMY_STATE = next_state;
	return ENEMY_STATE;
}

enum HIT_STATES {CHECK, RESET} HIT_STATE;
unsigned char hitFlag = 0x00;
int TickFct_Hit(int HIT_STATE){
	int next_state;
	switch(HIT_STATE){
		case CHECK:
			if (gameFlag == 1)
			{
				next_state = CHECK;
			}
			else{
				next_state = RESET;
			}
			break;
		default:
		case RESET:
		if (gameFlag == 1)
		{
			next_state = CHECK;
		}
		else{
			next_state = RESET;
		}
			break;
	}
	switch(HIT_STATE){
		case CHECK:
			if (gameFlag == 1)
			{
				for (unsigned char k = 0; k < 10; k++)
				{
					if ((arr[k].x < (player1.x + 8) && arr[k].x > player1.x) && ((arr[k].y > player1.y) && (arr[k].y < (player1.y + 8))))
					{
						hitFlag = 1;
					}
				}
			}
		break;
		case RESET:
			hitFlag = 0;
			break;
	}
	HIT_STATE = next_state;
	return HIT_STATE;
}

enum GAME_STATES {GAME_START, GAME_BUFFER, GAME_PLAY, GAME_WIN, GAME_LOSE} GAME_STATE;
char score;
char highScore;
char winFlag = 0;
int TickFct_Game(int GAME_STATE){
	int next_state;
	switch(GAME_STATE){
		case GAME_START:
			input = SNES_Read();
			if (input == SNES_START)
			{
				next_state = GAME_BUFFER;
				gameFlag = 1;
				//winFlag = 0;
			}
			else{
				next_state = GAME_START;
			}
			break;
		case GAME_BUFFER:
			if (gameFlag == 1)
			{
				next_state = GAME_PLAY;
			}
			else
			{
				next_state = GAME_BUFFER;
			}
			break;
		case GAME_PLAY:
			input = SNES_Read();
			if (hitFlag == 1)
			{
				next_state = GAME_LOSE;
			}
			else if(hitFlag == 0 && player1.y == 0){
				speed++;
				score++;
				next_state = GAME_WIN;
			}
			else if(hitFlag == 0 && input != SNES_SELECT && input != SNES_L){
				next_state = GAME_PLAY;
			}
			else if(hitFlag == 0 && input == SNES_SELECT){
				gameFlag = 0;
				next_state = GAME_START;
			}
			else if (hitFlag == 0 && input == SNES_L)
			{
				gameFlag = 0;
				speed = 1;
				score = 0;
				next_state = GAME_START;
			}
			break;
		case GAME_WIN:
			input = SNES_Read();
			if(input == SNES_SELECT)
			{
				next_state = GAME_START;
				gameFlag = 1;
			}
			else if (input == SNES_L)
			{
				gameFlag = 0;
				speed = 1;
				score = 0;
				next_state = GAME_START;
			}
			else{
				next_state = GAME_WIN;
			}
			break;
		case GAME_LOSE:
			input = SNES_Read();
			if(input == SNES_SELECT)
			{
				next_state = GAME_START;
				gameFlag = 1;
			}
			else if (input == SNES_L)
			{
				gameFlag = 0;
				speed = 1;
				score = 0;
				next_state = GAME_START;
			}
			else{
				next_state = GAME_LOSE;
			}
			break;
		default:
			next_state = GAME_START;
			break;
	}
	switch(GAME_STATE){
		case GAME_START:
			highScore = eeprom_read_byte((uint8_t) 0);
			if (highScore == 0xFF)
			{
				highScore = 0x00;
				eeprom_write_byte((uint8_t) 0, highScore);
			}
			if (hitFlag == 1)
			{
				score = 0;
			}
			PORTD = highScore;
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("Ready?",1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string("Press Start!",1);
			nokia_lcd_render();
			break;
		case GAME_BUFFER:
			//score++;
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("      ",1);
			nokia_lcd_set_cursor(0,10);
			nokia_lcd_write_string("            ",1);
			nokia_lcd_render();
			break;
		case GAME_PLAY:
			//score++;
			PORTD = score;
			nokia_lcd_render();
			break;
		case GAME_LOSE:
			if (score > highScore)
			{
				highScore = score;
				eeprom_write_byte((uint8_t)0, highScore);
			}
			gameFlag = 0;
			PORTD = score;
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("YOU LOST! Press Select",1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_char(':',3);
			nokia_lcd_set_cursor(20,20);
			nokia_lcd_write_char('(',3);
			nokia_lcd_render();
			break;
		case GAME_WIN:
			if (score > highScore)
			{
				highScore = score;
				eeprom_write_byte((uint8_t)0, highScore);
			}
			gameFlag = 0;
			PORTD = score;
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0,0);
			nokia_lcd_write_string("YOU WON! Press Select",1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_char(':',3);
			nokia_lcd_set_cursor(20,20);
			nokia_lcd_write_char(')',3);
			nokia_lcd_render();
			break;
	}
	GAME_STATE = next_state;
	return GAME_STATE;
};

int main(void)
{
	DDRA = 0x03; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//OLD CODE FROM WHEN I WASNT USING TASKS, FOR DEBUGGING!
	//player1.x = 42;
	//player1.y = 42;
	/*nokia_lcd_init();
	nokia_lcd_clear();
	SNES_init();
	int state = PLAYER_INIT;
	int state2 = ENEMY_INIT;
	int state3 = CHECK;
	int state4 = GAME_START;
    while (1) 
    {
		state = TickFct_PLayer(state);
		state2 = TickFct_Enemy(state2);
		state3 = TickFct_Hit(state3);
		state4 = TickFct_Game(state4);
		_delay_ms(1000);
    }
	*/
	long timerPeriod = 100;
	
	task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
		
	task1.state = PLAYER_INIT;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &TickFct_Player;

	task2.state = ENEMY_INIT;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &TickFct_Enemy;

	task3.state = CHECK;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &TickFct_Hit;

	task4.state = GAME_START;
	task4.period = 100;
	task4.elapsedTime = task4.period;
	task4.TickFct = &TickFct_Game;
	/*
	task5.state = SCORE_INIT;
	task5.period = 100;
	task5.elapsedTime = task5.period;
	task5.TickFct = &TickFct_Score;
	*/
	TimerSet(timerPeriod);
	TimerOn();
	nokia_lcd_init();
	SNES_init();
	
    while(1){
		unsigned char i;
		for (i = 0;i < 4;++i) {
			if (tasks[i]->elapsedTime >= tasks[i]->period) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += timerPeriod;
		}
		while(!TimerFlag){};
		TimerFlag = 0;
	}
}

