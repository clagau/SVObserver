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

#ifndef SVUSERMASKOPERATORCLASS_H
#define SVUSERMASKOPERATORCLASS_H

#include "SVUnaryImageOperatorClass.h"
#include "ISVCancel.h"

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

class SVUserMaskOperatorClass : public SVUnaryImageOperatorClass, public ISVCancel, public ISVCancel2
{
	SV_DECLARE_CLASS( SVUserMaskOperatorClass );
public:
	SVUserMaskOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVUSERMASKOPERATOR );
	virtual ~SVUserMaskOperatorClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray& p_rMultiLineArray );

	BOOL Refresh();
	HRESULT GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct );

	SVImageClass*      getMaskInputImage();

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual BOOL ConnectAllInputs();

	SVShapeMaskHelperClass* GetShapeHelper();

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
	
	SVImageInfoClass           m_MaskBufferInfo;
	SVSmartHandlePointer  m_MaskBufferHandlePtr;

	mutable SVGraphixClass m_graphixObject;
	//SVShapeMaskHelperClass     m_ShapeHelper;

	SVMaskOperatorCancelData   m_Data;
	/*
	SVEnumerateValueObjectClass  m_currentMaskOperator;
	SVBoolValueObjectClass       m_bActivated;
	SVDWordValueObjectClass      m_dwvoMaskType;
	SVEnumerateValueObjectClass  m_evoFillArea;
	SVEnumerateValueObjectClass  m_evoShape;
	SVEnumerateValueObjectClass  m_evoMaskArea;
	SVLongValueObjectClass       m_lvoFillColor;
	SVEnumerateValueObjectClass  m_evoDrawCriteria;
	//*/

	SVInObjectInfoStruct         m_inObjectInfo;
	SVExtentMultiLineStruct	     m_MultiLine;
	GUID                         m_guidShapeHelper;

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

	friend class SVToolAdjustmentDialogMaskPageClass;
	friend class SVMaskShapeEditorDlg;
	friend class SVShapeMaskHelperClass;

};

#endif	//	SVUSERMASKOPERATORCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUserMaskOperatorClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:31:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
