


//definiraš blocke kot 2x3 matriko

//definiraš štartno pozicijo v zgornjem levem kotu. pri rotaciji se spremeni, lahko bi imel vnaprej definirane rotirane kocke
//štartna pozicija lahko zunaj ekrana /frame-a (v primeru rotacije)
//brisanje ekrana oz. stare pozicije od neke točke gor nek omejen doseg

//to do:
//scoring system
//meje kdaj se parkirajo blocki
//speedup

/**
  ******************************************************************************
  * @file    BSP/Src/tetris,c
  * @author  Zindo
  * @brief   Draws block
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tetris.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SQUARE_X_DIM 10
#define SQUARE_Y_DIM 10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const bool blocks[5][6]={{0,1,0,1,1,1},{0,0,1,1,1,1},{0,0,0,1,1,1},{1,0,0,1,1,1},{0,1,1,0,1,1}};
/* Private function prototypes -----------------------------------------------*/
void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos, uint8_t rotation);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Block Draw
  * @param  block_pick: num of block
  * @retval None
  */
void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos, uint8_t rotation)
{
if(rotation>3)
{
	rotation=rotation-rotation/3*3;
}

if(rotation==0 || rotation==2)
{
  for(uint8_t y=0;y<2;y++)
	{
	  for(uint8_t x=0;x<3;x++)
		{
		  if(blocks[block_pick][(y*3+x)*(rotation==0) + ((1-y)*3+(2-x))*(rotation==2)])
		  {
			for(uint8_t block_x_dim=0;block_x_dim<SQUARE_X_DIM;block_x_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_Y_DIM,block_color);
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_Y_DIM+SQUARE_Y_DIM,block_color);
			  }
			for(uint8_t block_y_dim=0;block_y_dim<SQUARE_Y_DIM;block_y_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
			  }
		  }
		}
	}
}

else if (rotation==1 || rotation==3)
{
	for(uint8_t x=0;x<2;x++)
	{
	  for(uint8_t y=0;y<3;y++)
		{
		  if(blocks[block_pick][(x*3+(2-y))*(rotation==1) + ((1-x)*3+y)*(rotation==3)])
		  {
			for(uint8_t block_x_dim=0;block_x_dim<SQUARE_X_DIM;block_x_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_Y_DIM,block_color);
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+block_x_dim,block_y_pos+y*SQUARE_Y_DIM+SQUARE_Y_DIM,block_color);
			  }
			for(uint8_t block_y_dim=0;block_y_dim<SQUARE_Y_DIM;block_y_dim++)
			  {
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
				UTIL_LCD_SetPixel(block_x_pos+x*SQUARE_X_DIM+SQUARE_X_DIM,block_y_dim+block_y_pos+y*SQUARE_Y_DIM,block_color);
			  }
		  }
		}
	}
}

}

/**
  * @}
  */

/**
  * @}
  */



	  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

	  for(uint8_t a=0;a<5;a++)
	  {
		  for(uint8_t r=0;r<4;r++)
		  {
			  block_draw(a,UTIL_LCD_COLOR_LIGHTGREEN,20+r*100,20+a*50,r);
		  }
	  }
