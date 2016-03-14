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

