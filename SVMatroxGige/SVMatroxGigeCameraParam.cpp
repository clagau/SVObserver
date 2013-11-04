//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParam
//* .File Name       : $Workfile:   SVMatroxGigeCameraParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGigeCameraParam.h"

SVMatroxGigeCameraParam::SVMatroxGigeCameraParam(int p_ParameterID, int p_ParameterTypeID, _variant_t& p_rValue)
: m_ParameterID(p_ParameterID), m_ParameterTypeID(p_ParameterTypeID), m_value(p_rValue)
{
}
SVMatroxGigeCameraParam::SVMatroxGigeCameraParam(const SVMatroxGigeCameraParam& rParam)
: m_ParameterID(rParam.m_ParameterID), m_ParameterTypeID(rParam.m_ParameterTypeID), m_value(rParam.m_value)
{ 
}

SVMatroxGigeCameraParam::~SVMatroxGigeCameraParam()
{
}

SVMatroxGigeCameraParam& SVMatroxGigeCameraParam::operator=(const SVMatroxGigeCameraParam& rParam)
{
	Copy(rParam);
	return *this;
}

void SVMatroxGigeCameraParam::Copy(const SVMatroxGigeCameraParam& rParam)
{
	m_ParameterID = rParam.m_ParameterID;
	m_ParameterTypeID = rParam.m_ParameterTypeID;
	m_value = rParam.m_value;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeCameraParam.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:41:42   bWalter
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
