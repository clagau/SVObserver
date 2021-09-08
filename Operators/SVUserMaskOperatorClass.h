//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUserMaskOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:31:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUnaryImageOperatorClass.h"
#include "SVLibrary/SVGraphix.h"
#include "ObjectInterfaces/IMask.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary\SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary\SVDWordValueObjectClass.h"
#include "SVValueObjectLibrary\SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary\SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvIe
{
struct SVInputRequestStructMap;
}

namespace SvOp
{
class SVShapeMaskHelperClass;
struct SVMaskFillPropertiesStruct;

class SVUserMaskOperatorClass : public SVUnaryImageOperatorClass, public SvOi::IMask
{
	SV_DECLARE_CLASS
public:
	SVUserMaskOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUSERMASKOPERATOR );
	virtual ~SVUserMaskOperatorClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT onCollectOverlays(SvIe::SVImageClass *pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

	bool Refresh();

	SvIe::SVImageClass* getMaskInputImage(bool bRunMode = false) const;

	virtual void Persist(SvOi::IObjectWriter& rWriter) const override;

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	SVShapeMaskHelperClass* GetShapeHelper();
	const SVShapeMaskHelperClass* GetShapeHelper() const;

	virtual SvDef::StringVector getSpecialImageList() const override;
	virtual bool getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const override;

#pragma region IMask
	virtual HRESULT Import(const std::string& filename) override;
	virtual HRESULT Export(const std::string& filename) override;
	virtual HGLOBAL GetMaskData() const override;
	virtual bool SetMaskData(HGLOBAL hGlobal) override;
#pragma endregion IMask

	const SVImageInfoClass& getMaskBufferInfo() const { return m_MaskBufferInfo; }
	SvOi::SVImageBufferHandlePtr getImageBuffer() { return m_MaskBufferHandlePtr; }

	enum SVDrawCriteriaEnum 
	{
		SVNone=0,
		SVNonBlackPixels=1,
		SVNonWhitePixels=2,
	};

protected:
	virtual bool isInputImage(uint32_t imageId) const override;
	SvStl::MessageTextEnum onRunImageType(SvIe::RunStatus& rRunStatus);
	SvStl::MessageTextEnum onRunMask(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, DWORD dwMaskType);
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	virtual void addOverlayGroups(const SvIe::SVImageClass*, SvPb::Overlay& rOverlay) const override;
	
	SVImageInfoClass      m_MaskBufferInfo;
	SvOi::SVImageBufferHandlePtr  m_MaskBufferHandlePtr;

	mutable SVGraphixClass m_graphixObject;

	SvOl::InputObject m_userMaskImageInput;
	SVExtentMultiLineStruct	   m_MultiLine;

	//embeddedValues
	SvVol::SVEnumerateValueObjectClass  m_evoCurrentMaskOperator;
	SvVol::SVBoolValueObjectClass m_bvoActivated;
	SvVol::SVDWordValueObjectClass m_dwvoMaskType;
	SvVol::SVEnumerateValueObjectClass  m_evoFillArea;
	SvVol::SVLongValueObjectClass       m_lvoFillColor;
	SvVol::SVEnumerateValueObjectClass  m_evoDrawCriteria;
	SvVol::SVBoolValueObjectClass m_bvoContRecalc;
	SvStl::SourceFileParams m_lastErrorPosData;

	enum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

private:
	HRESULT AddLine(int p_iCol, int p_iRow, const SVPoint<double>& rPoint, const SVImageExtentClass& rExtent, SVExtentLineStruct& rLine, SVExtentMultiLineStruct& rMultiLine );
	HRESULT BuildMaskLines( SVExtentMultiLineStruct& p_MultiLine );
	HRESULT CreateLocalImageBuffer();
	void DestroyLocalImageBuffer();
	void updateMaskImageFlags(DWORD dwMaskType);

	void init();

	friend class SVShapeMaskHelperClass;
};

} //namespace SvOp
