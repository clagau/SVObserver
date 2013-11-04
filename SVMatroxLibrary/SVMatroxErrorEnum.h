//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxErrorEnum
//* .File Name       : $Workfile:   SVMatroxErrorEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:12:08  $
//******************************************************************************

#ifndef SVMATROXERRORENUM_H
#define SVMATROXERRORENUM_H

/**
@SVObjectName Matrox Error Enum

@SVObjectOverview This enum is used to hold basic errors for the Matrox interface.

@SVObjectOperations None

*/
enum SVMatroxErrorEnum
{
	SVMEE_STATUS_OK					= 0x00000000,
	SVMEE_INVALID_HANDLE			= 0xcf000001,
	SVMEE_INVALID_PARAMETER			= 0xcf000002,
	SVMEE_WRONG_PARAMETER			= 0xcf000003,
	SVMEE_STRING_TOO_LARGE			= 0xcf000004,
	SVMEE_BAD_POINTER				= 0xcf000005,
	SVMEE_INVALID_STATE				= 0xcf000006,
	SVMEE_RESULT_ARRAY_TOO_SMALL	= 0xcf000007,
	SVMEE_MATROX_THREW_EXCEPTION    = 0xcf000008,
	SVMEE_INVALID_LOCK              = 0xcf000009,

	SVMEE_MATROX_ERROR				= 0xcf000000,

	SVMEE_INTERNAL_CONVERSION_ERROR	= 0xcf000bad,
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxErrorEnum.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Sep 2011 14:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:34:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/