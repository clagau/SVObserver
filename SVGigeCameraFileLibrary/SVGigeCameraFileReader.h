//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraFileReader
//* .File Name       : $Workfile:   SVGigeCameraFileReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:42:02  $
//******************************************************************************

#ifndef SVGIGECAMERAFILEREADER_H
#define SVGIGECAMERAFILEREADER_H

#include <iostream>
#include "SVUtilityLibrary/SVString.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVGigeCameraFileInfoStruct.h"

class SVGigeCameraFileReader
{
public:
	SVGigeCameraFileReader(SVGigeCameraFileInfoStruct& rInfo);
	HRESULT ReadParams( SVDeviceParamCollection& rParams );
	HRESULT ReadGigeFeatureOverrides();

private:
	unsigned short GetChecksum(std::istream& rFile);
	
	HRESULT ReadCustomParams(const SVString& filename, SVDeviceParamCollection& rParams);

	HRESULT ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );

	HRESULT ReadCameraFileCommonParams(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParam* pParam);
//	HRESULT ReadCameraFileOptions(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParamOptionInsertor& rInsertor);
	HRESULT ReadCameraFileLongValueInfo(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVLongValueDeviceParam* pParam);

	SVGigeCameraFileInfoStruct& m_rInfo;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVGigeCameraFileReader.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Apr 2013 15:53:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ReadCustomParams method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2010 08:24:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 16:51:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
