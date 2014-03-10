//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIntekAcquisitionClass
//* .File Name       : $Workfile:   SVIntekAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 Mar 2014 18:17:04  $
//******************************************************************************

#ifndef SVINTEKACQUISITIONCLASS_H
#define SVINTEKACQUISITIONCLASS_H

#pragma region Includes
#include "SVAcquisitionClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVIntekDCamAcquisitionProxy.h"
#include "SVIntekDCamDriverProxy.h"
#pragma endregion Includes

#pragma region Declarations
struct SV1394CameraFileInfoStruct;
class SVLongValueDeviceParam;
#pragma endregion Declarations

class SVIntekAcquisitionClass : public SVAcquisitionClass  
{
public:
	SVIntekAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVIntekAcquisitionClass();

	virtual bool IsValid() const;

	virtual HRESULT Destroy();

	virtual HRESULT Start();
	virtual HRESULT Stop();
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

	friend class SVDCamDriver;	// to access these protected functions
	friend class SVIntekDCamDriverProxy;

	bool CameraMatchesCameraFile();

	HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam );

	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline() const;

	static HRESULT ReadCameraFileImpl( /*in,out*/SV1394CameraFileInfoStruct& rInfo, SVDeviceParamCollection& rParams );
	static HRESULT SetParameterDefaults( SVDeviceParamCollection& rParams );

	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
	virtual HRESULT GetCameraImageInfo(SVImageInfoClass *pImageInfo);
	virtual HRESULT GetLutImpl( SVLut& lut );
	virtual HRESULT SetLutImpl( const SVLut& lut );

	virtual HRESULT StartDigitizer();

	HRESULT DestroyLocal();

	enum
	{
		LR_BRIGHTNESS = 0,
		LR_CONTRAST   = 1
	};
	
	SVDCamDriver* m_pCameraDriver;

private:
	bool mbIsCorFilesLoaded;

	enum
	{
		DEFAULT_CONTRAST          = 10000,    // this is the correct default value, not 100
		DEFAULT_CONTRAST_RGB_MONO = 8100,
		DEFAULT_BRIGHTNESS        = 0
	};

	SVIntekDCamAcquisitionProxy m_DCamAcqProxy;
	SVIntekDCamDriverProxy m_cameraDriverProxy;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIntekAcquisitionClass.h_v  $
 * 
 *    Rev 1.1   07 Mar 2014 18:17:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Made methods const.
 *   Removed methods that did not change base class functionality.
 *   Reordered methods to be consistent with base class.
 *   Removed unused value CORECO_SCALE_VALUE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:05:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Sep 2011 16:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Jan 2011 11:20:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   07 Jul 2010 16:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Feb 2010 12:29:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Oct 2009 19:30:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Oct 2009 17:00:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Acquisition class to handle go-online functionality correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jul 2009 11:56:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Jun 2009 09:52:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5.1.0   14 Jul 2009 14:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Nov 2006 15:44:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Single Grab functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Nov 2006 14:51:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added includes for SVIntekDCamAcquisitionProxy and SVIntekDCamDriverProxy
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Nov 2006 14:26:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquisistion Proxy
 * Added DCamDriver Proxy
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Nov 2006 13:56:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2006 15:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  565
 * SCR Title:  Fix 1394 default greyscale camera mode problem with color camera file
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for mono product and adjust 1394 camera file reader for color camera files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Nov 2005 15:54:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
