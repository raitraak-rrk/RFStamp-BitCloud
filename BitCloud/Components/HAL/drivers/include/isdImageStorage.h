/**************************************************************************//**
\file   isdImageStorage.h

\brief  The public API of the image storage driver.

The image storage driver (ISD) uses commands equivalent to the OTAU cluster's commands
sent from the OTAU client to the OTAU server while transferring an image.
In fact, the OTAU server uses ISD only to pass OTAU client's commands to the image
storage system.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    25.05.11 A. Khromykh - Created
*******************************************************************************/

#ifndef _ISDIMAGESTORAGE_H
#define _ISDIMAGESTORAGE_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <zclOTAUCluster.h>
#if APP_INTERFACE == APP_INTERFACE_USBCDC
#include <usb.h>
#else
#include <usart.h>
#endif
/******************************************************************************
                   Define(s) section
******************************************************************************/

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Statuses returned by the driver */
typedef enum
{
  ISD_NO_COMMUNICATION,
  ISD_COMMUNICATION_ESTABLISHED,
  ISD_IDLE,
  ISD_BUSY,
  ISD_HARDWARE_FAULT
} ISD_Status_t;

typedef void (* IsdOpenCb_t)(ISD_Status_t);
typedef void (* IsdQueryNextImageCb_t)(ZCL_OtauQueryNextImageResp_t *);
typedef void (* IsdImageBlockCb_t)(ZCL_OtauImageBlockResp_t *);
typedef void (* IsdUpgradeEndCb_t)(ZCL_OtauUpgradeEndResp_t *);

#if APP_INTERFACE == APP_INTERFACE_USBCDC
typedef HAL_UsbHWDescriptor_t ISD_TunnelDescriptor_t;
#else
typedef  HAL_UsartDescriptor_t ISD_TunnelDescriptor_t;
#endif

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens the serial interface for connection with the image storage

The serial interface captured by executing this function may not be used
until OFD_Close() is called.

\param[in] cb - pointer to a callback function called once the interface is opened;
                can be set to NULL
******************************************************************************/
void ISD_Open(IsdOpenCb_t cb);

/**************************************************************************//**
\brief Closes the serial interface opened using ISD_Open()
******************************************************************************/
void ISD_Close(void);

/**************************************************************************//**
\brief Sends a query next image request to the storage system, which should decide
whether the request's issuer needs a new image

The response to this command received from the storage system should be somehow
delivered to the client. If the storage system decides that the client needs a new
image the client may then start sending image block requests to retrieve individual
blocks of the new image.

Whether the client device should download a new image is indicated by the \c status
field of the callback's argument (which is of ZCL_OtauQueryNextImageResp_t type)
having the ::ZCL_SUCCESS_STATUS value.

\param[in]
  addressing - pointer to structure that contains network information
  about the client including network address, endpoint, cluster ID and profile ID
\param[in]
  data - data payload containing information about client's firmware
\param[in]
  cb - pointer to a callback function called when the response is received
  from the storage system; can be set to NULL
******************************************************************************/
void ISD_QueryNextImageReq(ZCL_Addressing_t *addressing, ZCL_OtauQueryNextImageReq_t *data, IsdQueryNextImageCb_t cb);

/**************************************************************************//**
\brief Sends an image block request to the storage system, in order to retrieve
a certain block of data of the specified image

The function requests an individual block (specified by the offset inside the image
and having the OFD_BLOCK_SIZE length) of the specific image from the storage system.
The image is specified by its image type, manufacturer ID and firmware version.

The storage system replies with the response comman containing the requested data
(if all information is correct). Response reception is indicated by the callback
function.

\param[in]
  addressing - pointer to the structure that contains network information
  about the client including network address, endpoint, cluster ID and profile ID
\param[in]
  data - data payload containing information about the requested block of data and
  the image
\param[in]
  cb - pointer to a callback function called when the response is received
  from the storage system; can be set to NULL
******************************************************************************/
void ISD_ImageBlockReq(ZCL_Addressing_t *addressing, ZCL_OtauImageBlockReq_t *data, IsdImageBlockCb_t cb);

/**************************************************************************//**
\brief Sends an upgrade end request to the storage system, notifying it that
all blocks have been received

The function notifies the storage system that the client has received the whole
image. The storage system replies with the update end response that specifies the
time after which the client should switch to the new image.

\param[in]
  addressing - pointer to the structure that contains network information
  about the client including network address, endpoint, cluster ID and profile ID
\param[in]
  data - data payload containg information about the image that has been downloaded
  to the client
\param[in]
  cb - pointer to a callback function called when the response is received
  from the storage system; can be set to NULL
******************************************************************************/
void ISD_UpgradeEndReq(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndReq_t *data, IsdUpgradeEndCb_t cb);

/**************************************************************************//**
\brief Initialize ISD tunnel.

\param[in] descriptor - pointer to USART descriptor

\return zero value if tunnel was initialized, false otherwise.
******************************************************************************/
int ISD_OpenTunnel(ISD_TunnelDescriptor_t *descriptor);

/**************************************************************************//**
\brief Close ISD tunnel.

\param[in] descriptor - pointer to USART descriptor

\return always zero. (to be correspond public API).
******************************************************************************/
int ISD_CloseTunnel(ISD_TunnelDescriptor_t *descriptor);

/**************************************************************************//**
\brief Read contents from ISD tunnel to buffer.

\param[in] descriptor - pointer to USART descriptor
\param[in] buffer - pointer to destination buffer
\param[in] length - buffer length

\return number of read bytes or -1 if error
******************************************************************************/
int ISD_ReadTunnel(ISD_TunnelDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

/**************************************************************************//**
\brief Send contents of buffer over USART through ISD tunnel.

\param[in] descriptor - pointer to USART descriptor
\param[in] buffer - buffer to be sent
\param[in] length - buffer length

\return number of sent bytes or -1 if error
******************************************************************************/
int ISD_WriteTunnel(ISD_TunnelDescriptor_t *descriptor, uint8_t *buffer, uint16_t length);

#endif /* _ISDIMAGESTORAGE_H */
