
#ifndef INC_PID_H_
#define INC_PID_H_

#include "stm32f1xx.h"
//---------------------------------------------------------------------------------
struct PID{
	float KP,KI,KD,dt;
	uint16_t MAX_Control;
	float I;
	float D_Prev;
	float Point;
	float Point_Now;

};
//---------------------------------------------------------------------------------
void PID_ini(struct PID* self);
uint32_t PID_it(struct PID* self);
void PID_Set_Point(struct PID* self,int Point);
void PID_Set_Curent_Point(struct PID* self,int Point);
void PID_Set_MAX_Control(uint16_t MAX_Control_Value, struct PID* self);
//---------------------------------------------------------------------------------

#endif /* INC_PID_H_ */
