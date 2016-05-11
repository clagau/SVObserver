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

#include "stdafx.h"
#include "SVShapeMaskHelperClass.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVUserMaskOperatorClass.h"
//Moved to precompiled header: #include <algorithm>
#include "SVImageLibrary/SVImageBufferHandleImage.h"

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

	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVShapeMaskHelperObjectType;

	// do these eventually be changed to ResetItemOwner?
	RegisterEmbeddedObject( &m_Data.bvoAutoResize, SVShapeMaskAutoResizeGuid, IDS_OBJECTNAME_SHAPE_MASK_AUTO_RESIZE, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoShapeType, SVShapeMaskTypeGuid, IDS_OBJECTNAME_SHAPE_MASK_TYPE, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoMaskArea, SVShapeMaskMaskAreaGuid, IDS_OBJECTNAME_SHAPE_MASK_MASK_AREA, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoCenterX, SVShapeMaskPropertyCenterXGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoCenterY, SVShapeMaskPropertyCenterYGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_CENTER_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoWidth, SVShapeMaskPropertyWidthGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_WIDTH, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoHeight, SVShapeMaskPropertyHeightGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_HEIGHT, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoSideThickness, SVShapeMaskPropertySideThicknessGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_SIDE_THICKNESS, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoTopBottomThickness, SVShapeMaskPropertyTopBottomThicknessGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_TOP_BOTTOM_THICKNESS, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.lvoOffset, SVShapeMaskPropertyOffsetGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_OFFSET, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Data.evoXYSymmetry, SVShapeMaskPropertySymmetryOrientationGuid, IDS_OBJECTNAME_SHAPE_MASK_PROPERTY_XY_SYMMETRY, false, SVResetItemTool );

	m_Data.bvoAutoResize.SetDefaultValue( false, false );

	SVEnumerateVector vec;
	vec.push_back( SVEnumeratePair(SVLoadString(IDS_OBJECTNAME_SHAPE_TYPE_RECTANGLE), SVMaskShapeTypeRectangle ) );
	vec.push_back( SVEnumeratePair(SVLoadString(IDS_OBJECTNAME_SHAPE_TYPE_OVAL), SVMaskShapeTypeOval ) );
	vec.push_back( SVEnumeratePair(SVLoadString(IDS_OBJECTNAME_SHAPE_TYPE_DOUGHNUT), SVMaskShapeTypeDoughnut ) );
	vec.push_back( SVEnumeratePair(SVLoadString(IDS_OBJECTNAME_SHAPE_TYPE_SYMMETRIC_TRAPEZOID), SVMaskShapeTypeSymmetricTrapezoid ) );
	m_Data.evoShapeType.SetEnumTypes(vec);
	m_Data.evoShapeType.SetDefaultValue(SVMaskShapeTypeRectangle, TRUE);

	vec.clear();
	vec.push_back( SVEnumeratePair(_T( "Inside Shape" ), 1 ) );
	vec.push_back( SVEnumeratePair(_T( "Outside Shape" ), 2 ) );
	m_Data.evoMaskArea.SetEnumTypes(vec);
	m_Data.evoMaskArea.SetDefaultValue(2, TRUE);

	vec.clear();
	vec.push_back( SVEnumeratePair(_T( "Vertical Axis Top" ), 0 ) );
	vec.push_back( SVEnumeratePair(_T( "Vertical Axis Bottom" ), 180 ) );
	vec.push_back( SVEnumeratePair(_T( "Horizontal Axis Left" ), -90 ) );
	vec.push_back( SVEnumeratePair(_T( "Horizontal Axis Right" ), 90 ) );
	m_Data.evoXYSymmetry.SetEnumTypes(vec);
	m_Data.evoXYSymmetry.SetDefaultValue((long)0, TRUE);

	m_Data.lvoCenterX.SetDefaultValue( 50, TRUE );
	m_Data.lvoCenterY.SetDefaultValue( 50, TRUE );
	m_Data.lvoWidth.SetDefaultValue( 80, TRUE );
	m_Data.lvoHeight.SetDefaultValue( 80, TRUE );
	m_Data.lvoSideThickness.SetDefaultValue( 20, TRUE );
	m_Data.lvoTopBottomThickness.SetDefaultValue( 20, TRUE );
	m_Data.lvoOffset.SetDefaultValue( 20, TRUE );



	// Set default inputs and outputs
	addDefaultInputObjects();

}

SVShapeMaskHelperClass::~SVShapeMaskHelperClass()
{
	if ( m_pShape )
		delete m_pShape;
}

BOOL SVShapeMaskHelperClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	const DWORD dwAttributes = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_Data.bvoAutoResize.ObjectAttributesAllowedRef()         = dwAttributes;
	m_Data.evoShapeType.ObjectAttributesAllowedRef()          = dwAttributes;
	m_Data.evoMaskArea.ObjectAttributesAllowedRef()           = dwAttributes;
	m_Data.lvoCenterX.ObjectAttributesAllowedRef()            = dwAttributes | SV_SELECTABLE_FOR_EQUATION; 
	m_Data.lvoCenterY.ObjectAttributesAllowedRef()            = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.lvoWidth.ObjectAttributesAllowedRef()              = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.lvoHeight.ObjectAttributesAllowedRef()             = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.lvoSideThickness.ObjectAttributesAllowedRef()      = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.lvoTopBottomThickness.ObjectAttributesAllowedRef() = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.lvoOffset.ObjectAttributesAllowedRef()             = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	m_Data.evoXYSymmetry.ObjectAttributesAllowedRef()         = dwAttributes | SV_SELECTABLE_FOR_EQUATION;
	

	// Set / Reset Printable Flag

	m_isCreated = bOk;

	return m_isCreated;
}

HRESULT SVShapeMaskHelperClass::ResetObject()
{
	HRESULT hr = S_OK;

	if ( m_pShape )
		delete m_pShape;

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

	SVUserMaskOperatorClass* pMaskOperator = dynamic_cast<SVUserMaskOperatorClass*> ( GetOwner() );
	ASSERT( pMaskOperator );
	if ( pMaskOperator )
	{
		m_pShape->SetImageInfo( pMaskOperator->m_MaskBufferInfo );

		// Set value object attributes
		DWORD dwMaskType;
		pMaskOperator->m_Data.dwvoMaskType.GetValue( dwMaskType );
		
		const DWORD dwAttributes = SV_PRINTABLE;	// only change the printing flag; all other flags remain always on (remotely settable, etc.)
		const bool bIsShape = dwMaskType == SVUserMaskOperatorClass::MASK_TYPE_SHAPE;
		const bool bIsDoughnut = eShapeType == SVMaskShapeTypeDoughnut;
		const bool bIsTrapezoid = eShapeType == SVMaskShapeTypeSymmetricTrapezoid;

		SetBits( m_Data.bvoAutoResize.ObjectAttributesSetRef().operator volatile UINT&(),         dwAttributes, bIsShape );
		SetBits( m_Data.evoShapeType.ObjectAttributesSetRef().operator volatile UINT&(),          dwAttributes, bIsShape );
		SetBits( m_Data.evoMaskArea.ObjectAttributesSetRef().operator volatile UINT&(),           dwAttributes, bIsShape );
		SetBits( m_Data.lvoCenterX.ObjectAttributesSetRef().operator volatile UINT&(),            dwAttributes, bIsShape );
		SetBits( m_Data.lvoCenterY.ObjectAttributesSetRef().operator volatile UINT&(),            dwAttributes, bIsShape );
		SetBits( m_Data.lvoWidth.ObjectAttributesSetRef().operator volatile UINT&(),              dwAttributes, bIsShape );
		SetBits( m_Data.lvoHeight.ObjectAttributesSetRef().operator volatile UINT&(),             dwAttributes, bIsShape );

		SetBits( m_Data.lvoSideThickness.ObjectAttributesSetRef().operator volatile UINT&(),      dwAttributes, bIsShape && bIsDoughnut );
		SetBits( m_Data.lvoTopBottomThickness.ObjectAttributesSetRef().operator volatile UINT&(), dwAttributes, bIsShape && bIsDoughnut );
		SetBits( m_Data.lvoOffset.ObjectAttributesSetRef().operator volatile UINT&(),             dwAttributes, bIsShape && bIsTrapezoid );
		SetBits( m_Data.evoXYSymmetry.ObjectAttributesSetRef().operator volatile UINT&(),         dwAttributes, bIsShape && bIsTrapezoid );

		// ARGH!
		SetBits( m_Data.bvoAutoResize.ObjectAttributesAllowedRef(),         dwAttributes, bIsShape );
		SetBits( m_Data.evoShapeType.ObjectAttributesAllowedRef(),          dwAttributes, bIsShape );
		SetBits( m_Data.evoMaskArea.ObjectAttributesAllowedRef(),           dwAttributes, bIsShape );
		SetBits( m_Data.lvoCenterX.ObjectAttributesAllowedRef(),            dwAttributes, bIsShape );
		SetBits( m_Data.lvoCenterY.ObjectAttributesAllowedRef(),            dwAttributes, bIsShape );
		SetBits( m_Data.lvoWidth.ObjectAttributesAllowedRef(),              dwAttributes, bIsShape );
		SetBits( m_Data.lvoHeight.ObjectAttributesAllowedRef(),             dwAttributes, bIsShape );

		SetBits( m_Data.lvoSideThickness.ObjectAttributesAllowedRef(),      dwAttributes, bIsShape && bIsDoughnut );
		SetBits( m_Data.lvoTopBottomThickness.ObjectAttributesAllowedRef(), dwAttributes, bIsShape && bIsDoughnut );
		SetBits( m_Data.lvoOffset.ObjectAttributesAllowedRef(),             dwAttributes, bIsShape && bIsTrapezoid );
		SetBits( m_Data.evoXYSymmetry.ObjectAttributesAllowedRef(),         dwAttributes, bIsShape && bIsTrapezoid );
	}	

	SVTaskObjectClass::ResetObject();	// ignore return value.

	return hr;
}

BOOL SVShapeMaskHelperClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{
	return TRUE;	// what do we want to do here for the status?
}

HRESULT SVShapeMaskHelperClass::SetShape( ShapeTypeEnum eShapeType )
{
	HRESULT hr = m_Data.evoShapeType.SetValue( 1, (long) eShapeType );
	return hr;
}

SVShapeMaskHelperClass::ShapeTypeEnum SVShapeMaskHelperClass::GetShape( )
{
	long lValue;
	m_Data.evoShapeType.GetValue( lValue );
	return static_cast <ShapeTypeEnum> (lValue);
}

HRESULT SVShapeMaskHelperClass::SetProperties( const SVMaskShape::MapType& p_mapProperties )	// sets value objects based on properties
{
	HRESULT hr = S_OK;

	SVMaskShape::MapType l_mapProperties(p_mapProperties);

	m_Data.lvoCenterX.SetValue( 1, l_mapProperties[ SVShapeMaskPropertyCenterXGuid ].value );
	m_Data.lvoCenterY.SetValue( 1, l_mapProperties[ SVShapeMaskPropertyCenterYGuid ].value );
	m_Data.lvoWidth.SetValue  ( 1, l_mapProperties[ SVShapeMaskPropertyWidthGuid ].value   );
	m_Data.lvoHeight.SetValue ( 1, l_mapProperties[ SVShapeMaskPropertyHeightGuid ].value  );

	ShapeTypeEnum eShapeType;
	long lShapeType = SVMaskShapeTypeInvalid;
	m_Data.evoShapeType.GetValue( lShapeType );
	eShapeType = static_cast <ShapeTypeEnum> (lShapeType);

	// allow these values to be set regardless of current shape.
	// This will allow SIAC to to a set on all properties without worrying about order.

	SVMaskShape::MapType::iterator iter;
	if ( ( iter = l_mapProperties.find( SVShapeMaskPropertySideThicknessGuid )) != l_mapProperties.end() )
	{
		m_Data.lvoSideThickness.SetValue     ( 1, iter->second.value );
	}
	if ( ( iter = l_mapProperties.find( SVShapeMaskPropertyTopBottomThicknessGuid )) != l_mapProperties.end() )
	{
		m_Data.lvoTopBottomThickness.SetValue( 1, iter->second.value );
	}
	
	if ( ( iter = l_mapProperties.find( SVShapeMaskPropertyOffsetGuid ) ) != l_mapProperties.end() )
	{
		m_Data.lvoOffset.SetValue    ( 1, iter->second.value );
	}
	if ( ( iter = l_mapProperties.find( SVShapeMaskPropertySymmetryOrientationGuid ) ) != l_mapProperties.end() )
	{
		m_Data.evoXYSymmetry.SetValue( 1, iter->second.value );
	}

	return hr;	
}

HRESULT SVShapeMaskHelperClass::GetProperties( SVMaskShape::MapType& rmapProperties )	// gets properties based on value objects
{
	HRESULT hr = S_FALSE;

	if ( m_pShape )
	{
		hr = S_OK;
		m_pShape->GetProperties( rmapProperties );

		ShapeTypeEnum eShapeType;
		long lShapeType = SVMaskShapeTypeInvalid;
		m_Data.evoShapeType.GetValue( lShapeType );
		eShapeType = static_cast <ShapeTypeEnum> (lShapeType);

		long lValue;
		m_Data.lvoCenterX.GetValue( lValue );
		rmapProperties[ SVShapeMaskPropertyCenterXGuid ] = lValue;

		m_Data.lvoCenterY.GetValue( lValue );
		rmapProperties[ SVShapeMaskPropertyCenterYGuid ] = lValue;
		
		m_Data.lvoWidth.GetValue( lValue );
		rmapProperties[ SVShapeMaskPropertyWidthGuid ] = lValue;
		
		m_Data.lvoHeight.GetValue( lValue );
		rmapProperties[ SVShapeMaskPropertyHeightGuid ] = lValue;

		if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeDoughnut )
		{
			m_Data.lvoSideThickness.GetValue( lValue );
			rmapProperties[ SVShapeMaskPropertySideThicknessGuid ] = lValue;
			m_Data.lvoTopBottomThickness.GetValue( lValue );
			rmapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ] = lValue;
		}
		else if ( eShapeType == SVShapeMaskHelperClass::SVMaskShapeTypeSymmetricTrapezoid )
		{
			m_Data.lvoOffset.GetValue( lValue );
			rmapProperties[ SVShapeMaskPropertyOffsetGuid ] = lValue;
			m_Data.evoXYSymmetry.GetValue( lValue );
			rmapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ] = lValue;
		}
	}
	return hr;
}

HRESULT SVShapeMaskHelperClass::GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct ) const
{
	long lValue=0;
	m_Data.evoMaskArea.GetValue( lValue );
	rsvFillStruct.bMaskInsideShape = (lValue == 1 ? true : false);
	return S_OK;
}

bool SVShapeMaskHelperClass::IsAutoResize() const
{
	bool bAutoResize = false;
	HRESULT hr = m_Data.bvoAutoResize.GetValue( bAutoResize );
	return bAutoResize;
}

HRESULT SVShapeMaskHelperClass::SetAutoResize( bool bAutoResize )
{
	HRESULT hr = m_Data.bvoAutoResize.SetValue( 1, bAutoResize );
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
		
		SVUserMaskOperatorClass* pMaskOperator = dynamic_cast<SVUserMaskOperatorClass*> ( GetOwner() );
		ASSERT( pMaskOperator );
		if ( pMaskOperator && !( pMaskOperator->m_MaskBufferHandlePtr.empty() ) )
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

			bool bAutoResize = false;
			m_Data.bvoAutoResize.GetValue( bAutoResize );
			m_pShape->SetAutoResize( bAutoResize );

			HRESULT hrTemp = S_OK;

			hrTemp = m_pShape->SetImageInfo( pMaskOperator->m_MaskBufferInfo );

			hrTemp = m_pShape->SetProperties( mapProperties );

			// render based on new values
			hrTemp = m_pShape->Refresh();

			SVImageBufferHandleImage l_MilHandle;
			pMaskOperator->m_MaskBufferHandlePtr->GetData( l_MilHandle );

			// draw the mask to the MIL buffer
			hrTemp = m_pShape->Draw( l_MilHandle.GetBuffer() );
		}
	}
	return hr;
}

// ISVCancel
bool SVShapeMaskHelperClass::CanCancel()
{
	return true;
}

// ISVCancel
HRESULT SVShapeMaskHelperClass::GetCancelData(SVCancelData*& p_rpData)
{
	HRESULT hr = S_OK;
	SVMaskShapeCancelData* pData = new SVMaskShapeCancelData;
	p_rpData = pData;

	pData->bvoAutoResize = m_Data.bvoAutoResize;
	pData->evoShapeType = m_Data.evoShapeType;
	pData->evoMaskArea  = m_Data.evoMaskArea;

	hr = GetProperties( pData->mapProperties );

	return hr;
}

// ISVCancel
HRESULT SVShapeMaskHelperClass::SetCancelData(SVCancelData* p_pData)
{
	HRESULT hr = SV_FALSE;

	SVMaskShapeCancelData* pData = dynamic_cast <SVMaskShapeCancelData*> (p_pData);
	ASSERT( pData );
	if ( pData )
	{
		hr = S_OK;

		m_Data.bvoAutoResize = pData->bvoAutoResize;
		m_Data.evoShapeType = pData->evoShapeType;
		m_Data.evoMaskArea  = pData->evoMaskArea;

		hr = SetProperties( pData->mapProperties );
	}

	return hr;
}

// ISVCancel2
HRESULT SVShapeMaskHelperClass::GetCancelData(SVInputRequestStructMap& rMap)
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

