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
#include "SVOMFCLibrary\SVGraphix.h"
#include "ISVCancel.h"
#pragma endregion Includes

class SVShapeMaskHelperClass;
struct SVMaskFillPropertiesStruct;

struct SVMaskOperatorCancelData : public SVCancelData
{
	SVCancelData*                pShapeData;

	SVEnumerateValueObjectClass  evoCurrentMaskOperator;
	SVBoolValueObjectClass       bvoActivated;
	SVDWordValueObjectClass      dwvoMaskType;
	SVEnumerateValueObjectClass  evoFillArea;
	SVLongValueObjectClass       lvoFillColor;
	SVEnumerateValueObjectClass  evoDrawCriteria;

	SVMaskOperatorCancelData()
	{
		pShapeData = NULL;
	}

	~SVMaskOperatorCancelData()
	{
		if ( pShapeData )
		{
			delete pShapeData;
			pShapeData = NULL;
		}
	}
};

class SVUserMaskOperatorClass : public SVUnaryImageOperatorClass, public SvOi::IMask, public ISVCancel, public ISVCancel2
{
	SV_DECLARE_CLASS( SVUserMaskOperatorClass );
public:
	SVUserMaskOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUSERMASKOPERATOR );
	virtual ~SVUserMaskOperatorClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray& p_rMultiLineArray );

	BOOL Refresh();
	HRESULT GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct );

	SVImageClass* getMaskInputImage();

	virtual void Persist(SVObjectWriter& rWriter);

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual BOOL ConnectAllInputs();

	SVShapeMaskHelperClass* GetShapeHelper();

#pragma region IMask
	virtual SvOi::MatroxImageSmartHandlePtr GetReferenceImage() const;
	virtual SvOi::MatroxImageSmartHandlePtr GetMaskImage() const;
	virtual HRESULT Import(const SVString& filename);
	virtual HRESULT Export(const SVString& filename);
	virtual HGLOBAL GetMaskData() const;
	virtual bool SetMaskData(HGLOBAL hGlobal);
#pragma endregion IMask
// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rMap);

	enum SVDrawCriteriaEnum 
	{
		SVNone=0,
		SVNonBlackPixels=1,
		SVNonWhitePixels=2,
	};

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();
	
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

