//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCorecoAcquisitionClass
//* .File Name       : $Workfile:   SVCorecoAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 Mar 2014 18:12:44  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVCorecoAcquisitionClass : public SVAcquisitionClass  
{
public:
	SVCorecoAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVCorecoAcquisitionClass();

	virtual bool IsValid() const;

	virtual HRESULT Destroy();

	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray);
	virtual HRESULT UnloadFiles();

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT LoadLightReference( SVLightReference& rArray );
	virtual HRESULT ResetLightReference();

	virtual HRESULT GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const;
	virtual HRESULT GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const;

	virtual HRESULT CreateLut( const SVLutInfo& info );

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const;

	virtual bool IsValidBoard() const;
protected:
	virtual HRESULT SetLightReferenceImpl( SVLightReference& rLR );

	virtual HRESULT GetCameraImageInfo(SVImageInfoClass *pImageInfo);

	virtual HRESULT GetLutImpl( SVLut& lut );
	virtual HRESULT SetLutImpl( const SVLut& lut );

	HRESULT DestroyLocal();

	enum
	{
		LR_BRIGHTNESS = 0,
		LR_CONTRAST   = 1
	};
	
private:
	bool mbIsCorFilesLoaded;

	enum
	{
		DEFAULT_CONTRAST          = 10000,    // this is the correct default value, not 100
		DEFAULT_CONTRAST_RGB_MONO = 8100,
		DEFAULT_BRIGHTNESS        = 0
	};
};

