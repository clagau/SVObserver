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

#ifndef SVIOPARAMETERENUM_H
#define SVIOPARAMETERENUM_H

// Parameter Enum
enum SVIOParameterEnum
{
	SVSignalEdge = 0,
	SVSingleTriggerEdge = 1,
	SVBoardVersion = 100,
	SVFanState=101,
	SVBoardType=102,
	SVFanFreq=103,
	SVLogDump=104,
	SVRabbitRTC=105,
	SVRabbitWriteLog=106,
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

#endif // SVIOPARAMETERENUM_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOParameterEnum.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:47:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Oct 2009 11:34:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  666
 * SCR Title:  Change command format for  X-Series IO
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Enums to support new functionality in TVicLpt Interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 May 2009 14:44:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  663
 * SCR Title:  Add support to SVTVicLpt dll to lock and unlock outputs.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added LOCK_CMD and READ_LOCK_STATE to sub command enumeration for parallel board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Jan 2007 08:28:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  590
 * SCR Title:  Modify SVObserver to send the IO Board type to the parallel board
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new sub command to set system type in IO Board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2006 09:07:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new types for parameter enums.
 * Added Sub Commands enum for Parallel board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Mar 2006 14:58:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initialized all enum values to be safe.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2006 10:34:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/