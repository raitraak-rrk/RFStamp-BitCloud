/******************************************************************************
  \file at86rf231.h

  \brief Declarations for Atmel AT86RF231.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      12/09/07 ALuzhetsky - Created
      30/02/12 A. Razinkov - Refactored
******************************************************************************/

#ifndef AT86RF231_H_
#define AT86RF231_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <sysEndian.h>
#include <at86rf2xx.h>

#ifndef AT86RF231
#  error "AT86RF231 is not defined, but header file was used."
#endif

/******************************************************************************
                    Common definitions
******************************************************************************/
/* Mask which defines supported channels */
#define RF_SUPPORTED_CHANNELS   0x07fff800
/* Minimum channel number */
#define RF_MIN_CHANNEL          11
/* Maximum channel number */
#define RF_MAX_CHANNEL          26
/* Minimum RSSI sensitivity */
#define AT86RF231_RSSI_BASE_VAL (-90)

/******************************************************************************
                    Registers addresses
******************************************************************************/
/* Common register addresses */
#define TRX_STATUS_REG    0x01
#define TRX_STATE_REG     0x02
#define TRX_CTRL_0_REG    0x03
#define TRX_CTRL_1_REG    0x04
#define PHY_TX_PWR_REG    0x05
#define PHY_RSSI_REG      0x06
#define PHY_ED_LEVEL_REG  0x07
#define PHY_CC_CCA_REG    0x08
#define CCA_THRESH_REG    0x09
#define RX_CTRL_REG       0x0A
#define SFD_VALUE_REG     0x0B
#define TRX_CTRL_2_REG    0x0C
#define ANT_DIV_REG       0x0D
#define IRQ_MASK_REG      0x0E
#define IRQ_STATUS_REG    0x0F
#define VREG_CTRL_REG     0x10
#define BATMON_REG        0x11
#define XOSC_CTRL_REG     0x12
#define RX_SYN_REG        0x15
#define XAH_CTRL_REG      0x17
#define FTN_CTRL_REG      0x18
#define PLL_CF_REG        0x1A
#define PLL_DCU_REG       0x1B
#define PART_NUM_REG      0x1C
#define VERSION_NUM_REG   0x1D
#define MAN_ID_0_REG      0x1E
#define MAN_ID_1_REG      0x1F
#define SHORT_ADDR_0_REG  0x20
#define SHORT_ADDR_1_REG  0x21
#define PAN_ID_0_REG      0x22
#define PAN_ID_1_REG      0x23
#define IEEE_ADDR_0_REG   0x24
#define IEEE_ADDR_1_REG   0x25
#define IEEE_ADDR_2_REG   0x26
#define IEEE_ADDR_3_REG   0x27
#define IEEE_ADDR_4_REG   0x28
#define IEEE_ADDR_5_REG   0x29
#define IEEE_ADDR_6_REG   0x2A
#define IEEE_ADDR_7_REG   0x2B
#define XAH_CTRL_0_REG    0x2C
#define CSMA_SEED_0_REG   0x2D
#define CSMA_SEED_1_REG   0x2E
#define CSMA_BE_REG       0x2F

/* SRAM register addresses */
#define AESST_REG         0x82
#define AESCON_REG        0x83
#define AESCON_SHADOW_REG 0x94

/******************************************************************************
                    Command values
******************************************************************************/
#define RF_CMD_RW ((1<<7) | (1<<6))
#define RF_CMD_RR ((1<<7) | (0<<6))
#define RF_CMD_FW ((0<<7) | (1<<6) | (1<<5))
#define RF_CMD_FR ((0<<7) | (0<<6) | (1<<5))
#define RF_CMD_SW ((0<<7) | (1<<6) | (0<<5))
#define RF_CMD_SR ((0<<7) | (0<<6) | (0<<5))
#define RF_RADDRM ((0<<7) | (0<<6) | (1<<5) | (1<<4) | (1<<3) | (1<<2) |(1<<1) | (1<<0))

/******************************************************************************
                    Registers description
******************************************************************************/
BEGIN_PACK
/***************************
        TRX_STATUS
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t trxStatus : 5,
    uint8_t reserved  : 1,
    uint8_t ccaStatus : 1,
    uint8_t ccaDone   : 1
  ))
} MAY_ALIAS RegTrxStatus_t;

#define TRX_STATUS_MEANINGFUL_BITS     0x1F
#define REG_TRX_STATUS_TRX_STATUS_MASK 0x1F
// CCA_DONE
#define CCA_IN_PROGRESS                0
#define CCA_DONE                       1
// CCA_STATUS
#define CCA_STATUS_BUSY                0
#define CCA_STATUS_IDLE                1
// TRX_STATUS
#define TRX_STATUS_P_ON                0
#define TRX_STATUS_BUSY_RX             1
#define TRX_STATUS_BUSY_TX             2
#define TRX_STATUS_RX_ON               6
#define TRX_STATUS_TRX_OFF             8
#define TRX_STATUS_PLL_ON              9
#define TRX_STATUS_SLEEP               15
#define TRX_STATUS_BUSY_RX_AACK        17
#define TRX_STATUS_BUSY_TX_ARET        18
#define TRX_STATUS_RX_AACK_ON          22
#define TRX_STATUS_TX_ARET_ON          25
#define TRX_STATUS_RX_ON_NOCLK         28
#define TRX_STATUS_RX_AACK_ON_NOCLK    29
#define TRX_STATUS_BUSY_RX_AACK_NOCLK  30
#define TRX_STATUS_TRANS_IN_PROGRESS   31

/***************************
        TRX_STATE
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint8_t trxCmd     : 5,
    uint8_t tracStatus : 3
  ))
} MAY_ALIAS RegTrxState_t;

// TRAC_STATUS
#define TRAC_STATUS_SUCCESS                0
#define TRAC_STATUS_SUCCESS_DATA_PENDING   1
#define TRAC_STATUS_WAIT_FOR_ACK           2
#define TRAC_STATUS_CHANNEL_ACCESS_FAILURE 3
#define TRAC_STATUS_NO_ACK                 5
#define TRAC_STATUS_INVALID                7
// TRX_CMD
#define TRX_CMD_NOP                        0
#define TRX_CMD_TX_START                   2
#define TRX_CMD_FORCE_TRX_OFF              3
#define TRX_CMD_FORCE_PLL_ON               4
#define TRX_CMD_RX_ON                      6
#define TRX_CMD_TRX_OFF                    8
#define TRX_CMD_PLL_ON                     9
#define TRX_CMD_RX_AACK_ON                 22
#define TRX_CMD_TX_ARET_ON                 25

/***************************
        TRX_CTRL_0
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t clkmCtrl   : 3,
    uint8_t clkmShaSel : 1,
    uint8_t padIoClkm  : 2,
    uint8_t padIo      : 2
  ))
} MAY_ALIAS RegTrxCtrl0_t;

// PAD_IO
#define PAD_IO_2MA           0
#define PAD_IO_4MA           1
#define PAD_IO_6MA           2
#define PAD_IO_8MA           3
// PAD_IO_CLKM
#define PAD_IO_CLK_2MA       0
#define PAD_IO_CLK_4MA       1
#define PAD_IO_CLK_6MA       2
#define PAD_IO_CLK_8MA       3
// CLKM_SHA_SEL
#define CLKM_SHA_SEL_DISABLE 0
#define CLKM_SHA_SEL_ENABLE  1
// CLKM_CTRL
#define CLK_CTRL_NOCLK       0
#define CLK_CTRL_1MHZ        1
#define CLK_CTRL_2MHZ        2
#define CLK_CTRL_4MHZ        3
#define CLK_CTRL_8MHZ        4
#define CLK_CTRL_16MHZ       5
#define CLK_CTRL_1_4MHZ      6
#define CLK_CTRL_1_16MHZ     7

/***************************
        TRX_CTRL_1
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(7, (
    uint8_t irqPolarity  : 1,
    uint8_t irqMaskMode  : 1,
    uint8_t spiCmdMode   : 2,
    uint8_t rxBlCtrl     : 1,
    uint8_t txAutoCrcOn  : 1,
    uint8_t irq2ExtEn    : 1,
    uint8_t paExtEn      : 1
  ))
} MAY_ALIAS RegTrxCtrl1_t;

// Mask for TRX_CTRL_1
#define IRQ2_EXT_EN_MASK        (1 << 6)
// PA_EXT_EN
#define PA_EXT_DISABLE          0
#define PA_EXT_ENABLE           1
// IRQ_2_EXT_EN
#define IRQ_2_EXT_DISABLE       0
#define IRQ_2_EXT_ENABLE        1
// TX_AUTO_CRC_ON
#define TX_AUTO_CRC_ON_DISABLE  0
#define TX_AUTO_CRC_ON_ENABLE   1
// SPI_CMD_MODE
#define SPI_CMD_MODE_DEFAULT    0
#define SPI_CMD_MODE_TRX_STATUS 1
#define SPI_CMD_MODE_PHY_RSSI   2
#define SPI_CMD_MODE_IRQ_STATUS 3
// IRQ_MASK_MODE
#define NOT_SHOW_MASKED_IRQ     0
#define SHOW_MASKED_IRQ         1
// IRQ_POLARITY
#define IRQ_HIGH_ACTIVE         0
#define IRQ_LOW_ACTIVE          1

/***************************
        PHY_TX_PWR
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t txPwr       : 4,
    uint8_t paLt        : 2,
    uint8_t paBulLt     : 2
  ))
} MAY_ALIAS RegPhyTxPwr_t;

// Masks for PHY_TX_PWR register
#define REG_PHY_TX_PWR_PA_BUF_LT_MASK 0xC0
#define REG_PHY_TX_PWR_PA_LT_MASK     0x30
#define REG_PHY_TX_PWR_TX_PWR_MASK    0x0F
// PA_BUF_LT
#define PA_BUF_LT_0_MCS               0
#define PA_BUF_LT_2_MCS               1
#define PA_BUF_LT_4_MCS               2
#define PA_BUF_LT_6_MCS               3
// PA_LT
#define PA_LT_2_MCS                   0
#define PA_LT_4_MCS                   1
#define PA_LT_6_MCS                   2
#define PA_LT_8_MCS                   3
// TX_PWR
#define TX_PWR_3_2DBM                 0x00
#define TX_PWR_2_8DBM                 0x01
#define TX_PWR_2_3DBM                 0x02
#define TX_PWR_1_8DBM                 0x03
#define TX_PWR_1_3DBM                 0x04
#define TX_PWR_0_7DBM                 0x05
#define TX_PWR_0_DBM                  0x06
#define TX_PWR_MIN_1_DBM              0x07
#define TX_PWR_MIN_2_DBM              0x08
#define TX_PWR_MIN_3_DBM              0x09
#define TX_PWR_MIN_4_DBM              0x0A
#define TX_PWR_MIN_5_DBM              0x0B
#define TX_PWR_MIN_7_DBM              0x0C
#define TX_PWR_MIN_9_DBM              0x0D
#define TX_PWR_MIN_12_DBM             0x0E
#define TX_PWR_MIN_17_DBM             0x0F

/***************************
        PHY_RSSI
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t rssi       : 5,
    uint8_t rndValue   : 2,
    uint8_t rxCrcValid : 1
  ))
} MAY_ALIAS RegPhyRssi_t;

/***************************
        PHY_CC_CCA
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t channel    : 5,
    uint8_t ccaMode    : 2,
    uint8_t ccaRequest : 1
  ))
} MAY_ALIAS RegPhyCcCca_t;

// Masks for PHY_CC_CCA register
#define REG_PHY_CC_CCA_CCA_REQUEST_MASK 0x80
#define REG_PHY_CC_CCA_CCA_MODE_MASK    0x60
#define REG_PHY_CC_CCA_CHANNEL_MASK     0x1F
// CCA_REQUEST
#define CCA_REQUEST_START               1
// CCA_MODE
#define CCA_MODE_0                      0
#define CCA_MODE_1                      1
#define CCA_MODE_2                      2
#define CCA_MODE_3                      3

/***************************
        CCA_THRES
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint8_t ccaEdThres  : 4,
    uint8_t ccaCcsThres : 4 // reserved in datasheet
  ))
} MAY_ALIAS RegCcaThres_t;

//Mask for PHY_CCA_THRES register
#define REG_PHY_CCA_THRES_CCA_ED_THRES_MASK 0x0F

/***************************
        RX_CTRL
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint8_t pdtThres : 4,
    uint8_t reserved : 4
  ))
} MAY_ALIAS RegRxCtrl_t;

#define PDT_THRES_ANT_DIV_ENABLED 0x03
#define PDT_THRES_DEFAULT         0x07

/***************************
        PHY_TRX_CTRL_2
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t oqpskDataRate : 2,
    uint8_t reserved      : 5,
    uint8_t rxSafeMode    : 1
  ))
} MAY_ALIAS RegTrxCtrl2_t;

// Bit numbers
#define RF_SAFE_MODE          7
#define OQPSK_DATA_RATE_1     1
#define OQPSK_DATA_RATE_0     0
// RX_SAFE_MODE
#define RF_SAFE_MODE_DISABLE  0
#define RF_SAFE_MODE_ENABLE   1
// OQPSK_DATA_RATE
#define OQPSK_DATA_RATE_250K  0
#define OQPSK_DATA_RATE_500K  1
#define OQPSK_DATA_RATE_1M    2
#define OQPSK_DATA_RATE_2M    3

/***************************
        ANT_DIV
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(5, (
    uint8_t antCtrl     : 2,
	  uint8_t antExtSwEn  : 1,
    uint8_t antDivEn    : 1,
    uint8_t reserved    : 3,
    uint8_t antSel      : 1
  ))
} MAY_ALIAS RegAntDiv_t;

// Masks for ANT_DIV register
#define ANT_SEL_MASK          (1 << 7)
#define ANT_DIV_ENABLED_MASK  (1 << 3)
// ANT_EXT_SW_EN values of ANT_DIV
#define ANT_EXT_SW_ENABLED   0x04
#define ANT_EXT_SW_DISABLED  0x00
// ANT_CTRL values of ANT_DIV
#define ANT_CTRL_DEFAULT    0x03
#define ANT_CTRL_ANTENNA_0  0x02
#define ANT_CTRL_ANTENNA_1  0x01

/***************************
        IRQ_STATUS
****************************/
// Mask for IRQ_STATUS register
#define BAT_LOW_MASK      (1 << 7) // 0x80 IRQ7
#define TRX_UR_MASK       (1 << 6) // 0x40 IRQ6
#define AMI_MASK          (1 << 5) // 0x20 IRQ5
#define CCA_ED_READY_MASK (1 << 4) // 0x10 IRQ4
#define TRX_END_MASK      (1 << 3) // 0x08 IRQ3
#define RX_START_MASK     (1 << 2) // 0x04 IRQ2
#define PLL_UNLOCK_MASK   (1 << 1) // 0x02 IRQ1
#define PLL_LOCK_MASK     (1 << 0) // 0x01 IRQ0

/***************************
        VREG_CTRL
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(6, (
    uint8_t dvregTrim : 2,
    uint8_t dvddOk    : 1,
    uint8_t dvregExt  : 1,
    uint8_t avregTrim : 2,
    uint8_t avddOk    : 1,
    uint8_t avregExt  : 1
  ))
} MAY_ALIAS RegVregCtrl_t;

// AVREG_EXT
#define AVREG_USE_INTERNAL 0
#define AVREG_USE_EXTERNAL 1
// AVDD_OK
#define AVDD_OK_DISABLE    0
#define AVDD_OK_ENABLE     1
// DVREG_EXT
#define DVREG_USE_INTERNAL 0
#define DVREG_USE_EXTERNAL 1
// DVDD_OK
#define DVDD_OK_DISABLE    0
#define DVDD_OK_ENABLE     1

/***************************
        BATMON
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t batmonVth : 4,
    uint8_t batmonHr  : 1,
    uint8_t batmonOk  : 1,
    uint8_t reserved  : 2
  ))
} MAY_ALIAS RegBatmon_t;

#define BATMON_OK 1

/***************************
        XOSC_CTRL
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint8_t xtalTrim : 4,
    uint8_t xtalMode : 4
  ))
} MAY_ALIAS RegXoscCtrl_t;

// XTAL_MODE
#define XTAL_MODE_SWITCH_OFF    0
#define XTAL_MODE_EXTERNAL_OSC  4
#define XTAL_MODE_INTERNAL_OSC  15
// XTAL_TRIM
#define XTAL_TRIM_0_0PF         0
#define XTAL_TRIM_0_3PF         1
#define XTAL_TRIM_0_6PF         2
#define XTAL_TRIM_0_9PF         3
#define XTAL_TRIM_1_2PF         4
#define XTAL_TRIM_1_5PF         5
#define XTAL_TRIM_1_8PF         6
#define XTAL_TRIM_2_1PF         7
#define XTAL_TRIM_2_4PF         8
#define XTAL_TRIM_2_7PF         9
#define XTAL_TRIM_3_0PF         10
#define XTAL_TRIM_3_3PF         11
#define XTAL_TRIM_3_6PF         12
#define XTAL_TRIM_3_9PF         13
#define XTAL_TRIM_4_2PF         14
#define XTAL_TRIM_4_5PF         15

/***************************
        FTN_CTRL
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t ftnv      : 6, // reserved in datasheet
    uint8_t ftnRound  : 1, // reserved in datasheet
    uint8_t ftnStart  : 1
  ))
} MAY_ALIAS RegFtnCtrl_t;

#define FTN_START 1

/***************************
        XAH_CTRL_0
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3, (
    uint8_t slottedOperation : 1,
    uint8_t maxCsmaRetries   : 3,
    uint8_t maxFrameRetries  : 4
  ))
} RegXahCtrl0_t;

// Mask for XAH_CTRL_0 register
#define REG_XAH_CTRL_0_MAX_FRAME_RETRIES_MASK 0xF0
#define REG_XAH_CTRL_0_SLOTTED_OPERATION_MASK 0x01
#define REG_XAH_CTRL_0_MAX_CSMA_RETRIES_MASK  0x0E

/***************************
        CSMA_SEED_1
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(5, (
    uint8_t csmaSeed1 : 3,
    uint8_t iAmCoord  : 1,
    uint8_t aackDisAck  : 1,
    uint8_t aackSetPd   : 1,
    uint8_t aackFvnMode : 2
  ))
} MAY_ALIAS RegCsmaSeed1_t;

// Masks for CSMA_SEED_1 register
#define REG_CSMA_SEED_1_AACK_FVN_MODE_MASK 0xC0
#define REG_CSMA_SEED_1_AACK_SET_PD_MASK   0x20
#define REG_CSMA_SEED_1_AACK_DIS_ACK_MASK  0x10
#define REG_CSMA_SEED_1_I_AM_COORD_MASK    0x08
#define REG_CSMA_SEED_1_CSMA_SEED1_MASK    0x07
// AACK_FVN_MODE
#define AACK_FVN_MODE_ACK_0                0
#define AACK_FVN_MODE_ACK_0_1              1
#define AACK_FVN_MODE_ACK_0_1_2            2
#define AACK_FVN_MODE_ACK_ANY              3

/***************************
        CSMA_BE
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(2, (
    uint8_t minBe : 4,
    uint8_t maxBe : 4
  ))
} MAY_ALIAS RegCsmaBe_t;

// Masks for CSMA_BE register
#define REG_CSMA_BE_MAX_BE_MASK 0xF0
#define REG_CSMA_BE_MIN_BE_MASK 0x0F

/***************************
        AESCON
****************************/
typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t reserved  : 3,
    uint8_t aesd      : 1,
    uint8_t aesmode   : 3,
    uint8_t aesr      : 1
  ))
} MAY_ALIAS RegAescon_t;

typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(4, (
    uint8_t reserved  : 3,
    uint8_t aesd      : 1,
    uint8_t aesmode   : 3,
    uint8_t aesr      : 1
  ))
} MAY_ALIAS RegAesconShadow_t;

// Security mode control
#define AESR      7
#define AESMODE2  6
#define AESMODE1  5
#define AESMODE0  4
#define AESD      3
// AES_REQUEST
#define START_SECURITY_MODULE 1
// AES_MODE
#define ECB_MODE              0
#define KEY_MODE              1
#define CBC_MODE              2
// AES_DIR
#define AES_ENCRYPTION        0
#define AES_DECRYPTION        1

END_PACK
#endif /* AT86RF231_H_ */

// eof at86rf231.h
