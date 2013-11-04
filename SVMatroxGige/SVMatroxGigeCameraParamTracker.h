//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamTracker
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamTracker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2013 15:36:04  $
//******************************************************************************

#ifndef SVMATROXGIGECAMERAPARAMTRACKER_H
#define SVMATROXGIGECAMERAPARAMTRACKER_H

#include <map>
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxGigeCameraParam.h"
#include "SVMatroxGigeCameraParamList.h"

class SVMatroxGigeDigitizer;

class SVMatroxGigeCameraParamTracker
{
// List of Camera Parameters by cameraID (serialNo)
typedef std::map<SVString, SVMatroxGigeCameraParamList> SVMatroxGigeCameraParamCollection;

private:
	SVMatroxGigeCameraParamCollection m_cameraParamCollection;
	
public:
	void Clear();

	void TrackCameraParams(const SVString& cameraID);
	void UnTrackCameraParams(const SVString& cameraID);
	void SetMainListLocked(const SVString& cameraID, bool bLock=true);

	HRESULT AddCameraParam(const SVString& cameraID, const SVMatroxGigeCameraParam& param);
	HRESULT ReloadCameraParams(const SVString& cameraID, SVMatroxGigeDigitizer& p_rCamera);
	
private:	
	HRESULT ReloadCameraParamList(const SVMatroxGigeCameraParamDeque& list, SVMatroxGigeDigitizer& p_rCamera);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeCameraParamTracker.h_v  $
 * 
 *    Rev 1.1   22 Apr 2013 15:36:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed "Description" label from header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:42:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jun 2012 16:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/