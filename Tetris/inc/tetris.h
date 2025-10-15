/*
 * tetris.h
 *
 *  Created on: Nov 17, 2024
 *      Author: David Zindović
 */

#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

//#include "tetris.c"

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

void block_draw(uint8_t block_pick, uint32_t block_color, uint32_t block_x_pos, uint32_t block_y_pos, uint8_t rotation, uint8_t *cbc);
void *check_block_collision(uint32_t x, uint32_t y, uint8_t block_shape, uint8_t block_rot,uint32_t color,uint8_t *cbc);
void erase_moved_part(uint8_t block, uint32_t x, uint32_t y, uint32_t old_x, uint32_t old_y, uint8_t rot, uint8_t old_rot,uint8_t *cbc);
void TETRIS_SCORE_DISPLAY(uint32_t rezultat);
uint32_t potenca(uint32_t stevilka,uint32_t POTENCA);
void TETRIS_SCORE_SETUP(void);
void GAME_OVER(void);
void START_GAME(void);
void game_state_init(void);

uint64_t LINE_FILL_CHECK();
void LINE_DELETE(uint64_t line_delete_mask);
void LINE_DOWN_SHIFT(uint64_t gap_mask);

#endif /* INC_TETRIS_H_ */
