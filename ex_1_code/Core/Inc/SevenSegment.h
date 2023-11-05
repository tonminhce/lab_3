/*
 * SevenSegment.h
 *
 *  Created on: Nov 5, 2022
 *      Author: phamk
 */
#ifndef INC_SEVENSEGMENT_H_
#define INC_SEVENSEGMENT_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "main.h"

extern int bufferIndex;

void display7SEG(int inputNum, int ledOrder);
void update7SEG(int * segment1Buffer, int * segment2Buffer);

#endif /* INC_SEVENSEGMENT_H_ */
