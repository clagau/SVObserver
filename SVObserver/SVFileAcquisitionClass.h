//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionClass 
//* .File Name       : $Workfile:   SVFileAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 Mar 2014 18:15:32  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVFileAcquisitionClass : public SVAcquisitionClass
{
public:
	SVFileAcquisitionClass( const SvTh::SVAcquisitionConstructParams& p_rParams );
	virtual ~SVFileAcquisitionClass();

	virtual bool IsValid() const;

	virtual HRESULT Destroy();

	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT LoadFiles( SVFileNameArrayClass &rArray);
	virtual HRESULT UnloadFiles();

	virtual HRESULT CreateLightReference( int iBands, int iBrightness, int iContrast );
	virtual HRESULT GetLightReference( SVLightReference& rArray ) const;
	virtual HRESULT SetLightReference( SVLightReference& rArray, int iBand = -1 );
	virtual HRESULT ResetLightReference();

	virtual HRESULT CreateLightReferenceBand( int iBand, int iAttributes );

	virtual HRESULT ResetLut();
	virtual HRESULT GetLut( SVLut& lut );
	virtual HRESULT SetLut( const SVLut& lut, int iBand = -1 );
	virtual HRESULT SetLut( const SVLutBand& lutBand );

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const;

	virtual bool IsValidBoard() const;
protected:
	virtual HRESULT GetCameraImageInfo(SVImageInfoClass* pImageInfo);

	virtual HRESULT GetLutImpl( SVLut& lut );

	HRESULT DestroyLocal();

private:
	bool mbIsFilesLoaded;
};
