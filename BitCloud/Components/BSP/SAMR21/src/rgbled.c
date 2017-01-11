/**
  ******************************************************************************
  * @file    rgbled.c 
  * @author  Rait R‰‰k
  * @version V0.0.0
  * @date    02.10.2015
  * @brief   RGB LED Ring driver control routines source file
  ******************************************************************************
  */ 

#if ((BSP_SUPPORT == BOARD_RFRINGQT) || (BSP_SUPPORT == BOARD_RFRINGQM))

/* Includes ------------------------------------------------------------------*/
#include <bspTaskManager.h>
#include <sysAssert.h>
#include <sysTypes.h>
#include <gpio.h>
#include <spi.h>
#include <rgbled.h>

/** @addtogroup RFRING_DRV
  * @{
  */

/** @defgroup RGBLED RGBLED Driver
  * @{
  */ 

/** @defgroup RGBLED_Private_Defines Private Defines
  * @{
  */

/** TLC5971 Write command. */
#define TLC5971_WRITE_CMD 0x25

/** TLC5971 Function control bits. */
#define TLC5971_BLANK_DISABLE 0
#define TLC5971_BLANK_ENABLE 1
#define TLC5971_AUTO_DISPLAY_REPEAT_DISABLE 0
#define TLC5971_AUTO_DISPLAY_REPEAT_ENABLE 1
#define TLC5971_DISPLAY_TIMING_RESET_MODE_DISABLE 0
#define TLC5971_DISPLAY_TIMING_RESET_MODE_ENABLE 1
#define TLC5971_GS_CLOCK_INTERNAL 0
#define TLC5971_GS_CLOCK_EXTERNAL 1
#define TLC5971_GS_CLOCK_EDGE_FALLING 0
#define TLC5971_GS_CLOCK_EDGE_RISING 1

/** TLC5971 number of channels. */
#define TLC5971_NR_OF_CH 12

/** Number of drivers. */
#define TLC5971_NR_OF_DRIVERS 3

/**
  * @}
  */

/** @defgroup RGBLED_Private_Macros Private Macros
  * @{
  */
  
 /**
  * @}
  */

/** @defgroup RGBLED_Private_TypesDefinitions Private Types Definitions
  * @{
  */

/** TLC5971 Data. */
typedef struct PACK {
	/** Grayscale data. */
	uint16_t gsData[TLC5971_NR_OF_CH];
	/** Control data. */
	struct {
		/** Brightness control. */
		uint32_t bcData0 :7;
		uint32_t bcData1 :7;
		uint32_t bcData2 :7;
		/** Blank enable. */
		uint32_t BLANK :1;
		/** Auto display repeat enable. */
		uint32_t DSPRPT :1;
		/** Display timing reset mode enable. */
		uint32_t TMGRST :1;
		/** GS reference clock select. */
		uint32_t EXTGCK :1;
		/** GS reference clock edge select. */
		uint32_t OUTTMG :1;
		/** Command. */
		uint32_t cmd :6;
	} ctrlData;	
} TLC5971Data_t;

/**
  * @}
  */


/** @defgroup RGBLED_Global_Variables Global Variables
  * @{
  */

/**
  * @}
  */
  
/** @defgroup RGBLED_Private_Variables Private Variables
  * @{
  */

/** LED positions. */
static const uint8_t LEDPos[RGBLED_NR_OF_LEDS][3] = {
	{2, 1, 0},
	{11, 10, 9},
	{8, 7, 6},
	{17, 16, 15},
	{13, 12, 14},
	{21, 23, 22},
	{18, 20, 19},
	{27, 29, 28},
	{25, 24, 26},
	{33, 35, 34},
	{30, 32, 31},
	{5, 4, 3},
};

/** TLC5971 Data. */
TLC5971Data_t TLC5971Data[TLC5971_NR_OF_DRIVERS];
/** SPI Descritor. */
static HAL_SpiDescriptor_t rgbLEDSpiDescriptor;

/**
  * @}
  */

/** @defgroup RGBLED_Private_FunctionPrototypes Private Function Prototypes
  * @{
  */

static void RGBLED_Update_Values (void);

/**
  * @}
  */

/** @defgroup RGBLED_Functions Functions
  * @{
  */
  
/**
  * @brief  This function initializes LED driver.
  * @retval None
  */
void BSP_RGBLED_Init (void) {
	for (uint8_t i = 0; i < TLC5971_NR_OF_DRIVERS; i++) {
		/* Set all GS data to 0. */
		for (uint8_t j = 0; j < TLC5971_NR_OF_CH; j++)
			TLC5971Data[i].gsData[j] = 0;
		/* Set all BC data to max. */
		TLC5971Data[i].ctrlData.bcData0 = 127;
		TLC5971Data[i].ctrlData.bcData1 = 127;
		TLC5971Data[i].ctrlData.bcData2 = 127;
		/* Disable blanking. */
		TLC5971Data[i].ctrlData.BLANK = TLC5971_BLANK_DISABLE;
		/* Enable auto repeat. */
		TLC5971Data[i].ctrlData.DSPRPT = TLC5971_AUTO_DISPLAY_REPEAT_ENABLE;
		/* Disable display timing reset. */
		TLC5971Data[i].ctrlData.TMGRST = TLC5971_DISPLAY_TIMING_RESET_MODE_DISABLE;
		/* Set internal GS clock. */
		TLC5971Data[i].ctrlData.EXTGCK = TLC5971_GS_CLOCK_INTERNAL;
		/* Set clock edge falling. */
		TLC5971Data[i].ctrlData.OUTTMG = TLC5971_GS_CLOCK_EDGE_FALLING;
		/* Set write command. */
		TLC5971Data[i].ctrlData.cmd = TLC5971_WRITE_CMD;
	}
	
	/* Initialize SPI Interface */
	rgbLEDSpiDescriptor.tty       = SPI_CHANNEL_1;
	/* TODO: As we are writing byte-by-byte and HAL is slow the TLC5971 generates a latch in higher speeds. */
	rgbLEDSpiDescriptor.baudRate  = SPI_CLOCK_RATE_1000;
	rgbLEDSpiDescriptor.clockMode = SPI_CLOCK_MODE0;
	rgbLEDSpiDescriptor.dataOrder = SPI_DATA_MSB_FIRST;
	rgbLEDSpiDescriptor.callback  = NULL;

	HAL_OpenSpi(&rgbLEDSpiDescriptor);
		
	/* Set default values.*/
	RGBLED_Update_Values();
}

/**
  * @brief  This function sets RGB LED grayscale value.
  * @param  LEDNr LED number.
  * @param  Value Pointer to LED value structure.
  * @retval None
  */
void BSP_RGBLED_Set_Value (uint8_t ledNr, RGBValue_t* value) {
	//sysAssert(ledNr < RGBLED_NR_OF_LEDS, BSPRGBLED_INVALID_LED_NUMBER_0);
	
	if (ledNr >= RGBLED_NR_OF_LEDS)
		ledNr = RGBLED_NR_OF_LEDS;
	
	/* Store values. */
	TLC5971Data[LEDPos[ledNr][0] / TLC5971_NR_OF_CH].gsData[LEDPos[ledNr][0] % TLC5971_NR_OF_CH] = value->r;
	TLC5971Data[LEDPos[ledNr][1] / TLC5971_NR_OF_CH].gsData[LEDPos[ledNr][1] % TLC5971_NR_OF_CH] = value->g;
	TLC5971Data[LEDPos[ledNr][2] / TLC5971_NR_OF_CH].gsData[LEDPos[ledNr][2] % TLC5971_NR_OF_CH] = value->b;
	bspPostTask(BSP_RGBLED);
}

/**
  * @brief  This function sets TLC5971 brightness value.
  * @param  value Brightness value 0-127.
  */
void BSP_RGBLED_Set_Brightness_Value (uint8_t value) {
	
	/* Check range. */
	sysAssert((value <= 127), BSPRGBLED_INVALID_BRIGHTNESS_VALUE_0);
	
	if (value > 127)
		value = 127;
	
	/* Store value. */
	for (uint8_t i = 0; i < TLC5971_NR_OF_DRIVERS; i++) {
		TLC5971Data[i].ctrlData.bcData0 = value;
		TLC5971Data[i].ctrlData.bcData1 = value;
		TLC5971Data[i].ctrlData.bcData2 = value;
	}
	bspPostTask(BSP_RGBLED);
}

/**
  * @brief  This function handles RGBLED update.
  * @retval None
  */
void bspRGBLEDHandler (void) {
	RGBLED_Update_Values();
}

/**
  * @brief  This function handles RGBLED update.
  * @retval None
  */
static void RGBLED_Update_Values (void) {
	uint8_t ptr = sizeof(TLC5971Data) - 1;
	ATOMIC_SECTION_ENTER
	do {
		/* Need to write one byte at a time. */
		HAL_WriteSpi(&rgbLEDSpiDescriptor, (uint8_t*)TLC5971Data + ptr, 1);
	} while (ptr--);
	ATOMIC_SECTION_LEAVE
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif

/****END OF FILE****/