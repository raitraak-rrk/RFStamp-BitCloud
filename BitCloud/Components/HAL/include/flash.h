/****************************************************************************//**
  \file flash.h

  \brief The header file describes the program memory access interface.

         Program memomry interface needs some additional actions to be able to
         work.

         1. Linker script shall contain definition of the following sections:
            a. GCC:
               .boot_section - section in No Read-While-Write code memory;
               .access_point - section with size of pointer to function, shall
                               be placed in the very last bytes of code space.
               .nrww_section - No Read-While-Write (NRWW) code memory section;
               .wwf_poll_access_section - section with size of pointer to
                               function, shall be placed in the very last
                               bytes of NRWW code space related to the
                               application. Right before .boot_section.
            b. IAR:
               BOOT_SECTION - section in No Read-While-Write code memory;
               NRWW_SECTION - No Read-While-Write (NRWW) code memory section;
         2. Boot flash section witch is programmed by fuses shall have enough
            size to fit the boot section declared in linker script.
         3. If application uses flash memory access interface and is not intended
            to be used with bootloader then somewhere in code
            DECLARE_FLASH_ACCESS_ENTRY_POINT() macro from halFlashAccess.h
            shall be declared.
         4. In both cases when it is supposed to use bootloader or not
            the DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT() shall be placed
            somewhere in code. It defines a pointer to the poll handler
            which will be called during the flash writing process.
         5. If it is not supposed to use the program memory access interface
            just don't declare DECLARE_FLASH_ACCESS_ENTRY_POINT() and
            DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT() anywhere so this
            interface will be striped out by the linker.


  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    27/06/12 D. Kolmakov - Created
  Last change:
    $Id: flash.h 28120 2015-07-06 15:11:04Z agasthian.s $
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#ifndef _FLASH_H
#define _FLASH_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
#include <sysTypes.h>
#include <sysQueue.h>

/******************************************************************************
                   Definitions section
******************************************************************************/
#if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
/** Address within the code space where poiter to access point of code memory
    interface is stored. */
#define BOOT_SECTION_ACCESS_POINT       (FLASHEND + 1ul - sizeof(HAL_BootSectionAccessPoint_t))

/** Size in bytes of the bootloader section (specified by fuses). */
#define NRWW_SECTION_SIZE               8192ul

/** Address within the NRWW section where poiter to the poll during flash writing handler
    is stored. */
#ifndef POLL_DURING_FLASH_WRITING_ACCESS_POINT
#define POLL_DURING_FLASH_WRITING_ACCESS_POINT       (FLASHEND + 1ul - NRWW_SECTION_SIZE)
#endif

#if defined(__ICCAVR__)
/** This macro declares the constant pointer to the flash access entry function.
    It should be declared in application in case when it is not supposed to use
    bootloader. In other case this constant has been already declared in
    bootloader application and the macro shall not be used. */
  #define DECLARE_FLASH_ACCESS_ENTRY_POINT() \
    __root PROGMEM_DECLARE(HAL_BootSectionAccessPoint_t _flashAccessEntryPoint) @ BOOT_SECTION_ACCESS_POINT = HAL_FlashAccessRequestHandler; \
    DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT();
/** Declares the constant pointer to the poll handler which will be called during
    writing to the flash procedure.
    It should be declared in application in both case when it is supposed to use
    bootloader or not. But in the case of using HAL functionality this constant
    will be declared with the flash access point (see the macro above). */
  #define DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT() \
    __root PROGMEM_DECLARE(HAL_BootSectionAccessPoint_t _whileWriteFlashPollAccessPoint) @ POLL_DURING_FLASH_WRITING_ACCESS_POINT = HAL_PollDuringFlashWritingAccessPoint;
#elif defined(__GNUC__)
/** This macro declares the constant pointer to the flash access entry function.
    It should be declared in application in case when it is not supposed to use
    bootloader. In other case this constant has been already declared in
    bootloader application and the macro shall not be used. */
  #define DECLARE_FLASH_ACCESS_ENTRY_POINT() \
    __attribute__((section(".access_section"))) void HAL_FlashAccessEntryHandler(void) { \
      HAL_FlashAccessRequestHandler(); \
    }\
    DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT();
/** Declares the constant pointer to the poll handler which will be called during
    writing to the flash procedure.
    It should be declared in application in both case when it is supposed to use
    bootloader or not. But in the case of using HAL functionality this constant
    will be declared with the flash access point (see the macro above). */
  #define DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT() \
    __attribute__((section(".wwf_poll_access_section"))) HAL_BootSectionAccessPoint_t _whileWriteFlashPollAccessPoint = HAL_PollDuringFlashWritingAccessPoint;
#endif

#if defined(__ICCAVR__)
#define BOOT_SECTION_DECLARE _Pragma("location=\"BOOT_SECTION\"")
#define NRWW_SECTION_DECLARE _Pragma("location=\"NRWW_SECTION\"")
#define NO_RETURN __C_task
#elif defined(__GNUC__)
#define BOOT_SECTION_DECLARE __attribute__((section(".boot_section")))
#define NRWW_SECTION_DECLARE __attribute__((section(".nrww_section")))
#define NO_RETURN __attribute__((__noreturn__))
#else
  #error 'Unsupported compiler.'
#endif
#endif /* #if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2) */
/******************************************************************************
                   Types section
******************************************************************************/
/** Flash access request identificator */
typedef enum _HAL_FlashAccessReqId_t
{
  HAL_FLASH_ACCESS_FILL_PAGE_BUFFER,
  HAL_FLASH_ACCESS_PROGRAM_PAGE,
  HAL_FLASH_ACCESS_SWAP_IMAGES,
  HAL_FLASH_ACCESS_ERASE_PAGE,
  HAL_FLASH_ACCESS_WRITE_PAGE
} HAL_FlashAccessReqId_t;

/** Fill page buffer request payload */
typedef struct _HAL_FillPageBufferReq_t
{
  uint32_t offset;
  uint16_t length;
  uint8_t *data;
} HAL_FillPageBufferReq_t;

/** Program page request payload */
typedef struct
{
  uint32_t pageStartAddr;
} HAL_ProgramPageReq_t;

/** Swap images request payload */
typedef struct _HAL_SwapImagesReq_t
{
  uint32_t imageStartAddr;
  uint32_t imageSize;
} HAL_SwapImagesReq_t;

/** Erase page request payload */
typedef HAL_ProgramPageReq_t HAL_ErasePageReq_t;

/** Write page request payload */
typedef HAL_ProgramPageReq_t HAL_WritePageReq_t;

/** Flash access request common payload */
typedef union _HAL_FlashAccessReqPayload_t
{
  HAL_FillPageBufferReq_t fillPageBuffer;
  HAL_ProgramPageReq_t    programPage;
  HAL_SwapImagesReq_t     swapImages;
  HAL_ErasePageReq_t      erasePage;
  HAL_WritePageReq_t      writePage;
} HAL_FlashAccessReqPayload_t;

/** Flash access request */
typedef struct _HAL_FlashAccessReq_t
{
  HAL_FlashAccessReqId_t       id;
  HAL_FlashAccessReqPayload_t  payload;
} HAL_FlashAccessReq_t;

/** Prototype of the access point to the flash memory interface */
typedef void (* HAL_BootSectionAccessPoint_t)(void);

/*! \brief Structure for declaring poll during flash writing handler  */
typedef struct _HAL_PollDuringFlashWritingHandler_t
{
  // Internal service field for handler.
  //\cond internal
  QueueElement_t *next;
  //\endcond

  /*! Function to be called during writing to the flash process */
  void (*poll)(void);

  /*! Function to be called after flash writing process */
  void (*finished)(void);
} HAL_PollDuringFlashWritingHandler_t;


/******************************************************************************
                              Prototypes section
 ******************************************************************************/

/******************************************************************************
  \brief Writes data to the page buffer.
  \param[in] startOffset - start position for writing within the page
  \param[in] length - data to write length in bytes
  \param[in] data - pointer to data to write
******************************************************************************/
void HAL_FillFlashPageBuffer(uint32_t startOffset, uint16_t length, uint8_t *data);

/******************************************************************************
  \brief Writes data to the page buffer.
  \param[in] pageStartAddress - address within the target page
******************************************************************************/
void HAL_ProgramFlashPage(uint32_t pageStartAddress);

/******************************************************************************
  \brief Perfom swapping of images.
  \param[in] imageStartAddr - address of target image
  \param[in] imageSize - target image size
******************************************************************************/
void HAL_SwapImages(uint32_t imageStartAddr, uint32_t imageSize);

/******************************************************************************
  \brief Erases specified flash page.
  \param[in] pageStartAddress - address within the target page
******************************************************************************/
void HAL_EraseFlashPage(uint32_t pageStartAddress);

/******************************************************************************
  \brief Writes flash page buffer without erasing.
  \param[in] pageStartAddress - address within the target page
******************************************************************************/
void HAL_WriteFlashPage(uint32_t pageStartAddress);

/**************************************************************************//**
\brief Entry point to flash access module.
       For using in DECLARE_FLASH_ACCESS_ENTRY_POINT macro only.
******************************************************************************/
void HAL_FlashAccessRequestHandler(void);

/**************************************************************************//**
\brief Adds new entity to the queue of poll during flash writing handlers.

\param[in] handler - structure which shall consist a pointer to a function
                     to be called during flash writing.

                     This function shall be placed in NRWW section (use the
                     NRWW_SECTION_DECLARE macro to put the function here)
                     otherwise the software might end up in an unknown state.
******************************************************************************/
void HAL_RegisterPollDuringFlashWritingHandler(HAL_PollDuringFlashWritingHandler_t *handler);

/**************************************************************************//**
\brief Deletes an entity from the queue of poll during flash writing handlers.

\param[in] handler - structure which shall consist a pointer to a function
                     to be called during flash writing.
******************************************************************************/
void HAL_UnregisterPollDuringFlashWritingHandler(HAL_PollDuringFlashWritingHandler_t *handler);

/**************************************************************************//**
\brief Poll handler which is called periodicly during flash writing process.
       For using in DECLARE_POLL_DURING_FLASH_WRITING_ACCESS_POINT macro only.
******************************************************************************/
void HAL_PollDuringFlashWritingAccessPoint(void);

/**************************************************************************//**
\brief Handler which is called after flash writing process.
******************************************************************************/
void HAL_FlashAccessFinishedHandler(void);

#endif /* _FLASH_H */
// eof flash.h
