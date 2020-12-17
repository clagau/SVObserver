// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOParameterEnum
// * .File Name       : $Workfile:   SVIOParameterEnum.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:47:02  $
// ******************************************************************************

#pragma once

// Parameter Enum
enum SVIOParameterEnum
{
	SVSignalEdge = 0,
	SVSingleTriggerEdge = 1,
	SVBoardName = 2,
	SVModuleReady = 3,
	SVBoardVersion = 100,
	SVFanState=101,
	SVBoardType=102,
	SVFanFreq=103,
	SVLogDump=104,
	SVRabbitRTC=105,
	SVRabbitWriteLog=106,
	PlcSimulatedTrigger=200
};

// Sub Commands for Parallel Board.
enum
{
	NO_CMD = 0,
	UNLOCK_CMD = 0x55,
	GET_BOARD_VERSION = 0x56,
	READ_FANS = 0X57,
	// SYSTEM_X_SERIES = 0x58,
	// SYSTEM_D_SERIES = 0x59,
	// SYSTEM_A_SERIES = 0x5A,
	GET_SYSTEM_TYPE = 0x5B,
	SET_TRIGGER_EDGE= 0x5C,
	SET_SYSTEM_TYPE = 0x5D,
	LOCK_CMD = 0x5E,
	READ_LOCK_STATE = 0X5F,
	READ_FAN_FREQ = 0X60,
	READ_LOG = 0x61,
	WRITE_RTC = 0X62,
	READ_RTC = 0X63,
	WRITE_LOG = 0x64,
};

