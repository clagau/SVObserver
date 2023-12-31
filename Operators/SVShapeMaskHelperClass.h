//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVValueObjectLibrary\SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary\SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma endregion Includes


namespace SvIe
{
struct SVInputRequestStructMap;
}

namespace SvOp
{

class SVShapeMaskHelperClass : public SvIe::SVTaskObjectClass
{
	SV_DECLARE_CLASS
public:
	SVShapeMaskHelperClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SHAPE_MASK_HELPER );
	virtual ~SVShapeMaskHelperClass();
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	void createImageObject(bool useImageObject);

	HRESULT Refresh(SvIe::RunStatus* pRunStatus = nullptr);

	enum ShapeTypeEnum
	{
		SVMaskShapeTypeInvalid            = 0,
		SVMaskShapeTypeRectangle          = 1,
		SVMaskShapeTypeOval               = 2,
		SVMaskShapeTypeDoughnut           = 3,
		SVMaskShapeTypeSymmetricTrapezoid = 4,
	};

	ShapeTypeEnum GetShape();
	SvIe::SVImageClass* getImage() const { return m_pImage.get(); };

protected:
	SvVol::SVBoolValueObjectClass       m_bvoAutoResize;
	SvVol::SVEnumerateValueObjectClass  m_evoShapeType;
	SvVol::SVEnumerateValueObjectClass  m_evoMaskArea;
	SvVol::LinkedValue       m_voCenterX;
	SvVol::LinkedValue       m_voCenterY;
	SvVol::LinkedValue       m_voWidth;
	SvVol::LinkedValue       m_voHeight;
	SvVol::LinkedValue       m_voSideThickness;
	SvVol::LinkedValue       m_voTopBottomThickness;
	SvVol::LinkedValue       m_voOffset;
	SvVol::SVEnumerateValueObjectClass  m_evoXYSymmetry;

	SVMaskShape* m_pShape {nullptr};
	std::unique_ptr<SvIe::SVImageClass> m_pImage{ nullptr };

private:
	void init();

	friend class SVMaskShapeEditorDlg;
	friend class SVUserMaskOperatorClass;
};

} //namespace SvOp
