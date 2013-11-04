//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDatabase
//* .File Name       : $Workfile:   SVGigeCameraDatabase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Jun 2013 15:13:20  $
//******************************************************************************
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include "SVGigeCameraDatabase.h"

// Enum to Diplay Name String mapping
const SVGigeCameraDatabase::SVGigeEnumToString SVGigeCameraDatabase::m_enumToString = boost::assign::map_list_of<>
( SVGigeParameterExposureMode,				L"Trigger Mode" )
( SVGigeParameterBayerPattern,				L"Bayer Pattern" )
( SVGigeParameterXOffset,					L"X Offset" )
( SVGigeParameterYOffset,					L"Y Offset" )
( SVGigeParameterXSize,						L"X Size" )
( SVGigeParameterYSize,						L"Y Size" )
( SVGigeParameterColorFormat,				L"Color Format" )
( SVGigeParameterFeatureBrightness,			L"Brightness" )
( SVGigeParameterFeatureAutoExposure,		L"Auto Exposure" )
( SVGigeParameterFeatureSharpness,			L"Sharpness" )
( SVGigeParameterFeatureWhiteBalanceU,		L"White Balance - U" )
( SVGigeParameterFeatureWhiteBalanceV,		L"White Balance - V" )
( SVGigeParameterFeatureHue,				L"Hue" )
( SVGigeParameterFeatureSaturation,			L"Saturation" )
( SVGigeParameterFeatureGamma,				L"Gamma" )
( SVGigeParameterFeatureShutter,			L"Shutter" )
( SVGigeParameterFeatureGain,				L"Gain" )
( SVGigeParameterTriggerSource,				L"Trigger Source" )
( SVGigeParameterTriggerEdge,				L"Trigger Edge" )
( SVGigeParameterTriggerDelay,				L"Trigger Delay" )
( SVGigeParameterTriggerInvert,				L"Trigger Invert" )
( SVGigeParameterTriggerEnable,				L"Trigger Enable" )
( SVGigeParameterStrobeSource,				L"Strobe Source" )
( SVGigeParameterStrobeEdge,				L"Strobe Edge" )
( SVGigeParameterStrobeInvert,				L"Strobe Invert" )
( SVGigeParameterStrobeDelay,				L"Strobe Delay" )
( SVGigeParameterStrobeDuration,			L"Strobe Duration" )
( SVGigeParameterStrobeEnable,				L"Strobe Enable" )
( SVGigeParameterPacketSize,				L"Packet Size" )
( SVGigeParameterLutEnable,					L"LUT Enable" )
( SVGigeParameterLutArray,					L"LUT" )
( SVGigeParameterVendorName,				L"Vendor Name" )
( SVGigeParameterModelName,					L"Model Name" )
( SVGigeParameterSerialNumber,				L"Serial Number" )
( SVGigeParameterIPAddress,					L"IP Address" )
( SVGigeParameterHorizontalBinning,			L"Horizontal Binning" )
( SVGigeParameterVerticalBinning,			L"Vertical Binning" )
;

SVGigeCameraDatabase::SVGigeCameraDatabase()
{
}

long SVGigeCameraDatabase::GetFeatureCount()
{
	long count = m_enumToString.size();
	return count;
}

HRESULT SVGigeCameraDatabase::GetFeatureNames(SVGigeEnumNameList& list)
{
	list.clear();

	for (SVGigeEnumToString::const_iterator it = m_enumToString.begin();it != m_enumToString.end();++it)
	{
		list.push_back(it->second);
	}
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMatroxGige\SVGigeCameraDatabase.cpp_v  $
 * 
 *    Rev 1.2   12 Jun 2013 15:13:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 May 2013 10:15:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new parameter for SVGigeParameterIPAddress
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2013 19:40:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 May 2013 10:13:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVGigeParameterHorizontalBinning and SVGigeParameterVerticalBinning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:36:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jun 2010 08:01:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Feb 2010 10:19:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  688
 * SCR Title:  Add support for the SVS-Vistek eco204MVGE_r2 camera
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added an entry for the SVS-Vistek eco204MVGE_r2 camera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jan 2010 14:58:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  683
 * SCR Title:  Update parameter support for SVS-Vistek eco204MVGE Gige camera
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added camera database incoformation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jan 2010 14:56:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  683
 * SCR Title:  Update parameter support for SVS-Vistek eco204MVGE Gige camera
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera database to include new information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Dec 2009 12:52:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  678
 * SCR Title:  Add Support for SVS-Vistek SVS11002MSGEV2 Gige camera
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Entry for the SVS-Vistek SVS11002MSGEV2 camera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
