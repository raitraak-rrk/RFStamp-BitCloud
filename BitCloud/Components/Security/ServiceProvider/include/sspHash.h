/**************************************************************************//**
  \file sspHash.h

  \brief Security Hash Processor header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      01/05/2008 A.Potashov - Created
******************************************************************************/

#ifndef _SSPHASH_H
#define _SSPHASH_H

#include <sysTypes.h>
#include <private/sspHashHandler.h>
#include <sspCommon.h>

typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    void *next; /*!< Used for queue support */
#ifdef _ZAPPSI_
    /* to be compatible with MAC service. */
    uint8_t requestId;
    /* Sequence number to identify request-response pair. */
    uint8_t sequenceNumber;
    union
    {
      /* Callback wrapper routine pointer */
      void (*callback)(void*);
      /* Processing routine pointer to call after required memory will become free */
      void (*process)(void*);
    } zsi;
#else /* _ZAPPSI_ */
    uint8_t requestId;
#endif /* # _ZAPPSI_ */
  } service;
  /** \endcond **/
  // Service field - for internal needs.
  SSP_Service_t           sspService;
  // There is should be an additional memory before and after text (16 bytes in both cases).
  uint8_t *text/*[SECURITY_KEY_SIZE + textSize + 16]*/;
  // Pointer to key fir Keyed Hash Function.
  uint8_t (*key)/*[SECURITY_KEY_SIZE]*/;
  // Size of the text.
  uint8_t textSize;
  // Buffer for generated hash. Shoul be initialized by the user.
  uint8_t (*hash_i)/*[SECURITY_KEY_SIZE]*/;
  // Confirm handler pointer for internal use
  void (*sspKeyedHashMacConf)(void);

  // Confirm handler pointer
  void (*SSP_KeyedHashMacConf)(void);
} SSP_KeyedHashMacReq_t;


void SSP_KeyedHashMacReq(SSP_KeyedHashMacReq_t *req);
void sspKeyedHashReqHandler(void);
/**************************************************************************\\**
  \brief This function computes the AES MMO digest of the data parameter of
  length <tt>sz</tt>, and stores the result in.

  \param[out] digest  This is an unsigned char buffer to hold the message digest.
                    The length of the digest must be AES_MMO_HASH_SIZE bytes.
  \param[in] sz       The size in bytes of the message to be hashed.
  \param[in] data     This is an unsigned char buffer of data to be hashed.

  \return MCE_ERR_NULL_OUTPUT_BUF  digest is NULL
          MCE_ERR_NULL_INPUT_BUF   data is NULL
          MCE_SUCCESS              indicates successful completion.

******************************************************************************/
int SSP_BcbHash(unsigned char *digest, unsigned long sz, unsigned char *data);

#endif //#ifndef _SSPHASH_H

//eof sspHash.h
