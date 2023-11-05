/*
 * SevenSegment.c
 *
 *  Created on: Nov 5, 2022
 *      Author: phamk
 */
#include "SevenSegment.h"

int bufferIndex = 0;

GPIO_TypeDef * enablePort[2][2] = {
	{EN0_GPIO_Port, EN1_GPIO_Port},
	{EN2_GPIO_Port, EN3_GPIO_Port}
};

uint16_t enablePin[2][2] = {
	{EN0_Pin, EN1_Pin},
	{EN2_Pin, EN3_Pin}
};

// 7-SEG Port
GPIO_TypeDef * sevenSegPort[2][7] = {
	{
	 SEG_1_1_GPIO_Port,
	 SEG_1_2_GPIO_Port,
	 SEG_1_3_GPIO_Port,
	 SEG_1_4_GPIO_Port,
	 SEG_1_5_GPIO_Port,
	 SEG_1_6_GPIO_Port,
	 SEG_1_7_GPIO_Port
	},

	{
	 SEG_2_1_GPIO_Port,
	 SEG_2_2_GPIO_Port,
	 SEG_2_3_GPIO_Port,
	 SEG_2_4_GPIO_Port,
	 SEG_2_5_GPIO_Port,
	 SEG_2_6_GPIO_Port,
	 SEG_2_7_GPIO_Port
	}
};

// 7-SEG Pin
uint16_t sevenSegPin[2][7] = {
	{
	 SEG_1_1_Pin,
	 SEG_1_2_Pin,
	 SEG_1_3_Pin,
	 SEG_1_4_Pin,
	 SEG_1_5_Pin,
	 SEG_1_6_Pin,
	 SEG_1_7_Pin
	},

	{
	 SEG_2_1_Pin,
	 SEG_2_2_Pin,
	 SEG_2_3_Pin,
	 SEG_2_4_Pin,
	 SEG_2_5_Pin,
	 SEG_2_6_Pin,
	 SEG_2_7_Pin
	}
};

// 7-SEG Value
uint8_t sevenSegValue[10] = {
	0x40, // 0
	0x79, // 1
	0x24, // 2
	0x30, // 3
	0x19, // 4
	0x12, // 5
	0x02, // 6
	0x78, // 7
	0x00, // 8
	0x10  // 9
};

void display7SEG(int inputNum, int ledOrder){
	for(int i = 0; i < 7; i++) {
		HAL_GPIO_WritePin(sevenSegPort[ledOrder][i], sevenSegPin[ledOrder][i], (sevenSegValue[inputNum] >> i) & 0x01);
	}
}

void update7SEG(int segment1Buffer[], int segment2Buffer[]){
	if (bufferIndex > 1)
		bufferIndex = 0;

	if (bufferIndex == 0){
		display7SEG(segment1Buffer[bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[0][bufferIndex], enablePin[0][bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[0][1], enablePin[0][1], 1);

		display7SEG(segment2Buffer[bufferIndex], 1);
		HAL_GPIO_WritePin(enablePort[1][bufferIndex], enablePin[1][bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[1][1], enablePin[1][1], 1);
	}
	else{
		display7SEG(segment1Buffer[bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[0][bufferIndex], enablePin[0][bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[0][0], enablePin[0][0], 1);

		display7SEG(segment2Buffer[bufferIndex], 1);
		HAL_GPIO_WritePin(enablePort[1][bufferIndex], enablePin[1][bufferIndex], 0);
		HAL_GPIO_WritePin(enablePort[1][0], enablePin[1][0], 1);
	}

	bufferIndex++;
}
