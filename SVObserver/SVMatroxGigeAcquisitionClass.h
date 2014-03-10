//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxGigeAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Mar 2014 18:20:08  $
//******************************************************************************

#ifndef SVMATROXGIGEACQUISITIONCLASS_H
#define SVMATROXGIGEACQUISITIONCLASS_H

#pragma region Includes
#include "SVAcquisitionClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMatroxGigeCameraProxy.h"
#pragma endregion Includes

#pragma region Declarations
struct SVGigeCameraFileInfoStruct;
#pragma endregion Declarations

class SVMatroxGigeAcquisitionClass : public SVAcquisitionClass
{
	friend class SVMatroxGigeCameraProxy;

public:
	SVMatroxGigeAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVMatroxGigeAcquisitionClass();

	virtual bool IsValid() const;
	virtual bool IsStarted() const;

	virtual HRESULT Create( unsigned long ulSize = 15 );
	virtual HRESULT Destroy();

	virtual HRESULT GetFileNameArraySize( long &rlSize ) const;
	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray );
	virtual HRESULT ReadCameraFile( const CString& sFile, SVDeviceParamCollection& rParams );

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );

	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const;

	virtual HRESULT CreateLut( const SVLutInfo& info );
	virtual HRESULT DestroyLut();

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const;

	virtual bool IsValidBoard() const;

	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual void ClearDeviceIdentifier();
	virtual HRESULT SingleGrab( SVSmartHandlePointer p_SingleGrabHandle );

protected:
	bool CameraMatchesCameraFile();

	HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam );

	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline() const;
	
	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
	virtual HRESULT GetLutImpl( SVLut& lut );
	virtual HRESULT SetLutImpl( const SVLut& lut );

	virtual HRESULT StartDigitizer();

	virtual HRESULT GetCameraImageInfo(SVImageInfoClass *pImageInfo);
	HRESULT DestroyLocal();

	long mlLastIndex;
	long mlHandleIndexCount;

	enum
	{
		LR_BRIGHTNESS = 0,
		LR_CONTRAST   = 1
	};

private:
	bool mbIsCamFilesLoaded;
	bool mbIsStarted;

	enum
	{
		DEFAULT_CONTRAST          = 10000,    // this is the correct default value, not 100
		DEFAULT_CONTRAST_RGB_MONO = 8100,
		DEFAULT_BRIGHTNESS        = 0
	};

	// Callbacks
	static HRESULT CALLBACK StartTrashCallback( void *pvOwner, void *pvDevice );
	static HRESULT CALLBACK StopTrashCallback( void *pvOwner, void *pvDevice );
	static HRESULT CALLBACK StartAcquireCallback( void *pvOwner, void *pvDevice );
	static HRESULT CALLBACK StopAcquireCallback( void *pvOwner, void *pvDevice );
	static HRESULT CALLBACK AcquisitionCallbackProc( void *pvOwner, void *pvCaller, void *pvRequest );

	SVMatroxGigeCameraProxy m_cameraProxy;

	HRESULT SetGigeFeatureOverrides(const SVString& featureOverrides);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMatroxGigeAcquisitionClass.h_v  $
 * 
 *    Rev 1.2   07 Mar 2014 18:20:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Removed methods that did not change base class functionality.
 *   Reordered methods to be consistent with base class.
 *   Moved mbIsBufferCreated to SVAcquisitionClass.
 *   Removed unused value CORECO_SCALE_VALUE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Jul 2013 09:11:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Jun 2013 09:40:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  840
 * SCR Title:  Fix the GigE camera to camera file validation
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added override Is ValidCameraFileParameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:33:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jun 2012 16:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Sep 2011 16:06:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:33:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Jul 2010 16:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jun 2010 08:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 12:37:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2009 19:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 12:04:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:50:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
