/******************************************************************************
  \file at86rf2xx.h

  \brief Declarations for Atmel AT86RF2XX. Common definitions are presented.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      12/09/07 ALuzhetsky, A. Mandychev - Created
      30/02/12 A. Razinkov - Refactored
******************************************************************************/

#ifndef AT86RF2XX_H_
#define AT86RF2XX_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <sysEndian.h>

#ifdef AT86RF230
#  include <at86rf230.h>
#endif // AT86RF230
#ifdef AT86RF230B
#  include <at86rf230b.h>
#endif // AT86RF230B

#ifdef AT86RF230B
#  include <at86rf230b.h>
#endif // AT86RF230B

#ifdef AT86RF231
#  include <at86rf231.h>
#endif // AT86RF231

#ifdef AT86RF233
#  include <at86rf233.h>
#endif // AT86RF233

#ifdef AT86RF212
#  include <at86rf212.h>
#endif // AT86RF212

#endif /* AT86RF2XX_H_ */

// eof at86rf2xx.h
