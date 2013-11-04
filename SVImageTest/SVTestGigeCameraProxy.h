//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeCameraProxy.h
//* .File Name       : $Workfile:   SVTestGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2013 19:38:20  $
//******************************************************************************

#ifndef SVTESTGIGECAMERAPROXY_H
#define SVTESTGIGECAMERAPROXY_H

#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"

class SVDigitizerLoadLibraryClass;
class SVTestAcquisitionClass;

class SVTestGigeCameraProxy : public SVGigeCameraProxy
{
private:
	SVTestAcquisitionClass* m_pAcquisition;

	//
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVTestGigeCameraProxy();
	virtual ~SVTestGigeCameraProxy();

	void SetOwner(SVTestAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SetGigeFeatureOverrides(const SVString& xmlData, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVImageTest\SVTestGigeCameraProxy.h_v  $
 * 
 *    Rev 1.1   15 May 2013 19:38:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 May 2013 10:25:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for binning
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 11:13:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2010 08:07:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 18:17:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
