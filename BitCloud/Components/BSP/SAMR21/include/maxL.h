/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
***************************************************************************************************/
/**************************************************************************//**
\brief Calculates maximum Lumen value for  X/Y color.

\param[in]
    valX - X value of the color
\param[in]
    valY - Y value of the color
	
\param[out]
    calcMaxL - Pointer to the calculated maximum Lumen value for X/Y color
******************************************************************************/
void maxL_FromXY(unsigned int valX, unsigned int valY, int *calcMaxL);
