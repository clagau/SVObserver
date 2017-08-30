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
#include "ObjectInterfaces/GlobalConst.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVTool.h"
#include "SVOCore/SVAnalyzer.h"
#include "SVGlobal.h"

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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVLinearEdgeProcessingObjectType;

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

	m_svDirection.SetEnumTypes( cDirectionEnums );
	m_svPolarisation.SetEnumTypes( cPolarisationEnums );
	m_svEdgeSelect.SetEnumTypes( cEdgeSelectEnums );
	m_svPosition.SetEnumTypes( cPositionEnums );

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
	m_svLowerThresholdValue.SetDefaultValue(SvOi::cDefaultToolLowerThreshold,TRUE);
	m_svLowerMaxMinusPercentDiffValue.SetDefaultValue(0,TRUE);
	m_svLowerMaxMinusOffsetValue.SetDefaultValue(0,TRUE);
	m_svLowerMinPlusOffsetValue.SetDefaultValue(0,TRUE);

	m_svUseUpperThresholdSelectable.SetDefaultValue(TRUE,TRUE);
	m_svUseUpperThresholdMaxMinusPercentDiff.SetDefaultValue(FALSE,TRUE);
	m_svUseUpperThresholdMaxMinusOffset.SetDefaultValue(FALSE,TRUE);
	m_svUseUpperThresholdMinPlusOffset.SetDefaultValue(FALSE,TRUE);
	m_svUpperThresholdValue.SetDefaultValue(SvOi::cDefaultToolUpperThreshold,TRUE);
	m_svUpperMaxMinusPercentDiffValue.SetDefaultValue(0,TRUE);
	m_svUpperMaxMinusOffsetValue.SetDefaultValue(0,TRUE);
	m_svUpperMinPlusOffsetValue.SetDefaultValue(0,TRUE);

	m_cfThresholds = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
	m_cfHistogram = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
	m_cfEdges = SV_DEFAULT_SUB_FUNCTION_COLOR_2;

	m_svLinearEdges.SetLegacyVectorObjectCompatibility();
	m_svLinearEdges.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgeProcessingClass::~SVLinearEdgeProcessingClass()
{
}

bool SVLinearEdgeProcessingClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
  SVImageInfoClass ImageInfo;

	bool bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	bOk = bOk && S_OK == GetPixelDepth();

	if( bOk )
	{
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( m_lPixelDepth ));	
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( m_lPixelDepth ));	
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( m_lPixelDepth ));
	}

	//set attributes for Upper and Lower Threshold values.
	m_svUpperThresholdValue.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
	m_svUpperThresholdValue.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );

	m_svLowerThresholdValue.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
	m_svLowerThresholdValue.SetObjectAttributesAllowed( SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearEdges.SetObjectAttributesAllowed( SvOi::SV_VIEWABLE | SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );


	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;

	m_svUseLowerThresholdSelectable.GetValue(bLower);
	m_svUseUpperThresholdSelectable.GetValue(bUpper);

	SvOi::SetAttributeType AddRemoveType = bLower ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svLowerThresholdValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	AddRemoveType = bUpper ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svUpperThresholdValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );


	if ( SVDoubleValueObjectClass* pdvoLinearData = GetInputLinearData() )
	{
		m_svLinearEdges.SetArraySize( pdvoLinearData->getArraySize() );
	}

	return bOk;
}

bool SVLinearEdgeProcessingClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVTaskObjectClass::ResetObject(pErrorMessages);

	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;

	m_svUseLowerThresholdSelectable.GetValue(bLower);
	m_svLowerThresholdValue.setSaveValueFlag(TRUE == bLower);
	m_svUseUpperThresholdSelectable.GetValue(bUpper);
	m_svUpperThresholdValue.setSaveValueFlag(TRUE == bUpper);

	SvOi::SetAttributeType AddRemoveType = bLower ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svLowerThresholdValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	AddRemoveType = bUpper ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svUpperThresholdValue.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, AddRemoveType );

	if( S_OK != GetPixelDepth() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetPixelDepthFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( SVDoubleValueObjectClass* pdvoLinearData = GetInputLinearData() )
	{
		m_svLinearEdges.SetArraySize( pdvoLinearData->getArraySize() );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

bool SVLinearEdgeProcessingClass::onRun( SVRunStatusClass &p_rsvRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = __super::onRun(p_rsvRunStatus, pErrorMessages);

	if ( S_OK != UpdateUpperThresholdValues() ||
		  S_OK != UpdateLowerThresholdValues() ||
		  S_OK != UpdateEdgeList() )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return l_bOk;
}

SVImageClass *SVLinearEdgeProcessingClass::GetInputImage()
{
	SVImageClass *l_psvImage = nullptr;

	if( m_svInputImageObjectInfo.IsConnected() && 
		  nullptr != m_svInputImageObjectInfo.GetInputObjectInfo().m_pObject )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(m_svInputImageObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return l_psvImage;
}

SVDoubleValueObjectClass* SVLinearEdgeProcessingClass::GetInputLinearData()
{
	SVDoubleValueObjectClass* pData = nullptr;

	if (m_svInputLinearData.IsConnected() && m_svInputLinearData.GetInputObjectInfo().m_pObject)
	{
		pData = dynamic_cast <SVDoubleValueObjectClass*> (m_svInputLinearData.GetInputObjectInfo().m_pObject);
	}

	return pData;
}

HRESULT SVLinearEdgeProcessingClass::GetInputMinThreshold(double& rMinThreshold)
{
	HRESULT Result(E_FAIL);

	if( m_svInputMinThreshold.IsConnected() && nullptr != m_svInputMinThreshold.GetInputObjectInfo().m_pObject )
	{
		Result = m_svInputMinThreshold.GetInputObjectInfo().m_pObject->getValue(rMinThreshold);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetInputMaxThreshold(double& rMaxThreshold)
{
	HRESULT Result(E_FAIL);

	if( m_svInputMaxThreshold.IsConnected() && nullptr != m_svInputMaxThreshold.GetInputObjectInfo().m_pObject )
	{
		Result = m_svInputMaxThreshold.GetInputObjectInfo().m_pObject->getValue(rMaxThreshold);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetInputLinearData(std::vector<double>& rData)
{
	HRESULT Result(E_FAIL);

	if( m_svInputLinearData.IsConnected() && nullptr != m_svInputLinearData.GetInputObjectInfo().m_pObject )
	{
		Result = m_svInputLinearData.GetInputObjectInfo().m_pObject->getValues(rData);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;

	if( nullptr != GetInputImage() )
	{
	  SVImageInfoClass ImageInfo = GetInputImage()->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetOutputEdgeDistance( double& rValue )
{
	HRESULT l_hrOk = S_FALSE;

	std::vector<double> Data;

	rValue = 0.0;

	if( S_OK == GetInputLinearData(Data) )
	{
		BOOL IsFixedEdgeMarker = false;
		long Position( 0L );

		double l_dWidth = static_cast<double>(Data.size());
		double l_dEndOfLine = static_cast<double>(Data.size() - 1);

		if( ( S_OK == m_svIsFixedEdgeMarker.GetValue( IsFixedEdgeMarker ) ) && 
				IsFixedEdgeMarker &&
				( S_OK == m_svPosition.GetValue( Position ) ) )
		{
			// Get the selected fixed position...
			switch ( Position )
			{
				case SV_START_POSITION:
				{
					// Get measure line start position
					// Allways 0!

					rValue = 0.0;

					l_hrOk = S_OK;

					break;
				}

				case SV_END_POSITION:
				{
					// Get measure line end position
					// Allways line length - 1, if length > 0!

					rValue = l_dEndOfLine;

					l_hrOk = S_OK;

					break;
				}
			
				case SV_CENTER_POSITION:
				{
					// Get measure line center position
					// Allways ( line length / 2 ), if ( length / 2 ) > 0!
					rValue = l_dWidth / 2.0;

					l_hrOk = S_OK;

					break;
				}

				case SV_OFFSET_POSITION:
				{
					double Offset = 0.0;

					if( ( S_OK == m_svPositionOffsetValue.GetValue( Offset ) ) && 
					    Offset <= l_dEndOfLine )
					{
						rValue = Offset;

						l_hrOk = S_OK;
					}

					break;
				}
			}
		}
		else
		{
			std::vector<double> Edges;

			long EdgeSelect( 0 );

			if( ( S_OK == m_svLinearEdges.GetArrayValues( Edges ) ) &&
				( S_OK == m_svEdgeSelect.GetValue( EdgeSelect ) ) )
			{
				switch ( EdgeSelect )
				{
					case SV_FIRST_EDGE:
					{
						// Get first found edge position
						// Allways first edge, if edge number > 0!
						if( 0 < Edges.size() )
						{
							rValue = Edges[ 0 ];

							l_hrOk = S_OK;
						}

						break;
					}

					case SV_LAST_EDGE:
					{
						// Get last found edge position
						// Allways last edge, if edge number > 0!
						if( 0 < Edges.size() )
						{
							rValue = Edges[ Edges.size() - 1 ];

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

						double dEdgeSelect( 0.0 );
						long Direction( 0 );

						if( ( S_OK == m_svEdgeSelectThisValue.GetValue( dEdgeSelect ) ) &&
						    ( S_OK == m_svDirection.GetValue( Direction ) ) )
						{
							long Edge = static_cast<long> (dEdgeSelect);
							double dPercent = dEdgeSelect - Edge;

							switch( Direction )
							{
								case SV_HEAD_TO_TAIL_DIRECTION:
								{
									if( Edge - 1 < static_cast<long>(Edges.size()) )
									{
										double l_dFirstEdge = ( 0 <= Edge - 1 && Edge - 1 < static_cast<long>(Edges.size()) ) ? Edges[ Edge - 1 ] : 0.0;
										double l_dSecondEdge = ( 0 <= Edge && Edge < static_cast<long>(Edges.size()) ) ? Edges[ Edge ] : l_dEndOfLine;

										rValue = l_dFirstEdge + ( l_dSecondEdge - l_dFirstEdge ) * dPercent;

										rValue = ( 0.0 < rValue ) ? rValue : 0.0;

										l_hrOk = S_OK;
									}

									break;
								}

								case SV_TAIL_TO_HEAD_DIRECTION:
								{
									if( Edge - 1 < static_cast<long>(Edges.size()) )
									{
										double l_dFirstEdge = ( 0 <= Edge - 1 && Edge - 1 < static_cast<long>(Edges.size()) ) ? Edges[ Edge - 1 ] : l_dEndOfLine;
										double l_dSecondEdge = ( 0 <= Edge && Edge < static_cast<long>(Edges.size()) ) ? Edges[ Edge ] : 0.0;

										rValue = l_dFirstEdge + ( l_dSecondEdge - l_dFirstEdge ) * dPercent;

										rValue = ( 0.0 < rValue ) ? rValue : 0.0;

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

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( l_svExtents ) ||
		S_OK != l_svExtents.GetOutputRectangle( l_oRect ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
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

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( l_svExtents ) ||
		S_OK != l_svExtents.GetOutputRectangle( l_oRect ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
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

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( l_svExtents ) ||
		S_OK != l_svExtents.GetOutputRectangle( l_oRect ) ||
		( S_OK != m_svLowerThresholdValue.GetValue( l_ulLower ) ) ||
		( S_OK != m_svUpperThresholdValue.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		double l_dDistance = 0.0;

		l_hrOk = GetOutputEdgeDistance( l_dDistance );

		if( S_OK == l_hrOk )
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

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( l_svExtents ) ||
		S_OK != l_svExtents.GetOutputRectangle( l_oRect ) ||
		( S_OK != m_svLowerThresholdValue.GetValue( l_ulLower ) ) ||
		( S_OK != m_svUpperThresholdValue.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
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

HRESULT SVLinearEdgeProcessingClass::GetHistogramOverlay( SVExtentLineStruct &rLine )
{
	RECT l_oRect;

	SVImageExtentClass Extents;

	std::vector<double> Data;

	HRESULT l_hrOk = rLine.Initialize();

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( Extents ) ||
		S_OK != Extents.GetOutputRectangle( l_oRect ) ||
		(S_OK != GetInputLinearData( Data )) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		double divisor = m_dwColorNumber;

		SVExtentPointStruct l_svPoint;
	
		rLine.m_dwColor = m_cfHistogram;

		for( size_t l = 0; l < Data.size(); l++ )
		{
			long l_lPixel = ( long ) ( Data[ l ] / divisor * ( ( double ) l_oRect.bottom ) );

			SVExtentPointStruct l_oGraphPoint;
			l_oGraphPoint.m_dPositionX = static_cast<double>(l);
			l_oGraphPoint.m_dPositionY = l_oRect.bottom - l_lPixel;

			rLine.m_svPointArray.SetAtGrow( static_cast<int>(l), l_oGraphPoint );
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetEdgesOverlay( SVExtentMultiLineStruct &rMultiLine )
{
	RECT l_oRect;

	SVImageExtentClass Extents;

	HRESULT l_hrOk = rMultiLine.Initialize();

	std::vector<double> Edges;

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( Extents ) ||
		S_OK != Extents.GetOutputRectangle( l_oRect ) ||
		 ( S_OK != m_svLinearEdges.GetArrayValues( Edges ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		long Direction( SV_UNDEFINED_DIRECTION );

		m_svDirection.GetValue( Direction );

		SVExtentLineStruct EdgeLine;

		EdgeLine.m_bIsAngleValid = true;
		EdgeLine.m_dAngle = 90.0;
		EdgeLine.m_dwColor = m_cfEdges;

		for( size_t l = 0; l < Edges.size(); l++ )
		{
			double l_dX = Edges[ l ];

			if( SV_UNDEFINED_DIRECTION == Direction )
			{
				l_dX = static_cast<int>(l);

				if( Edges[ l ] == 0 )
				{
					EdgeLine.m_dwColor = SV_DEFAULT_BLACK_COLOR;
				}
				else if( Edges[ l ] == m_dwColorNumber - 1 )
				{
					EdgeLine.m_dwColor = SV_DEFAULT_WHITE_COLOR;
				}
				else
				{
					EdgeLine.m_dwColor = m_cfEdges;
				}
			}

			SVExtentPointStruct l_oGraphPoint;

			l_oGraphPoint.m_dPositionX = l_dX;
			l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 - 5;
			
			EdgeLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

			l_oGraphPoint.m_dPositionX = l_dX;
			l_oGraphPoint.m_dPositionY = l_oRect.bottom / 2 + 5;
			
			EdgeLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

			rMultiLine.m_svLineArray.Add( EdgeLine );
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

	SVAnalyzerClass* pAnalyzer = dynamic_cast<SVAnalyzerClass*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent( l_svExtents ) ||
		S_OK != l_svExtents.GetOutputRectangle( l_oRect ) ||
		S_OK != GetOutputEdgeDistance( l_dDistance ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
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

HRESULT SVLinearEdgeProcessingClass::UpdateUpperThresholdValues()
{
	HRESULT Result( S_OK );
	HRESULT hTemp( S_OK );

	BOOL State( FALSE );

	double dMin = 0.0;
	double dMax = 0.0;

	double dUpper = 0.0;

	if( ( S_OK != GetInputMinThreshold(dMin) ) )
	{
		Result = S_FALSE;
	}

	if( S_OK != GetInputMaxThreshold(dMax) )
	{
		Result = S_FALSE;
	}

	if( S_OK == ( hTemp = m_svUseUpperThresholdSelectable.GetValue( State ) ) )
	{
		if( ! State )
		{
			if( ( S_OK == m_svUseUpperThresholdMaxMinusPercentDiff.GetValue( State ) ) && State )
			{
				DWORD PercentDiff( 0 );

				if( S_OK == m_svUpperMaxMinusPercentDiffValue.GetValue( PercentDiff )  )
				{
					if( 0 != PercentDiff )
					{
						double dPercent = static_cast<double> (PercentDiff) / 100.0;

						dUpper = dMax - ( ( dMax - dMin ) * dPercent );
					}
					else
					{
						dUpper = dMax;
					}
				}
				else
				{
					Result = S_FALSE;
				}
			}
			else if( ( S_OK == m_svUseUpperThresholdMaxMinusOffset.GetValue( State ) ) && State )
			{
				DWORD Offset( 0 );

				if( S_OK == (hTemp = m_svUpperMaxMinusOffsetValue.GetValue( Offset )) )
				{
					dUpper = dMax -  static_cast<double> (Offset);
				}
				else
				{
					Result = hTemp;
				}
			}
			else if( ( S_OK == m_svUseUpperThresholdMinPlusOffset.GetValue( State ) ) && State )
			{
				DWORD Offset( 0 );

				if( S_OK == ( hTemp = m_svUpperMinPlusOffsetValue.GetValue( Offset ) ) )
				{
					dUpper = dMin + static_cast<double> (Offset);
				}
				else
				{
					Result = hTemp;
				}
			}

			if( dUpper < 0.0 )
			{
				dUpper = 0.0;
			}

			if( 255.0 < dUpper )
			{
				dUpper = 255.0;
			}

			if( S_OK != ( hTemp = m_svUpperThresholdValue.SetValue(static_cast<DWORD> (dUpper)) ) )
			{
				Result = hTemp;
			}
		}
	}
	else
	{
		Result = hTemp;
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::UpdateLowerThresholdValues()
{
	HRESULT Result( S_OK );
	HRESULT hTemp( S_OK );

	BOOL bState( false );
	double dMin( 0.0 );
	double dMax( 0.0 );
	double dLower( 0.0 );

	if( S_OK != GetInputMinThreshold(dMin) )
	{
		Result = S_FALSE;
	}

	if( S_OK != GetInputMaxThreshold(dMax) )
	{
		Result = S_FALSE;
	}

	if( S_OK == ( hTemp = m_svUseLowerThresholdSelectable.GetValue( bState ) ) )
	{
		if( ! bState )
		{
			if( ( S_OK == m_svUseLowerThresholdMaxMinusPercentDiff.GetValue( bState ) ) && bState )
			{
				DWORD PercentDiff( 0 );

				if( S_OK == ( hTemp = m_svLowerMaxMinusPercentDiffValue.GetValue( PercentDiff ) ) )
				{
					if( 0 != PercentDiff )
					{
						double l_dPercent = static_cast<double> (PercentDiff) / 100.0;

						dLower = dMax - ( ( dMax - dMin ) * l_dPercent );
					}
					else
					{
						dLower = dMax;
					}
				}
				else
				{
					Result = hTemp;
				}
			}
			else if( ( S_OK == m_svUseLowerThresholdMaxMinusOffset.GetValue( bState ) ) && bState )
			{
				DWORD Offset( 0 );

				if( S_OK == ( hTemp = m_svLowerMaxMinusOffsetValue.GetValue( Offset ) ) )
				{
					dLower = dMax - static_cast<double> (Offset);
				}
				else
				{
					Result = hTemp;
				}
			}
			else if( ( S_OK == m_svUseLowerThresholdMinPlusOffset.GetValue( bState ) ) && bState )
			{
				DWORD Offset( 0 );

				if( S_OK == ( hTemp = m_svLowerMinPlusOffsetValue.GetValue( Offset ) ) )
				{
					dLower = dMin + static_cast<double> (Offset);
				}
				else
				{
					Result = hTemp;
				}
			}

			if( dLower < 0.0 )
			{
				dLower = 0.0;
			}

			if( 255.0 < dLower )
			{
				dLower = 255.0;
			}

			if( S_OK != ( hTemp = m_svLowerThresholdValue.SetValue(static_cast<DWORD> (dLower))))
			{
				Result = hTemp;
			}
		}
	}
	else
	{
		Result = hTemp;
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::UpdateEdgeList()
{
	HRESULT Result( S_OK );

	std::vector<double> Data;
	std::vector<double> Edges;

	DWORD Upper( 0 );
	DWORD Lower( 0 );
	long Direction( 0L );
	long Polarisation( 0L );

	Result = GetInputLinearData( Data );

	if( S_OK == Result )
	{
		Result = m_svUpperThresholdValue.GetValue( Upper );
	}

	if( S_OK == Result )
	{
		Result = m_svLowerThresholdValue.GetValue( Lower );
	}

	if( S_OK == Result )
	{
		Result = m_svDirection.GetValue( Direction );
	}

	if( S_OK == Result )
	{
		Result = m_svPolarisation.GetValue( Polarisation );
	}

	if( S_OK == Result )
	{
		long l_lCount = static_cast<long>(Data.size());

		switch( Direction )
		{
			case SV_HEAD_TO_TAIL_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < l_lCount - 1; ++l )
				{
					if( S_OK == IsEdge( Data[ l ], Data[ l + 1 ], Upper, Lower, Polarisation ) )
					{
						if( Polarisation == SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], Upper, Lower, SV_POSITIVE_POLARISATION, l_dDistance1 );
							bool l_bDistance2 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], Upper, Lower, SV_NEGATIVE_POLARISATION, l_dDistance2 );

							if ( l_dDistance1 <= l_dDistance2 )
							{
								if( l_bDistance1 )
								{
									l_dDistance1 = l + l_dDistance1;

									Edges.push_back( l_dDistance1 );
								}

								if( l_bDistance2 )
								{
									l_dDistance2 = l + l_dDistance2;

									Edges.push_back( l_dDistance2 );
								}
							}
							else
							{
								if( l_bDistance2 )
								{
									l_dDistance2 = l + l_dDistance2;

									Edges.push_back( l_dDistance2 );
								}

								if( l_bDistance1 )
								{
									l_dDistance1 = l + l_dDistance1;

									Edges.push_back( l_dDistance1 );
								}
							}
						}
						else
						{
							double l_dDistance = 0.0;

							if( S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], Upper, Lower, Polarisation, l_dDistance ) )
							{
								l_dDistance = l + l_dDistance;

								Edges.push_back( l_dDistance );
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
					if( S_OK == IsEdge( Data[ l ], Data[ l - 1 ], Upper, Lower, Polarisation ) )
					{
						if( Polarisation == SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], Upper, Lower, SV_POSITIVE_POLARISATION, l_dDistance1 );
							bool l_bDistance2 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], Upper, Lower, SV_NEGATIVE_POLARISATION, l_dDistance2 );

							if ( l_dDistance1 <= l_dDistance2 )
							{
								if( l_bDistance1 )
								{
									l_dDistance1 = l - l_dDistance1;

									Edges.push_back( l_dDistance1 );
								}

								if( l_bDistance2 )
								{
									l_dDistance2 = l - l_dDistance2;

									Edges.push_back( l_dDistance2 );
								}
							}
							else
							{
								if( l_bDistance2 )
								{
									l_dDistance2 = l - l_dDistance2;

									Edges.push_back( l_dDistance2 );
								}

								if( l_bDistance1 )
								{
									l_dDistance1 = l - l_dDistance1;

									Edges.push_back( l_dDistance1 );
								}
							}
						}
						else
						{
							double l_dDistance = 0.0;

							if( S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], Upper, Lower, Polarisation, l_dDistance ) )
							{
								l_dDistance = l - l_dDistance;

								Edges.push_back( l_dDistance );
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

					if( S_OK == GetBlackWhiteEdgeValue( Data[ l ], Upper, Lower, l_dValue ) )
					{
						Edges.push_back( l_dValue );
					}
					else
					{
						Edges.push_back( Data[ l ] );
					}
				}

				break;
			}
		}
	}

	// always do the Set
	HRESULT hrSet = m_svLinearEdges.SetArrayValues(Edges);

	if ( S_OK == Result )
	{
		Result = hrSet;
	}

	return Result;
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

		if( S_OK == l_hrOk )
		{
			p_rdDistance = fabs( l_dwThreshold - p_dStart ) / fabs( p_dEnd - p_dStart );
		}
	}

	return l_hrOk;
}

