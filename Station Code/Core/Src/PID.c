#include "PID.h"
//---------------------------------------------------------------------------------
void PID_ini(struct PID* self){
	/**
	 * Reset I nad P buffers
	 */
	self->D_Prev=0;
	self->I=0;
}
//---------------------------------------------------------------------------------
uint32_t PID_it(struct PID* self){
	/**
	 * PID IT Counting control value for a PID algorytm
	 * Return control value
	 * if res control value <0 return 0;
	 * if res control MAX_Control value >0 return MAX_Control;
	 */
	float P,D,Res;

	P=self->Point-self->Point_Now;
	self->I=self->I+P*self->dt;
	D=(P-self->D_Prev)/self->dt;
	self->D_Prev=P;
	Res=((P*self->KP)+(self->I*self->KI)+(D*self->KD));
	if(self->Point_Now-self->Point>100){
		PID_ini(self);
	}

	if(Res<0){
		return 0;
	}
	if (Res<=self->MAX_Control)
		return Res;
	else
		return self->MAX_Control;
}
//---------------------------------------------------------------------------------
void PID_Set_Point(struct PID* self,int Point){
	/**
	 * Set point for what system will be going
	 */
	self->Point=Point;
}
//---------------------------------------------------------------------------------
void PID_Set_Curent_Point(struct PID* self,int Point){
	/*
	 * Set current point
	 */
	self->Point_Now=Point;
}
//---------------------------------------------------------------------------------
void PID_Set_MAX_Control(uint16_t MAX_Control_Value, struct PID* self){
	/*
	 * Set max value witch can PID_it return as result
	 */
	self->MAX_Control=MAX_Control_Value;
}
//---------------------------------------------------------------------------------



