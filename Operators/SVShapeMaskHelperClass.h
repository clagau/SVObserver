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
#include "InspectionEngine/SVTaskObject.h"
#include "SVMaskShape.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVValueObjectLibrary\SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary\SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary\SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvIe
{
struct SVInputRequestStructMap;
}

namespace SvOp
{

struct SVMaskShapeCancelData
{
	SVMaskShape::MapType   mapProperties;

	SvVol::SVBoolValueObjectClass       bvoAutoResize;
	SvVol::SVEnumerateValueObjectClass  evoShapeType;
	SvVol::SVEnumerateValueObjectClass  evoMaskArea;
	SvVol::SVLongValueObjectClass       lvoCenterX;
	SvVol::SVLongValueObjectClass       lvoCenterY;
	SvVol::SVLongValueObjectClass       lvoWidth;
	SvVol::SVLongValueObjectClass       lvoHeight;
	SvVol::SVLongValueObjectClass       lvoSideThickness;
	SvVol::SVLongValueObjectClass       lvoTopBottomThickness;
	SvVol::SVLongValueObjectClass       lvoOffset;
	SvVol::SVEnumerateValueObjectClass  evoXYSymmetry;
};

class SVShapeMaskHelperClass : public SvIe::SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVShapeMaskHelperClass );
public:
	SVShapeMaskHelperClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SHAPE_MASK_HELPER );
	virtual ~SVShapeMaskHelperClass();
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	//First definition of onRun with this parameter (SVTaskObjectClass has different parameter), so override is not possible
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	HRESULT Refresh();

	enum ShapeTypeEnum
	{
		SVMaskShapeTypeInvalid            = 0,
		SVMaskShapeTypeRectangle          = 1,
		SVMaskShapeTypeOval               = 2,
		SVMaskShapeTypeDoughnut           = 3,
		SVMaskShapeTypeSymmetricTrapezoid = 4,
	};

	ShapeTypeEnum GetShape();
	HRESULT SetProperties(const SVMaskShape::MapType& rMapProperties);	// sets value objects based on properties
	HRESULT GetProperties(SVMaskShape::MapType& rMapProperties);	// gets properties based on value objects
	HRESULT GetFillProperties(SVMaskFillPropertiesStruct& rFillStruct) const;
	bool    IsAutoResize() const;
	HRESULT SetAutoResize( bool bAutoResize );

	HRESULT GetCancelData(SvIe::SVInputRequestStructMap& rData);

	const SVMaskShapeCancelData& getCancelData() const { return m_Data; }

protected:
	SVMaskShapeCancelData        m_Data;

	SVMaskShape*           m_pShape;

private:
	void init();

	friend class SVMaskShapeEditorDlg;
	friend class SVUserMaskOperatorClass;
};

} //namespace SvOp
