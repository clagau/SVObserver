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
#include "CameraLibrary/SVDeviceParamCollection.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "TriggerInformation/SVAcquisitionConstructParams.h"
#include "SVSystemLibrary/SVFileNameArrayClass.h"
#include "SVOCore/SVImageObjectClass.h"
#pragma endregion Includes

/**
@SVObjectName Acquisition Class

@SVObjectOverview This object is the device wrapper for the attached cameras.  This object manages the connection to the image signals and the image buffers.

@SVObjectOperations This object maintains operations for connecting and configuring the camera device information.  This includes but is not limited to setting up the notification signals, starting the notification process, stopping the notification process, creating buffers based on the image parameters, etc. 

*/
class SVDigitizerProcessingClass;

class SVAcquisitionClass :
	public SVODataDeviceClass,
	public SVAcquisitionBufferInterface
{
public:
	SVAcquisitionClass( const SvTi::SVAcquisitionConstructParams& p_rParams );
	virtual ~SVAcquisitionClass();

	virtual bool IsValid() const override;  // SVODataDeviceClass Override
	virtual bool IsStarted() const override;  // SVODataDeviceClass Override

	virtual void ClearDevice() override;  // SVODataDeviceClass Override

	virtual HRESULT Create( unsigned long ulSize = 50 ) override;  // SVODataDeviceClass Override
	virtual HRESULT Destroy() override;  // SVODataDeviceClass Override

	virtual HRESULT Start() override;  // SVODataDeviceClass Override
	virtual HRESULT Stop() override;  // SVODataDeviceClass Override
	virtual HRESULT Reset() override;  // SVODataDeviceClass Override

	HRESULT CreateBuffers( SVImageInfoClass IInfo, unsigned long ulSize );
	HRESULT DestroyBuffers();

	virtual HRESULT GetFileNameArraySize( long &rlSize ) const;
	virtual HRESULT GetFileName( long lIndex, SVFileNameClass &rFileName ) const;
	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray );
	virtual HRESULT UnloadFiles();
	virtual HRESULT ReadCameraFile( const SVString& filename,  SVDeviceParamCollection& rParams );

	virtual HRESULT CreateLightReference( int iBands );
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

	SVImageObjectClassPtr GetCircleBuffer();
	long GetCircleBufferSize() const;

	HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle ) const;
	HRESULT GetNextIndex( SVDataManagerHandle &rDMHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	inline SVString DeviceName() const { return m_DeviceName; }
	inline SVString DigName() const { return m_DigName; }
	inline int Channel() const { return miChannel; }
	inline int DigNumber() const { return miDigNumber; }
	inline int BandSize() const { return miBandSize; }
	inline int BandMaxSize() const { return miBandMaxSize; }

	inline void	   SetDigName(LPCTSTR Name) {m_DigName = Name;}
	inline int     SetChannel(int i) {return miChannel = i;}
	inline int     SetDigNumber(int i) {return miDigNumber = i;}
	inline int     SetBandSize(int i) {return miBandSize = i;}
	inline int     SetBandMaxSize(int i) {return miBandMaxSize = i;}

	HRESULT SetNumberOfBands( int NumberOfBands );

	virtual bool IsValidBoard() const;

	bool IsColor() const { return (miBandSize > 1) ; };

	HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	HRESULT GetHardwareDeviceCapabilities( SVDeviceParamCollection& rDeviceParams );

	// called from SVImageBoardProcessingClass::DisconnectDevices
	virtual void ClearDeviceIdentifier();

	bool IsDigitizerSubsystemValid() const;

	virtual HRESULT SingleGrab( SVSmartHandlePointer p_SingleGrabHandle );

	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	virtual BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	void DumpDMInfo( LPCTSTR p_szName ) const;

	virtual SvTl::SVTimeStamp GetTimeStamp() const override;

	virtual unsigned long GetBufferWidth() const override;
	virtual unsigned long GetBufferHeight() const override;
	virtual int GetBufferFormat() const override;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer ) override;
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer ) override;

	HRESULT StoreLastImage();
	HRESULT RestoreLastImage();

	SVHANDLE m_hDigitizer;
	BSTR m_LastImage;

protected:
	static DWORD WINAPI SingleGrabHelperFn(LPVOID lpParameter);

	friend class SVMatroxGigeCameraProxy;		// to access these protected functions

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)

	virtual bool IsOnline() const;
	bool IsBufferCreated() const { return mbIsBufferCreated; };

	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );
	SVDeviceParamCollection& HardwareCapabilities();
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

	SVString m_DigName;
	int miChannel; //&&& Var needs to be set
	int miDigNumber; // &&& Var needs to be set
	int miBandSize;
	int miBandMaxSize;

	long mlStartFrameIndex;
	long mlDMStartFrameIndex;

	bool mbIsBufferCreated;
	bool mbTempOnline;
	SVSmartHandlePointer m_SingleGrabHandle;
	SVDigitizerProcessingClass& m_rDigitizerProc;

private:
	SVAcquisitionClass();

	unsigned long mulSize;
	bool m_ImageAquired;
	bool m_LUTAndLRSet;
};

typedef SVSharedPtr< SVAcquisitionClass > SVAcquisitionClassPtr;

