//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDeviceParameterManager
//* .File Name       : $Workfile:   SVMatroxGigeDeviceParameterManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:16  $
//******************************************************************************
#ifndef SVMATROXGIGEDEVICEPARAMETERMANAGER_H
#define SVMATROXGIGEDEVICEPARAMETERMANAGER_H

#include <map>
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"

class SVMatroxGigeDigitizer;

class SVMatroxGigeDeviceParameterManager
{
private:
	static const SVGigeDeviceParameterMap& GetParameterMap();

public:
	static HRESULT GetParameterName(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, BSTR* p_pBstrName);
	static HRESULT GetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue);
	static HRESULT SetParameter(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID, int p_piParameterTypeID, VARIANT *p_pvarValue);
	static HRESULT IsParameterSupported(const SVMatroxGigeDigitizer& p_rCamera, int p_iParameterID);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeDeviceParameterManager.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:41:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Apr 2013 16:01:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetParameterName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jun 2012 16:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2010 13:50:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
