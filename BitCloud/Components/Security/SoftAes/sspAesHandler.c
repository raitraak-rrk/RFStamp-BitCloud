/**************************************************************************//**
  \file sspAesHandler.c

  \brief File contains AES functions implementations.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2006 - MCherkashin created.
      2006 - VGrybanovsky optimized.
      207.11.29 ALuzhetsky API corrected.
******************************************************************************/

/******************************************************************************
                                Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <stdlib.h>
#include <string.h> // For mem* functions.
#include <sspMem.h>
#include <sspAesHandler.h>
#include <sspManager.h>
#include <sspDbg.h>
#include <sysAssert.h>

/******************************************************************************
                                Definitions section.
******************************************************************************/
#define S_BOX_SIZE 256

#ifdef _SSP_USE_FLASH_FOR_CONST_DATA
  PROGMEM_DECLARE(uint8_t btable[S_BOX_SIZE]) = {
#else //_SSP_USE_FLASH_FOR_CONST_DATA
  const uint8_t btable[S_BOX_SIZE] = {
#endif //_SSP_USE_FLASH_FOR_CONST_DATA
                        0,   1, 141, 246, 203,  82, 123, 209, 232,  79,  41, 192, 176, 225, 229, 199,
                      116, 180, 170,  75, 153,  43,  96,  95,  88,  63, 253, 204, 255,  64, 238, 178,
                       58, 110,  90, 241,  85,  77, 168, 201, 193,  10, 152,  21,  48,  68, 162, 194,
                       44,  69, 146, 108, 243,  57, 102,  66, 242,  53,  32, 111, 119, 187,  89,  25,
                       29, 254,  55, 103,  45,  49, 245, 105, 167, 100, 171,  19,  84,  37, 233,   9,
                      237,  92,   5, 202,  76,  36, 135, 191,  24,  62,  34, 240,  81, 236,  97,  23,
                       22,  94, 175, 211,  73, 166,  54,  67, 244,  71, 145, 223,  51, 147,  33,  59,
                      121, 183, 151, 133,  16, 181, 186,  60, 182, 112, 208,   6, 161, 250, 129, 130,
                      131, 126, 127, 128, 150, 115, 190,  86, 155, 158, 149, 217, 247,   2, 185, 164,
                      222, 106,  50, 109, 216, 138, 132, 114,  42,  20, 159, 136, 249, 220, 137, 154,
                      251, 124,  46, 195, 143, 184, 101,  72,  38, 200,  18,  74, 206, 231, 210,  98,
                       12, 224,  31, 239,  17, 117, 120, 113, 165, 142, 118,  61, 189, 188, 134,  87,
                       11,  40,  47, 163, 218, 212, 228,  15, 169,  39,  83,   4,  27, 252, 172, 230,
                      122,   7, 174,  99, 197, 219, 226, 234, 148, 139, 196, 213, 157, 248, 144, 107,
                      177,  13, 214, 235, 198,  14, 207, 173,   8,  78, 215, 227,  93,  80,  30, 179,
                       91,  35,  56,  52, 104,  70,   3, 140, 221, 156, 125, 160, 205,  26,  65,  28
                      };

/******************************************************************************
                            Global functions declaration section.
******************************************************************************/
void sspAesReqHandler(void);

/******************************************************************************
                            Static functions declaration section.
******************************************************************************/
static void aesExpandKey(uint8_t *in/*[AES_TEMP_BUF_SIZE]*/);
static void aesAddRoundKey(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/, uint8_t *key/*[SECURITY_KEY_SIZE]*/);
static void aesSubBytes(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/);
static void aesShiftRows(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/);
static void aesMixColumns(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/);
static void aesMixColumn(uint8_t *state/*[4]*/);
void sspAesEncrypt(SspAesEncryptReq_t *reqParam);

/******************************************************************************
                            Implementation section.
******************************************************************************/
#if  defined(_SSP_SW_AES_)
/**************************************************************************//**
  \brief
  \param
  \return
******************************************************************************/
void sspAesEncryptReq(SspAesEncryptReq_t *param)
{
  param = param;
  sspPostTask(SSP_TASK_AES);
}

/**************************************************************************//**
  \brief TBD

  \param TBD
  \return TBD
******************************************************************************/
void sspAesReqHandler(void)
{
  SspAesEncryptReq_t *reqParam = &sspGetAesMem()->encryptReq;
  sspAesEncrypt(reqParam);
  SYS_E_ASSERT_FATAL(reqParam->sspAesEncryptConf, SSP_AESENCRYPT_NULLCALLBACK0);
  reqParam->sspAesEncryptConf();//(&confParam);
}
#endif  //defined(_SSP_SW_AES_)

/**************************************************************************//**
  \brief
  \param
  \return
******************************************************************************/
//void aesEncrypt(uint8_t key[ /*SECURITY_KEY_SIZE*/ ], uint8_t buf[ /*AES_TEXT_SIZE*/ ], uint8_t temp[ /*AES_TEMP_BUF_SIZE*/ ])
void sspAesEncrypt(SspAesEncryptReq_t *reqParam)
{
  uint8_t round = 0;

  memcpy(reqParam->temp, reqParam->key, SECURITY_KEY_SIZE);
  aesExpandKey(reqParam->temp);
  // Number of Rounds Nr = 10 (for Nk words = 4 and Nb words = 4).
  for (round = 0; round < 9; round++)
  {
    aesAddRoundKey(reqParam->text, (uint8_t *)reqParam->temp + 16 * round);
    aesSubBytes(reqParam->text);
    aesShiftRows(reqParam->text);
    aesMixColumns(reqParam->text);
  }
  aesAddRoundKey(reqParam->text, (uint8_t *)reqParam->temp + 144);
  aesSubBytes(reqParam->text);
  aesShiftRows(reqParam->text);
  aesAddRoundKey(reqParam->text, (uint8_t *)reqParam->temp + 160);
}

/**************************************************************************//**
  \brief Calculate the s-box for a given number

  \param TBD
  \return TBD
******************************************************************************/
uint8_t sbox(uint8_t in)
{
  uint8_t c;
  uint8_t s;
  uint8_t x;

#ifdef _SSP_USE_FLASH_FOR_CONST_DATA
  memcpy_P(&x, &btable[in], sizeof(uint8_t));
  s = x;
#else //_SSP_USE_FLASH_FOR_CONST_DATA
  s = x = btable[in];
#endif //_SSP_USE_FLASH_FOR_CONST_DATA
  for (c = 0; c < 4; c++)
  {
    /* One bit circular rotate to the left */
    s = (s << 1) | (s >> 7);
    /* xor with x */
    x ^= s;
  }
  x ^= 99; /* 0x63 */
  return x;
}

/**************************************************************************//**
  \brief Expanding 128-bit key.

  \param TBD
  \return TBD
******************************************************************************/
static void aesExpandKey(uint8_t *in/*[AES_TEMP_BUF_SIZE]*/)
{
  uint8_t t[4];
  uint8_t c = 16; // c is 16 because the first sub-key is the user-supplied key
  uint8_t i = 1;
  uint8_t a;

  /* We need 11 sets of sixteen bytes each for 128-bit mode */
  while(c < AES_TEMP_BUF_SIZE)
  {
    /* Copy the temporary variable over from the last 4-byte
     * block */
    for (a = 0; a < 4; a++)
            t[a] = in[a + c - 4];
    /* Every four blocks (of four bytes),
     * do a complex calculation */
    if(c % 16 == 0)
    {
      a = t[0];
      t[0] = t[1];
      t[1] = t[2];
      t[2] = t[3];
      t[3] = a;
      for (a = 0; a < 4; a++)
        t[a] = sbox(t[a]);
      if (i != 0)
      {
        uint8_t cc = 1;
        uint8_t j = i;

        while(j != 1)
        {
          uint8_t bb;

          bb = cc & 0x80;
          cc <<= 1;
          if ( bb == 0x80 )
            cc ^= 0x1b;
          j--;
        }
        t[0] ^= cc;
      }
      i++;
    }
    for (a = 0; a < 4; a++)
    {
      in[c] = in[c - 16] ^ t[a];
      c++;
    }
  }
}

/**************************************************************************//**
  \brief TBD.

  \param TBD
  \return TBD
******************************************************************************/
static void aesAddRoundKey(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/, uint8_t *key/*[SECURITY_KEY_SIZE]*/)
{
  uint8_t l;

  for (l = 0; l < 16; l++)
    state[l] ^= key[l];
}

/**************************************************************************//**
  \brief TBD.

  \param TBD
  \return TBD
******************************************************************************/
static void aesSubBytes(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/)
{
  uint8_t l;

  for (l = 0; l < 16; l++)
    state[l] = sbox(state[l]);
}


/**************************************************************************//**
  \brief TBD.

  \param TBD
  \return TBD
******************************************************************************/
static void aesShiftRows(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/)
{
  uint8_t l;
  uint8_t m;
  uint8_t s;

  for (l = 0; l < 4; l++)
  {
    for (s = 0; s < l; s++)
    {
      uint8_t a = state[l];

      for (m = 0; m < 12; m += 4)
        state[m + l] = state[m + l + 4];
      state[12 + l] = a;
    }
  }
}


/**************************************************************************//**
  \brief TBD.

  \param TBD
  \return TBD
******************************************************************************/
static void aesMixColumn(uint8_t *state/*[4]*/)
{
  uint8_t a[4];
  uint8_t b[4];
  uint8_t c;
  uint8_t h;

  for (c = 0; c < 4; c++)
  {
    a[c] = state[c];
    h = state[c] & 0x80; /* hi bit */
    b[c] = state[c] << 1;
    if(h == 0x80)
      b[c] ^= 0x1b; /* Rijndael's Galois field */
  }
  state[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
  state[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
  state[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
  state[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];
}

/**************************************************************************//**
  \brief TBD.

  \param TBD
  \return TBD
******************************************************************************/
static void aesMixColumns(uint8_t *state/*[SECURITY_BLOCK_SIZE]*/)
{
  uint8_t l;

  for (l = 0; l < 16; l += 4)
    aesMixColumn((uint8_t *)state + l);
}

// eof sspAesHandler.c
