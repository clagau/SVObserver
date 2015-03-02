// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekFireStackClass
// * .File Name       : $Workfile:   SVIntekFireStackClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:53:34  $
// ******************************************************************************

#ifndef SVINTEKFIRESTACKCLASS_H
#define SVINTEKFIRESTACKCLASS_H

/* Image Formats for FireStack

	SVImageFormatUnknown
	SVImageFormatMono8      CM_Y8
	SVImageFormatMono16     CM_Y16
	SVImageFormatMono32     
	SVImageFormatRGB5551    
	SVImageFormatRGB565     
	SVImageFormatRGB888     CM_RGB8
	SVImageFormatRGB8888    
	SVImageFormatRGB101010  
	SVImageFormatRGB161616  CM_RGB16
	SVImageFormatRGBP8      
	SVImageFormatRGBP16     
	SVImageFormatUYVY       
	SVImageFormatYUY2       
	SVImageFormatYVYU       
	SVImageFormatYUYV       
	SVImageFormatY411       CM_YUV411 
	SVImageFormatY422       CM_YUV422
	SVImageFormatY444       CM_YUV444

//*/

#include "SVIntekExports.h"
#include "SVIntekEnums.h"
#include "SVOLibrary/SVQueueObject.h"

#include "SVIntekBoardStruct.h"
#include "SVIntekCameraStruct.h"

#include "SVIntekCameraParamManager.h"
#include "SVTriggerLibrary/SVInternalTrigger.h"
#include "SVTriggerLibrary/SVTriggerCallbackMap.h"
#include "SVMaterialsLibrary/SVMaterials.h"

class SVAcquisitionBufferInterface;
struct SVCallbackStruct;

class SVIntekFireStackClass  
{
public:
	static DWORD WINAPI SVIntekEndFrameThreadProc( LPVOID p_lpParam );

	SVIntekFireStackClass();
	virtual ~SVIntekFireStackClass();

	virtual HRESULT Create();
	virtual HRESULT Destroy( bool p_bClose = false );

	virtual HRESULT Open();
	virtual HRESULT ThreadOpen();
	virtual HRESULT Close();

	virtual HRESULT GetStatusHandles( unsigned long &p_rulSize, HANDLE **p_pphArray );

	virtual HRESULT BufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, int p_iFormat, SVIntekBufferStruct &p_rBuffer );
	virtual HRESULT BufferDestroy( SVIntekBufferStruct &p_rBuffer );

	virtual HRESULT CameraCreate( SVIntekBoardStruct *p_pBoard, CCamera *p_pCamera, bool& bNewCamera );
	virtual HRESULT CameraDestroy( CCamera *p_pCamera );
	virtual HRESULT CameraGetCount( unsigned long &p_rulCount );
	virtual HRESULT CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName );
	virtual HRESULT CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight );
	virtual HRESULT CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth );
	virtual HRESULT CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat );
	virtual HRESULT CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName );
	virtual HRESULT CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue );
	virtual HRESULT CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue );
	virtual HRESULT CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
	virtual HRESULT CameraBufferDestroyAll( unsigned long p_ulIndex );
	virtual HRESULT CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
	virtual HRESULT CameraUnregisterBufferInterface( unsigned long p_ulIndex );
	virtual HRESULT CameraStart( unsigned long p_ulIndex );
	virtual HRESULT CameraStop( unsigned long p_ulIndex );
	virtual HRESULT CameraProcessStartFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraAPCProcessEndFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraProcessEndFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames);

	virtual HRESULT InternalTriggerEnable( unsigned long p_ulIndex );
	virtual HRESULT InternalTrigger( unsigned long p_ulIndex );
	virtual HRESULT RegisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex );

	HRESULT ReloadCameraParams(unsigned long p_ulIndex, CCamera* p_pCamera);

	// Trigger functions ...
	virtual HRESULT TriggerGetCount(unsigned long& p_ulCount);
	
	// convert ordinal to handle
	unsigned long TriggerGetHandle(unsigned long p_ulIndex);

	virtual HRESULT TriggerGetName(unsigned long p_ulHandle, BSTR& p_rbstrName);
	virtual HRESULT TriggerRegisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback);
	virtual HRESULT TriggerUnregisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback);
	virtual HRESULT TriggerUnregisterAllCallbacks(unsigned long p_ulHandle);
	virtual HRESULT TriggerStart(unsigned long p_ulHandle);
	virtual HRESULT TriggerStop(unsigned long p_ulHandle);

	virtual HRESULT TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
	virtual HRESULT TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
	virtual HRESULT TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

protected:
	virtual HRESULT CameraGetFormatModeRate( unsigned long p_ulIndex, unsigned long &p_rulValue );
	virtual HRESULT CameraSetFormatModeRate( unsigned long p_ulIndex, unsigned long p_ulValue );
	virtual HRESULT CameraUpdateFormatModeRateColorFormat( unsigned long p_ulIndex );

	//This attribute holds the reference count for this object.  It can only be create and destroyed once.
	long m_lRefCount;

	//This attribute is a container that holds the available acquisition boards in the system.
	SVIntekBoardStructPtrCArray m_caBoards;
	//This attribute is a container that holds the available digitizers in the system.
	SVIntekCameraStructCArray m_caCameras;

	//This attribute holds the handle to the processing thread that process the acquisition buffer to the application.
	HANDLE m_hThread;
	//This attribute holds the handle to the shutdown event that stops the processing thread.
	HANDLE m_hShutdown;

private:
	HRESULT SetFeature(int p_ulIndex, unsigned char featureId, unsigned long ulVal, bool bSubValue=false);
	HRESULT WriteQuad(int p_ulIndex, unsigned long ulVal);
	HRESULT WriteBlock(int p_ulIndex, VARIANT* p_pvarValue);
	HRESULT SetIntekCameraParams(unsigned long p_ulIndex);
	void TrackCameraParams(unsigned long p_ulIndex);
	void LockMainCameraParamList(unsigned long p_ulIndex);
	void UnTrackCameraParams(unsigned long p_ulIndex);

	HRESULT FireOneShot( unsigned long p_ulIndex );
	HRESULT FireSoftwareTrigger( unsigned long p_ulIndex );
	HRESULT DispatchTriggerCallback( unsigned long p_ulIndex );

	bool IsValidDigitizer(unsigned long p_ulIndex) const;
	SVIntekCameraStruct& GetDigitizer(unsigned long p_ulHandle);
	const SVIntekCameraStruct& GetDigitizer(unsigned long p_ulHandle) const;

	HRESULT IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered);
	void DoAcquisitionTrigger(const SVIntekCameraStruct& p_rCamera);

	//This attribute is a container that manages the camera parameters.
	SVIntekCameraParamManager m_cameraParamsMgr;
	//This attribute holds all of the trigger connections for non-hardware based triggers.
	SVInternalTrigger m_triggerMgr;
	//This attribute holds all of the callback information for the non-hardware based triggers.
	SVTriggerCallbackMap m_triggerMap;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekFireStackClass.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:53:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jan 2013 09:50:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Aug 2012 09:19:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  786
 * SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Fire Bus callback and Intek Fire Stack class Create Camera method to fix problems with sending parameters during the re-connect process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 10:23:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Dec 2009 12:14:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Oct 2009 10:36:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FireSoftwareTrigger method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jul 2009 16:10:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Sep 2008 10:05:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for Software triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   14 Jul 2009 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Nov 2006 17:43:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Camera ParamertersTracking logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Jul 2006 15:04:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added a new method of handle the serialization of the card starts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 13:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
