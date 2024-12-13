#include "PID.h"
//---------------------------------------------------------------------------------
void PID_ini(struct PID* self){
	self->D_Prev=0;
	self->I=0;
}
//---------------------------------------------------------------------------------
void PID_Reset(struct PID* self){
	self->D_Prev=0;
	self->I=0;
	self->Point_Now=0;

}
//---------------------------------------------------------------------------------
uint32_t PID_it(struct PID* self){
	float P,D,Res;

	//uint32_t Ress=0;

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
	self->Point=Point;
}
//---------------------------------------------------------------------------------
void PID_Set_Point_Now(struct PID* self,int Point){
	self->Point_Now=Point;
}
//---------------------------------------------------------------------------------



