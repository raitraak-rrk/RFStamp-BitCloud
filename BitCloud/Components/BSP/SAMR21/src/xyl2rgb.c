/**************************************************************************//**
  \file xyl2rgb.c

  \brief
    X/Y/Lumen to RGB converter implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
******************************************************************************/

#if defined(APP_USE_PWM)
/******************************************************************************
                   Includes section
******************************************************************************/

#include "xyl2rgb.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/

#define NR_OF_PRIMES (3u)    /* Red, Blue and Green */
#define SL_LIGHTLEVEL_RED   0u
#define SL_LIGHTLEVEL_GREEN 1u
#define SL_LIGHTLEVEL_BLUE  2u

#define LED_DATA_RED   {0.7079f,0.2920f,150.0f,0.0f} /* 630 nm */
#define LED_DATA_GREEN {0.1547f,0.8059f,220.0f,0.0f} /* 530 nm */
#define LED_DATA_BLUE  {0.1241f,0.0578f,30.0f,0.0f}  /* 470 nm */

/******************************************************************************
                   Types section
******************************************************************************/

typedef struct
{
    float fx;        // x point
    float fy;        // y point
    float fLm;       // lumen output
    float usDC;      // last DutyCycle divided by 65535 (between 0 and 1)
} LedData_t;


/******************************************************************************
                   Prototypes section
******************************************************************************/

/******************************************************************************
                    Local variables
******************************************************************************/

static LedData_t s_ledData[NR_OF_PRIMES] = \
        {LED_DATA_RED, LED_DATA_GREEN, LED_DATA_BLUE};

/******************************************************************************
                   Implementations section
******************************************************************************/
static void calcUsDC(float s_setX, float s_setY, float s_setL)
{
    float fraction;
    float rbg = 0.0F;
    float bbg = 0.0F;
    float rxMSetx = (s_ledData[SL_LIGHTLEVEL_RED].fx - s_setX);
    float ryMSety = (s_ledData[SL_LIGHTLEVEL_RED].fy - s_setY);
    float gxMSetx = (s_ledData[SL_LIGHTLEVEL_GREEN].fx - s_setX);
    float gyMSety = (s_ledData[SL_LIGHTLEVEL_GREEN].fy - s_setY);
    float bxMSetx = (s_ledData[SL_LIGHTLEVEL_BLUE].fx - s_setX);
    float byMSety = (s_ledData[SL_LIGHTLEVEL_BLUE].fy - s_setY);

    // matrix inversion
    float temp = s_ledData[SL_LIGHTLEVEL_GREEN].fy * ((rxMSetx * byMSety) - (bxMSetx * ryMSety));
    if (temp != 0.0F)
    { //robustness to assure that divider != 0
        rbg = (-s_ledData[SL_LIGHTLEVEL_RED].fy * ((gxMSetx * byMSety) - (bxMSetx * gyMSety)))/ temp;
        temp = s_ledData[SL_LIGHTLEVEL_GREEN].fy * ((bxMSetx * ryMSety) - (rxMSetx * byMSety));
        if (temp != 0.0F)
        { //robustness to assure that divider != 0
            bbg = (-s_ledData[SL_LIGHTLEVEL_BLUE].fy * ((gxMSetx * ryMSety) - (rxMSetx * gyMSety)))/ temp;
            temp = rbg + 1.0f + bbg;
            if (temp != 0.0F)
            { //robustness to assure that divider != 0
                temp = s_setL / temp;
            }
        }
    }

    // introduce the lumen output in the fraction

    // calculate the red portion of the mixed light
    if (s_ledData[SL_LIGHTLEVEL_RED].fLm != 0.0F)
    {
        fraction = (temp * rbg) / s_ledData[SL_LIGHTLEVEL_RED].fLm;
        if (fraction > 0.0f)
        {
            s_ledData[SL_LIGHTLEVEL_RED].usDC =  fraction;
        }
    }

    // calculate the green portion of the mixed light
    if (s_ledData[SL_LIGHTLEVEL_GREEN].fLm != 0.0F)
    {
        fraction = temp / s_ledData[SL_LIGHTLEVEL_GREEN].fLm;
        if (fraction > 0.0f)
        {
            s_ledData[SL_LIGHTLEVEL_GREEN].usDC =  fraction;
        }
    }

    // calculate the blue portion of the mixed light
    if (s_ledData[SL_LIGHTLEVEL_BLUE].fLm != 0.0F)
    {
        fraction = (temp * bbg) / s_ledData[SL_LIGHTLEVEL_BLUE].fLm;
        if (fraction > 0.0f)
        {
            s_ledData[SL_LIGHTLEVEL_BLUE].usDC = fraction;
        }
    }
}

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
void XYL2RGB(unsigned int valX, unsigned int valY, int valL, int maxL, unsigned int *calcR, unsigned int *calcG, unsigned int *calcB)
{
  float s_setX, s_setY, valLf, s_setL, maxLf;  
  float s_brightnessTemperatureFactor = 1.0f;
  float s_minimumBri, s_1minMinimumBri;

  maxLf = (float)maxL; // depends on XY point.

  s_minimumBri = 0.01f;  // 1%
  s_1minMinimumBri = 1.0f - s_minimumBri;

  s_setX = (float)valX/65535.0f;
  s_setY = (float)valY/65535.0f;    

  valLf = (float)valL; //normalize the value between 0..254
  if (valLf < 0.1f) //level = 0..254 whereby 1 = minimum and 0 = off
  {
      valLf = 0.0f;
  }
  else if (valLf < 1.0f)
  {
      // Between 0 and 1: linear interpolation between 0 and s_minimumBri (valLf * maxLf * (minimum_percentage of 1.0))
      valLf = valLf * maxLf * s_minimumBri;
  }
  else
  {
      // between 1 and 254: quadratic curve from s_minimumBri*maxLf to maxLf
      valLf -= 1.0f;  //level = 0..253
      valLf *= valLf;
      valLf = ((maxLf * s_minimumBri) + (((s_1minMinimumBri * maxLf)* valLf) / (253.0f * 253.0f)));//253 is now maxlevel (0-253)
  }
  s_setL = valLf;


  for (unsigned int cnt = 0u; cnt < NR_OF_PRIMES; cnt++)
  {
      s_ledData[cnt].usDC = 0.0f;
  }

  if ((s_setX != 0.0f) && (s_setY != 0.0f) && (s_setL != 0.0f))
  {

      calcUsDC(s_setX, s_setY, s_setL);

      /* check for duty cycle > 1.0 */
      float maxFactor = s_ledData[SL_LIGHTLEVEL_RED].usDC;
      if (maxFactor < s_ledData[SL_LIGHTLEVEL_GREEN].usDC)
      {
          maxFactor = s_ledData[SL_LIGHTLEVEL_GREEN].usDC;
      }
      if (maxFactor < s_ledData[SL_LIGHTLEVEL_BLUE].usDC)
      {
          maxFactor = s_ledData[SL_LIGHTLEVEL_BLUE].usDC;
      }

      if (maxFactor > 1.0f)
      {
          maxFactor = s_brightnessTemperatureFactor/maxFactor;
      }
      else
      {
          maxFactor = s_brightnessTemperatureFactor;
      }

      if (maxFactor != 1.0f)
      {
          s_ledData[SL_LIGHTLEVEL_RED].usDC *= maxFactor;
          s_ledData[SL_LIGHTLEVEL_GREEN].usDC *= maxFactor;
          s_ledData[SL_LIGHTLEVEL_BLUE].usDC *= maxFactor;
      }
  }

  float red = s_ledData[SL_LIGHTLEVEL_RED].usDC * 65535.0f;
  float green = s_ledData[SL_LIGHTLEVEL_GREEN].usDC * 65535.0f;
  float blue = s_ledData[SL_LIGHTLEVEL_BLUE].usDC * 65535.0f;

  *calcR = (unsigned int) red;
  *calcG = (unsigned int) green;
  *calcB = (unsigned int) blue;
}
#endif // defined(APP_USE_PWM)
