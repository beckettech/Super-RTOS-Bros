/*
 * sprites.h
 *
 *  Created on: Dec 13, 2022
 *      Author: Beck
 */

#ifndef SPRITES_H_
#define SPRITES_H_

#include "threads.h"
#include "BoardSupport/inc/ILI9341_Lib.h"
#include "BoardSupport/inc/demo_sysctl.h"

#define PLAYER1		10
#define PLAYER2		11
#define PLAYER3		12
#define JUMP		13
#define OBSTACLE1	14
#define LOGO 		15
#define CLOUD1		16
#define CLOUD2		17
#define GROUND		18


void drawSprite(uint16_t x,uint16_t y,uint16_t w,uint16_t h, int sprite);

void drawGoomba(uint16_t x,uint16_t y,uint16_t w,uint16_t h);

#endif /* SPRITES_H_ */
