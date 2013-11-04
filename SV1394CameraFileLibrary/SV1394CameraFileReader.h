//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraFileReader
//* .File Name       : $Workfile:   SV1394CameraFileReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:56  $
//******************************************************************************

#ifndef SV1394CAMERAFILEREADER_H
#define SV1394CAMERAFILEREADER_H

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"


struct SV1394CameraFileInfoStruct
{
	// input params
	CString sFilename;
	bool bColorSystem;

	// output params
	CString sVersion;
	CString sCameraType;

	SV1394CameraFileInfoStruct() : bColorSystem(false) {}
};


class SV1394CameraFileReader
{
public:
	SV1394CameraFileReader(SV1394CameraFileInfoStruct& rInfo);
	HRESULT ReadParams( SVDeviceParamCollection& rParams );

protected:

	WORD GetChecksum(CStdioFile& rFile);
	HRESULT ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );

	SV1394CameraFileInfoStruct& m_rInfo;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SV1394CameraFileReader.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:32:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 11:13:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:55:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2006 15:10:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  565
 * SCR Title:  Fix 1394 default greyscale camera mode problem with color camera file
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for mono product and adjust 1394 camera file reader for color camera files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Feb 2004 17:21:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Sep 2003 14:53:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut param
 * fixed checksum check
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Sep 2003 09:46:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
