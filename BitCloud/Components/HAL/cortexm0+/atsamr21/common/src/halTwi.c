/**************************************************************************//**
  \file  halTwi.c

  \brief Implementation of Two wire interface interface module.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     28/2/14 Viswanadham Kotla - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_TWI)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <i2cPacket.h>
#include <halInterrupt.h>
#include <atomic.h>

/*****************************************************************************/
/* Extern */
/******************************************************************************/
extern HAL_I2cDescriptor_t *halI2cDesc;

/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void halSetI2cConfiguration(void);
static void halI2cMasterWaitForSync(void);
static void halSendStopI2c(void);
static void halI2cBusError(void);

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Set configuraton for TWI module.

\param[in] TWI descriptor
  
******************************************************************************/
static void halSetI2cConfiguration(void)
{   
  uint8_t sercomNo;
  /* Configure Port Pins for I2C */
  GPIO_pinfunc_config(&halI2cDesc->tty->i2cPinConfig[I2C_SDA_SIG]);
  GPIO_pinfunc_config(&halI2cDesc->tty->i2cPinConfig[I2C_SCK_SIG]);

  sercomNo = ((uint32_t)halI2cDesc->tty->sercom - (uint32_t)&SC0_I2C_M_CTRLA) / 0x400;

  /* clk settings */
  GCLK_CLKCTRL_s.id = 0x14 + sercomNo;
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  /* enable the clock of I2C */
  PM_APBCMASK |= (1 << (2 + sercomNo));
}

/**************************************************************************//**
\brief Inits TWI module. Setup the speed of TWI.

\param[in]
  rate - the speed of TWI.
******************************************************************************/
void halInitI2c(I2cClockRate_t rate)
{  
  halSetI2cConfiguration();
  uint32_t timeout_counter = 0;

  halI2cDesc->tty->sercom->CTRLA.bit.RUNSTDBY = 0x00; //enable in all sleep modes
  halI2cDesc->tty->sercom->CTRLA.bit.PINOUT = 0x00; //4wire disabled.
  halI2cDesc->tty->sercom->CTRLA.bit.SDAHOLD = 0x00; //hold time disabled
  halI2cDesc->tty->sercom->CTRLA.reg |= I2C_MASTER_START_HOLD_TIME_300NS_600NS;

  halI2cMasterWaitForSync();
  /* Set configurations in CTRLB. */
  halI2cDesc->tty->sercom->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
  /*Baud Rate*/
  halI2cDesc->tty->sercom->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(rate);
  /* Debugging purpose */
  //halI2cDesc->tty->sercom->DBGCTRL.reg = 0x01;

  halI2cDesc->tty->sercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_MODE_I2C_MASTER; //master mode

  halI2cMasterWaitForSync();
  halI2cDesc->tty->sercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;

  /* make sure teh bus state is Idle */
  while (!(halI2cDesc->tty->sercom->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1)))
  {
    timeout_counter++;
    if(timeout_counter >= RESPONSE_TIMEOUT)
    {
      /* Timeout, force bus state to idle. */
      halI2cDesc->tty->sercom->STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);
      /* Workaround */
      return;
    }
  }
}

/******************************************************************
\brief wait for I2c module to sync
********************************************************************/
static void halI2cMasterWaitForSync(void)
{
  while (halI2cDesc->tty->sercom->SYNCBUSY.reg & SC1_I2C_S_SYNCBUSY);
}

/******************************************************************
\brief send STOP command
********************************************************************/
static void halSendStopI2c(void)
{
  halI2cMasterWaitForSync();
  halI2cDesc->tty->sercom->CTRLB.reg |= SC1_I2C_M_CTRLB_CMD(3); //stop cmd
}

/******************************************************************
\brief wait for bus access
********************************************************************/
static void halWaitforBusAccess(void)
{
  while (!(halI2cDesc->tty->sercom->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&
               !(halI2cDesc->tty->sercom->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
}

/******************************************************************************
\brief The Handler of NACK
******************************************************************************/
static void halI2cBusError(void)
{
  halI2cDesc->tty->sercom->INTENCLR.reg = (uint8_t)ALL_PERIPHERIAL_INTERRUPT_DISABLE;
  halI2cDesc->service.state = I2C_PACKET_TRANSAC_FAIL;
  halPostTask(HAL_TASK_TWI);
}

/******************************************************************
\brief address response from slave
********************************************************************/
static int8_t halI2cAddressResponse(void)
{
  uint8_t intFlags = halI2cDesc->tty->sercom->INTFLAG.reg;
  uint8_t statusFlgs;
  int8_t status = 0;
  
  /* check the interrupt flag */ 
  if (intFlags & SERCOM_I2CM_INTFLAG_SB)
  {
    /* Clear write interrupt flag */
    halI2cDesc->tty->sercom->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;

    statusFlgs =  halI2cDesc->tty->sercom->STATUS.reg;
    /* check for buss error or arbitration loss */
    if (( statusFlgs & SC1_I2C_M_STATUS_ARBLOST) || (status & SC1_I2C_M_STATUS_BUSERR))
    {
      status = -1;
      halI2cBusError();
    }
    else if (statusFlgs & SC1_I2C_M_STATUS_RXNACK) //No ack from slave
    {
      status = -1;
      halSendStopI2c();
    }
  }
  return status;
}

/******************************************************************************
\brief start write transaction
******************************************************************************/
void halSendStartWriteI2c(void)
{
  HalI2cPacketService_t *service = &halI2cDesc->service;
  halI2cMasterWaitForSync();

  halI2cDesc->tty->sercom->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

  /* Setting address and direction bit. Will send start command on bus. */
  halI2cDesc->tty->sercom->ADDR.reg = (halI2cDesc->internalAddr << 1) | I2C_WRITE;   

  /* wait for the bus to be free */
  halWaitforBusAccess();

  /* response received successfully */
  if(!halI2cAddressResponse())
  {
     service->state = I2C_PACKET_WRITE_DATA;
     while (service->index < halI2cDesc->length)
     {
       /* Write the data */
       halI2cMasterWaitForSync();
       halI2cDesc->tty->sercom->DATA.reg = halI2cDesc->data[service->index++];

       /* Wait for the bus acccess */
       halWaitforBusAccess();

       /* Check NACK is received from Slave */
       if (!(halI2cDesc->tty->sercom->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK))
        continue;
       else
         break;
     }
  }
  service->state = I2C_PACKET_IDLE;
}

/******************************************************************************
\brief start read transaction
******************************************************************************/
void halSendStartReadI2c(void)
{
  HalI2cPacketService_t *service = &halI2cDesc->service;
  bool sclsm = halI2cDesc->tty->sercom->CTRLA.bit.SCLSM;
  uint16_t count = halI2cDesc->length;

  halI2cDesc->tty->sercom->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
  halI2cDesc->tty->sercom->ADDR.reg  = (halI2cDesc->internalAddr << 1)| I2C_READ;

  /* Wait for the bus access */
  halWaitforBusAccess();

  halI2cDesc->tty->sercom->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

  if (!halI2cAddressResponse())
  {
    if (!(halI2cDesc->tty->sercom->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) 
      return;

     service->state = I2C_PACKET_READ_DATA;
     while (count--)
     {
       if (((!sclsm && !count)) || (sclsm && (count == 1)))
       {
         halI2cDesc->tty->sercom->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
       }
       else
       {
          /* Write for the sync */
          halI2cMasterWaitForSync();
          halI2cDesc->data[service->index++] = halI2cDesc->tty->sercom->DATA.reg;

          /* Wait for the bus access */
          halWaitforBusAccess();
       }
     }
     /* Write for the sync */
     halI2cMasterWaitForSync();
     halSendStopI2c();

     /* Write for the sync */
     halI2cMasterWaitForSync();
     halI2cDesc->data[service->index] = halI2cDesc->tty->sercom->DATA.reg;
     service->state = I2C_PACKET_TRANSAC_SUCCESS;
  }
  service->state = I2C_PACKET_IDLE;
}

#endif //HAL_USE_TWI
// eof halPwm.c
