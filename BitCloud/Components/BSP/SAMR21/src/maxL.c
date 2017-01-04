/**************************************************************************//**
  \file MaxL.c

  \brief
    Calculates maximum Lumen value for  X/Y color.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
******************************************************************************/
/******************************************************************************
                    Local variables
******************************************************************************/
#if defined(APP_USE_PWM)

static int s_maxL[10][10] = 
{
{150,150,150,150,150,125,100,100,100,100},
{150,150,160,160,160,150,150,125,100,100},
{150,155,165,165,170,170,160,150,100,100},
{150,155,160,170,180,180,150,100,100,100},
{150,155,165,180,190,180,150,115,100,100},
{150,160,175,190,200,160,130,100,100,100},
{100,125,150,160,160,120,130,100,100,100},
{50,60,100,100,130,105,105,100,100,100},
{30,30,45,75,105,105,105,100,100,100},
{25,25,35,65,105,105,100,100,100,100}
};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Calculates maximum Lumen value for  X/Y color.

\param[in]
    valX - X value of the color
\param[in]
    valY - Y value of the color

\param[out]
    calcMaxL - Pointer to the calculated maximum Lumen value for X/Y color
******************************************************************************/
void maxL_FromXY(unsigned int valX, unsigned int valY, int *calcMaxL)
{
  float x,y,maxl;
  unsigned int xi, yi;

  xi = valX / 6553;
  yi = valY / 6553;

  x = (float)valX/65535.0f - (float)xi/10.0f;
  y = (float)valY/65535.0f - (float)yi/10.0f;

  maxl = (float)s_maxL[9-yi][xi]*(1-x)*(1-y) +
         (float)s_maxL[9-yi][xi+1]*x*(1-y) +
         (float)s_maxL[9-(yi+1)][xi]*(1-x)*y +
         (float)s_maxL[9-(yi+1)][xi+1]*x*y;

  *calcMaxL = (int) maxl;
}
#endif // defined(APP_USE_PWM)
