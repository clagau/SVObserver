//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamList
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:58  $
//******************************************************************************
#ifndef SVMATROXGIGECAMERAPARAMLIST_H
#define SVMATROXGIGECAMERAPARAMLIST_H

#include "SVMatroxGigeCameraParam.h"

// There are 2 lists, one main list and one transient
// The transient list is for test mode support of LUT and Light Reference

struct SVMatroxGigeCameraParamList
{
	bool m_bUseTransient;
	SVMatroxGigeCameraParamDeque m_mainList;
	SVMatroxGigeCameraParamDeque m_transientList;

	SVMatroxGigeCameraParamList();
	void clear();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeCameraParamList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:41:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
