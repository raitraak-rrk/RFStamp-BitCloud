/**************************************************************************//**
  \file  halEeprom.h

  \brief  Provides interface for the access to hardware dependent
      EEPROM module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      3/11/14 P. MAHENDRAN - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _HALEEPROM_H
#define _HALEEPROM_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halTaskManager.h>
#include <eeprom.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/** \brief the mask to rise interrupt when operation on EEPROM was completed */
#define HAL_EEPROM_WRITE_MASK_INT (1 << EEMPE | 1 << EERIE)
#define HAL_EEPROM_WRITE_MASK (1 << EEMPE)

/******************************************************************************
                   Prototypes section
******************************************************************************/

/**************************************************************************//**
  \brief Initializes the EEPROM Emulator.
  \param[in] None
******************************************************************************/
void halInitEepromEmulator(void);

/**************************************************************************//**
\brief Writes a byte to EEPROM.
\param[in]
  EECRMask - mask that define capability of interrupt after byte writing.
\param[in]
  address - address of byte
\param[in]
  data - data.
******************************************************************************/
void halEepromWrite(uint8_t EECRMask, uint16_t address, uint8_t data);

/**
 * \file
 *
 * \brief SAM EEPROM Emulator
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED


/**
 * \brief EEPROM memory parameter structure.
 *
 * Structure containing the memory layout parameters of the EEPROM emulator module.
 */
struct eeprom_emulator_parameters {
	/** Number of bytes per emulated EEPROM page. */
	uint8_t  page_size;
	/** Number of emulated pages of EEPROM. */
	uint16_t eeprom_number_of_pages;
};

/** Status code error categories. */
enum status_categories {
	STATUS_CATEGORY_OK                = 0x00,
	STATUS_CATEGORY_COMMON            = 0x10,
	STATUS_CATEGORY_ANALOG            = 0x30,
	STATUS_CATEGORY_COM               = 0x40,
	STATUS_CATEGORY_IO                = 0x50,
};

enum status_code {
	STATUS_OK                         = STATUS_CATEGORY_OK     | 0x00,
	STATUS_VALID_DATA                 = STATUS_CATEGORY_OK     | 0x01,
	STATUS_NO_CHANGE                  = STATUS_CATEGORY_OK     | 0x02,
	STATUS_ABORTED                    = STATUS_CATEGORY_OK     | 0x04,
	STATUS_BUSY                       = STATUS_CATEGORY_OK     | 0x05,
	STATUS_SUSPEND                    = STATUS_CATEGORY_OK     | 0x06,

	STATUS_ERR_IO                     = STATUS_CATEGORY_COMMON | 0x00,
	STATUS_ERR_REQ_FLUSHED            = STATUS_CATEGORY_COMMON | 0x01,
	STATUS_ERR_TIMEOUT                = STATUS_CATEGORY_COMMON | 0x02,
	STATUS_ERR_BAD_DATA               = STATUS_CATEGORY_COMMON | 0x03,
	STATUS_ERR_NOT_FOUND              = STATUS_CATEGORY_COMMON | 0x04,
	STATUS_ERR_UNSUPPORTED_DEV        = STATUS_CATEGORY_COMMON | 0x05,
	STATUS_ERR_NO_MEMORY              = STATUS_CATEGORY_COMMON | 0x06,
	STATUS_ERR_INVALID_ARG            = STATUS_CATEGORY_COMMON | 0x07,
	STATUS_ERR_BAD_ADDRESS            = STATUS_CATEGORY_COMMON | 0x08,
	STATUS_ERR_BAD_FORMAT             = STATUS_CATEGORY_COMMON | 0x0A,
	STATUS_ERR_BAD_FRQ                = STATUS_CATEGORY_COMMON | 0x0B,
	STATUS_ERR_DENIED                 = STATUS_CATEGORY_COMMON | 0x0c,
	STATUS_ERR_ALREADY_INITIALIZED    = STATUS_CATEGORY_COMMON | 0x0d,
	STATUS_ERR_OVERFLOW               = STATUS_CATEGORY_COMMON | 0x0e,
	STATUS_ERR_NOT_INITIALIZED        = STATUS_CATEGORY_COMMON | 0x0f,

	STATUS_ERR_SAMPLERATE_UNAVAILABLE = STATUS_CATEGORY_ANALOG | 0x00,
	STATUS_ERR_RESOLUTION_UNAVAILABLE = STATUS_CATEGORY_ANALOG | 0x01,

	STATUS_ERR_BAUDRATE_UNAVAILABLE   = STATUS_CATEGORY_COM    | 0x00,
	STATUS_ERR_PACKET_COLLISION       = STATUS_CATEGORY_COM    | 0x01,
	STATUS_ERR_PROTOCOL               = STATUS_CATEGORY_COM    | 0x02,

	STATUS_ERR_PIN_MUX_INVALID        = STATUS_CATEGORY_IO     | 0x00,
};

/** @} */

/** \name Configuration and initialization
 * @{
 */

enum status_code eeprom_emulator_init(void);

void eeprom_emulator_erase_memory(void);

enum status_code eeprom_emulator_get_parameters(
		struct eeprom_emulator_parameters *const parameters);

/** @} */


/** \name Logical EEPROM page reading/writing
 * @{
 */

enum status_code eeprom_emulator_commit_page_buffer(void);

enum status_code eeprom_emulator_write_page(
		const uint8_t logical_page,
		const uint8_t *const data);

enum status_code eeprom_emulator_read_page(
		const uint8_t logical_page,
		uint8_t *const data);

/** @} */

/** \name Buffer EEPROM reading/writing
 * @{
 */

enum status_code eeprom_emulator_write_buffer(
		uint16_t offset,
		uint8_t *data,
		uint16_t length);

enum status_code eeprom_emulator_read_buffer(
		const uint16_t offset,
		uint8_t *const data,
		const uint16_t length);

#endif /* EEPROM_H_INCLUDED */
#endif /*_HALEEPROM_H*/
//eof halEeprom.h
