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
#include "ObjectInterfaces\IMask.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVLibrary\SVGraphix.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVTaskObjectInterfaceInputRequestStruct.h"
#pragma endregion Includes

namespace SvOp
{
class SVShapeMaskHelperClass;
struct SVMaskFillPropertiesStruct;

struct SVMaskOperatorCancelData
{
	SvVol::SVEnumerateValueObjectClass  evoCurrentMaskOperator;
	SvVol::SVBoolValueObjectClass bvoActivated;
	SvVol::SVDWordValueObjectClass dwvoMaskType;
	SvVol::SVEnumerateValueObjectClass  evoFillArea;
	SvVol::SVLongValueObjectClass       lvoFillColor;
	SvVol::SVEnumerateValueObjectClass  evoDrawCriteria;
};

class SVUserMaskOperatorClass : public SVUnaryImageOperatorClass, public SvOi::IMask
{
	SV_DECLARE_CLASS( SVUserMaskOperatorClass );
public:
	SVUserMaskOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUSERMASKOPERATOR );
	virtual ~SVUserMaskOperatorClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT onCollectOverlays(SvIe::SVImageClass *pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

	bool Refresh();
	HRESULT GetFillProperties(SVMaskFillPropertiesStruct& rFillStruct);

	SvIe::SVImageClass* getMaskInputImage(bool bRunMode = false) const;

	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

	virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	SVShapeMaskHelperClass* GetShapeHelper();

	virtual SvDef::StringVector getSpecialImageList() const override;
	virtual bool getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const override;

#pragma region IMask
	virtual HRESULT Import(const std::string& filename) override;
	virtual HRESULT Export(const std::string& filename) override;
	virtual HGLOBAL GetMaskData() const override;
	virtual bool SetMaskData(HGLOBAL hGlobal) override;
#pragma endregion IMask

	HRESULT GetCancelData(SvIe::SVInputRequestStructMap& rMap);

	const SVMaskOperatorCancelData& getCancelData() const { return m_Data; }
	const SVImageInfoClass& getMaskBufferInfo() const { return m_MaskBufferInfo; }
	SvOi::SVImageBufferHandlePtr getImageBuffer() { return m_MaskBufferHandlePtr; }

	enum SVDrawCriteriaEnum 
	{
		SVNone=0,
		SVNonBlackPixels=1,
		SVNonWhitePixels=2,
	};

protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;
	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
	virtual bool hasToAskFriendForConnection( const SvDef::SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner ) const override;
	
	SVImageInfoClass      m_MaskBufferInfo;
	SvOi::SVImageBufferHandlePtr  m_MaskBufferHandlePtr;

	mutable SVGraphixClass m_graphixObject;

	SVMaskOperatorCancelData   m_Data;

	SvOl::SVInObjectInfoStruct m_inObjectInfo;
	SVExtentMultiLineStruct	   m_MultiLine;
	GUID                       m_guidShapeHelper;

	enum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

private:
	HRESULT AddLine(int p_iCol, int p_iRow, SVPoint<double>& rPoint, const SVImageExtentClass& rExtent, SVExtentLineStruct& rLine, SVExtentMultiLineStruct& rMultiLine );
	HRESULT BuildMaskLines( SVExtentMultiLineStruct& p_MultiLine );
	HRESULT CreateLocalImageBuffer();
	HRESULT DestroyLocalImageBuffer();

	void init();

	friend class SVMaskShapeEditorDlg;
	friend class SVShapeMaskHelperClass;
};

} //namespace SvOp
