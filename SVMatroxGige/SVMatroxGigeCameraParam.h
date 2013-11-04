//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParam
//* .File Name       : $Workfile:   SVMatroxGigeCameraParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:48  $
//******************************************************************************

#ifndef SVMATROXGIGECAMERAPARAM_H
#define SVMATROXGIGECAMERAPARAM_H

#include <deque>
#include <comdef.h>

struct SVMatroxGigeCameraParam
{
	int m_ParameterID;
	int m_ParameterTypeID;
	_variant_t m_value;
	
	SVMatroxGigeCameraParam(int p_ParameterID, int p_ParameterTypeID, _variant_t& p_rValue);
	SVMatroxGigeCameraParam(const SVMatroxGigeCameraParam& rParam);
	~SVMatroxGigeCameraParam();
	
	SVMatroxGigeCameraParam& operator=(const SVMatroxGigeCameraParam& rParam);
	void Copy(const SVMatroxGigeCameraParam& rParam);
};

// List of Camera Parameters (SetParameters)
typedef std::deque<SVMatroxGigeCameraParam> SVMatroxGigeCameraParamDeque;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeCameraParam.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:41:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2012 16:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
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
