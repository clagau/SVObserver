//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShapeMaskHelperClass
//* .File Name       : $Workfile:   SVShapeMaskHelperClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   13 Aug 2013 09:28:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "SVShapeMaskHelperClass.h"
#include "SVUserMaskOperatorClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVShapeMaskHelperClass, SvPb::ShapeMaskHelperClassId)

SVShapeMaskHelperClass::SVShapeMaskHelperClass( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectClass ( POwner, StringResourceID )
{
	init();
}

void SVShapeMaskHelperClass::init()
{
	m_pShape = nullptr;
	m_bUseOverlays = false;

	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVShapeMaskHelperObjectType;

	// do these eventually be changed to ResetItemOwner?
	RegisterEmbeddedObject( &m_bvoAutoResize, SvPb::ShapeMaskAutoResizeEId, IDS_OBJECTNAME_SHAPE_MASK_AUTO_RESIZE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_evoShapeType, SvPb::ShapeMaskTypeEId, IDS_OBJECTNAME_SHAPE_MASK_TYPE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_evoMaskArea, SvPb::ShapeMaskMaskAreaEId, IDS_OBJECTNAME_SHAPE_MASK_MASK_AREA, false, SvOi::SVResetItemTool );
	registerEmbeddedLinkedValue(&m_voCenterX, SvPb::ShapeMaskPropertyCenterXEId, SvPb::ShapeMaskPropertyCenterXLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_X, _variant_t(50L));
	registerEmbeddedLinkedValue( &m_voCenterY, SvPb::ShapeMaskPropertyCenterYEId, SvPb::ShapeMaskPropertyCenterYLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_Y, _variant_t(50L));
	registerEmbeddedLinkedValue( &m_voWidth, SvPb::ShapeMaskPropertyWidthEId, SvPb::ShapeMaskPropertyWidthLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_WIDTH, _variant_t(80L));
	registerEmbeddedLinkedValue( &m_voHeight, SvPb::ShapeMaskPropertyHeightEId, SvPb::ShapeMaskPropertyHeightLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_HEIGHT, _variant_t(80L));
	registerEmbeddedLinkedValue( &m_voSideThickness, SvPb::ShapeMaskPropertySideThicknessEId, SvPb::ShapeMaskPropertySideThicknessLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_SIDE_THICKNESS, _variant_t(20L));
	registerEmbeddedLinkedValue( &m_voTopBottomThickness, SvPb::ShapeMaskPropertyTopBottomThicknessEId, SvPb::ShapeMaskPropertyTopBottomThicknessLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_TOP_BOTTOM_THICKNESS, _variant_t(20L));
	registerEmbeddedLinkedValue( &m_voOffset, SvPb::ShapeMaskPropertyOffsetEId, SvPb::ShapeMaskPropertyOffsetLinkEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_OFFSET, _variant_t(20L));
	RegisterEmbeddedObject( &m_evoXYSymmetry, SvPb::ShapeMaskPropertySymmetryOrientationEId, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_XY_SYMMETRY, false, SvOi::SVResetItemTool );

	m_bvoAutoResize.SetDefaultValue( false, false );

	SvOi::NameValueVector EnumVector
	{
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_RECTANGLE), SVMaskShapeTypeRectangle},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_OVAL), SVMaskShapeTypeOval},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_DOUGHNUT), SVMaskShapeTypeDoughnut},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_SYMMETRIC_TRAPEZOID), SVMaskShapeTypeSymmetricTrapezoid}
	};
	m_evoShapeType.SetEnumTypes(EnumVector);
	m_evoShapeType.SetDefaultValue(SVMaskShapeTypeRectangle);

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("Inside Shape"), 1 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Outside Shape"), 2 });
	m_evoMaskArea.SetEnumTypes(EnumVector);
	m_evoMaskArea.SetDefaultValue(2);

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("Vertical Axis Top"), 0 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Vertical Axis Bottom"), 180 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Horizontal Axis Left"), -90 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Horizontal Axis Right"), 90 });
	m_evoXYSymmetry.SetEnumTypes(EnumVector);
	m_evoXYSymmetry.SetDefaultValue(0l);
}

SVShapeMaskHelperClass::~SVShapeMaskHelperClass()
{
	if ( m_pShape )
		delete m_pShape;
}

bool SVShapeMaskHelperClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	const UINT cAttributes = SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::remotelySetable | SvPb::setableOnline | SvPb::audittrail;
	m_bvoAutoResize.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_evoShapeType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_evoMaskArea.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_voCenterX.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voCenterY.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voWidth.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voHeight.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voSideThickness.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voTopBottomThickness.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_voOffset.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_evoXYSymmetry.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	

	// Set / Reset Printable Flag

	m_isCreated = bOk;

	return m_isCreated;
}

bool SVShapeMaskHelperClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (nullptr != m_pShape )
	{
		delete m_pShape;
	}

	ShapeTypeEnum eShapeType = GetShape();
	switch ( eShapeType )
	{
		case SVMaskShapeTypeOval:
			m_pShape = new SVMaskShapeOval;
			break;
		case SVMaskShapeTypeDoughnut:
			m_pShape = new SVMaskShapeDoughnut;
			break;
		case SVMaskShapeTypeSymmetricTrapezoid:
			m_pShape = new SVMaskShapeSymmetricTrapezoid;
			break;
		case SVMaskShapeTypeRectangle:	// fall through
		default:
			m_pShape = new SVMaskShapeRectangle;
	}

	SVUserMaskOperatorClass* pMaskOperator = dynamic_cast<SVUserMaskOperatorClass*> ( GetParent() );
	assert( pMaskOperator );
	if (nullptr != pMaskOperator )
	{
		m_pShape->SetImageInfo( pMaskOperator->m_MaskBufferInfo );

		// Set value object attributes
		DWORD dwMaskType;
		pMaskOperator->m_dwvoMaskType.GetValue( dwMaskType );
		
		const bool bIsShape = dwMaskType == SVUserMaskOperatorClass::MASK_TYPE_SHAPE;
		const bool bIsDoughnut = eShapeType == SVMaskShapeTypeDoughnut;
		const bool bIsTrapezoid = eShapeType == SVMaskShapeTypeSymmetricTrapezoid;

		SvOi::SetAttributeType AddRemoveType = bIsShape ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_bvoAutoResize.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_evoShapeType.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_evoMaskArea.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_voCenterX.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_voCenterY.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_voWidth.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_voHeight.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

		AddRemoveType = bIsShape && bIsDoughnut ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_voSideThickness.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_voTopBottomThickness.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

		AddRemoveType = bIsShape && bIsTrapezoid ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_voOffset.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
		m_evoXYSymmetry.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

bool SVShapeMaskHelperClass::onRun( bool, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& , SvStl::MessageContainerVector * )
{
	return TRUE;	// what do we want to do here for the status?
}

SVShapeMaskHelperClass::ShapeTypeEnum SVShapeMaskHelperClass::GetShape( )
{
	long lValue;
	m_evoShapeType.GetValue( lValue );
	return static_cast <ShapeTypeEnum> (lValue);
}

HRESULT SVShapeMaskHelperClass::Refresh()
{
	HRESULT hr = S_FALSE;
	if ( m_pShape )
	{
		hr = S_OK;
		
		// set shape properties based on value objects
		ShapeTypeEnum eShapeType = GetShape();

		SVMaskShape::MapType mapProperties;
		m_pShape->GetProperties( mapProperties );

		_variant_t variantValue;
		m_voCenterX.GetValue(variantValue);
		mapProperties[SvPb::ShapeMaskPropertyCenterXEId] = variantValue;

		m_voCenterY.GetValue(variantValue);
		mapProperties[SvPb::ShapeMaskPropertyCenterYEId] = variantValue;
		
		m_voWidth.GetValue(variantValue);
		mapProperties[SvPb::ShapeMaskPropertyWidthEId] = variantValue;
		
		m_voHeight.GetValue(variantValue);
		mapProperties[SvPb::ShapeMaskPropertyHeightEId] = variantValue;

		if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
		{
			m_voSideThickness.GetValue(variantValue);
			mapProperties[SvPb::ShapeMaskPropertySideThicknessEId] = variantValue;
			m_voTopBottomThickness.GetValue(variantValue);
			mapProperties[SvPb::ShapeMaskPropertyTopBottomThicknessEId] = variantValue;
		}
		else if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
		{
			m_voOffset.GetValue(variantValue);
			mapProperties[SvPb::ShapeMaskPropertyOffsetEId] = variantValue;
			long lValue;
			m_evoXYSymmetry.GetValue( lValue );
			mapProperties[SvPb::ShapeMaskPropertySymmetryOrientationEId] = lValue;
		}
		
		SVUserMaskOperatorClass* pMaskOperator = dynamic_cast<SVUserMaskOperatorClass*> ( GetParent() );
		assert( pMaskOperator );
		if ( pMaskOperator && nullptr != pMaskOperator->m_MaskBufferHandlePtr )
		{
			////////////////////
			// FILL PROPERTIES
			SVMaskFillPropertiesStruct svFillStruct;
			long lValue;
			m_evoMaskArea.GetValue( lValue );
			svFillStruct.bMaskInsideShape = (lValue != 2);
			pMaskOperator->m_evoFillArea.GetValue( lValue );
			svFillStruct.bFillMaskArea = ( lValue == 0 );
			pMaskOperator->m_lvoFillColor.GetValue( lValue );
			lValue = std::max< long >(   0, lValue );
			lValue = std::min< long >( 255, lValue );
			svFillStruct.rgbFillColor = RGB( lValue, lValue, lValue );

			BOOL bAutoResize = false;
			m_bvoAutoResize.GetValue( bAutoResize );
			m_pShape->SetAutoResize( bAutoResize ? true : false );
			m_pShape->SetImageInfo( pMaskOperator->m_MaskBufferInfo );
			m_pShape->SetProperties( mapProperties );

			// render based on new values
			m_pShape->Refresh(svFillStruct);

			// draw the mask to the MIL buffer
			m_pShape->Draw(pMaskOperator->m_MaskBufferHandlePtr->GetBuffer() );
		}
	}
	return hr;
}
} //namespace SvOp
