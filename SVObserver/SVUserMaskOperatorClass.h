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
#include "SVOCore/SVUnaryImageOperatorClass.h"
#include "CameraLibrary\SVGraphix.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVLibrary/SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVShapeMaskHelperClass;
struct SVMaskFillPropertiesStruct;

struct SVMaskOperatorCancelData
{
	SVEnumerateValueObjectClass  evoCurrentMaskOperator;
	SVBoolValueObjectClass       bvoActivated;
	SVDWordValueObjectClass      dwvoMaskType;
	SVEnumerateValueObjectClass  evoFillArea;
	SVLongValueObjectClass       lvoFillColor;
	SVEnumerateValueObjectClass  evoDrawCriteria;
};

class SVUserMaskOperatorClass : public SVUnaryImageOperatorClass, public SvOi::IMask
{
	SV_DECLARE_CLASS( SVUserMaskOperatorClass );
public:
	SVUserMaskOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUSERMASKOPERATOR );
	virtual ~SVUserMaskOperatorClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

	BOOL Refresh();
	HRESULT GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct );

	SVImageClass* getMaskInputImage();

	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	SVShapeMaskHelperClass* GetShapeHelper();

#pragma region IMask
	virtual SvOi::MatroxImageSmartHandlePtr GetReferenceImage() const override;
	virtual SvOi::MatroxImageSmartHandlePtr GetMaskImage() const override;
	virtual HRESULT Import(const SVString& filename) override;
	virtual HRESULT Export(const SVString& filename) override;
	virtual HGLOBAL GetMaskData() const override;
	virtual bool SetMaskData(HGLOBAL hGlobal) override;
#pragma endregion IMask

	HRESULT GetCancelData(SVInputRequestStructMap& rMap);

	enum SVDrawCriteriaEnum 
	{
		SVNone=0,
		SVNonBlackPixels=1,
		SVNonWhitePixels=2,
	};

protected:
	virtual bool onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual bool hasToAskFriendForConnection( const SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner ) const override;
	
	SVImageInfoClass      m_MaskBufferInfo;
	SVSmartHandlePointer  m_MaskBufferHandlePtr;

	mutable SVGraphixClass m_graphixObject;

	SVMaskOperatorCancelData   m_Data;

	SVInObjectInfoStruct       m_inObjectInfo;
	SVExtentMultiLineStruct	   m_MultiLine;
	GUID                       m_guidShapeHelper;

	enum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

private:
	HRESULT AddLine(int p_iCol, int p_iRow, SVExtentPointStruct& p_svStartPoint, SVImageExtentClass& p_svExtent, SVExtentLineStruct& p_Line, SVExtentMultiLineStruct& p_MultiLine );
	HRESULT BuildMaskLines( SVExtentMultiLineStruct& p_MultiLine );
	HRESULT CreateLocalImageBuffer();
	HRESULT DestroyLocalImageBuffer();

	void init();

	friend class SVMaskShapeEditorDlg;
	friend class SVShapeMaskHelperClass;
};

