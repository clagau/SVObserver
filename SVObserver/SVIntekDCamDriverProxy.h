// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekDCamDriverProxy.h
// * .File Name       : $Workfile:   SVIntekDCamDriverProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:05:50  $
// ******************************************************************************

#ifndef INCL_SVINTEKDCAMDRIVERPROXY_H
#define INCL_SVINTEKDCAMDRIVERPROXY_H

#include "SV1394CameraFileLibrary/SV1394CameraFileReader.h"
#include "SV1394CameraFileLibrary/SVDCamDriverProxy.h"

class SVDCamDriver;
class SVDigitizerLoadLibraryClass;
class SVIntekAcquisitionClass;

class SVIntekDCamDriverProxy : public SVDCamDriverProxy
{
private:
	SVIntekAcquisitionClass* m_pAcquisition;

public:
	SVIntekDCamDriverProxy();
	virtual ~SVIntekDCamDriverProxy();
	void SetOwner(SVIntekAcquisitionClass* pAcquisition);
	void Init(SVDCamDriver* pCameraDriver);
	bool IsOnline() const;

//	HRESULT LoadFiles(SVFileNameArrayClass& rFiles, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
//	HRESULT ReadCameraFile( const CString& sFilename, SVDeviceParamCollection& rParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
		
	
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

//	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
//	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
//	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIntekDCamDriverProxy.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:05:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 11:08:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:30:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:36:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
