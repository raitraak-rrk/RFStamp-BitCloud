/**************************************************************************//**
\file   ofdMemoryDriver.h

\brief  The implementation interface of external flash.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    31/07/09 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _OFDMEMORYDRIVER_H
#define _OFDMEMORYDRIVER_H

#if APP_USE_OTAU == 1
/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if EXTERNAL_MEMORY == AT25F2048

  #if !defined(ATMEGA1281) && !defined(ATMEGA128RFA1)
    #error 'at25f2048 is supported only for atmega1281/128rfa1.'
  #endif

  // flash instruction format (see at25f2048 manual)
  #define WREN          0x06
  #define WRDI          0x04
  #define RDSR          0x05
  #define WRSR          0x01
  #define READ          0x03
  #define PROGRAM       0x02
  #define SECTOR_ERASE  0x52
  #define CHIP_ERASE    0x62
  #define RDID          0x15

  // status register bits
  #define RDY         0x01
  #define WEN         0x02
  #define BP0         0x04
  #define BP1         0x08
  #define WPEN        0x80

  // Atmel ID
  #define OFD_MANUFACTURER_ID   0x1F
  // at25f2048
  #define OFD_DEVICE_ID         0x63

  #define PAGE_SIZE                 256

  // flash sectors
  #define SECTOR_ONE             0x00000000ul
  #define SECTOR_TWO             0x00010000ul
  #define SECTOR_THREE           0x00020000ul
  #define SECTOR_FOUR            0x00030000ul
  // 2 Mbits
  #define OFD_FLASH_SIZE         262144
  // image consists mcu flash - bootloader size + eeprom size. 128k - 4k + 4k
  #define OFD_IMAGE_SIZE                                  131072
  #define OFD_FLASH_START_ADDRESS                              0
  #define OFD_IMAGE1_START_ADDRESS       OFD_FLASH_START_ADDRESS
  #define OFD_IMAGE2_START_ADDRESS       (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)

  #define OFD_MCU_FLASH_SIZE                       OFD_IMAGE_SIZE
  #define OFD_MCU_EEPROM_SIZE                                4096
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#elif (EXTERNAL_MEMORY == AT45DB041) || (EXTERNAL_MEMORY == AT45DB041E)

  #if !defined(ATMEGA1281) && !defined(ATMEGA128RFA1) && \
      !defined(ATXMEGA256A3) && !defined(ATXMEGA256D3) && \
      !defined(ATSAMR21G18A) && !defined(ATSAMR21E18A)
    #error 'at45db041 is supported only for atmega1281/128rfa1, atxmega256a/d3, atsamr21g18a and atsamr21e18a '
  #endif

  // flash instruction format (see at45db041 manual)
  #define WRITE_BUF1    0x84
  #define WRITE_BUF2    0x87
  #define RDSR          0xD7
  #define READ          0xE8
  #define PROGRAM_BUF1  0x88
  #define PROGRAM_BUF2  0x89
  #define BLOCK_ERASE   0x50
  #define RDID          0x9F
  #define CHIP_ERASE    0x9A8094C7

  // Adesto ID
  #define OFD_MANUFACTURER_ID           0x1F
  #define OFD_DEVICE_ID_1               0x24
  #define OFD_DEVICE_ID_2               0x00
  
  // status register bits
  #define RDY         0x80
  // unchanged bit mask within status register xx0111xx
  #define STATUS_UNCHANGED_BIT_MASK      0x3C
  #define STATUS_UNCHANGED_BITS          0x1C

  #define PAGE_SIZE                  256
  // block addresses
  #define FIRST_HALF_BLOCK_NUMBER    0
  #define SECOND_HALF_BLOCK_NUMBER   128

  // 4 Mbits
  #define OFD_FLASH_SIZE         540672

  #if defined(ATMEGA1281) || defined(ATMEGA128RFA1)
    // image consists mcu flash - bootloader size + eeprom size. 128k - 4k + 4k
    // image uses 497 pages. 1 block = 8 pages
    #define OFD_USED_BLOCKS_AMOUNT                              63
    #define OFD_IMAGE_SIZE                                  131072
    #define OFD_MCU_FLASH_SIZE                      OFD_IMAGE_SIZE
    #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
    #define OFD_LOAD_NO_COMMAND_TO_NVM
  #elif  defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
    // image consists mcu flash + eeprom size. 256k + 4k
    // image uses 1009 pages. 1 block = 8 pages
    #define OFD_USED_BLOCKS_AMOUNT                             128
    #define OFD_IMAGE_SIZE                                  266240
    #define OFD_MCU_FLASH_SIZE                              262144
    #define OFD_EEPROM_OFFSET_WITHIN_IMAGE       OFD_MCU_FLASH_SIZE
    #define OFD_LOAD_NO_COMMAND_TO_NVM      NVM.CMD = NVM_CMD_NO_OPERATION_gc
  #elif defined(ATSAMR21G18A) || defined(ATSAMR21E18A)
    // image consists mcu flash size - bootloader size. 256k - 8k(boot)
    //image uses 962 pages.1 block = 8 pages
    #define OFD_IMAGE_SIZE                253952
    #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
    #define OFD_BOOTLOADER_SIZE              0x2000
    #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE + OFD_BOOTLOADER_SIZE)
  #endif

  #define OFD_FLASH_START_ADDRESS                              0
  #define OFD_IMAGE1_START_ADDRESS       OFD_FLASH_START_ADDRESS
  #define OFD_IMAGE2_START_ADDRESS       (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)

  #define OFD_MCU_EEPROM_SIZE                                4096
  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#elif EXTERNAL_MEMORY == AT25DF041A

  #if !defined(ATXMEGA256A3) && !defined(ATXMEGA256D3) && !defined(ATMEGA256RFR2) && !defined(ATMEGA2564RFR2) && \
  	  !defined(ATSAMR21G18A) && !defined(ATSAMR21E18A)
    #error 'at25df041a is supported only for atxmega256a3, atmega256rfr2, atmega2564rfr2, atsamr21g18a and atsamr21e18a .'
  #endif

  // flash instruction format (see at25df041a manual)
  #define WREN             0x06
  #define WRDI             0x04
  #define RDSR             0x05
  #define WRSR             0x01
  #define READ             0x03
  #define PROGRAM          0x02
  #define SECTOR_ERASE     0xD8
  #define CHIP_ERASE       0x60
  #define RDID             0x9F
  #define PROTECT_SECTOR   0x36
  #define UNPROTECT_SECTOR 0x39

  // status register arguments
  #define GLOBAL_UNPROTECT 0x00
  #define GLOBAL_PROTECT   0x7F

  // status register bits
  #define RDY         0x01
  #define WEN         0x02
  #define SWP0        0x04
  #define SWP1        0x08
  #define WPP         0x10
  #define EPE         0x20
  #define SPM         0x40
  #define WPRL        0x80

  // Atmel ID
  #define OFD_MANUFACTURER_ID   0x1F
  // at25df041a
  #define OFD_DEVICE_ID_1       0x44
  #define OFD_DEVICE_ID_2       0x01
  #define EXT_STRING_LENGTH     0x00

  #define PAGE_SIZE                     256

  #define OFD_SECTOR_SIZE               0x00010000ul

  // 4 Mbits
  #define OFD_FLASH_SIZE                524288

  #if defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  #define OFD_MCU_EEPROM_SIZE             8192
  // image consists mcu flash size - bootloader size + eeprom size. 256k -8k + 8k
  #define OFD_IMAGE_SIZE                  262144
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
  #elif defined(ATSAMR21G18A) || defined(ATSAMR21E18A)
  #define OFD_IMAGE_SIZE                253952
  #define OFD_MCU_EEPROM_SIZE             4096
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_BOOTLOADER_SIZE              0x2000
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE + OFD_BOOTLOADER_SIZE)
  #else
  #define OFD_MCU_EEPROM_SIZE             4096
  // image consists mcu flash size + eeprom size. 256k + 8k
  #define OFD_IMAGE_SIZE                  266240
  #endif
  #define OFD_FLASH_START_ADDRESS                              0
  #define OFD_IMAGE1_START_ADDRESS       OFD_FLASH_START_ADDRESS
  // this is fake start address used for code compatibility
  #define OFD_IMAGE2_START_ADDRESS       (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)

  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#elif EXTERNAL_MEMORY == M25P40VMN6PB

  #if !defined(ATSAMR21G18A) && !defined(ATMEGA256RFR2) && !defined(ATMEGA2564RFR2) && !defined(ATSAMR21E18A)
    #error 'M25P40VMN6PB is supported only for atsamr21g18a, atsamr21e18a, atmega256rfr2 & atmega2564rfr2'
  #endif

  // flash instruction format (see M25P40VMN6PB manual)
  #define WREN             0x06
  #define WRDI             0x04
  #define RDSR             0x05
  #define WRSR             0x01
  #define READ             0x03
  #define PROGRAM          0x02
  #define SECTOR_ERASE     0xD8
  #define CHIP_ERASE       0xC7
  #define RDID             0x9F

  // status register arguments
  #define GLOBAL_UNPROTECT 0x00
  #define GLOBAL_PROTECT   0x7F

  // status register bits
  #define RDY         0x01
  #define WEN         0x02
  #define SWP0        0x04
  #define SWP1        0x08
  #define WPP         0x10
  #define EPE         0x20
  #define SPM         0x40
  #define WPRL        0x80

  // Micron ID
  #define OFD_MANUFACTURER_ID           0x20
  // M25P40VMN6PB
  #define OFD_DEVICE_ID_1               0x20

  #define PAGE_SIZE                     256

  #define OFD_SECTOR_SIZE               0x00010000ul

  // 4 Mbits
  #define OFD_FLASH_SIZE                524288
  #if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  // image consists mcu flash size - bootloader size. 256k - 8k(boot)
  #define OFD_IMAGE_SIZE                253952
  #define OFD_MCU_EEPROM_SIZE              4096
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_BOOTLOADER_SIZE              0x2000
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE + OFD_BOOTLOADER_SIZE)

  #elif defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  #define OFD_IMAGE_SIZE                262144
  #define OFD_MCU_EEPROM_SIZE              8192
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
  #endif

  #define OFD_FLASH_START_ADDRESS       0
  #define OFD_IMAGE1_START_ADDRESS      OFD_FLASH_START_ADDRESS

  // this is fake start address used for code compatibility
  #define OFD_IMAGE2_START_ADDRESS      (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)

  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#elif EXTERNAL_MEMORY == MX25V4006E

  #if !defined(ATSAMR21G18A) && !defined(ATMEGA256RFR2) && !defined(ATMEGA2564RFR2) && !defined(ATSAMR21E18A) && !defined(ATSAMR21E19A)
    #error 'mx25v4006e is supported only for atsamr21g18a, atsamr21e18a, atsamr21e19a, atmega256rfr2 & atmega2564rfr2'
  #endif

  // flash instruction format (see mx25l2006e manual)
  #define WREN             0x06
  #define WRDI             0x04
  #define WRSR             0x01
  #define RDID             0x9f
  #define RDSR             0x05
  #define READ             0x03
  #define FAST_READ        0x0B
  #define RES              0xAB
  #define REMS             0x90
  #define DREAD            0x3B
  #define SECTOR_ERASE     0x20
  #define BLOCK_ERASE      0x52
  #define CHIP_ERASE       0x60
  #define PAGE_PROGRAM     0x02
  #define DEEP_POWERDOWN   0xB9
  #define RELEASE_DP       0xAB

  // // status register arguments
  #define STATUS_WRITE_INPROGRESS 0x01
  // status register bits
  #define WIP         0x01
  #define WEL         0x02
  #define SWP0        0x04
  #define SWP1        0x08
  #define WPP         0x10
  #define EPE         0x20
  #define SPM         0x40
  #define WPRL        0x80

  // Micron ID
  #define OFD_MANUFACTURER_ID           0xC2
  // M25P40VMN6PB
  #define OFD_DEVICE_ID_1               0x20

  #define PAGE_SIZE                     256

  #define OFD_SECTOR_SIZE               0x00010000ul

  // 4 Mbits
  #define OFD_FLASH_SIZE                524288
  #if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  // image consists mcu flash size - bootloader size. 256k - 8k(boot)
  #define OFD_IMAGE_SIZE                253952
  #define OFD_MCU_EEPROM_SIZE              4096
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_BOOTLOADER_SIZE              0x2000
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE + OFD_BOOTLOADER_SIZE)

  #elif defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  #define OFD_IMAGE_SIZE                262144
  #define OFD_MCU_EEPROM_SIZE              8192
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
  #endif

  #define OFD_FLASH_START_ADDRESS       0
  #define OFD_IMAGE1_START_ADDRESS      OFD_FLASH_START_ADDRESS

  // this is fake start address used for code compatibility
  #define OFD_IMAGE2_START_ADDRESS      (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)


  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#elif EXTERNAL_MEMORY == MX25L2006E

  #if !defined(ATSAMR21G18A) && !defined(ATMEGA256RFR2) && !defined(ATMEGA2564RFR2) && !defined(ATSAMR21E18A)
    #error 'mx25l2006e is supported only for atsamr21g18a, atsamr21e18a, atmega256rfr2 & atmega2564rfr2'
  #endif

  // flash instruction format (see mx25l2006e manual)
  #define WREN             0x06
  #define WRDI             0x04
  #define WRSR             0x01
  #define RDID             0x9f
  #define RDSR             0x05
  #define READ             0x03
  #define FAST_READ        0x0B
  #define RES              0xAB
  #define REMS             0x90
  #define DREAD            0x3B
  #define SECTOR_ERASE     0x20
  #define BLOCK_ERASE      0x52
  #define CHIP_ERASE       0x60
  #define PAGE_PROGRAM     0x02
  #define DEEP_POWERDOWN   0xB9
  #define RELEASE_DP       0xAB

  // // status register arguments
  #define STATUS_WRITE_INPROGRESS 0x01
  // status register bits
  #define WIP         0x01
  #define WEL         0x02
  #define SWP0        0x04
  #define SWP1        0x08
  #define WPP         0x10
  #define EPE         0x20
  #define SPM         0x40
  #define WPRL        0x80

  // Micron ID
  #define OFD_MANUFACTURER_ID           0xC2
  // M25P40VMN6PB
  #define OFD_DEVICE_ID_1               0x20

  #define PAGE_SIZE                     256

  #define OFD_SECTOR_SIZE               0x00010000ul

  // 4 Mbits
  #define OFD_FLASH_SIZE                262144
  #if defined(ATSAMR21G18A) || defined(ATSAMR21E18A)
  // image consists mcu flash size - bootloader size. 256k - 8k(boot)
  #define OFD_IMAGE_SIZE                253952
  #define OFD_MCU_EEPROM_SIZE              4096
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_BOOTLOADER_SIZE              0x2000
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE + OFD_BOOTLOADER_SIZE)

  #elif defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  #define OFD_IMAGE_SIZE                262144
  #define OFD_MCU_EEPROM_SIZE              8192
  #define OFD_MCU_FLASH_SIZE               OFD_IMAGE_SIZE
  #define OFD_EEPROM_OFFSET_WITHIN_IMAGE   (OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE)
  #endif

  #define OFD_FLASH_START_ADDRESS       0
  #define OFD_IMAGE1_START_ADDRESS      OFD_FLASH_START_ADDRESS

  // this is fake start address used for code compatibility
  #define OFD_IMAGE2_START_ADDRESS      (OFD_FLASH_START_ADDRESS + OFD_IMAGE_SIZE)


  // 1 byte (action for bootloader), 2 bytes (images crc), 1 byte table of image types.
  #define OFD_SERVICE_INFO_SIZE                                 4

#else
  #error 'Unknown memory type.'
#endif

#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
#define OFD_START_EEPROM_SREC_ADDRESS     0x3F000
#define IMAGE_START_OFFSET OFD_BOOTLOADER_SIZE
#else
#define OFD_START_EEPROM_SREC_ADDRESS     0x810000
#define IMAGE_START_OFFSET 0x0
#endif

// Used for CRC calculation
#define OFD_APP_IMAGE1_START_ADDRESS (OFD_IMAGE1_START_ADDRESS + IMAGE_START_OFFSET)
#define OFD_APP_IMAGE2_START_ADDRESS (OFD_IMAGE2_START_ADDRESS + IMAGE_START_OFFSET)

#define OFD_LITTLE_TO_BIG_ENDIAN(A)  ((((uint32_t)A & 0xFFul) << 24)   \
                                   | (((uint32_t)A & 0xFF00ul) << 8)   \
                                   | (((uint32_t)A >> 8) & 0xFF00ul)   \
                                   | (((uint32_t)A >> 24) & 0xFFul))


typedef struct
{
  union
  {
    uint32_t   flashOffset;
    uint16_t   eepromOffset;
  };
  uint8_t  *data;
  uint32_t  length;
} OfdInternalMemoryAccessParam_t;

// image type table
typedef uint8_t OfdImageTable_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Starts image erasing in the external memory.
******************************************************************************/
void ofdEraseImage(void);

/**************************************************************************//**
\brief Writes data to the external memory.
******************************************************************************/
void ofdWriteData(void);

/**************************************************************************//**
\brief Checks image crc.
******************************************************************************/
void ofdCheckCrc(void);

/**************************************************************************//**
\brief Starts saving internal flash.
******************************************************************************/
void ofdSaveCurrentFlashImage(void);

/**************************************************************************//**
\brief Reads image crc from internal eeprom.
******************************************************************************/
void ofdReadCrc(void);

/**************************************************************************//**
\brief Counts crc current memory area. CRC-8. Polynom 0x31    x^8 + x^5 + x^4 + 1.

\param[in]
  crc - first crc state
\param[in]
  pcBlock - pointer to the memory for crc counting
\param[in]
  length - memory size

\return
  current area crc
******************************************************************************/
uint8_t ofdCrc(uint8_t crc, uint8_t *pcBlock, uint8_t length);

/**************************************************************************//**
\brief Finds storage space.
******************************************************************************/
void ofdFindStorageSpace(void);

/**************************************************************************//**
\brief Sets action for internal bootloader.
******************************************************************************/
void ofdSetActionForBootloader(void);

/**************************************************************************//**
\brief Flushs memory buffer to flash.
******************************************************************************/
void ofdFlushData(void);

#if EXTERNAL_MEMORY == AT25DF041A
/**************************************************************************//**
\brief Unprotects memory sectors for writing and erasing.
******************************************************************************/
void ofdUnprotectMemorySectors(void);
#endif

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
// Macros for the EXT_MEM_CS pin manipulation.
#if defined(ATMEGA1281)

HAL_ASSIGN_PIN(EXT_MEM_CS, F, 3);

#elif defined(ATMEGA128RFA1)

HAL_ASSIGN_PIN(EXT_MEM_CS, G, 5);

#elif defined(ATXMEGA256A3) || defined(ATXMEGA256D3)

HAL_ASSIGN_PIN(EXT_MEM_CS, D, 4);

#elif defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)

HAL_ASSIGN_PIN(EXT_MEM_CS, E, 3);

#elif defined(ATSAMR21G18A)

#if (EXTERNAL_MEMORY == AT45DB041E)
HAL_ASSIGN_PIN(EXT_MEM_CS, B, 2);
#else
HAL_ASSIGN_PIN(EXT_MEM_CS, A, 14);
#endif

#elif defined(ATSAMR21E18A)

HAL_ASSIGN_PIN(EXT_MEM_CS, A, 27);

#elif defined(ATSAMR21E19A)

HAL_ASSIGN_PIN(EXT_MEM_CS, A, 23);
HAL_ASSIGN_PIN(SPI_HOLD,    A, 0);
HAL_ASSIGN_PIN(SPI_WP,      A, 12);
#endif

#endif // APP_USE_OTAU == 1

#endif /* _OFDMEMORYDRIVER_H */
