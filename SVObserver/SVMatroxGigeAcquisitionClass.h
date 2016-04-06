//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxGigeAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Nov 2014 03:35:08  $
//******************************************************************************

#pragma once

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

	HRESULT GetCameraImageInfo(SVImageInfoClass &pImageInfo);
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
