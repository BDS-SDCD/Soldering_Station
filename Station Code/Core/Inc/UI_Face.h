/*
 * Face_UI.c
 *
 *  Created on: Dec 18, 2024
 *      Author: I
 */

#ifndef INC_FACE_UI_C_
#define INC_FACE_UI_C_

#include "UI_Base_Func.h"
#include "Soldering_Station.h"
#include "Button.h"

extern struct Soldering_Iron Soldering_Iron;
extern struct Soldering_Heat_Gun Soldering_Heat_Gun;
extern struct Soldering_Separator Soldering_Separator;
extern struct PAC *PAC;


void Face_UI_it(struct Encoder *Encoder_P1, struct Encoder *Encoder_P2, struct Button_Vector *Button_Vector);
#endif /* INC_FACE_UI_C_ */
