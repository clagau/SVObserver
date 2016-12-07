//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVShapeMaskHelperClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:28:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOCore/SVTaskObject.h"
#include "ISVCancel.h"
#include "SVMaskShape.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#pragma endregion Includes

struct SVMaskShapeCancelData : public SVCancelData
{
	SVMaskShape::MapType         mapProperties;

	SVBoolValueObjectClass       bvoAutoResize;
	SVEnumerateValueObjectClass  evoShapeType;
	SVEnumerateValueObjectClass  evoMaskArea;
	SVLongValueObjectClass       lvoCenterX;
	SVLongValueObjectClass       lvoCenterY;
	SVLongValueObjectClass       lvoWidth;
	SVLongValueObjectClass       lvoHeight;
	SVLongValueObjectClass       lvoSideThickness;
	SVLongValueObjectClass       lvoTopBottomThickness;
	SVLongValueObjectClass       lvoOffset;
	SVEnumerateValueObjectClass  evoXYSymmetry;
};

class SVShapeMaskHelperClass : public SVTaskObjectClass, public ISVCancel, public ISVCancel2
{
	SV_DECLARE_CLASS( SVShapeMaskHelperClass );
public:
	SVShapeMaskHelperClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SHAPE_MASK_HELPER );
	virtual ~SVShapeMaskHelperClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual HRESULT ResetObject() override;
	BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

	HRESULT Refresh();

	enum ShapeTypeEnum
	{
		SVMaskShapeTypeInvalid            = 0,
		SVMaskShapeTypeRectangle          = 1,
		SVMaskShapeTypeOval               = 2,
		SVMaskShapeTypeDoughnut           = 3,
		SVMaskShapeTypeSymmetricTrapezoid = 4,
	};

	HRESULT SetShape( ShapeTypeEnum eShapeType );
	ShapeTypeEnum GetShape();
	HRESULT SetProperties( const SVMaskShape::MapType& mapProperties );	// sets value objects based on properties
	HRESULT GetProperties( SVMaskShape::MapType& rmapProperties );	// gets properties based on value objects
	HRESULT GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct ) const;
	bool    IsAutoResize() const;
	HRESULT SetAutoResize( bool bAutoResize );

	// ISVCancel
	virtual bool CanCancel() override;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rData) override;

protected:
	SVMaskShapeCancelData        m_Data;

	SVMaskShape*                 m_pShape;

private:
	void init();

	friend class SVMaskShapeEditorDlg;
	friend class SVUserMaskOperatorClass;
};

