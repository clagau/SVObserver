//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionClass.h
//* .File Name       : $Workfile:   SVAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Mar 2014 18:05:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVOLibrary/SVODataDeviceClass.h"
#include "SVOLibrary/SVODeviceClass.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

#include "SVFileNameArrayClass.h"
#include "SVImageObjectClass.h"
#include "SVSubscriberProviderInterfaces.h"
#pragma endregion Includes

#pragma region Declarations
struct SVAcquisitionConstructParams;
#pragma endregion Declarations

/**
@SVObjectName Acquisition Class

@SVObjectOverview This object is the device wrapper for the attached cameras.  This object manages the connection to the image signals and the image buffers.

@SVObjectOperations This object maintains operations for connecting and configuring the camera device information.  This includes but is not limited to setting up the notification signals, starting the notification process, stopping the notification process, creating buffers based on the image parameters, etc. 

*/
class SVAcquisitionClass :
	public SVODataDeviceClass,
	public SVSubscriberInterface,
	public SVAcquisitionBufferInterface
{
public:
	SVAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVAcquisitionClass();

	virtual bool IsValid() const;  // SVODataDeviceClass Override
	virtual bool IsStarted() const;  // SVODataDeviceClass Override

	virtual void ClearDevice();  // SVODataDeviceClass Override

	virtual HRESULT Create( unsigned long ulSize = 50 );  // SVODataDeviceClass Override
	virtual HRESULT Destroy();  // SVODataDeviceClass Override

	virtual HRESULT Start();  // SVODataDeviceClass Override
	virtual HRESULT Stop();  // SVODataDeviceClass Override
	virtual HRESULT Reset();  // SVODataDeviceClass Override

	virtual HRESULT AllocDigitizer( SVDeviceParamCollection& rDevice );

	virtual HRESULT CreateBuffers( SVImageInfoClass IInfo, unsigned long ulSize );
	virtual HRESULT DestroyBuffers();

	virtual HRESULT GetFileNameArraySize( long &rlSize ) const;
	virtual HRESULT GetFileName( long lIndex, SVFileNameClass &rFileName ) const;
	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray );
	virtual HRESULT UnloadFiles();
	virtual HRESULT ReadCameraFile( const CString& sFile, SVDeviceParamCollection& rParams );

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT LoadLightReference( SVLightReference& rArray );
	virtual HRESULT GetLightReference( SVLightReference& rLR ) const;
	virtual HRESULT SetLightReference( SVLightReference& rLR, int iBand = -1 );
	virtual HRESULT ResetLightReference();

	virtual HRESULT CreateLightReferenceBand( int iBand, int iAttributes );
	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const;

	virtual HRESULT CreateLut( const SVLutInfo& info );
	virtual HRESULT DestroyLut();
	virtual HRESULT ResetLut();
	virtual HRESULT GetLut( SVLut& lut );
	virtual HRESULT SetLut( const SVLut& lut, int iBand = -1 );
	virtual HRESULT SetLut( const SVLutBand& lutBand );

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const;

	virtual SVImageObjectClassPtr GetCircleBuffer();
	virtual long GetCircleBufferSize() const;

	virtual CString GetRootDeviceName() const;

	virtual HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle ) const;
	virtual HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	inline CString DeviceName() const { return mDeviceName.c_str(); }
	inline CString DigName() const { return mcsDigName; }
	inline int Channel() const { return miChannel; }
	inline int DigNumber() const { return miDigNumber; }
	inline int BandSize() const { return miBandSize; }
	inline int BandMaxSize() const { return miBandMaxSize; }

	inline CString SetDigName(const CString& s) {return mcsDigName = s;}
	inline int     SetChannel(int i) {return miChannel = i;}
	inline int     SetDigNumber(int i) {return miDigNumber = i;}
	inline int     SetBandSize(int i) {return miBandSize = i;}
	inline int     SetBandMaxSize(int i) {return miBandMaxSize = i;}

	virtual bool IsValidBoard() const;

	virtual bool IsColor() const { return (miBandSize > 1) ; };

	virtual HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	HRESULT GetHardwareDeviceCapabilities( SVDeviceParamCollection& rDeviceParams );

	// called on creation
	virtual HRESULT SetHardwareDeviceCapabilities( const SVDeviceParamCollection& rDeviceParams );

	// called from SVImageBoardProcessingClass::DisconnectDevices
	virtual void ClearDeviceIdentifier();

	bool IsDigitizerSubsystemValid() const;

	virtual HRESULT SingleGrab( SVSmartHandlePointer p_SingleGrabHandle );

	// temp hack
	virtual SVOCallbackPtr GetCallbackFunction(const CString& sType) const {return nullptr;}

	virtual BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const;
	virtual BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const;
	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	void DumpDMInfo( LPCTSTR p_szName ) const;

	virtual SVClock::SVTimeStamp GetTimeStamp() const;

	virtual unsigned long GetBufferWidth() const;
	virtual unsigned long GetBufferHeight() const;
	virtual int GetBufferFormat() const;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer );

	virtual HRESULT StoreLastImage();
	virtual HRESULT RestoreLastImage();

	SVHANDLE m_hDigitizer;
	BSTR m_LastImage;

protected:
	static DWORD WINAPI SingleGrabHelperFn(LPVOID lpParameter);

	friend class SVMatroxGigeCameraProxy;		// to access these protected functions

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline() const;
	virtual bool IsBufferCreated() const { return mbIsBufferCreated; };

	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
	virtual SVDeviceParamCollection& HardwareCapabilities();
	virtual HRESULT GetLutImpl( SVLut& lut );
	virtual HRESULT SetLutImpl( const SVLut& lut );

	virtual HRESULT StartDigitizer();

	SVSmartIndexArrayHandlePtr m_pDataManagerHandle;
	
	SVImageInfoClass msvImageInfo;
	SVImageObjectClassPtr m_AcquisitionBuffersPtr;

	SVDeviceParamCollection m_DeviceParams;
	SVDeviceParamCollection m_CameraFileDeviceParams;
	SVLut& Lut();
	SVLightReference mLightReference;
	SVFileNameArrayClass mFiles;

	CString mcsDigName;
	int miChannel; //&&& Var needs to be set
	int miDigNumber; // &&& Var needs to be set
	int miBandSize;
	int miBandMaxSize;

	long mlStartFrameIndex;
	long mlDMStartFrameIndex;

	bool mbIsBufferCreated;
	bool mbTempOnline;
	SVSmartHandlePointer m_SingleGrabHandle;

private:
	SVAcquisitionClass();

	unsigned long mulSize;
	bool m_ImageAquired;
};

typedef SVSharedPtr< SVAcquisitionClass > SVAcquisitionClassPtr;

