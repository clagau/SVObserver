//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvIe
{

class SVFileAcquisitionClass : public SVAcquisitionClass
{
public:
	explicit SVFileAcquisitionClass( const SVAcquisitionConstructParams& p_rParams );
	virtual ~SVFileAcquisitionClass();

	virtual bool IsValid() const override;

	virtual HRESULT Destroy() override;

	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams ) override;
	virtual HRESULT SetDeviceParameter(int parameter, const _variant_t& rValue) override;

	virtual HRESULT LoadFiles(const SvFs::FileNameContainer &rArray) override;
	virtual HRESULT UnloadFiles() override;

	virtual HRESULT CreateLightReference( int iBands ) override;
	virtual HRESULT GetLightReference( SVLightReference& rArray ) const override;
	virtual HRESULT SetLightReference( SVLightReference& rArray, int iBand = -1 ) override;
	virtual HRESULT ResetLightReference() override;

	virtual HRESULT CreateLightReferenceBand( int iBand, int iAttributes ) override;

	virtual HRESULT ResetLut() override;
	virtual HRESULT GetLut( SVLut& lut ) override;
	virtual HRESULT SetLut( const SVLut& lut, int iBand = -1 ) override;
	virtual HRESULT SetLut( const SVLutBand& lutBand ) override;

	virtual HRESULT GetImageInfo( SVImageInfoClass* pImageInfo ) const override;

	virtual bool IsValidBoard() const override;
protected:
	HRESULT GetCameraImageInfo(SVImageInfoClass* pImageInfo);

	virtual HRESULT GetLutImpl( SVLut& lut ) override;

	HRESULT DestroyLocal();

private:
	bool mbIsFilesLoaded;
};

} //namespace SvIe
