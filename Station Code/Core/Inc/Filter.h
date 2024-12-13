/*
 * Filter.h
 *
 *  Created on: Jul 29, 2024
 *      Author: I
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_
//---------------------------------------------------------------------------------
#include "stm32f1xx.h"
#include "math.h"
#include "stdlib.h"
//---------------------------------------------------------------------------------
struct Filter{
	float k;
	float k_min;
	float k_max;
	float Filter_Buffer;
	float Val_Delata;
	float Val_Now;
	enum Filter_Mode{
		Nine_Samples,
		Three_Samples
	}Filter_Mode;
	uint16_t *mass;
};
//---------------------------------------------------------------------------------

void Filter_ini(struct Filter * self);
uint16_t Filter_Combined(struct Filter* self);
void Filter_Set_Point(struct Filter* self,uint16_t Value);
uint16_t Filter_Mediana_9 (uint16_t *mas);

#endif /* INC_FILTER_H_ */
