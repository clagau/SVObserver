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

#pragma once

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

