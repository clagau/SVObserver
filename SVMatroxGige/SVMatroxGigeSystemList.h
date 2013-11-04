//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeSystemList
//* .File Name       : $Workfile:   SVMatroxGigeSystemList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:43:50  $
//******************************************************************************

#ifndef SVMATROXGIGESYSTEMLIST_H
#define SVMATROXGIGESYSTEMLIST_H

#include "SVMatroxGigeSystem.h"
#include "SVMatroxGigeHandleList.h"

namespace
{
	enum
	{
		INVALID_SVMATROXGIGESYSTEM_HANDLE = 0xFF
	};
}

// allow up to 16 Matrox Gige Systems 
typedef SVMatroxGigeHandleList< 16, unsigned char, INVALID_SVMATROXGIGESYSTEM_HANDLE, SVMatroxGigeSystem> SVMatroxGigeSystemList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeSystemList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:43:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

