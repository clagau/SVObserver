//******************************************************************************
/// COPYRIGHT (c) by Hilscher GmbH
/// All Rights Reserved 
/// \file EplCn_If_Public.h
//******************************************************************************	
/**************************************************************************************
Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.
***************************************************************************************
$Id: EplCn_If_Public.h 4596 2020-12-01 15:26:38Z MAzarkan $:

Description:
  Powerlink Controlled Node Interface Task public definitions
**************************************************************************************/

#ifndef __EPLCN_IF_PUBLIC_H
#define __EPLCN_IF_PUBLIC_H

#include "TLR_Types.h"
#include "EplCn_PacketAPI.h"

//This is to avoid CheckInclude false positives TLR_INT8
//This is to avoid CheckInclude false positives MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_USE_CUSTOM_PDO_OBJ

/*************************************************************************************/
/* Queue name                                                                        */
/*************************************************************************************/
#define EPLCN_IF_QUEUE_NAME                     "QUE_EPLCN_IF"

/*************************************************************************************/
/* Commands                                                                          */
/*************************************************************************************/
/* Start of packet range. Used to filter commands to be handled by the IF task */
#define EPLCN_IF_COMMAND_RANGE_START                                  0xA200

/* All commands and packet definitions were moved to new file EplCn_PacketAPI.h */

#endif /* __EPLCN_IF_PUBLIC_H */
/*************************************************************************************/
/*-----------------------------------------------------------------------------------*/
/*------------------- EOF -----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/*************************************************************************************/
