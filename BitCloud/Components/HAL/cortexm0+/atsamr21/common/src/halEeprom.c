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
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_EE_READY)
#if defined(HAL_USE_EEPROM_EMULATION)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <atomic.h>
#include <halEeprom.h>
#include <halDbg.h>
#include <halFlash.h>
#include <halDiagnostic.h>
#include <eeprom.h>
#include <string.h>
#include <halDbg.h>
#include <sysAssert.h>


/**
 * \defgroup asfdoc_sam0_eeprom_group SAM EEPROM Emulator Service (EEPROM)
 *
 * This driver for SAM devices provides an emulated EEPROM memory space in
 * the device's FLASH memory, for the storage and retrieval of user-application
 * configuration data into and out of non-volatile memory.
 *
 * The following peripherals are used by this module:
 *  - NVM (Non-Volatile Memory Controller)
 *
 * The following devices can use this module:
 *  - SAM R21
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_eeprom_prerequisites
 *  - \ref asfdoc_sam0_eeprom_module_overview
 *  - \ref asfdoc_sam0_eeprom_special_considerations
 *  - \ref asfdoc_sam0_eeprom_extra_info
 *  - \ref asfdoc_sam0_eeprom_examples
 *  - \ref asfdoc_sam0_eeprom_api_overview
 *
 *
 * \section asfdoc_sam0_eeprom_prerequisites Prerequisites
 *
 * The SAM device fuses must be configured via an external programmer or
 * debugger, so that an EEPROM section is allocated in the main NVM flash
 * memory contents. If a NVM section is not allocated for the EEPROM emulator,
 * or if insufficient space for the emulator is reserved, the module will fail
 * to initialize.
 *
 *
 * \section asfdoc_sam0_eeprom_module_overview Module Overview
 *
 * As the SAM devices do not contain any physical EEPROM memory, the storage
 * of non-volatile user data is instead emulated using a special section of the
 * device's main FLASH memory. The use of FLASH memory technology over EEPROM
 * presents several difficulties over true EEPROM memory; data must be written
 * as a number of physical memory pages (of several bytes each) rather than
 * being individually byte addressable, and entire rows of FLASH must be erased
 * before new data may be stored. To help abstract these characteristics away
 * from the user application an emulation scheme is implemented to present a
 * more user-friendly API for data storage and retrieval.
 *
 * This module provides an EEPROM emulation layer on top of the device's
 * internal NVM controller, to provide a standard interface for the reading and
 * writing of non-volatile configuration data. This data is placed into the
 * EEPROM emulated section of the device's main FLASH memory storage section,
 * the size of which is configured using the device's fuses. Emulated EEPROM is
 * exempt from the usual device NVM region lock bits, so that it may be read
 * from or written to at any point in the user application.
 *
 * There are many different algorithms that may be employed for EEPROM emulation
 * using FLASH memory, to tune the write and read latencies, RAM usage, wear
 * levelling and other characteristics. As a result, multiple different emulator
 * schemes may be implemented, so that the most appropriate scheme for a
 * specific application's requirements may be used.
 *
 * \subsection asfdoc_sam0_eeprom_module_overview_implementation Implementation Details
 * The following information is relevant for <b>EEPROM Emulator scheme 1,
 * version 1.0.0</b>, as implemented by this module. Other revisions or
 * emulation schemes may vary in their implementation details and may have
 * different wear-leveling, latency and other characteristics.
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_ec Emulator Characteristics
 * This emulator is designed for <b>best reliability, with a good balance of
 * available storage and write-cycle limits</b>. It is designed to ensure that
 * page data is atomically updated so that in the event of a failed update the
 * previous data is not lost (when used correctly). With the exception of a
 * system reset with data cached to the internal write-cache buffer, at most
 * only the latest write to physical non-volatile memory will be lost in the
 * event of a failed write.
 *
 * This emulator scheme is tuned to give best write-cycle longevity when writes
 * are confined to the same logical EEPROM page (where possible) and when writes
 * across multiple logical EEPROM pages are made in a linear fashion through the
 * entire emulated EEPROM space.
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_pf Physical Memory
 * The SAM non-volatile FLASH is divided into a number of physical rows, each
 * containing four identically sized flash pages. Pages may be read or written
 * to individually, however pages must be erased before being reprogrammed and
 * the smallest granularity available for erasure is one single row.
 *
 * This discrepancy results in the need for an emulator scheme that is able to
 * handle the versioning and moving of page data to different physical rows as
 * needed, erasing old rows ready for re-use by future page write operations.
 *
 * Physically, the emulated EEPROM segment is located at the end of the physical
 * FLASH memory space, as shown in
 * \ref asfdoc_sam0_eeprom_module_mem_layout "the figure below".
 *
 * \anchor asfdoc_sam0_eeprom_module_mem_layout
 * \dot
 * digraph memory_layout {
 *  size="5,5"
 *  node [shape=plaintext]
 *  memory [label=<
 *   <table border="0" cellborder="1" cellspacing="0" >
 *    <tr>
 *     <td align="right" border="0"> End of NVM Memory </td>
 *     <td rowspan="3" align="center"> Reserved EEPROM Section </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> Start of EEPROM Memory </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> End of Application Memory </td>
 *     <td rowspan="3" align="center"> Application Section </td>
 *    </tr>
 *    <tr>
 *     <td height="300" align="right" border="0"> </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> Start of Application Memory </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> End of Bootloader Memory </td>
 *     <td rowspan="3" align="center"> BOOT Section </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> </td>
 *    </tr>
 *    <tr>
 *     <td align="right" border="0"> Start of NVM Memory</td>
 *    </tr>
 *   </table>
 *  >]
 * }
 * \enddot
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_mp Master Row
 * One physical FLASH row at the end of the emulated EEPROM memory space is
 * reserved for use by the emulator to store configuration data. The master row
 * is not user-accessible, and is reserved solely for internal use by the
 * emulator.
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_sr Spare Row
 * As data needs to be preserved between row erasures, a single FLASH row is kept
 * unused to act as destination for copied data when a write request is made to
 * an already full row. When the write request is made, any logical pages of
 * data in the full row that need to be preserved are written to the spare row
 * along with the new (updated) logical page data, before the old row is erased
 * and marked as the new spare.
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_rc Row Contents
 * Each physical FLASH row initially stores the contents of two logical EEPROM
 * memory pages. This halves the available storage space for the emulated EEPROM
 * but reduces the overall number of row erases that are required, by reserving
 * two pages within each row for updated versions of the logical page contents.
 * See \ref asfdoc_sam0_eeprom_init_layout "here" for a visual layout of the
 * EEPROM Emulator physical memory.
 *
 * As logical pages within a physical row are updated, the new data is filled
 * into the remaining unused pages in the row. Once the entire row is full, a
 * new write request will copy the logical page not being written to in the
 * current row to the spare row with the new (updated) logical page data, before
 * the old row is erased.
 *
 * This system allows for the same logical page to be updated up to three times
 * into physical memory before a row erasure procedure is needed. In the case of
 * multiple versions of the same logical EEPROM page being stored in the same
 * physical row, the right-most (highest physical FLASH memory page address)
 * version is considered to be the most current.
 *
 * \subsubsection asfdoc_sam0_eeprom_module_overview_implementation_wc Write Cache
 * As a typical EEPROM use case is to write to multiple sections of the same
 * EEPROM page sequentially, the emulator is optimized with a single logical
 * EEPROM page write cache to buffer writes before they are written to the
 * physical backing memory store. The cache is automatically committed when a
 * new write request to a different logical EEPROM memory page is requested, or
 * when the user manually commits the write cache.
 *
 * Without the write cache, each write request to an EEPROM memory page would
 * require a full page write, reducing the system performance and significantly
 * reducing the lifespan of the non-volatile memory.
 *
 * \subsection asfdoc_sam0_eeprom_special_considerations_memlayout Memory Layout
 * A single logical EEPROM page is physically stored as the page contents and a
 * header inside a single physical FLASH page, as shown in
 * \ref asfdoc_sam0_eeprom_page_layout "the following figure".
 *
 * \anchor asfdoc_sam0_eeprom_page_layout
 * \image html page_layout.svg "Internal layout of an emulated EEPROM page"
 *
 * Within the EEPROM memory reservation section at the top of the NVM memory
 * space, this emulator will produce the layout as shown in
 * \ref asfdoc_sam0_eeprom_init_layout "the figure below" when initialized for
 * the first time.
 *
 * \anchor asfdoc_sam0_eeprom_init_layout
 * \image html init_layout.svg "Initial physical layout of the emulated EEPROM memory"
 *
 * When an EEPROM page needs to be committed to physical memory, the next free
 * FLASH page in the same row will be chosen - this makes recovery simple, as the
 * right-most version of a logical page in a row is considered the most current.
 * With four pages to a physical NVM row, this allows for up to three updates to
 * the same logical page to be made before an erase is needed.
 * \ref asfdoc_sam0_eeprom_page_write1 "The figure below" shows the result of
 * the user writing an updated version of logical EEPROM page <tt>N-1</tt> to
 * the physical memory.
 *
 * \anchor asfdoc_sam0_eeprom_page_write1
 * \image html nm1_page_write.svg "First write to logical EEPROM page N-1"
 *
 * A second write of the same logical EEPROM page results in the layout shown
 * in \ref asfdoc_sam0_eeprom_page_write2 "the figure below".
 *
 * \anchor asfdoc_sam0_eeprom_page_write2
 * \image html nm1_page_write2.svg "Second write to logical EEPROM page N-1"
 *
 * A third write of the same logical page requires that the EEPROM emulator
 * erase the row, as it has become full. Prior to this, the contents of the
 * unmodified page in the same row as the page being updated will be copied into
 * the spare row, along with the new version of the page being updated. The old
 * (full) row is then erased, resulting in the layout shown in
 * \ref asfdoc_sam0_eeprom_page_write3 "the figure below".
 *
 * \anchor asfdoc_sam0_eeprom_page_write3
 * \image html nm1_page_write3.svg "Third write to logical EEPROM page N-1"
 *
 *
 * \section asfdoc_sam0_eeprom_special_considerations Special Considerations
 *
 * \subsection asfdoc_sam0_eeprom_special_considerations_nvm_config NVM Controller Configuration
 * The EEPROM Emulator service will initialize the NVM controller as part of its
 * own initialization routine; the NVM controller will be placed in Manual Write
 * mode, so that explicit write commands must be sent to the controller to
 * commit a buffered page to physical memory. The manual write command must thus
 * be issued to the NVM controller whenever the user application wishes to write
 * to a NVM page for its own purposes.
 *
 * \subsection asfdoc_sam0_eeprom_special_considerations_pagesize Logical EEPROM Page Size
 * As a small amount of information needs to be stored in a header before the
 * contents of a logical EEPROM page in memory (for use by the emulation
 * service), the available data in each EEPROM page is less than the total size
 * of a single NVM memory page by several bytes.
 *
 * \subsection asfdoc_sam0_eeprom_special_considerations_committing Committing of the Write Cache
 * A single-page write cache is used internally to buffer data written to pages
 * in order to reduce the number of physical writes required to store the user
 * data, and to preserve the physical memory lifespan. As a result, it is
 * important that the write cache is committed to physical memory <b>as soon as
 * possible after a BOD low power condition</b>, to ensure that enough power is
 * available to guarantee a completed write so that no data is lost.
 *
 * The write cache must also be manually committed to physical memory if the
 * user application is to perform any NVM operations using the NVM controller
 * directly.
 *
 *
 * \section asfdoc_sam0_eeprom_extra_info Extra Information
 *
 * For extra information see \ref asfdoc_sam0_eeprom_extra. This includes:
 *  - \ref asfdoc_sam0_eeprom_extra_acronyms
 *  - \ref asfdoc_sam0_eeprom_extra_dependencies
 *  - \ref asfdoc_sam0_eeprom_extra_errata
 *  - \ref asfdoc_sam0_eeprom_extra_history
 *
 *
 * \section asfdoc_sam0_eeprom_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_eeprom_exqsg.
 *
 *
 * \section asfdoc_sam0_eeprom_api_overview API Overview
 * @{
 */

/******************************************************************************
                   Defines section
******************************************************************************/
#define NVMCTRL_STATUS_MASK         0x011Fu      /**< \brief (NVMCTRL_STATUS) MASK Register */

/* Magic key is the sequence "AtEEPROMEmu." in ASCII. The key is encoded as a
 * sequence of 32-bit values to speed up checking of the key, which can be
 * implemented as a number of simple integer comparisons,
 */
#define EEPROM_MAGIC_KEY                 {0x41744545, 0x50524f4d, 0x456d752e}

/* Length of the magic key, in 32-bit elements. */
#define EEPROM_MAGIC_KEY_COUNT           3

#define FLASH_ADDR            (0x00000000U) /**< FLASH base address */
#define NVM_MEMORY            ((volatile uint16_t *)FLASH_ADDR)
#define NVMCTRL_PARAM_NVMP_Pos      0
#define NVMCTRL_PARAM_NVMP_Msk      (0xFFFFu << NVMCTRL_PARAM_NVMP_Pos)
#define NVMCTRL_PARAM_PSZ_Msk       (0x7u << NVMCTRL_PARAM_PSZ_Pos)
#define NVMCTRL_PARAM_PSZ_Pos       16           /**< \brief (NVMCTRL_PARAM) Page Size */
/* Pointer to the NVM USER MEMORY region start address */
#define NVM_USER_MEMORY   ((volatile uint16_t *)NVMCTRL_USER)
/* EEPROM Defines */
#define NVMCTRL_FUSES_EEPROM_SIZE_Pos 4
#define NVMCTRL_FUSES_EEPROM_SIZE_Msk (0x7u << NVMCTRL_FUSES_EEPROM_SIZE_Pos)
#define NVMCTRL_FUSES_EEPROM_SIZE(value) ((NVMCTRL_FUSES_EEPROM_SIZE_Msk & ((value) << NVMCTRL_FUSES_EEPROM_SIZE_Pos)))

#define PAGE_SIZE                     64
#define NVMCTRL_ROW_PAGES           4
#define FLASH_SIZE            0x40000 /* 256 kB */
//#define NVMCTRL_PAGE_SIZE           (1<<NVMCTRL_PSZ_BITS)
#define EEPROM_MAX_ROWS               64
#  define EEPROM_MAX_PAGES            (EEPROM_MAX_ROWS * NVMCTRL_ROW_PAGES)
#  define EEPROM_MASTER_PAGE_NUMBER   (_eeprom_instance.physical_pages - 1)
#  define EEPROM_INVALID_PAGE_NUMBER  0xFF
#  define EEPROM_INVALID_ROW_NUMBER   (EEPROM_INVALID_PAGE_NUMBER / NVMCTRL_ROW_PAGES)
#  define EEPROM_HEADER_SIZE          4

/** \name EEPROM emulator information
 * @{
 */

/** Emulator scheme ID, identifying the scheme used to emulated EEPROM storage. */
#define EEPROM_EMULATOR_ID          1

/** Emulator major version number, identifying the emulator major version. */
#define EEPROM_MAJOR_VERSION        1

/** Emulator minor version number, identifying the emulator minor version. */
#define EEPROM_MINOR_VERSION        0

/** Emulator revision version number, identifying the emulator revision. */
#define EEPROM_REVISION             0

/** Size of the user data portion of each logical EEPROM page, in bytes. */
#define EEPROM_PAGE_SIZE            (PAGE_SIZE - EEPROM_HEADER_SIZE)

/******************************************************************************
                    Types section
******************************************************************************/

/* NVM memory parameter structure. Structure containing the memory layout parameters of the NVM module.*/
struct nvm_parameters 
{
  /* Number of bytes per page */
  uint8_t  page_size;
  /* Number of pages in the main array */
  uint16_t nvm_number_of_pages;
  /* Size of the emulated EEPROM memory section configured in the NVM auxiliary memory space */
  uint32_t eeprom_number_of_pages;
  /* Size of the Bootloader memory section configured in the NVM auxiliary memory space */
  uint32_t bootloader_number_of_pages;
};

BEGIN_PACK
/* Structure describing the EEPROM Emulation master page */
struct _eeprom_master_page 
{
  /* Magic key which in ASCII will show as AtEEPROMEmu */
  uint32_t magic_key[EEPROM_MAGIC_KEY_COUNT];
  /* Emulator major version information */
  uint8_t  major_version;
  /* Emulator minor version information */
  uint8_t  minor_version;
  /* Emulator revision version information */
  uint8_t  revision;

  /* Emulator identification value (to distinguish between different emulator
    schemes that carry the same version numbers) */
  uint8_t  emulator_id;
  /* Unused reserved bytes in the master page */
  uint8_t  reserved[48];
};
END_PACK

/* Structure describing emulated pages of EEPROM data */
struct _eeprom_page 
{
  /* Header information of the EEPROM page. */
  struct 
  {
    uint8_t logical_page;
    uint8_t reserved[EEPROM_HEADER_SIZE - 1];
  } header;

  /* Data content of the EEPROM page */
  uint8_t data[EEPROM_PAGE_SIZE];
};

/* Internal device instance struct */
struct _eeprom_module 
{
  /* Initialization state of the EEPROM emulator */
  bool initialized;

  /* Absolute byte pointer to the first byte of FLASH where the emulated EEPROM is stored */
  const struct _eeprom_page *flash;

  /* Number of physical FLASH pages occupied by the EEPROM emulator */
  uint16_t physical_pages;
  /* Number of logical FLASH pages occupied by the EEPROM emulator */
  uint8_t  logical_pages;

  /* Mapping array from logical EEPROM pages to physical FLASH pages */
  uint8_t page_map[EEPROM_MAX_PAGES / 2 - 4];

  /* Row number for the spare row (used by next write) */
  uint8_t spare_row;

  /* Buffer to hold the currently cached page */
  struct _eeprom_page cache;
  /* Indicates if the cache contains valid data */
  bool cache_active;
};

/* EEPROM emulator instance */
static struct _eeprom_module _eeprom_instance = {.initialized = false,};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
  \brief Initializes the EEPROM Emulator.
  \param[in] None
******************************************************************************/
void halInitEepromEmulator(void)
{
  enum status_code error_code = eeprom_emulator_init();
  
  /* No EEPROM section has been set in the device's fuses */
  SYS_E_ASSERT_FATAL((error_code != STATUS_ERR_NO_MEMORY) , EEPROM_EMULATION_NO_MEMORY);
  if (error_code != STATUS_OK) 
  {  /* Erase the emulated EEPROM memory (assume it is unformatted or irrecoverably corrupt) */
    eeprom_emulator_erase_memory();
    eeprom_emulator_init();
  }
  else {}
}

/**************************************************************************//**
  \brief Erases a given row within the physical EEPROM memory space.
  \param[in] row  Physical row in EEPROM space to erase
******************************************************************************/
static void _eeprom_emulator_nvm_erase_row(const uint8_t row)
{
    halEraseFlashPage((uint32_t)&_eeprom_instance.flash[row * NVMCTRL_ROW_PAGES]);
}

/**************************************************************************//**
  \brief Fills and commits the internal NVM controller page buffer in physical EEPROM memory space.
  \param[in] physical_page  Physical page in EEPROM space to fill
  \param[in] data           Data to write to the physical memory page
******************************************************************************/
static void _eeprom_emulator_nvm_fill_cache(const uint16_t physical_page, const void* const data)
{
  halFillFlashPageBuffer((uint32_t)&_eeprom_instance.flash[physical_page], PAGE_SIZE, (uint8_t*)data);
  halWriteFlashPage((uint32_t)&_eeprom_instance.flash[physical_page]);
}

/**************************************************************************//**
  \brief Reads a page of data stored in physical EEPROM memory space.
  \param[in]  physical_page  Physical page in EEPROM space to read
  \param[out] data           Destination buffer to fill with the read data
******************************************************************************/
static void _eeprom_emulator_nvm_read_page(const uint16_t physical_page, void* const data)
{
  uint32_t page_address = (uint32_t)&_eeprom_instance.flash[physical_page] / 2;

  /* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
   * to ensure alignment */
  for (uint16_t i = 0; i < PAGE_SIZE; i += 2) 
  {
    /* Fetch next 16-bit chunk from the NVM memory space */
    uint16_t data1 = NVM_MEMORY[page_address++];

    /* Copy first byte of the 16-bit chunk to the destination buffer */
    ((uint8_t*)data)[i] = (data1 & 0xFF);

    /* If we are not at the end of a read request with an odd byte count,
     * store the next byte of data as well */
    if (i < (PAGE_SIZE - 1))
    {
      ((uint8_t*)data)[i + 1] = (data1 >> 8);
    }
  }
}

/**************************************************************************//**
  \brief Initializes the emulated EEPROM memory, destroying the current contents.
  \param[in] None
******************************************************************************/
static void _eeprom_emulator_format_memory(void)
{
  uint16_t logical_page = 0;

  /* Set row 0 as the spare row */
  _eeprom_instance.spare_row = 0;
  _eeprom_emulator_nvm_erase_row(_eeprom_instance.spare_row);

  for (uint16_t physical_page = NVMCTRL_ROW_PAGES; physical_page < _eeprom_instance.physical_pages; physical_page++) 
  {
    if (physical_page == EEPROM_MASTER_PAGE_NUMBER)
    {
      continue;
    }

    /* If we are at the first page in a new row, erase the entire row */
    if ((physical_page % NVMCTRL_ROW_PAGES) == 0) 
    {
      _eeprom_emulator_nvm_erase_row(physical_page / NVMCTRL_ROW_PAGES);
    }

    /* Two logical pages are stored in each physical row; program in a pair of initialized but blank set of emulated EEPROM pages */
    if ((physical_page % NVMCTRL_ROW_PAGES) < 2)
    {
      /* Make a buffer to hold the initialized EEPROM page */
      struct _eeprom_page data;
      memset(&data, 0xFF, sizeof(data));

      /* Set up the new EEPROM row's header */
      data.header.logical_page = logical_page;

      /* Write the page out to physical memory */
      _eeprom_emulator_nvm_fill_cache(physical_page, &data);

      /* Increment the logical EEPROM page address now that the current address' page has been initialized */
      logical_page++;
    }
  }
}

/* Creates a map in SRAM to translate logical EEPROM pages to physical FLASH pages */
static void _eeprom_emulator_update_page_mapping(void)
{
  /* Scan through all physical pages, to map physical and logical pages */
  for (uint16_t c = 0; c < _eeprom_instance.physical_pages; c++)
  {
    if (c == EEPROM_MASTER_PAGE_NUMBER)
    {
      continue;
    }

    /* Read in the logical page stored in the current physical page */
    uint16_t logical_page = _eeprom_instance.flash[c].header.logical_page;

    /* If the logical page number is valid, add it to the mapping */
    if ((logical_page != EEPROM_INVALID_PAGE_NUMBER) &&
        (logical_page < _eeprom_instance.logical_pages)) 
    {
      _eeprom_instance.page_map[logical_page] = c;
    }
  }

  /* Use an invalid page number as the spare row until a valid one has been found */
  _eeprom_instance.spare_row = EEPROM_INVALID_ROW_NUMBER;

  /* Scan through all physical rows, to find an erased row to use as the spare */
  for (uint16_t c = 0; c < (_eeprom_instance.physical_pages / NVMCTRL_ROW_PAGES); c++)
  {
    bool spare_row_found = true;

    /* Look through pages within the row to see if they are all erased */
    for (uint8_t c2 = 0; c2 < NVMCTRL_ROW_PAGES; c2++)
    {
      uint16_t physical_page = (c * NVMCTRL_ROW_PAGES) + c2;

      if (physical_page == EEPROM_MASTER_PAGE_NUMBER)
      {
        continue;
      }

      if (_eeprom_instance.flash[physical_page].header.logical_page !=
          EEPROM_INVALID_PAGE_NUMBER)
       {
         spare_row_found = false;
       }
    }

    /* If we've now found the spare row, store it and abort the search */
    if (spare_row_found == true)
    {
      _eeprom_instance.spare_row = c;
      break;
    }
  }
}

/**************************************************************************//**
  \brief Finds the next free page in the given row if one is available.
  \param[in]  start_physical_page  Physical FLASH page index of the row tosearch
  \param[out] free_physical_page   Index of the physical FLASH page that is 
       currently free (if one was found)
  \return Whether a free page was found in the specified row.
******************************************************************************/
static bool _eeprom_emulator_is_page_free_on_row(const uint8_t start_physical_page, uint8_t *const free_physical_page)
{
  /* Convert physical page number to a FLASH row and page within the row */
  uint8_t row         = (start_physical_page / NVMCTRL_ROW_PAGES);
  uint8_t page_in_row = (start_physical_page % NVMCTRL_ROW_PAGES);

  /* Look in the current row for a page that isn't currently used */
  for (uint8_t c = page_in_row; c < NVMCTRL_ROW_PAGES; c++)
  {
    /* Calculate the page number for the current page being examined */
    uint8_t page = (row * NVMCTRL_ROW_PAGES) + c;

    /* If the page is free, pass it to the caller and exit */
    if (_eeprom_instance.flash[page].header.logical_page == EEPROM_INVALID_PAGE_NUMBER)
    {
      *free_physical_page = page;
      return true;
    }
  }

  /* No free page in the current row was found */
  return false;
}

/**************************************************************************//**
  \brief Moves data from the specified logical page to the spare row.
     Moves the contents of the specified row into the spare row, so that the
     original row can be erased and re-used. The contents of the given logical
     page is replaced with a new buffer of data
  \param[in] row_number    Physical row to examine
  \param[in] logical_page  Logical EEPROM page number in the row to update
  \param[in] data          New data to replace the old in the logical page
  \return Status code indicating the status of the operation.
******************************************************************************/
static enum status_code _eeprom_emulator_move_data_to_spare(const uint8_t row_number, 
    const uint8_t logical_page,
    const uint8_t *const data)
{
  enum status_code error_code = STATUS_OK;
  struct
  {
    uint8_t logical_page;
    uint8_t physical_page;
  } page_trans[2];

  const struct _eeprom_page *row_data =
    (struct _eeprom_page *)&_eeprom_instance.flash[row_number * NVMCTRL_ROW_PAGES];

  /* There should be two logical pages of data in each row, possibly with
     multiple revisions (right-most version is the newest). Start by assuming
     the left-most two pages contain the newest page revisions. */
  page_trans[0].logical_page  = row_data[0].header.logical_page;
  page_trans[0].physical_page = (row_number * NVMCTRL_ROW_PAGES);

  page_trans[1].logical_page  = row_data[1].header.logical_page;
  page_trans[1].physical_page = (row_number * NVMCTRL_ROW_PAGES) + 1;

  /* Look for newer revisions of the two logical pages stored in the row */
  for (uint8_t c = 0; c < 2; c++) 
  {
    /* Look through the remaining pages in the row for any newer revisions */
    for (uint8_t c2 = 2; c2 < NVMCTRL_ROW_PAGES; c2++)
    {
      if (page_trans[c].logical_page == row_data[c2].header.logical_page)
      {
        page_trans[c].physical_page =(row_number * NVMCTRL_ROW_PAGES) + c2;
      }
    }
  }

  /* Need to move both saved logical pages stored in the same row */
  for (uint8_t c = 0; c < 2; c++)
  {
    /* Find the physical page index for the new spare row pages */
    uint32_t new_page = ((_eeprom_instance.spare_row * NVMCTRL_ROW_PAGES) + c);

    /* Check if we we are looking at the page the calling function wishes
    to change during the move operation */
    if (logical_page == page_trans[c].logical_page)
    {
      /* Fill out new (updated) logical page's header in the cache */
      _eeprom_instance.cache.header.logical_page = logical_page;

      /* Write data to SRAM cache */
      memcpy(_eeprom_instance.cache.data, data, EEPROM_PAGE_SIZE);
    } 
    else
    {
      /* Copy existing EEPROM page to cache buffer wholesale */
      _eeprom_emulator_nvm_read_page(
      page_trans[c].physical_page, &_eeprom_instance.cache);
    }

    /* Fill the physical NVM buffer with the new data so that it can be
       quickly committed in the future if needed due to a low power
       condition */
    _eeprom_emulator_nvm_fill_cache(new_page, &_eeprom_instance.cache);

    /* Update the page map with the new page location and indicate that
     * the cache now holds new data */
    _eeprom_instance.page_map[page_trans[c].logical_page] = new_page;
    _eeprom_instance.cache_active = true;
  }

  /* Erase the row that was moved and set it as the new spare row */
  _eeprom_emulator_nvm_erase_row(row_number);

  /* Keep the index of the new spare row */
  _eeprom_instance.spare_row = row_number;

  return error_code;
}


/**************************************************************************//**
  \brief Create master emulated EEPROM management page.
   Creates a new master page in emulated EEPROM, giving information on the
   emulator used to store the EEPROM data
******************************************************************************/
static void _eeprom_emulator_create_master_page(void)
{
  const uint32_t magic_key[] = EEPROM_MAGIC_KEY;

  struct _eeprom_master_page master_page;
  memset(&master_page, 0xFF, sizeof(master_page));

  /* Fill out the magic key header to indicate an initialized master page */
  for (uint8_t c = 0; c < EEPROM_MAGIC_KEY_COUNT; c++)
  {
    master_page.magic_key[c] = magic_key[c];
  }

  /* Update master header with version information of this emulator */
  master_page.emulator_id   = EEPROM_EMULATOR_ID;
  master_page.major_version = EEPROM_MAJOR_VERSION;
  master_page.minor_version = EEPROM_MINOR_VERSION;
  master_page.revision      = EEPROM_REVISION;

  _eeprom_emulator_nvm_erase_row(EEPROM_MASTER_PAGE_NUMBER / NVMCTRL_ROW_PAGES);

  /* Write the new master page data to physical memory */
  _eeprom_emulator_nvm_fill_cache(EEPROM_MASTER_PAGE_NUMBER, &master_page);
}

/**************************************************************************//**
  \brief Verify the contents of a master EEPROM page.
   Verify the contents of a master EEPROM page to ensure that it contains the
   correct information for this version of the EEPROM emulation service
  \return  STATUS_OK              Given master page contents is valid
           STATUS_ERR_BAD_FORMAT  Master page contents was invalid
           STATUS_ERR_IO          Master page indicates the data is incompatible
                                 with this version of the EEPROM emulator
******************************************************************************/
static enum status_code _eeprom_emulator_verify_master_page(void)
{
  const uint32_t magic_key[] = EEPROM_MAGIC_KEY;
  struct _eeprom_master_page master_page;

  /* Copy the master page to the RAM buffer so that it can be inspected */
  _eeprom_emulator_nvm_read_page(EEPROM_MASTER_PAGE_NUMBER, &master_page);

  /* Verify magic key is correct in the master page header */
  for (uint8_t c = 0; c < EEPROM_MAGIC_KEY_COUNT; c++)
  {
    if (master_page.magic_key[c] != magic_key[c])
    {
      return STATUS_ERR_BAD_FORMAT;
    }
  }

  /* Verify emulator ID in header to ensure the same scheme is used */
  if (master_page.emulator_id   != EEPROM_EMULATOR_ID)
  {
    return STATUS_ERR_IO;
  }

  /* Verify major version in header to ensure the same version is used */
  if (master_page.major_version != EEPROM_MAJOR_VERSION)
  {
    return STATUS_ERR_IO;
  }

  /* Verify minor version in header to ensure the same version is used */
  if (master_page.minor_version != EEPROM_MINOR_VERSION)
  {
    return STATUS_ERR_IO;
  }

  /* Don't verify revision number - same major/minor is considered enough
   * to ensure the stored data is compatible. */

  return STATUS_OK;
}

/**************************************************************************//**
  \brief Retrieves the parameters of the EEPROM Emulator memory layout.
    Retrieves the configuration parameters of the EEPROM Emulator, after it has
    been initialized
  \return  STATUS_OK  If the emulator parameters were retrieved successfully
           STATUS_ERR_NOT_INITIALIZED If the EEPROM Emulator is not initialized
******************************************************************************/
enum status_code eeprom_emulator_get_parameters(struct eeprom_emulator_parameters *const parameters)
{
  if (_eeprom_instance.initialized == false)
  {
    SYS_E_ASSERT_FATAL(false, EEPROM_EMULATION_NOT_INITIALIZED_2);
    return STATUS_ERR_NOT_INITIALIZED;
  }

  parameters->page_size              = EEPROM_PAGE_SIZE;
  parameters->eeprom_number_of_pages = _eeprom_instance.logical_pages;

  return STATUS_OK;
}

/**************************************************************************//**
  \brief Initializes the EEPROM Emulator service.
    Initializes the emulated EEPROM memory space; if the emulated EEPROM memory
    has not been previously initialized, it will need to be explicitly formatted
    via \ref eeprom_emulator_erase_memory(). The EEPROM memory space will \b not
    be automatically erased by the initialization function, so that partial data
    may be recovered by the user application manually if the service is unable to
    initialize successfully
  \return  STATUS_OK  EEPROM emulation service was successfully initialized
           STATUS_ERR_NO_MEMORY No EEPROM section has been allocated in the device
           STATUS_ERR_BAD_FORMAT  Emulated EEPROM memory is corrupt or not formatted
           STATUS_ERR_IO          EEPROM data is incompatible with this version
                                  or scheme of the EEPROM emulator
******************************************************************************/
enum status_code eeprom_emulator_init(void)
{
  enum status_code error_code = STATUS_OK;
  struct nvm_parameters parameters;
  /* Clear error flags */
  NVMCTRL_STATUS |= NVMCTRL_STATUS_MASK;

  /* Read out from the PARAM register */
  /* Mask out page size exponent and convert to a number of bytes */
  parameters.page_size = 8 << ((NVMCTRL_PARAM & NVMCTRL_PARAM_PSZ_Msk) >> NVMCTRL_PARAM_PSZ_Pos);

  /* Mask out number of pages count */
  parameters.nvm_number_of_pages = (NVMCTRL_PARAM & NVMCTRL_PARAM_NVMP_Msk) >> NVMCTRL_PARAM_NVMP_Pos;

  /* Read the current EEPROM fuse value from the USER row */
  uint16_t eeprom_fuse_value = (NVM_USER_MEMORY[NVMCTRL_FUSES_EEPROM_SIZE_Pos / 16] & 
  NVMCTRL_FUSES_EEPROM_SIZE_Msk) >> NVMCTRL_FUSES_EEPROM_SIZE_Pos;

  /* Translate the EEPROM fuse byte value to a number of NVM pages */
  if (eeprom_fuse_value == 7)
  {
    parameters.eeprom_number_of_pages = 0;
  }
  else
  {
    parameters.eeprom_number_of_pages = NVMCTRL_ROW_PAGES << (6 - eeprom_fuse_value);
  }
        
  /* Ensure the device fuses are configured for at least one master page row,
   * one user EEPROM data row and one spare row */
  if (parameters.eeprom_number_of_pages < (3 * NVMCTRL_ROW_PAGES))
  {
    return STATUS_ERR_NO_MEMORY;
  }

  /* Configure the EEPROM instance physical and logical number of pages:
   *  - One row is reserved for the master page
   *  - One row is reserved for the spare row
   *  - Two logical pages can be stored in one physical row
   */
  _eeprom_instance.physical_pages = parameters.eeprom_number_of_pages;
  _eeprom_instance.logical_pages  = (parameters.eeprom_number_of_pages - (2 * NVMCTRL_ROW_PAGES)) / 2;

  /* Configure the EEPROM instance starting physical address in FLASH and
   * pre-compute the index of the first page in FLASH used for EEPROM */
  _eeprom_instance.flash = (void*)(FLASH_SIZE - ((uint32_t)_eeprom_instance.physical_pages * PAGE_SIZE));

  /* Clear EEPROM page write cache on initialization */
  _eeprom_instance.cache_active = false;

  /* Scan physical memory and re-create logical to physical page mapping
   * table to locate logical pages of EEPROM data in physical FLASH */
  _eeprom_emulator_update_page_mapping();

  /* Could not find spare row - abort as the memory appears to be corrupt */
  if (_eeprom_instance.spare_row == EEPROM_INVALID_ROW_NUMBER)
  {
    return STATUS_ERR_BAD_FORMAT;
  }

  /* Verify that the master page contains valid data for this service */
  error_code = _eeprom_emulator_verify_master_page();
  if (error_code != STATUS_OK)
  {
    return error_code;
  }

  /* Mark initialization as complete */
  _eeprom_instance.initialized = true;

  return error_code;
}

/**************************************************************************//**
  \brief Erases the entire emulated EEPROM memory space.
    Erases and re-initializes the emulated EEPROM memory space, destroying any
    existing data
******************************************************************************/
void eeprom_emulator_erase_memory(void)
{
  /* Create new EEPROM memory block in EEPROM emulation section */
  _eeprom_emulator_format_memory();

  /* Write EEPROM emulation master block */
  _eeprom_emulator_create_master_page();

  /* Map the newly created EEPROM memory block */
  _eeprom_emulator_update_page_mapping();
}

/**************************************************************************//**
  \brief Writes a page of data to an emulated EEPROM memory page.
    Writes an emulated EEPROM page of data to the emulated EEPROM memory space
  \note Data stored in pages may be cached in volatile RAM memory; to commit
        any cached data to physical non-volatile memory, the
        \ref eeprom_emulator_commit_page_buffer() function should be called.
  \param[in] logical_page  Logical EEPROM page number to write to
  \param[in] data  Pointer to the data buffer containing source data to write
  \return STATUS_OK                    If the page was successfully read
          STATUS_ERR_NOT_INITIALIZED   If the EEPROM emulator is not initialized
          STATUS_ERR_BAD_ADDRESS       If an address outside the valid emulated
                                       EEPROM memory space was supplied
******************************************************************************/
enum status_code eeprom_emulator_write_page(const uint8_t logical_page, const uint8_t *const data)
{
  /* Ensure the emulated EEPROM has been initialized first */
  if (_eeprom_instance.initialized == false)
  {
    SYS_E_ASSERT_FATAL(false, EEPROM_EMULATION_NOT_INITIALIZED_0);
    return STATUS_ERR_NOT_INITIALIZED;
  }

  /* Make sure the write address is within the allowable address space */
  if (logical_page >= _eeprom_instance.logical_pages)
  {
    SYS_E_ASSERT_FATAL(false, EEPROM_EMULATION_BAD_ADDRESS_0);
    return STATUS_ERR_BAD_ADDRESS;
  }

  /* Check if we have space in the current page location's physical row for
     a new version, and if so get the new page index */
  uint8_t new_page = 0;
  bool page_spare  = _eeprom_emulator_is_page_free_on_row(
                         _eeprom_instance.page_map[logical_page], &new_page);

  /* Check if the current row is full, and we need to swap it out with a spare row */
  if (page_spare == false)
  {
    /* Move the other page we aren't writing that is stored in the same page to the new row, 
       and replace the old current page with the new page contents (cache is updated to match) */
    _eeprom_emulator_move_data_to_spare(_eeprom_instance.page_map[logical_page] / NVMCTRL_ROW_PAGES,
      logical_page, data);

    /* New data is now written and the cache is updated, exit */
    return STATUS_OK;
  }

  /* Update the page cache header section with the new page header */
  _eeprom_instance.cache.header.logical_page = logical_page;

  /* Update the page cache contents with the new data */
  memcpy(&_eeprom_instance.cache.data, data, EEPROM_PAGE_SIZE);

  /* Fill the physical NVM buffer with the new data so that it can be quickly 
     committed in the future if needed due to a low power condition */
  _eeprom_emulator_nvm_fill_cache(new_page, &_eeprom_instance.cache);

  /* Update the cache parameters and mark the cache as active */
  _eeprom_instance.page_map[logical_page] = new_page;
  _eeprom_instance.cache_active           = true;

  return STATUS_OK;
}

/**************************************************************************//**
  \brief Reads a page of data from an emulated EEPROM memory page
   Reads an emulated EEPROM page of data from the emulated EEPROM memory space
  \param[in]  logical_page  Logical EEPROM page number to read from
  \param[out] data          Pointer to the destination data buffer to fill
  \return Status code indicating the status of the operation.
         STATUS_OK   If the page was successfully read
         STATUS_ERR_NOT_INITIALIZED   If the EEPROM emulator is not initialized
         STATUS_ERR_BAD_ADDRESS       If an address outside the valid emulated
                                      EEPROM memory space was supplied
 ******************************************************************************/
enum status_code eeprom_emulator_read_page(const uint8_t logical_page,
  uint8_t *const data)
{
  /* Ensure the emulated EEPROM has been initialized first */
  if (_eeprom_instance.initialized == false)
  {
    SYS_E_ASSERT_FATAL(false, EEPROM_EMULATION_NOT_INITIALIZED_1);
    return STATUS_ERR_NOT_INITIALIZED;
  }

  /* Make sure the read address is within the allowable address space */
  if (logical_page >= _eeprom_instance.logical_pages)
  {
    SYS_E_ASSERT_FATAL(false, EEPROM_EMULATION_BAD_ADDRESS_1);
    return STATUS_ERR_BAD_ADDRESS;
  }

  /* Check if the page to read is currently cached (and potentially out of
     sync/newer than the physical memory) */
  if ((_eeprom_instance.cache_active == true) && (_eeprom_instance.cache.header.logical_page == logical_page))
  {
    /* Copy the potentially newer cached data into the user buffer */
    memcpy(data, _eeprom_instance.cache.data, EEPROM_PAGE_SIZE);
  }
  else
  {
    struct _eeprom_page temp;
    /* Copy the data from non-volatile memory into the temporary buffer */
    _eeprom_emulator_nvm_read_page(
    _eeprom_instance.page_map[logical_page], &temp);

    /* Copy the data portion of the read page to the user's buffer */
    memcpy(data, temp.data, EEPROM_PAGE_SIZE);
  }

  return STATUS_OK;
}

/**************************************************************************//**
  \brief Writes a buffer of data to the emulated EEPROM memory space
   Writes a buffer of data to a section of emulated EEPROM memory space. The
   source buffer may be of any size, and the destination may lie outside of an
   emulated EEPROM page boundary.
   \note Data stored in pages may be cached in volatile RAM memory; to commit
         any cached data to physical non-volatile memory, the
         \ref eeprom_emulator_commit_page_buffer() function should be called.
   \param[in] offset  Starting byte offset to write to, in emulated EEPROM memory space
   \param[in] data    Pointer to the data buffer containing source data to write
   \param[in] length  Length of the data to write, in bytes
   \return Status code indicating the status of the operation.
      STATUS_OK                    If the page was successfully read
      STATUS_ERR_NOT_INITIALIZED   If the EEPROM emulator is not initialized
      STATUS_ERR_BAD_ADDRESS       If an address outside the valid emulated
                                   EEPROM memory space was supplied 
 ******************************************************************************/
enum status_code eeprom_emulator_write_buffer(uint16_t offset,uint8_t *data,
    uint16_t length)
{
  enum status_code error_code = STATUS_OK;
  uint8_t buffer[EEPROM_PAGE_SIZE];
  uint8_t logical_page = offset / EEPROM_PAGE_SIZE;
  uint16_t c = offset;
  /* Keep track of whether the currently updated page has been written */
  bool page_dirty = false;
  /* Perform the initial page read if necessary*/
  if ((offset % EEPROM_PAGE_SIZE) || length < EEPROM_PAGE_SIZE)
  {
    error_code = eeprom_emulator_read_page(logical_page, buffer);
    if (error_code != STATUS_OK)
    {
      return error_code;
    }
  }

  /* To avoid entering into the initial if in the loop the first time */
  if ((offset % EEPROM_PAGE_SIZE) == 0)
  {
    buffer[c % EEPROM_PAGE_SIZE] = data[c - offset];
    page_dirty = true;
    c=c+1;
  }

  /* Write the specified data to the emulated EEPROM memory space */
  for (; c < (length + offset); c++)
  {
    /* Check if we have written up to a new EEPROM page boundary */
    if ((c % EEPROM_PAGE_SIZE) == 0)
    {
      /* Write the current page to non-volatile memory from the temporary buffer */
      error_code = eeprom_emulator_write_page(logical_page, buffer);
      page_dirty = false;

      if (error_code != STATUS_OK)
      {
        break;
      }

    /* Increment the page number we are looking at */
    logical_page++;

    /* Read the next page from non-volatile memory into the temporary buffer in case of a partial page write */
    error_code = eeprom_emulator_read_page(logical_page, buffer);

    if (error_code != STATUS_OK)
    {
      return error_code;
    }
  }
  /* Copy the next byte of data from the user's buffer to the temporary
     buffer */
    buffer[c % EEPROM_PAGE_SIZE] = data[c - offset];
    page_dirty = true;
  }

  /* If the current page is dirty, write it */
  if (page_dirty)
  {
    error_code = eeprom_emulator_write_page(logical_page, buffer);
  }

  return error_code;
}

/**************************************************************************//**
  \brief Reads a buffer of data from the emulated EEPROM memory space
   Reads a buffer of data from a section of emulated EEPROM memory space. The
   destination buffer may be of any size, and the source may lie outside of an
   emulated EEPROM page boundary.
   \param[in]  offset  Starting byte offset to read from, in emulated EEPROM
                       memory space
   \param[out] data    Pointer to the data buffer containing source data to read
   \param[in]  length  Length of the data to read, in bytes
   \return Status code indicating the status of the operation.
   \retval STATUS_OK                    If the page was successfully read
   \retval STATUS_ERR_NOT_INITIALIZED   If the EEPROM emulator is not initialized
   \retval STATUS_ERR_BAD_ADDRESS       If an address outside the valid emulated
                                        EEPROM memory space was supplied
 */
enum status_code eeprom_emulator_read_buffer(const uint16_t offset, uint8_t *const data,
   const uint16_t length)
{
  enum status_code error_code;
  uint8_t buffer[EEPROM_PAGE_SIZE];
  uint8_t logical_page = offset / EEPROM_PAGE_SIZE;
  uint16_t c = offset;

  /** Perform the initial page read  */
  error_code = eeprom_emulator_read_page(logical_page, buffer);
  if (error_code != STATUS_OK)
  {
    return error_code;
  }

  /* To avoid entering into the initial if in the loop the first time */
  if ((offset % EEPROM_PAGE_SIZE) == 0)
  {
    data[0] = buffer[0];
    c=c+1;
  }

  /* Read in the specified data from the emulated EEPROM memory space */
  for (; c < (length + offset); c++)
  {
    /* Check if we have read up to a new EEPROM page boundary */
    if ((c % EEPROM_PAGE_SIZE) == 0)
    {
      /* Increment the page number we are looking at */
      logical_page++;

      /* Read the next page from non-volatile memory into the temporary buffer */
      error_code = eeprom_emulator_read_page(logical_page, buffer);

      if (error_code != STATUS_OK)
      {
        return error_code;
      }
  }

    /* Copy the next byte of data from the temporary buffer to the user's buffer */
    data[c - offset] = buffer[c % EEPROM_PAGE_SIZE];
  }

  return error_code;
}
#endif //#defined(HAL_USE_EEPROM_EMULATION)
#endif // defined(HAL_USE_EE_READY)
// eof helEeprom.c
