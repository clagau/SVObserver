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


class SVMatroxGigeAcquisitionClass : public SVAcquisitionClass
{
public:
	SVMatroxGigeAcquisitionClass( const SvTi::SVAcquisitionConstructParams& p_rParams );
	virtual ~SVMatroxGigeAcquisitionClass();

	virtual bool IsValid() const override;
	virtual bool IsStarted() const override;

	virtual HRESULT Create( unsigned long ulSize = 15 ) override;
	virtual HRESULT Destroy() override;

	virtual HRESULT GetFileNameArraySize( long &rlSize ) const override;
	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray ) override;
	virtual HRESULT ReadCameraFile( const std::string& filename, SVDeviceParamCollection& rParams ) override;

	virtual HRESULT CreateLightReference( int iBands ) override;

	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const override;
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const override;
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const override;

	virtual HRESULT CreateLut( const SVLutInfo& info ) override;
	virtual HRESULT DestroyLut() override;

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const override;

	virtual bool IsValidBoard() const override;

	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams ) override;

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams ) override;
	virtual void ClearDeviceIdentifier() override;
	virtual HRESULT SingleGrab( SVImageBufferHandlePtr p_SingleGrabHandle ) override;
	virtual bool IsOnline() const override;

protected:
	bool CameraMatchesCameraFile();

	HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam );

	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam ) override;	// for parameters that are settable through MIL (all standard DCAM params)

	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR ) override;
	virtual HRESULT GetLutImpl( SVLut& lut ) override;
	virtual HRESULT SetLutImpl( const SVLut& lut ) override;

	virtual HRESULT StartDigitizer() override;

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

	HRESULT SetGigeFeatureOverrides(const std::string& featureOverrides);
};
