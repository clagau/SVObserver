// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeProcessingClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.5  $
// * .Check In Date   : $Date:   10 Jul 2014 17:46:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearEdgeProcessingClass.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVTool.h"
#include "SVAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearEdgeProcessingClass, SVLinearEdgeProcessingClassGuid );


SVLinearEdgeProcessingClass::SVLinearEdgeProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                  :SVTaskObjectClass( POwner, StringResourceID )
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVLinearEdgeProcessingObjectType;

	m_svInputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_svInputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputImageObjectInfo, _T( "LinearEdgeProcessingImage" ) );

	m_svInputMinThreshold.SetInputObjectType( SVLinearThresholdMinObjectGuid, SVDoubleValueObjectType );
	m_svInputMinThreshold.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputMinThreshold, _T( "LinearEdgeProcessingMinThreshold" ) );

	m_svInputMaxThreshold.SetInputObjectType( SVLinearThresholdMaxObjectGuid, SVDoubleValueObjectType );
	m_svInputMaxThreshold.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputMaxThreshold, _T( "LinearEdgeProcessingMaxThreshold" ) );

	m_svInputLinearData.SetInputObjectType( SVLinearDataClassGuid, SVDoubleValueObjectType );
	m_svInputLinearData.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputLinearData, _T( "LinearEdgeProcessingInputLinearData" ) );

	m_lPixelDepth = 0;
	m_dwMinThreshold = 0;
	m_dwMaxThreshold = 0;
	m_dwColorNumber = 0;

	m_svDirection.SetEnumTypes( g_strDirectionEnums );
	m_svPolarisation.SetEnumTypes( g_strPolarisationEnums );
	m_svEdgeSelect.SetEnumTypes( g_strEdgeSelectEnums );
	m_svPosition.SetEnumTypes( g_strPositionEnums );

	m_svDirection.SetDefaultValue(SV_HEAD_TO_TAIL_DIRECTION,TRUE);
	m_svPolarisation.SetDefaultValue(SV_ANY_POLARISATION,TRUE);
	m_svEdgeSelect.SetDefaultValue(SV_FIRST_EDGE,TRUE);
	m_svEdgeSelectThisValue.SetDefaultValue(1.0,TRUE);

	m_svIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_svPosition.SetDefaultValue(SV_START_POSITION,TRUE);
	m_svPositionOffsetValue.SetDefaultValue(1.0,TRUE);		

	m_svUseLowerThresholdSelectable.SetDefaultValue(TRUE,TRUE);
	m_svUseLowerThresholdMaxMinusPercentDiff.SetDefaultValue(FALSE,TRUE);
	m_svUseLowerThresholdMaxMinusOffset.SetDefaultValue(FALSE,TRUE);
	m_svUseLowerThresholdMinPlusOffset.SetDefaultValue(FALSE,TRUE);
	m_svLowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_svLowerMaxMinusPercentDiffValue.SetDefaultValue(0,TRUE);
	m_svLowerMaxMinusOffsetValue.SetDefaultValue(0,TRUE);
	m_svLowerMinPlusOffsetValue.SetDefaultValue(0,TRUE);

	m_svUseUpperThresholdSelectable.SetDefaultValue(TRUE,TRUE);
	m_svUseUpperThresholdMaxMinusPercentDiff.SetDefaultValue(FALSE,TRUE);
	m_svUseUpperThresholdMaxMinusOffset.SetDefaultValue(FALSE,TRUE);
	m_svUseUpperThresholdMinPlusOffset.SetDefaultValue(FALSE,TRUE);
	m_svUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);
	m_svUpperMaxMinusPercentDiffValue.SetDefaultValue(0,TRUE);
	m_svUpperMaxMinusOffsetValue.SetDefaultValue(0,TRUE);
	m_svUpperMinPlusOffsetValue.SetDefaultValue(0,TRUE);

	m_cfThresholds = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
	m_cfHistogram = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
	m_cfEdges = SV_DEFAULT_SUB_FUNCTION_COLOR_2;

	m_svLinearEdges.SetLegacyVectorObjectCompatibility();

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgeProcessingClass::~SVLinearEdgeProcessingClass()
{
}

BOOL SVLinearEdgeProcessingClass::CreateObject( SVObjectLevelCreateStruct *PCreateStructure )
{
  SVImageInfoClass ImageInfo;

	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	bOk = bOk && GetPixelDepth() == S_OK;

	if( bOk )
	{
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( m_lPixelDepth ));	
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( m_lPixelDepth ));	
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( m_lPixelDepth ));
	}

	//set attributes for Upper and Lower Threshold values.
	m_svUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_svUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;

	m_svLowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_svLowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;

	m_svLinearEdges.ObjectAttributesAllowedRef() &= ~( SV_VIEWABLE | SV_PRINTABLE );


	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;

	m_svUseLowerThresholdSelectable.GetValue(bLower);
	m_svUseUpperThresholdSelectable.GetValue(bUpper);

	if ( !bLower )
	{
		m_svLowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_svLowerThresholdValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	if ( !bUpper )
	{
		m_svUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_svUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}


	if ( SVDoubleValueObjectClass* pdvoLinearData = GetInputLinearData() )
	{
		m_svLinearEdges.SetArraySize( pdvoLinearData->GetArraySize() );
	}

	return bOk;
}

HRESULT SVLinearEdgeProcessingClass::ResetObject()
{
	HRESULT hRet = SVTaskObjectClass::ResetObject();

	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;

	m_svUseLowerThresholdSelectable.GetValue(bLower);
	m_svUseUpperThresholdSelectable.GetValue(bUpper);

	if ( !bLower )
	{
		m_svLowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_svLowerThresholdValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	if ( !bUpper )
	{
		m_svUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}
	else
	{
		m_svUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}

	if( GetPixelDepth() != S_OK )
	{
		hRet = S_FALSE;
	}

	if ( SVDoubleValueObjectClass* pdvoLinearData = GetInputLinearData() )
	{
		m_svLinearEdges.SetArraySize( pdvoLinearData->GetArraySize() );
	}

	return hRet;
}

BOOL SVLinearEdgeProcessingClass::OnValidate()
{
	if( SVTaskObjectClass::OnValidate() )
	{
		if( GetInputLinearData() != NULL )
		{
			return TRUE;
		}
	}

	SetInvalid();
	return FALSE;
}

BOOL SVLinearEdgeProcessingClass::onRun( SVRunStatusClass &p_rsvRunStatus )
{
	BOOL l_bOk = TRUE;

	l_bOk = SVTaskObjectClass::onRun(p_rsvRunStatus);

	l_bOk &= UpdateUpperThresholdValues( p_rsvRunStatus.m_lResultDataIndex ) == S_OK;
	l_bOk &= UpdateLowerThresholdValues( p_rsvRunStatus.m_lResultDataIndex ) == S_OK;

	l_bOk &= UpdateEdgeList( p_rsvRunStatus.m_lResultDataIndex ) == S_OK;

	return l_bOk;
}

SVImageClass *SVLinearEdgeProcessingClass::GetInputImage()
{
	SVImageClass *l_psvImage = NULL;

	if( m_svInputImageObjectInfo.IsConnected() && 
		  m_svInputImageObjectInfo.GetInputObjectInfo().PObject != NULL )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(m_svInputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return l_psvImage;
}

SVDoubleValueObjectClass *SVLinearEdgeProcessingClass::GetInputMinThreshold()
{
	SVDoubleValueObjectClass *l_psvValue = NULL;

	if( m_svInputMinThreshold.IsConnected() && 
		  m_svInputMinThreshold.GetInputObjectInfo().PObject != NULL )
	{
		l_psvValue = dynamic_cast<SVDoubleValueObjectClass *>(m_svInputMinThreshold.GetInputObjectInfo().PObject);
	}

	return l_psvValue;
}

SVDoubleValueObjectClass *SVLinearEdgeProcessingClass::GetInputMaxThreshold()
{
	SVDoubleValueObjectClass *l_psvValue = NULL;

	if( m_svInputMaxThreshold.IsConnected() && 
		  m_svInputMaxThreshold.GetInputObjectInfo().PObject != NULL )
	{
		l_psvValue = dynamic_cast<SVDoubleValueObjectClass *>(m_svInputMaxThreshold.GetInputObjectInfo().PObject);
	}

	return l_psvValue;
}

SVDoubleValueObjectClass* SVLinearEdgeProcessingClass::GetInputLinearData()
{
	SVDoubleValueObjectClass* l_psvData = NULL;

	if( m_svInputLinearData.IsConnected() && m_svInputLinearData.GetInputObjectInfo().PObject )
	{
		l_psvData = dynamic_cast <SVDoubleValueObjectClass*> (m_svInputLinearData.GetInputObjectInfo().PObject);
#ifdef _DEBUG
		if ( m_svInputLinearData.GetInputObjectInfo().PObject != NULL )
		{
			ASSERT( l_psvData != NULL );
		}
#endif
	}

	return l_psvData;
}

HRESULT SVLinearEdgeProcessingClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;

	if( GetInputImage() != NULL )
	{
	  SVImageInfoClass ImageInfo = GetInputImage()->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetOutputEdgeDistance( double &p_rdValue )
{
	HRESULT l_hrOk = S_FALSE;

	std::vector<double> l_svData;

	p_rdValue = 0.0;

	if( GetInputLinearData() != NULL &&
		  ( GetInputLinearData()->GetValues( l_svData ) == S_OK ) )
	{
		BOOL l_bIsFixedEdgeMarker = FALSE;
		DWORD l_dwPosition = 0;

		double l_dWidth = static_cast<double>(l_svData.size());
		double l_dEndOfLine = static_cast<double>(l_svData.size() - 1);

		if( ( m_svIsFixedEdgeMarker.GetValue( l_bIsFixedEdgeMarker ) == S_OK ) && 
				l_bIsFixedEdgeMarker &&
				( m_svPosition.GetValue( l_dwPosition ) == S_OK ) )
		{
			// Get the selected fixed position...
			switch ( l_dwPosition )
			{
				case SV_START_POSITION:
				{
					// Get measure line start position
					// Allways 0!

					p_rdValue = 0.0;

					l_hrOk = S_OK;

					break;
				}

				case SV_END_POSITION:
				{
					// Get measure line end position
					// Allways line length - 1, if length > 0!

					p_rdValue = l_dEndOfLine;

					l_hrOk = S_OK;

					break;
				}
			
				case SV_CENTER_POSITION:
				{
					// Get measure line center position
					// Allways ( line length / 2 ), if ( length / 2 ) > 0!
					p_rdValue = l_dWidth / 2.0;

					l_hrOk = S_OK;

					break;
				}

				case SV_OFFSET_POSITION:
				{
					double l_dOffset = 0.0;

					if( ( m_svPositionOffsetValue.GetValue( l_dOffset ) == S_OK ) && 
					    l_dOffset <= l_dEndOfLine )
					{
						p_rdValue = l_dOffset;

						l_hrOk = S_OK;
					}

					break;
				}
			}
		}
		else
		{
			std::vector<double> l_svEdges;

			DWORD l_dwEdgeSelect = 0;

			if( ( m_svLinearEdges.GetValues( l_svEdges ) == S_OK ) &&
				  ( m_svEdgeSelect.GetValue( l_dwEdgeSelect ) == S_OK ) )
			{
				switch ( l_dwEdgeSelect )
				{
					case SV_FIRST_EDGE:
					{
						// Get first found edge position
						// Allways first edge, if edge number > 0!
						if( 0 < l_svEdges.size() )
						{
							p_rdValue = l_svEdges[ 0 ];

							l_hrOk = S_OK;
						}

						break;
					}

					case SV_LAST_EDGE:
					{
						// Get last found edge position
						// Allways last edge, if edge number > 0!
						if( 0 < l_svEdges.size() )
						{
							p_rdValue = l_svEdges[ l_svEdges.size() - 1 ];

							l_hrOk = S_OK;
						}

						break;
					}

					case SV_THIS_EDGE:
					{
						// Get user defined edge position
						// Allways user defined edge, if edge number >= user defined edge
						// and result position is inside the line extent!
						// If the user defined edge is between two existing edges,
						// the result position is interpolated.

						double l_dEdgeSelect = 0.0;
						DWORD l_dwDirection = 0;

						if( ( m_svEdgeSelectThisValue.GetValue( l_dEdgeSelect ) == S_OK ) &&
						    ( m_svDirection.GetValue( l_dwDirection ) == S_OK ) )
						{
							long l_lEdge = (long)l_dEdgeSelect;
							double l_dPercent = l_dEdgeSelect - l_lEdge;

							switch( l_dwDirection )
							{
								case SV_HEAD_TO_TAIL_DIRECTION:
								{
									if( l_lEdge - 1 < static_cast<long>(l_svEdges.size()) )
									{
										double l_dFirstEdge = ( 0 <= l_lEdge - 1 && l_lEdge - 1 < static_cast<long>(l_svEdges.size()) ) ? l_svEdges[ l_lEdge - 1 ] : 0.0;
										double l_dSecondEdge = ( 0 <= l_lEdge && l_lEdge < static_cast<long>(l_svEdges.size()) ) ? l_svEdges[ l_lEdge ] : l_dEndOfLine;

										p_rdValue = l_dFirstEdge + ( l_dSecondEdge - l_dFirstEdge ) * l_dPercent;

										p_rdValue = ( 0.0 < p_rdValue ) ? p_rdValue : 0.0;

										l_hrOk = S_OK;
									}

									break;
								}

								case SV_TAIL_TO_HEAD_DIRECTION:
								{
									if( l_lEdge - 1 < static_cast<long>(l_svEdges.size()) )
									{
										double l_dFirstEdge = ( 0 <= l_lEdge - 1 && l_lEdge - 1 < static_cast<long>(l_svEdges.size()) ) ? l_svEdges[ l_lEdge - 1 ] : l_dEndOfLine;
										double l_dSecondEdge = ( 0 <= l_lEdge && l_lEdge < static_cast<long>(l_svEdges.size()) ) ? l_svEdges[ l_lEdge ] : 0.0;

										p_rdValue = l_dFirstEdge + ( l_dSecondEdge - l_dFirstEdge ) * l_dPercent;

										p_rdValue = ( 0.0 < p_rdValue ) ? p_rdValue : 0.0;

										l_hrOk = S_OK;
									}

									break;
								}
							}
						}

						break;
					}
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetPointFromDistance( double p_dDistance, SVExtentPointStruct &p_rsvPoint )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	HRESULT l_hrOk = p_rsvPoint.Initialize();

	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		p_rsvPoint.m_dPositionX = p_dDistance;
		p_rsvPoint.m_dPositionY = l_oRect.bottom / 2;
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetEdgeOverlayFromDistance( double p_dDistance, SVExtentLineStruct &p_rsvLine )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	long l_lBottom = 0;

	HRESULT l_hrOk = p_rsvLine.Initialize();

	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		SVExtentPointStruct l_oGraphPoint;

		p_rsvLine.m_bIsAngleValid = true;
		p_rsvLine.m_dAngle = 90.0;

		l_oGraphPoint.m_dPositionX = p_dDistance;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 - 10;
		
		p_rsvLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

		l_oGraphPoint.m_dPositionX = p_dDistance;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 + 10;
		
		p_rsvLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

		p_rsvLine.m_dwColor = GetObjectColor();
	}

	return l_hrOk;

}

HRESULT SVLinearEdgeProcessingClass::GetOutputEdgePoint( SVExtentPointStruct &p_rsvPoint )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	unsigned long l_ulUpper = 0;
	unsigned long l_ulLower = 0;

	HRESULT l_hrOk = p_rsvPoint.Initialize();

	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK ||
		  ( S_OK != m_svLowerThresholdValue.GetValue( l_ulLower ) ) ||
			( S_OK != m_svUpperThresholdValue.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		double l_dDistance = 0.0;

		l_hrOk = GetOutputEdgeDistance( l_dDistance );

		if( l_hrOk == S_OK )
		{
			p_rsvPoint.m_dPositionX = l_dDistance;
			p_rsvPoint.m_dPositionY = l_oRect.bottom / 2;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	unsigned long l_ulUpper = 0;
	unsigned long l_ulLower = 0;

	HRESULT l_hrOk = p_rsvMiltiLine.Initialize();
		
	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK ||
		  ( S_OK != m_svLowerThresholdValue.GetValue( l_ulLower ) ) ||
			( S_OK != m_svUpperThresholdValue.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		SVExtentLineStruct l_svUpperLine;
		SVExtentLineStruct l_svLowerLine;

		double divisor = m_dwColorNumber;

		SVExtentPointStruct l_svPoint;
	
		l_svUpperLine.m_bIsAngleValid = true;
		l_svUpperLine.m_dAngle = 0.0;
		l_svUpperLine.m_dwColor = m_cfThresholds;

		l_svLowerLine.m_bIsAngleValid = true;
		l_svLowerLine.m_dAngle = 0.0;
		l_svLowerLine.m_dwColor = m_cfThresholds;

		// Calc current Threshold bars...
		long lMaxThresholdValue = ( long ) ( l_ulUpper / divisor * ( ( double ) l_oRect.bottom ) );
		long lMinThresholdValue = ( long ) ( l_ulLower / divisor * ( ( double ) l_oRect.bottom ) );

		// MaxThresholdBar
		// Max left...
		SVExtentPointStruct l_oGraphPoint;
		l_oGraphPoint.m_dPositionX = 0;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom - lMaxThresholdValue;
		
		l_svUpperLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

		l_oGraphPoint.m_dPositionX = l_oRect.right;

		l_svUpperLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

		// MinThresholdBar
		// Min left...
		l_oGraphPoint.m_dPositionX = 0;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom - lMinThresholdValue;
		
		l_svLowerLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

		l_oGraphPoint.m_dPositionX = l_oRect.right;

		l_svLowerLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

		p_rsvMiltiLine.m_svLineArray.Add( l_svUpperLine );
		p_rsvMiltiLine.m_svLineArray.Add( l_svLowerLine );
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetHistogramOverlay( SVExtentLineStruct &p_rsvLine )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	std::vector<double> l_svData;

	HRESULT l_hrOk = p_rsvLine.Initialize();
		
	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK ||
		  GetInputLinearData() == NULL ||
			(S_OK != GetInputLinearData()->GetValues( l_svData )) )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		double divisor = m_dwColorNumber;

		SVExtentPointStruct l_svPoint;
	
		p_rsvLine.m_dwColor = m_cfHistogram;

		for( size_t l = 0; l < l_svData.size(); l++ )
		{
			long l_lPixel = ( long ) ( l_svData[ l ] / divisor * ( ( double ) l_oRect.bottom ) );

			SVExtentPointStruct l_oGraphPoint;
			l_oGraphPoint.m_dPositionX = static_cast<double>(l);
			l_oGraphPoint.m_dPositionY = l_oRect.bottom - l_lPixel;

			p_rsvLine.m_svPointArray.SetAtGrow( static_cast<int>(l), l_oGraphPoint );
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	HRESULT l_hrOk = p_rsvMiltiLine.Initialize();

	std::vector<double> l_svEdges;
	
	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK ||
		  ( S_OK != m_svLinearEdges.GetValues( l_svEdges ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		DWORD l_dwDirection = SV_UNDEFINED_DIRECTION;

		m_svDirection.GetValue( l_dwDirection );

		SVExtentLineStruct l_svEdgeLine;

		l_svEdgeLine.m_bIsAngleValid = true;
		l_svEdgeLine.m_dAngle = 90.0;
		l_svEdgeLine.m_dwColor = m_cfEdges;

		for( size_t l = 0; l < l_svEdges.size(); l++ )
		{
			double l_dX = l_svEdges[ l ];

			if( l_dwDirection == SV_UNDEFINED_DIRECTION )
			{
				l_dX = static_cast<int>(l);

				if( l_svEdges[ l ] == 0 )
				{
					l_svEdgeLine.m_dwColor = SV_DEFAULT_BLACK_COLOR;
				}
				else if( l_svEdges[ l ] == m_dwColorNumber - 1 )
				{
					l_svEdgeLine.m_dwColor = SV_DEFAULT_WHITE_COLOR;
				}
				else
				{
					l_svEdgeLine.m_dwColor = m_cfEdges;
				}
			}

			SVExtentPointStruct l_oGraphPoint;

			l_oGraphPoint.m_dPositionX = l_dX;
			l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 - 5;
			
			l_svEdgeLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

			l_oGraphPoint.m_dPositionX = l_dX;
			l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 + 5;
			
			l_svEdgeLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

			p_rsvMiltiLine.m_svLineArray.Add( l_svEdgeLine );
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetSelectedEdgeOverlay( SVExtentLineStruct &p_rsvLine )
{
	RECT l_oRect;

	SVImageExtentClass l_svExtents;

	long l_lBottom = 0;
	double l_dDistance = 0.0;

	HRESULT l_hrOk = p_rsvLine.Initialize();

	if( GetAnalyzer() == NULL ||
		  GetAnalyzer()->GetImageExtent( l_svExtents ) != S_OK  ||
			l_svExtents.GetOutputRectangle( l_oRect ) != S_OK ||
		  GetOutputEdgeDistance( l_dDistance ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		SVExtentPointStruct l_oGraphPoint;

		p_rsvLine.m_bIsAngleValid = true;
		p_rsvLine.m_dAngle = 90.0;

		l_oGraphPoint.m_dPositionX = l_dDistance;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 - 10;
		
		p_rsvLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

		l_oGraphPoint.m_dPositionX = l_dDistance;
		l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 + 10;
		
		p_rsvLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

		p_rsvLine.m_dwColor = GetObjectColor();
	}

	return l_hrOk;
}

long SVLinearEdgeProcessingClass::getLowerThresholdValue() const
{
	DWORD thresholdValue = 0;
	if (S_OK == m_svLowerThresholdValue.GetValue( thresholdValue ))
	{
		return static_cast<long>(thresholdValue);
	}
	else
	{
		return -1;
	}
}

long SVLinearEdgeProcessingClass::getUpperThresholdValue() const
{
	DWORD thresholdValue = 0;
	if (S_OK == m_svUpperThresholdValue.GetValue( thresholdValue ))
	{
		return static_cast<long>(thresholdValue);
	}
	else
	{
		return -1;
	}
}

HRESULT SVLinearEdgeProcessingClass::UpdateUpperThresholdValues( long p_lIndex )
{
	HRESULT l_hrOk = S_OK;
	HRESULT l_hrTemp = S_OK;

	BOOL l_bState = FALSE;

	double l_dMin = 0.0;
	double l_dMax = 0.0;

	double l_dUpper = 0.0;

	if( GetInputMinThreshold() == NULL || ( S_OK != GetInputMinThreshold()->GetValue( p_lIndex, l_dMin ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( GetInputMaxThreshold() == NULL || ( S_OK != GetInputMaxThreshold()->GetValue( p_lIndex, l_dMax ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( ( l_hrTemp = m_svUseUpperThresholdSelectable.GetValue( l_bState ) ) == S_OK )
	{
		if( ! l_bState )
		{
			if( ( m_svUseUpperThresholdMaxMinusPercentDiff.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dPercentDiff = 0.0;

				if( m_svUpperMaxMinusPercentDiffValue.GetValue( l_dPercentDiff ) == S_OK )
				{
					if( l_dPercentDiff != 0.0 )
					{
						double l_dPercent = l_dPercentDiff / 100.0;

						l_dUpper = l_dMax - ( ( l_dMax - l_dMin ) * l_dPercent );
					}
					else
					{
						l_dUpper = l_dMax;
					}
				}
				else
				{
					l_hrOk = S_FALSE;
				}
			}
			else if( ( m_svUseUpperThresholdMaxMinusOffset.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dOffset = 0.0;

				if( (l_hrTemp = m_svUpperMaxMinusOffsetValue.GetValue( l_dOffset )) == S_OK )
				{
					l_dUpper = l_dMax - l_dOffset;
				}
				else
				{
					l_hrOk = l_hrTemp;
				}
			}
			else if( ( m_svUseUpperThresholdMinPlusOffset.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dOffset = 0.0;

				if( ( l_hrTemp = m_svUpperMinPlusOffsetValue.GetValue( l_dOffset ) ) == S_OK )
				{
					l_dUpper = l_dMin + l_dOffset;
				}
				else
				{
					l_hrOk = l_hrTemp;
				}
			}

			if( l_dUpper < 0.0 )
			{
				l_dUpper = 0.0;
			}

			if( 255.0 < l_dUpper )
			{
				l_dUpper = 255.0;
			}

			if( S_OK != ( l_hrTemp = m_svUpperThresholdValue.SetValue( p_lIndex, l_dUpper ) ) )
			{
				l_hrOk = l_hrTemp;
			}
		}
	}
	else
	{
		l_hrOk = l_hrTemp;
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::UpdateLowerThresholdValues( long p_lIndex )
{
	HRESULT l_hrOk = S_OK;
	HRESULT l_hrTemp = S_OK;

	BOOL l_bState = FALSE;

	double l_dMin = 0.0;
	double l_dMax = 0.0;

	double l_dLower = 0.0;

	if( GetInputMinThreshold() == NULL || ( S_OK != GetInputMinThreshold()->GetValue( p_lIndex, l_dMin ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( GetInputMaxThreshold() == NULL || ( S_OK != GetInputMaxThreshold()->GetValue( p_lIndex, l_dMax ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( ( l_hrTemp = m_svUseLowerThresholdSelectable.GetValue( l_bState ) ) == S_OK )
	{
		if( ! l_bState )
		{
			if( ( m_svUseLowerThresholdMaxMinusPercentDiff.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dPercentDiff = 0.0;

				if( ( l_hrTemp = m_svLowerMaxMinusPercentDiffValue.GetValue( l_dPercentDiff ) ) == S_OK )
				{
					if( l_dPercentDiff != 0.0 )
					{
						double l_dPercent = l_dPercentDiff / 100.0;

						l_dLower = l_dMax - ( ( l_dMax - l_dMin ) * l_dPercent );
					}
					else
					{
						l_dLower = l_dMax;
					}
				}
				else
				{
					l_hrOk = l_hrTemp;
				}
			}
			else if( ( m_svUseLowerThresholdMaxMinusOffset.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dOffset = 0.0;

				if( ( l_hrTemp = m_svLowerMaxMinusOffsetValue.GetValue( l_dOffset ) ) == S_OK )
				{
					l_dLower = l_dMax - l_dOffset;
				}
				else
				{
					l_hrOk = l_hrTemp;
				}
			}
			else if( ( m_svUseLowerThresholdMinPlusOffset.GetValue( l_bState ) == S_OK ) && l_bState )
			{
				double l_dOffset = 0.0;

				if( ( l_hrTemp = m_svLowerMinPlusOffsetValue.GetValue( l_dOffset ) ) == S_OK )
				{
					l_dLower = l_dMin + l_dOffset;
				}
				else
				{
					l_hrOk = l_hrTemp;
				}
			}

			if( l_dLower < 0.0 )
			{
				l_dLower = 0.0;
			}

			if( 255.0 < l_dLower )
			{
				l_dLower = 255.0;
			}

			if( S_OK != ( l_hrTemp = m_svLowerThresholdValue.SetValue( p_lIndex, l_dLower ) ) )
			{
				l_hrOk = l_hrTemp;
			}
		}
	}
	else
	{
		l_hrOk = l_hrTemp;
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::UpdateEdgeList( long p_lIndex )
{
	HRESULT l_hrOk = S_OK;

	std::vector<double> l_svData;
	std::vector<double> l_svEdges;

	DWORD l_dwUpper = 0;
	DWORD l_dwLower = 0;
	DWORD l_dwDirection = 0;
	DWORD l_dwPolarisation = 0;

	if( GetInputLinearData() == NULL )
	{
		l_hrOk = S_FALSE;
	}
	
	if ( l_hrOk == S_OK )
	{
		l_hrOk = GetInputLinearData()->GetValues( l_svData );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_svUpperThresholdValue.GetValue( l_dwUpper );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_svLowerThresholdValue.GetValue( l_dwLower );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_svDirection.GetValue( l_dwDirection );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_svPolarisation.GetValue( l_dwPolarisation );
	}

	if( l_hrOk == S_OK )
	{
		long l_lCount = static_cast<long>(l_svData.size());

		switch( l_dwDirection )
		{
			case SV_HEAD_TO_TAIL_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < l_lCount - 1; ++l )
				{
					if( IsEdge( l_svData[ l ], l_svData[ l + 1 ], l_dwUpper, l_dwLower, l_dwPolarisation ) == S_OK )
					{
						if( l_dwPolarisation == SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = CalculateSubPixelEdge( l_svData[ l ], l_svData[ l + 1 ], l_dwUpper, l_dwLower, SV_POSITIVE_POLARISATION, l_dDistance1 ) == S_OK;
							bool l_bDistance2 = CalculateSubPixelEdge( l_svData[ l ], l_svData[ l + 1 ], l_dwUpper, l_dwLower, SV_NEGATIVE_POLARISATION, l_dDistance2 ) == S_OK;

							if ( l_dDistance1 <= l_dDistance2 )
							{
								if( l_bDistance1 )
								{
									l_dDistance1 = l + l_dDistance1;

									l_svEdges.push_back( l_dDistance1 );
								}

								if( l_bDistance2 )
								{
									l_dDistance2 = l + l_dDistance2;

									l_svEdges.push_back( l_dDistance2 );
								}
							}
							else
							{
								if( l_bDistance2 )
								{
									l_dDistance2 = l + l_dDistance2;

									l_svEdges.push_back( l_dDistance2 );
								}

								if( l_bDistance1 )
								{
									l_dDistance1 = l + l_dDistance1;

									l_svEdges.push_back( l_dDistance1 );
								}
							}
						}
						else
						{
							double l_dDistance = 0.0;

							if( CalculateSubPixelEdge( l_svData[ l ], l_svData[ l + 1 ], l_dwUpper, l_dwLower, l_dwPolarisation, l_dDistance ) == S_OK )
							{
								l_dDistance = l + l_dDistance;

								l_svEdges.push_back( l_dDistance );
							}
						}
					}
				}

				break;
			}

			case SV_TAIL_TO_HEAD_DIRECTION:
			{
				for( long l = l_lCount - 1; 0 < l; --l )
				{
					if( IsEdge( l_svData[ l ], l_svData[ l - 1 ], l_dwUpper, l_dwLower, l_dwPolarisation ) == S_OK )
					{
						if( l_dwPolarisation == SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = CalculateSubPixelEdge( l_svData[ l ], l_svData[ l - 1 ], l_dwUpper, l_dwLower, SV_POSITIVE_POLARISATION, l_dDistance1 ) == S_OK;
							bool l_bDistance2 = CalculateSubPixelEdge( l_svData[ l ], l_svData[ l - 1 ], l_dwUpper, l_dwLower, SV_NEGATIVE_POLARISATION, l_dDistance2 ) == S_OK;

							if ( l_dDistance1 <= l_dDistance2 )
							{
								if( l_bDistance1 )
								{
									l_dDistance1 = l - l_dDistance1;

									l_svEdges.push_back( l_dDistance1 );
								}

								if( l_bDistance2 )
								{
									l_dDistance2 = l - l_dDistance2;

									l_svEdges.push_back( l_dDistance2 );
								}
							}
							else
							{
								if( l_bDistance2 )
								{
									l_dDistance2 = l - l_dDistance2;

									l_svEdges.push_back( l_dDistance2 );
								}

								if( l_bDistance1 )
								{
									l_dDistance1 = l - l_dDistance1;

									l_svEdges.push_back( l_dDistance1 );
								}
							}
						}
						else
						{
							double l_dDistance = 0.0;

							if( CalculateSubPixelEdge( l_svData[ l ], l_svData[ l - 1 ], l_dwUpper, l_dwLower, l_dwPolarisation, l_dDistance ) == S_OK )
							{
								l_dDistance = l - l_dDistance;

								l_svEdges.push_back( l_dDistance );
							}
						}
					}
				}

				break;
			}

			case SV_UNDEFINED_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < l_lCount; ++l )
				{
					double l_dValue = 0.0;

					if( GetBlackWhiteEdgeValue( l_svData[ l ], l_dwUpper, l_dwLower, l_dValue ) == S_OK )
					{
						l_svEdges.push_back( l_dValue );
					}
					else
					{
						l_svEdges.push_back( l_svData[ l ] );
					}
				}

				break;
			}
		}
	}

	// always do the Set
	HRESULT hrSet = m_svLinearEdges.SetArrayValues( p_lIndex, l_svEdges );

	if ( l_hrOk == S_OK )
		l_hrOk = hrSet;

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::IsEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation )
{
	HRESULT l_hrOk = S_FALSE;

	switch( p_dwPolarisation )
	{
		case SV_POSITIVE_POLARISATION:
		{
			bool l_bIsPositive = false;

			// Positive:BLACK ---> WHITE
			l_bIsPositive |= p_dStart < p_dwLower && p_dwLower <= p_dEnd;
			l_bIsPositive |= p_dwUpper < p_dStart && p_dEnd <= p_dwUpper;

			if( l_bIsPositive )
			{
				l_hrOk = S_OK;
			}
			break;
		}
		case SV_NEGATIVE_POLARISATION:
		{
			bool l_bIsNegative = false;

			// Negitive:WHITE ---> BLACK
			l_bIsNegative |= p_dwLower <= p_dStart && p_dEnd < p_dwLower;
			l_bIsNegative |= p_dStart <= p_dwUpper && p_dwUpper < p_dEnd;

			if( l_bIsNegative )
			{
				l_hrOk = S_OK;
			}
			break;
		}
		case SV_ANY_POLARISATION:
		{
			bool l_bIsAny = false;

			// Positive:BLACK ---> WHITE
			l_bIsAny |= p_dStart < p_dwLower && p_dwLower <= p_dEnd;
			l_bIsAny |= p_dwUpper < p_dStart && p_dEnd <= p_dwUpper;

			// Negitive:WHITE ---> BLACK
			l_bIsAny |= p_dwLower <= p_dStart && p_dEnd < p_dwLower;
			l_bIsAny |= p_dStart <= p_dwUpper && p_dwUpper < p_dEnd;

			if( l_bIsAny )
			{
				l_hrOk = S_OK;
			}
			break;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetBlackWhiteEdgeValue( double p_dCurrent, DWORD p_dwUpper, DWORD p_dwLower, double &l_rdValue )
{
	HRESULT l_hrOk = S_OK;

	// Binarizing...
	if( p_dwLower <= p_dwUpper )
	{
		l_rdValue = ( p_dwLower <= p_dCurrent && p_dCurrent <= p_dwUpper ) ? m_dwColorNumber - 1 : 0;
	}
	else
	{
		l_rdValue = ( p_dwUpper <= p_dCurrent && p_dCurrent <= p_dwLower ) ? 0 : m_dwColorNumber - 1;
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::CalculateSubPixelEdge( double p_dStart, double p_dEnd, DWORD p_dwUpper, DWORD p_dwLower, DWORD p_dwPolarisation, double &p_rdDistance )
{
	HRESULT l_hrOk = S_OK;

	p_rdDistance = 0.0;

	if( p_dStart != p_dEnd )
	{
		double l_dwThreshold = p_dwLower;

		if( p_dwLower != p_dwUpper )
		{
			switch( p_dwPolarisation )
			{
				case SV_POSITIVE_POLARISATION:
				{
					if( p_dStart < p_dwLower && p_dwLower <= p_dEnd )
					{
						l_dwThreshold = p_dwLower;
					}
					else if( p_dwUpper < p_dStart && p_dEnd <= p_dwUpper )
					{
						l_dwThreshold = p_dwUpper;
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				case SV_NEGATIVE_POLARISATION:
				{
					if( p_dwLower <= p_dStart && p_dEnd < p_dwLower )
					{
						l_dwThreshold = p_dwLower;
					}
					else if( p_dStart <= p_dwUpper && p_dwUpper < p_dEnd )
					{
						l_dwThreshold = p_dwUpper;
					}
					else
					{
						l_hrOk = S_FALSE;
					}

					break;
				}
				default:
				{
					l_hrOk = S_FALSE;

					break;
				}
			}
		}

		if( l_hrOk == S_OK )
		{
			p_rdDistance = fabs( l_dwThreshold - p_dStart ) / fabs( p_dEnd - p_dStart );
		}
	}

	return l_hrOk;
}

DWORD_PTR SVLinearEdgeProcessingClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}

	return (DwResult | SVTaskObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

