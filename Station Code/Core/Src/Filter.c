#include"Filter.h"
//--------------------------------------------------------------------------------- Middle Average Filter
uint16_t Filter_Mediana_3(uint16_t a,uint16_t b, uint16_t c){
	if(a<=b){
		if(a<=c){
			if(b<=c)
				return b;
			else
				return c;
		}
		else
			return a;
	}
	else
	{
		if(a<=c){
			return c;
		}
		else
			if(b<=c)
				return c;
			else
				return b;
	}
}

uint16_t Filter_Mediana_9 (uint16_t *mas){
	return	Filter_Mediana_3(
			Filter_Mediana_3(*mas,*(mas+1), *(mas+2)),
			Filter_Mediana_3(*(mas+3),*(mas+4), *(mas+5)),
			Filter_Mediana_3(*(mas+6),*(mas+7), *(mas+8)));
}
//---------------------------------------------------------------------------------
void Filter_ini(struct Filter * self){
	self->Filter_Buffer=0;
}
//---------------------------------------------------------------------------------  Exponential Moving Average Filter
uint16_t Filter_Exp_Mov_Average(struct Filter* self){
		if(abs((int)self->Val_Now - self->Filter_Buffer)>self->Val_Delata)
			self->k=self->k_max;
		else
			self->k=self->k_min;

	  self->Filter_Buffer += (self->Val_Now - self->Filter_Buffer) * self->k;
	  return (uint16_t)self->Filter_Buffer;
}
//---------------------------------------------------------------------------------

uint16_t Filter_Combined(struct Filter* self){
	if(self->Filter_Mode==Three_Samples){
		self->Val_Now=Filter_Mediana_3(*(self->mass),*((self->mass)+1), *((self->mass)+2));
	}
	if(self->Filter_Mode==Nine_Samples){
		self->Val_Now=Filter_Mediana_9(self->mass);
	}
	return Filter_Exp_Mov_Average(self);
}
//---------------------------------------------------------------------------------







