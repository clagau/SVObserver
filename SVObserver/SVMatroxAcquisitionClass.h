//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:30:20  $
//******************************************************************************

#if !defined(AFX_SVMATROXACQUISITIONCLASS_H__0D5AF0AA_66E3_439A_87D4_EE26EA69633E__INCLUDED_)
#define AFX_SVMATROXACQUISITIONCLASS_H__0D5AF0AA_66E3_439A_87D4_EE26EA69633E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVAcquisitionClass.h"
#include "SVMatroxDeviceParamCollection.h"

class SVMatroxDeviceManager;
struct SV1394CameraFileInfoStruct;
#include <SVDCamDriver.h>
#include <SVImagingDeviceParams.h>
//class SVDCamDriver;
#include "SVMatroxDCAMAcquisitionProxy.h"

class SVMatroxAcquisitionClass : public SVAcquisitionClass  
{
public:
	SVMatroxAcquisitionClass();
	virtual ~SVMatroxAcquisitionClass();

	virtual bool IsValid();
	virtual bool IsStarted();
    virtual bool IsValidBoard();

	virtual HRESULT Create( unsigned long ulSize = 15 );
	virtual HRESULT Destroy();
	virtual HRESULT AllocDigitizer( SVDeviceParamCollection& rDevice );
	virtual HRESULT DeallocDigitizer( SVDeviceParamCollection& rDevice );

	virtual HRESULT LoadFiles( SVFileNameArrayClass& rArray);
	virtual HRESULT UnloadFiles();
	virtual HRESULT ReadCameraFile( const CString& sFile, SVDeviceParamCollection& rParams );

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT ResetLightReference();
	virtual HRESULT LoadLightReference( SVLightReference& rArray );

	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long& rlValue );
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long& rlValue );
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long& rulValue );
protected:
//	virtual HRESULT GetCurrentLightReferenceValue( unsigned long ulType, long& rlValue );
	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
public:

	virtual HRESULT CreateLut( const SVLutInfo& info );
	virtual HRESULT DestroyLut( );

	virtual HRESULT GetImageInfo(SVImageInfoClass* pImageInfo);
	
	virtual HRESULT CreateBuffers( SVImageProcessingClass* pImaging, SVImageInfoClass IInfo, unsigned long ulSize );
	virtual HRESULT DestroyBuffers();

	virtual HRESULT CreateImageAcquisitionBuffer( SVImageInfoClass& rInfo,
	                                SVAcquisitionImageBufferHandleStruct& rHandle );
	virtual HRESULT DestroyImageAcquisitionBuffer( SVAcquisitionImageBufferHandleStruct& rHandle );

	virtual HRESULT CreateSingleGrabImageBuffer(SVImageInfoClass& rInfo, SVAcquisitionImageBufferHandleStruct& rHandle);
	virtual HRESULT DestroySingleGrabImageBuffer(SVAcquisitionImageBufferHandleStruct& rHandle);

	virtual HRESULT Start();
	virtual HRESULT Stop();

	virtual HRESULT StartProcess( SVORequestClass* pRequest );
	virtual HRESULT EndProcess( SVORequestClass* pRequest );

	virtual HRESULT NotifyStartFrame( SVAcquisitionImageBufferHandleStruct* pHandle );
	virtual HRESULT NotifyEndFrame( SVAcquisitionImageBufferHandleStruct* pHandle );

	virtual HRESULT LockBuffer( SVAcquisitionImageBufferHandleStruct* pHandle );
	virtual HRESULT UnlockBuffer( SVAcquisitionImageBufferHandleStruct* pHandle );

	virtual HRESULT SetHardwareDeviceCapabilities( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	HRESULT SetMatroxDeviceManagerPointer( SVMatroxDeviceManager* pDeviceManager );
	HRESULT InitBuffer( SVImageBufferHandleStruct& rHandle, DWORD dwValue );

	HRESULT SingleGrab( SVAcquisitionImageBufferHandleStruct* pHandle );

	// SVSubscriberInterface
	public:
	// temp hack; SVSubscriberInterface virtual
	virtual SVOCallbackPtr GetCallbackFunction(const CString& sType) const;

	static HRESULT CALLBACK StrobeStartFrameCallback( void* pvOwner, void* pvCaller, void* pvData );

protected:

	bool CameraMatchesCameraFile();

	static HRESULT ReadCameraFileImpl( /*in,out*/SV1394CameraFileInfoStruct& rInfo, SVDeviceParamCollection& rParams );
	static HRESULT SetParameterDefaults( SVDeviceParamCollection& rParams );

	virtual SVDeviceParamCollection& HardwareCapabilities();
	virtual HRESULT GetCameraImageInfo(SVImageInfoClass* pImageInfo);

	virtual HRESULT SetLutImpl( const SVLut& lut );
	virtual HRESULT GetLutImpl( SVLut& lut );

	virtual HRESULT ExecuteCallback( SVOCallbackPtr pCallback,
	                                 void *pvOwner, void *pvCaller,
	                                 SVORequestClass *pRequest);


	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	HRESULT DestroyLocal();

	static void NeutralLutToRawLut(const SVLut& lut, CByteArray& aRawBytes);
	static void RawLutToNeutralLut(const CByteArray& aRawBytes, SVLut& lut);

	static HRESULT MFTYPE StartGrabCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser);
	static HRESULT MFTYPE StartFrameCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser);
	static HRESULT MFTYPE EndFrameCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser);
	static HRESULT MFTYPE EndGrabCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser);

	HRESULT OnStartFrame(__int64 iTimestamp=0);
	HRESULT OnEndFrame();
	HRESULT OnGrabBufferFill( MIL_ID milBuffer );

	long QueueGrabs(long lNumToQueue = -1);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam );

	// provide basic MIL services to the camera driver
	friend class SVDCamDriver;	// to access these protected functions
	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline();

	long PrimeTheQueue();


	struct SingleGrabHelperStruct
	{
		SVMatroxAcquisitionClass* pThis;
		SVAcquisitionImageBufferHandleStruct* pHandle;
	};

	static DWORD WINAPI SingleGrabHelperFn(LPVOID lpParameter);

	//int m_iServerIndex;   // ???
	//int m_iDigitizerIndex;// ???
	int m_iBandSize;
	//int m_iBandMaxSize;

	bool mbAreBuffersCreated;
	bool mbGoingOffline;
	//bool mbIsStarted;
	bool mbTempOnline;
	bool mbDontReallocDigFormat;
	CStdioFile m_LogFile;


	volatile long m_lGrabStartFrameLock;
	volatile long m_lOutstandingTriggerCount;
	volatile long m_lOutstandingGrabRequests;
	volatile bool m_bStartedGrabRequests;
	bool m_bStopProcessing;
	long m_lCalculatedLatency;
	__int64 m_iLastTrigger;
	__int64 m_iLastEndFrame;

	struct RunningAverageTimeoutStruct
	{
		std::vector<long> vecLatencyQueue;
		std::vector<__int64> vecTriggerTimestampQueue;
		int iLatencyQueueIndex;
		__int64 iLatencySum;
		long lLatencyCount;
		void Clear() {vecLatencyQueue.clear(); vecTriggerTimestampQueue.clear();
		              vecLatencyQueue.resize(100); vecTriggerTimestampQueue.reserve(10);
		              iLatencyQueueIndex=0; iLatencySum=0; lLatencyCount=0;}
	};

	RunningAverageTimeoutStruct m_ratInfo;
	long ComputeNextRunningAverage(__int64 iCurrentTime);

	//SVMatroxDeviceStruct m_Device;
	SVMatroxDeviceParamCollection m_Device;
	SVMatroxDeviceManager* m_pDeviceManager;
	SVDCamDriver* m_pCameraDriver;

	SVCameraFormat m_PreviousCameraFormat;

	//CList < SVAcquisitionImageBufferHandleStruct*, SVAcquisitionImageBufferHandleStruct* > m_AcquisitionBufferQueue;
	SVTQueueObject < SVAcquisitionImageBufferHandleStruct* > m_AcquisitionBufferQueue;


	TFactory<CString, SVDCamDriver> myfactory;

private:
	SVMatroxDCamAcquisitionProxy m_DCamAcqProxy;
};

#endif // !defined(AFX_SVMATROXACQUISITIONCLASS_H__0D5AF0AA_66E3_439A_87D4_EE26EA69633E__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxAcquisitionClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:30:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   29 Nov 2006 15:40:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CreateSingleGrabImageBuffer method
 * Added DestroySingleGrabImageBuffer method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   29 Nov 2006 14:23:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquisition Proxy
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   21 Jun 2006 15:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  565
 * SCR Title:  Fix 1394 default greyscale camera mode problem with color camera file
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for mono product and adjust 1394 camera file reader for color camera files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   08 Nov 2005 15:59:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   01 Apr 2004 14:51:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  408
 * SCR Title:  Fix Go Online problem with ISG camera
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with reading of the camera.  now it will only read, before it read and reset the camera info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   18 Feb 2004 09:35:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with previous start frame / end frame logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Feb 2004 16:58:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem of continuous NAKs when triggering faster than the inspection can handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Feb 2004 17:31:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added CalculatedLatency acquisition method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Jan 2004 12:27:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with RunningAverageTimeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jan 2004 15:29:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added RunningAverageTimeout method of acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Jan 2004 07:38:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added SVSubscriberInterface support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Dec 2003 15:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed:
 * - problem with Light Reference tabs not being updated
 * - problem with loading config with different physical camera attached
 * - changed T T T handling
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   05 Dec 2003 14:34:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed logic to allow multiple outstanding Triggers (2 by default)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Nov 2003 09:39:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed unused function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   31 Oct 2003 08:24:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general enhancements; grab queue; block register transfer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Oct 2003 09:57:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for color image formats
 * added Event Log entry if not enough Non-paged memory.
 * added support for Hue, Saturation, Sharpness, WhiteBalance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Oct 2003 09:12:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed LoadLightReference to work correctly with 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Sep 2003 09:53:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   put camera reading into separate class (SV1394CameraFileReader)
 * changed acquisitoin logic to NAK both triggers on T T EF
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Sep 2003 09:10:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Trigger, SF, EF handling
 * fixed LightReference handling
 * added logging
 * added functions:
 * AllocDigitizer, DeallocDigitizer
 * SetParameterDefaults
 * ExecuteCallback
 * UpdateLightReferenceAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Aug 2003 13:18:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made grab queue thread safe (using SVTQueueObject)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 2003 10:26:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes;
 * changed from using a queue to single outstanding grab
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2003 13:03:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * changed DestroyImageBuffer to DestroyImageAcquisitionBuffer
 * added SingleGrab
 * changed SetDCAMParameters to SetStandardCameraParameters
 * other general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Jul 2003 09:00:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVCameraFormatsDeviceParam
 * moved camera-specific code to SVDCamDriver class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 09:59:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Matrox 1394 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:17:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:56:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/