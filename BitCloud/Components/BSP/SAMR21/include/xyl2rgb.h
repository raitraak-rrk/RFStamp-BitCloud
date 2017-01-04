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
\brief Converts X/Y/L values of the color to corresponding R/G/B values

\param[in]
    valX - X value of the color
\param[in]
    valY - Y value of the color
\param[in]
	valL - level value of the color
\param[in]
	maxL - maximum Lumen value of the X/Y color	
	
\param[out]
    calcR - Pointer to the calculated R (red) value of the color
\param[out]
    calcG - Pointer to the calculated G (green) value of the color
\param[out]
    calcB - Pointer to the calculated B (blue) value of the color
******************************************************************************/
void XYL2RGB(unsigned int valX, unsigned int valY, int valL, int maxL, unsigned int *calcR, unsigned int *calcG, unsigned int *calcB);

