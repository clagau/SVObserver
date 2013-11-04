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

#ifndef SVSHAPEMASKHELPERCLASS_H
#define SVSHAPEMASKHELPERCLASS_H

#include "SVTaskObject.h"
#include "ISVCancel.h"
#include "SVMaskShape.h"

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
	SVShapeMaskHelperClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SHAPE_MASK_HELPER );
	virtual ~SVShapeMaskHelperClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

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
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	// ISVCancel2
	virtual HRESULT GetCancelData(SVInputRequestStructMap& rData);

protected:
	SVMaskShapeCancelData        m_Data;

	SVMaskShape*                 m_pShape;

private:
	void init();

	friend class SVToolAdjustmentDialogMaskPageClass;
	friend class SVMaskShapeEditorDlg;
	friend class SVUserMaskOperatorClass;
};

#endif	//	SVSHAPEMASKHELPERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVShapeMaskHelperClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:28:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
