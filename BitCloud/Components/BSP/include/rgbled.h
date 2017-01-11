/**
  ******************************************************************************
  * @file    rgbled.c 
  * @author  Rait R‰‰k
  * @version V0.0.0
  * @date    02.10.2015
  * @brief   RGB LED Ring driver control routines header file.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RGBLED_H
#define RGBLED_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup RFRING_DRV
  * @{
  */

/** @addtogroup RGBLED
  * @{
  */ 

/** @defgroup RGBLED_Exported_Types Exported Types
  * @{
  */ 

/** RGB LED value structure. */
typedef struct {
	uint16_t r;
	uint16_t g;
	uint16_t b;
} RGBValue_t;

/**
  * @}
  */ 

/** @defgroup RGBLED_Exported_Constants Exported Constants
  * @{
  */ 

/** Number of LEDs. */
#define RGBLED_NR_OF_LEDS 12

/**
  * @}
  */

/** @defgroup RGBLED_Exported_Variables Exported Variables
  * @{
  */ 

/**
  * @}
  */

/** @defgroup RGBLED_Exported_Macros Exported Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup RGBLED_Exported_Functions Exported Functions
  * @{
  */

void BSP_RGBLED_Init (void);
void BSP_RGBLED_Set_Value (uint8_t ledNr, RGBValue_t* value);
void BSP_RGBLED_Set_Brightness_Value (uint8_t value);
void bspRGBLEDHandler (void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* RGBLED_H */

/****END OF FILE****/
