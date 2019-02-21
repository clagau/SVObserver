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
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVShapeMaskHelperClass, SVShapeMaskHelperClassGuid )

SVShapeMaskHelperClass::SVShapeMaskHelperClass( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectClass ( POwner, StringResourceID )
{
	init();
}

void SVShapeMaskHelperClass::init()
{
	m_pShape = nullptr;
	m_bUseOverlays = false;

	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVShapeMaskHelperObjectType;

	// do these eventually be changed to ResetItemOwner?
	RegisterEmbeddedObject( &m_Data.bvoAutoResize, SVShapeMaskAutoResizeGuid, IDS_OBJECTNAME_SHAPE_MASK_AUTO_RESIZE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoShapeType, SVShapeMaskTypeGuid, IDS_OBJECTNAME_SHAPE_MASK_TYPE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoMaskArea, SVShapeMaskMaskAreaGuid, IDS_OBJECTNAME_SHAPE_MASK_MASK_AREA, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoCenterX, SVShapeMaskPropertyCenterXGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoCenterY, SVShapeMaskPropertyCenterYGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoWidth, SVShapeMaskPropertyWidthGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_WIDTH, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoHeight, SVShapeMaskPropertyHeightGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_HEIGHT, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoSideThickness, SVShapeMaskPropertySideThicknessGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_SIDE_THICKNESS, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoTopBottomThickness, SVShapeMaskPropertyTopBottomThicknessGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_TOP_BOTTOM_THICKNESS, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoOffset, SVShapeMaskPropertyOffsetGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_OFFSET, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoXYSymmetry, SVShapeMaskPropertySymmetryOrientationGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_XY_SYMMETRY, false, SvOi::SVResetItemTool );

	m_Data.bvoAutoResize.SetDefaultValue( false, false );

	SvOi::NameValueVector EnumVector
	{
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_RECTANGLE), SVMaskShapeTypeRectangle},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_OVAL), SVMaskShapeTypeOval},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_DOUGHNUT), SVMaskShapeTypeDoughnut},
		{SvUl::LoadStdString(IDS_OBJECTNAME_SHAPE_TYPE_SYMMETRIC_TRAPEZOID), SVMaskShapeTypeSymmetricTrapezoid}
	};
	m_Data.evoShapeType.SetEnumTypes(EnumVector);
	m_Data.evoShapeType.SetDefaultValue(SVMaskShapeTypeRectangle);

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("Inside Shape"), 1 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Outside Shape"), 2 });
	m_Data.evoMaskArea.SetEnumTypes(EnumVector);
	m_Data.evoMaskArea.SetDefaultValue(2);

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("Vertical Axis Top"), 0 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Vertical Axis Bottom"), 180 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Horizontal Axis Left"), -90 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Horizontal Axis Right"), 90 });
	m_Data.evoXYSymmetry.SetEnumTypes(EnumVector);
	m_Data.evoXYSymmetry.SetDefaultValue(0l);

	m_Data.lvoCenterX.SetDefaultValue( 50 );
	m_Data.lvoCenterY.SetDefaultValue( 50 );
	m_Data.lvoWidth.SetDefaultValue( 80 );
	m_Data.lvoHeight.SetDefaultValue( 80 );
	m_Data.lvoSideThickness.SetDefaultValue( 20 );
	m_Data.lvoTopBottomThickness.SetDefaultValue( 20 );
	m_Data.lvoOffset.SetDefaultValue( 20 );



	// Set default inputs and outputs
	addDefaultInputObjects();

}

SVShapeMaskHelperClass::~SVShapeMaskHelperClass()
{
	if ( m_pShape )
		delete m_pShape;
}

bool SVShapeMaskHelperClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	const UINT cAttributes = SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::remotelySetable | SvPb::setableOnline | SvPb::printable;
	m_Data.bvoAutoResize.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.evoShapeType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.evoMaskArea.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoCenterX.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoCenterY.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoWidth.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoHeight.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoSideThickness.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoTopBottomThickness.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.lvoOffset.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	m_Data.evoXYSymmetry.SetObjectAttributesAllowed( cAttributes | SvPb::selectableForEquation, SvOi::SetAttributeType::OverwriteAttribute );
	

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

	long lShapeType;
	m_Data.evoShapeType.GetValue( lShapeType );
	ShapeTypeEnum eShapeType = static_cast<ShapeTypeEnum> (lShapeType);
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
		pMaskOperator->m_Data.dwvoMaskType.GetValue( dwMaskType );
		
		const bool bIsShape = dwMaskType == SVUserMaskOperatorClass::MASK_TYPE_SHAPE;
		const bool bIsDoughnut = eShapeType == SVMaskShapeTypeDoughnut;
		const bool bIsTrapezoid = eShapeType == SVMaskShapeTypeSymmetricTrapezoid;

		SvOi::SetAttributeType AddRemoveType = bIsShape ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.bvoAutoResize.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.evoShapeType.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.evoMaskArea.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.lvoCenterX.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.lvoCenterY.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.lvoWidth.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.lvoHeight.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );

		AddRemoveType = bIsShape && bIsDoughnut ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.lvoSideThickness.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.lvoTopBottomThickness.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );

		AddRemoveType = bIsShape && bIsTrapezoid ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.lvoOffset.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
		m_Data.evoXYSymmetry.SetObjectAttributesAllowed( SvPb::printable, AddRemoveType );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

bool SVShapeMaskHelperClass::onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	return TRUE;	// what do we want to do here for the status?
}

HRESULT SVShapeMaskHelperClass::SetShape( ShapeTypeEnum eShapeType )
{
	HRESULT hr = m_Data.evoShapeType.SetValue(static_cast<long> (eShapeType));
	return hr;
}

SVShapeMaskHelperClass::ShapeTypeEnum SVShapeMaskHelperClass::GetShape( )
{
	long lValue;
	m_Data.evoShapeType.GetValue( lValue );
	return static_cast <ShapeTypeEnum> (lValue);
}

HRESULT SVShapeMaskHelperClass::SetProperties( const SVMaskShape::MapType& rMapProperties )	// sets value objects based on properties
{
	HRESULT hr = S_OK;

	SVMaskShape::MapType mapProperties(rMapProperties);

	m_Data.lvoCenterX.SetValue(mapProperties[SVShapeMaskPropertyCenterXGuid].value);
	m_Data.lvoCenterY.SetValue(mapProperties[SVShapeMaskPropertyCenterYGuid].value);
	m_Data.lvoWidth.SetValue  (mapProperties[SVShapeMaskPropertyWidthGuid].value);
	m_Data.lvoHeight.SetValue (mapProperties[SVShapeMaskPropertyHeightGuid].value);

	ShapeTypeEnum eShapeType;
	long lShapeType = SVMaskShapeTypeInvalid;
	m_Data.evoShapeType.GetValue( lShapeType );
	eShapeType = static_cast <ShapeTypeEnum> (lShapeType);

	// allow these values to be set regardless of current shape.
	// This will allow SIAC to to a set on all properties without worrying about order.

	SVMaskShape::MapType::iterator iter;
	if ( ( iter = mapProperties.find( SVShapeMaskPropertySideThicknessGuid )) != mapProperties.end() )
	{
		m_Data.lvoSideThickness.SetValue(iter->second.value);
	}
	if ( ( iter = mapProperties.find( SVShapeMaskPropertyTopBottomThicknessGuid )) != mapProperties.end() )
	{
		m_Data.lvoTopBottomThickness.SetValue(iter->second.value);
	}
	
	if ( ( iter = mapProperties.find( SVShapeMaskPropertyOffsetGuid ) ) != mapProperties.end() )
	{
		m_Data.lvoOffset.SetValue( iter->second.value);
	}
	if ( ( iter = mapProperties.find( SVShapeMaskPropertySymmetryOrientationGuid ) ) != mapProperties.end() )
	{
		m_Data.evoXYSymmetry.SetValue(iter->second.value);
	}

	return hr;	
}

HRESULT SVShapeMaskHelperClass::GetProperties(SVMaskShape::MapType& rMapProperties)	// gets properties based on value objects
{
	HRESULT hr = S_FALSE;

	if ( m_pShape )
	{
		hr = S_OK;
		m_pShape->GetProperties( rMapProperties );

		ShapeTypeEnum eShapeType;
		long lShapeType = SVMaskShapeTypeInvalid;
		m_Data.evoShapeType.GetValue( lShapeType );
		eShapeType = static_cast <ShapeTypeEnum> (lShapeType);

		long lValue;
		m_Data.lvoCenterX.GetValue( lValue );
		rMapProperties[ SVShapeMaskPropertyCenterXGuid ] = lValue;

		m_Data.lvoCenterY.GetValue( lValue );
		rMapProperties[ SVShapeMaskPropertyCenterYGuid ] = lValue;
		
		m_Data.lvoWidth.GetValue( lValue );
		rMapProperties[ SVShapeMaskPropertyWidthGuid ] = lValue;
		
		m_Data.lvoHeight.GetValue( lValue );
		rMapProperties[ SVShapeMaskPropertyHeightGuid ] = lValue;

		if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
		{
			m_Data.lvoSideThickness.GetValue( lValue );
			rMapProperties[ SVShapeMaskPropertySideThicknessGuid ] = lValue;
			m_Data.lvoTopBottomThickness.GetValue( lValue );
			rMapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ] = lValue;
		}
		else if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
		{
			m_Data.lvoOffset.GetValue( lValue );
			rMapProperties[ SVShapeMaskPropertyOffsetGuid ] = lValue;
			m_Data.evoXYSymmetry.GetValue( lValue );
			rMapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ] = lValue;
		}
	}
	return hr;
}

HRESULT SVShapeMaskHelperClass::GetFillProperties(SVMaskFillPropertiesStruct& rFillStruct ) const
{
	long lValue=0;
	m_Data.evoMaskArea.GetValue( lValue );
	rFillStruct.bMaskInsideShape = (lValue == 1 ? true : false);
	return S_OK;
}

bool SVShapeMaskHelperClass::IsAutoResize() const
{
	BOOL bAutoResize = false;
	HRESULT hr = m_Data.bvoAutoResize.GetValue( bAutoResize );
	return bAutoResize ? true : false;
}

HRESULT SVShapeMaskHelperClass::SetAutoResize( bool bAutoResize )
{
	HRESULT hr = m_Data.bvoAutoResize.SetValue(BOOL(bAutoResize));
	return hr;
}

HRESULT SVShapeMaskHelperClass::Refresh()
{
	HRESULT hr = S_FALSE;
	if ( m_pShape )
	{
		hr = S_OK;
		
		// set shape properties based on value objects
		ShapeTypeEnum eShapeType;
		long lShapeType = SVMaskShapeTypeInvalid;
		m_Data.evoShapeType.GetValue( lShapeType );
		eShapeType = static_cast <ShapeTypeEnum> (lShapeType);

		SVMaskShape::MapType mapProperties;
		m_pShape->GetProperties( mapProperties );

		long lValue;
		m_Data.lvoCenterX.GetValue( lValue );
		mapProperties[ SVShapeMaskPropertyCenterXGuid ] = lValue;

		m_Data.lvoCenterY.GetValue( lValue );
		mapProperties[ SVShapeMaskPropertyCenterYGuid ] = lValue;
		
		m_Data.lvoWidth.GetValue( lValue );
		mapProperties[ SVShapeMaskPropertyWidthGuid ] = lValue;
		
		m_Data.lvoHeight.GetValue( lValue );
		mapProperties[ SVShapeMaskPropertyHeightGuid ] = lValue;

		if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
		{
			m_Data.lvoSideThickness.GetValue( lValue );
			mapProperties[ SVShapeMaskPropertySideThicknessGuid ] = lValue;
			m_Data.lvoTopBottomThickness.GetValue( lValue );
			mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ] = lValue;
		}
		else if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
		{
			m_Data.lvoOffset.GetValue( lValue );
			mapProperties[ SVShapeMaskPropertyOffsetGuid ] = lValue;
			m_Data.evoXYSymmetry.GetValue( lValue );
			mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ] = lValue;
		}
		
		SVUserMaskOperatorClass* pMaskOperator = dynamic_cast<SVUserMaskOperatorClass*> ( GetParent() );
		assert( pMaskOperator );
		if ( pMaskOperator && nullptr != pMaskOperator->m_MaskBufferHandlePtr )
		{
			////////////////////
			// FILL PROPERTIES
			SVMaskFillPropertiesStruct svFillStruct;

			m_Data.evoMaskArea.GetValue( lValue );
			svFillStruct.bMaskInsideShape = (lValue != 2);
			pMaskOperator->m_Data.evoFillArea.GetValue( lValue );
			svFillStruct.bFillMaskArea = ( lValue == 0 );
			pMaskOperator->m_Data.lvoFillColor.GetValue( lValue );
			lValue = std::max< long >(   0, lValue );
			lValue = std::min< long >( 255, lValue );
			svFillStruct.rgbFillColor = RGB( lValue, lValue, lValue );

			m_pShape->SetFillProperties( svFillStruct );
			////////////////////

			BOOL bAutoResize = false;
			m_Data.bvoAutoResize.GetValue( bAutoResize );
			m_pShape->SetAutoResize( bAutoResize ? true : false );

			HRESULT hrTemp = S_OK;

			hrTemp = m_pShape->SetImageInfo( pMaskOperator->m_MaskBufferInfo );

			hrTemp = m_pShape->SetProperties( mapProperties );

			// render based on new values
			hrTemp = m_pShape->Refresh();

			// draw the mask to the MIL buffer
			hrTemp = m_pShape->Draw(pMaskOperator->m_MaskBufferHandlePtr->GetBuffer() );
		}
	}
	return hr;
}

HRESULT SVShapeMaskHelperClass::GetCancelData(SvIe::SVInputRequestStructMap& rMap)
{
	HRESULT hr = S_OK;

	rMap.Add( &m_Data.bvoAutoResize );
	rMap.Add( &m_Data.evoShapeType );
	rMap.Add( &m_Data.evoMaskArea );

	rMap.Add( &m_Data.lvoCenterX );
	rMap.Add( &m_Data.lvoCenterY );
	rMap.Add( &m_Data.lvoWidth );
	rMap.Add( &m_Data.lvoHeight );
	rMap.Add( &m_Data.lvoSideThickness );
	rMap.Add( &m_Data.lvoTopBottomThickness );
	rMap.Add( &m_Data.lvoOffset );
	rMap.Add( &m_Data.evoXYSymmetry );

	return hr;
}

} //namespace SvOp