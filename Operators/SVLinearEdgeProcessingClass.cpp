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

#pragma region Includes
#include "stdafx.h"
#include "SVLinearEdgeProcessingClass.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/LinearEdgeEnums.h"
#include "InspectionEngine/SVImageClass.h"
#include "Tools/SVTool.h"
#include "AnalyzerOperators/Analyzer.h"
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVLinearEdgeProcessingClass, SVLinearEdgeProcessingClassGuid );


SVLinearEdgeProcessingClass::SVLinearEdgeProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                  :SVTaskObjectClass( POwner, StringResourceID )
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVLinearEdgeProcessingObjectType;

	m_InputImage.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_InputImage, _T( "LinearEdgeProcessingImage" ), SvPb::ImageInputEId);

	m_InputMinThreshold.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::LinearThresholdMinEId);
	registerInputObject( &m_InputMinThreshold, _T( "LinearEdgeProcessingMinThreshold" ), SvPb::MinThresholdInputEId);
	m_InputMinThreshold.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_InputMaxThreshold.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::LinearThresholdMaxEId);
	registerInputObject(&m_InputMaxThreshold, _T("LinearEdgeProcessingMaxThreshold"), SvPb::MaxThresholdInputEId);
	m_InputMaxThreshold.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_InputDelta.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::LinearThresholdDeltaEId);
	registerInputObject(&m_InputDelta, _T("LinearEdgeProcessingDeltaThreshold"), SvPb::DeltaThresholdInputEId);
	m_InputDelta.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_InputLinearData.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::LinearDataClassEId);
	registerInputObject( &m_InputLinearData, _T( "LinearEdgeProcessingInputLinearData" ), SvPb::LinearDataInputEId);
	m_InputLinearData.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_lPixelDepth = 0;
	m_dwMinThreshold = 0;
	m_dwMaxThreshold = 0;
	m_dwColorNumber = 0;

	m_svDirection.SetEnumTypes( SvDef::cDirectionEnums );
	m_svPolarisation.SetEnumTypes( SvDef::cPolarisationEnums );
	m_svEdgeSelect.SetEnumTypes( SvDef::cEdgeSelectEnums );
	m_svPosition.SetEnumTypes( SvDef::cPositionEnums );

	m_svDirection.SetDefaultValue(SvDef::SV_HEAD_TO_TAIL_DIRECTION);
	m_svPolarisation.SetDefaultValue(SvDef::SV_ANY_POLARISATION);
	m_svEdgeSelect.SetDefaultValue(SvDef::SV_FIRST_EDGE);
	m_svEdgeSelectThisValue.SetDefaultValue(1.0);

	m_svIsFixedEdgeMarker.SetDefaultValue(BOOL(false));
	m_svPosition.SetDefaultValue(SvDef::SV_START_POSITION);
	m_svPositionOffsetValue.SetDefaultValue(1.0);

	m_svUseLowerThresholdSelectable.SetDefaultValue(BOOL(true));
	m_svUseLowerThresholdMaxMinusPercentDiff.SetDefaultValue(BOOL(false));
	m_svUseLowerThresholdMaxMinusOffset.SetDefaultValue(BOOL(false));
	m_svUseLowerThresholdMinPlusOffset.SetDefaultValue(BOOL(false));
	m_svLowerThresholdSelected.SetDefaultValue(SvDef::cDefaultToolLowerThreshold);
	m_svLowerMaxMinusPercentDiffValue.SetDefaultValue(0l);
	m_svLowerMaxMinusOffsetValue.SetDefaultValue(0l);
	m_svLowerMinPlusOffsetValue.SetDefaultValue(0l);

	m_svUseUpperThresholdSelectable.SetDefaultValue(BOOL(true));
	m_svUseUpperThresholdMaxMinusPercentDiff.SetDefaultValue(BOOL(false));
	m_svUseUpperThresholdMaxMinusOffset.SetDefaultValue(BOOL(false));
	m_svUseUpperThresholdMinPlusOffset.SetDefaultValue(BOOL(false));
	m_svUpperThresholdSelected.SetDefaultValue(SvDef::cDefaultToolUpperThreshold);
	m_svUpperMaxMinusPercentDiffValue.SetDefaultValue(0l);
	m_svUpperMaxMinusOffsetValue.SetDefaultValue(0l);
	m_svUpperMinPlusOffsetValue.SetDefaultValue(0l);

	m_cfThresholds = SvDef::DefaultSubFunctionColor1;
	m_cfHistogram = SvDef::DefaultSubFunctionColor1;
	m_cfEdges = SvDef::DefaultSubFunctionColor2;

	m_svLinearEdges.SetLegacyVectorObjectCompatibility();
	m_svLinearEdges.setSaveValueFlag(false);
}

SVLinearEdgeProcessingClass::~SVLinearEdgeProcessingClass()
{
}

bool SVLinearEdgeProcessingClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
  SVImageInfoClass ImageInfo;

	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	bOk = bOk && S_OK == GetPixelDepth();

	if( bOk )
	{
		m_dwMinThreshold = 0;
		m_dwMaxThreshold = (1 << m_lPixelDepth) - 1;
		m_dwColorNumber = 1 << m_lPixelDepth;
	}

	//set attributes for Upper and Lower Threshold values.
	m_svUpperThresholdSelected.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
	m_svUpperThresholdSelected.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );

	m_svLowerThresholdSelected.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
	m_svLowerThresholdSelected.SetObjectAttributesAllowed( SvPb::extentObject, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearEdges.SetObjectAttributesAllowed( SvPb::viewable | SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );


	BOOL bUpper = FALSE;
	BOOL bLower = FALSE;

	m_svUseLowerThresholdSelectable.GetValue(bLower);
	m_svUseUpperThresholdSelectable.GetValue(bUpper);

	SvOi::SetAttributeType AddRemoveType = bLower ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svLowerThresholdSelected.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

	AddRemoveType = bUpper ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
	m_svUpperThresholdSelected.SetObjectAttributesAllowed( SvPb::audittrail, AddRemoveType );

	SvVol::SVDoubleValueObjectClass* pLinearData = m_InputLinearData.getInput<SvVol::SVDoubleValueObjectClass>();
	if (nullptr != pLinearData)
	{
		m_svLinearEdges.SetArraySize( pLinearData->getArraySize() );
	}

	return bOk;
}

SVLinearEdgeProcessingClass::UpdateEnum SVLinearEdgeProcessingClass::getThresholdMode(const SvVol::SVBoolValueObjectClass& rUseSelected, const SvVol::SVBoolValueObjectClass& rUsePercentDiff, const SvVol::SVBoolValueObjectClass& rUseMinOffset, const SvVol::SVBoolValueObjectClass& rUseMaxOffset)
{
	BOOL bUsed {false};
	rUseSelected.GetValue(bUsed);
	if (bUsed)
	{
		return UpdateEnum::Selected;
	}

	rUsePercentDiff.GetValue(bUsed);
	if (bUsed)
	{
		return UpdateEnum::PercentDiff;
	}

	rUseMinOffset.GetValue(bUsed);
	if (bUsed)
	{
		return UpdateEnum::MinOffset;
	}

	rUseMaxOffset.GetValue(bUsed);
	if (bUsed)
	{
		return UpdateEnum::MaxOffset;
	}
	return UpdateEnum::None;
}

bool SVLinearEdgeProcessingClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVTaskObjectClass::ResetObject(pErrorMessages);

	m_InputImage.validateInput();
	m_InputMinThreshold.validateInput();
	m_InputMaxThreshold.validateInput();
	m_InputDelta.validateInput();
	m_InputLinearData.validateInput();

	m_updateLowerState = getThresholdMode(m_svUseLowerThresholdSelectable, m_svUseLowerThresholdMaxMinusPercentDiff, m_svUseLowerThresholdMinPlusOffset, m_svUseLowerThresholdMaxMinusOffset);
	UpdateThresholdValues(m_updateLowerState, m_svLowerThresholdSelected, m_svLowerMaxMinusPercentDiffValue, m_svLowerMinPlusOffsetValue, m_svLowerMaxMinusOffsetValue, m_lowerThreshold, m_svLowerThresholdValueObject);
	m_needUpdateThresholdValueLower = (UpdateEnum::Selected != m_updateLowerState || SvPb::LinkedSelectedType::DirectValue != m_svLowerThresholdSelected.getSelectedType());

	m_updateUpperState = getThresholdMode(m_svUseUpperThresholdSelectable, m_svUseUpperThresholdMaxMinusPercentDiff, m_svUseUpperThresholdMinPlusOffset, m_svUseUpperThresholdMaxMinusOffset);
	UpdateThresholdValues(m_updateUpperState, m_svUpperThresholdSelected, m_svUpperMaxMinusPercentDiffValue, m_svUpperMinPlusOffsetValue, m_svUpperMaxMinusOffsetValue, m_upperThreshold, m_svUpperThresholdValueObject);
	m_needUpdateThresholdValueUpper = (UpdateEnum::Selected != m_updateUpperState || SvPb::LinkedSelectedType::DirectValue != m_svUpperThresholdSelected.getSelectedType());

	if( S_OK != GetPixelDepth() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetPixelDepthFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	SvVol::SVDoubleValueObjectClass* pLinearData = m_InputLinearData.getInput<SvVol::SVDoubleValueObjectClass>();
	if (nullptr != pLinearData)
	{
		m_svLinearEdges.SetArraySize( pLinearData->getArraySize() );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		} 
	}

	return Result;
}

bool SVLinearEdgeProcessingClass::onRun( SvIe::RunStatus &p_rsvRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool l_bOk = __super::onRun(p_rsvRunStatus, pErrorMessages);

	if ((m_needUpdateThresholdValueLower && false == UpdateThresholdValues(m_updateLowerState, m_svLowerThresholdSelected, m_svLowerMaxMinusPercentDiffValue, m_svLowerMinPlusOffsetValue, m_svLowerMaxMinusOffsetValue, m_lowerThreshold, m_svLowerThresholdValueObject)) ||
		(m_needUpdateThresholdValueUpper && false == UpdateThresholdValues(m_updateUpperState, m_svUpperThresholdSelected, m_svUpperMaxMinusPercentDiffValue, m_svUpperMinPlusOffsetValue, m_svUpperMaxMinusOffsetValue, m_upperThreshold, m_svUpperThresholdValueObject)) ||
		  S_OK != UpdateEdgeList() )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return l_bOk;
}

HRESULT SVLinearEdgeProcessingClass::GetInputMinThreshold(double& rMinThreshold)
{
	HRESULT Result(E_FAIL);

	if( m_InputMinThreshold.IsConnected() && nullptr != m_InputMinThreshold.GetInputObjectInfo().getObject() )
	{
		Result = m_InputMinThreshold.GetInputObjectInfo().getObject()->getValue(rMinThreshold);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetInputMaxThreshold(double& rMaxThreshold)
{
	HRESULT Result(E_FAIL);

	if( m_InputMaxThreshold.IsConnected() && nullptr != m_InputMaxThreshold.GetInputObjectInfo().getObject() )
	{
		Result = m_InputMaxThreshold.GetInputObjectInfo().getObject()->getValue(rMaxThreshold);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetInputLinearVectorData(std::vector<double>& rData)
{
	HRESULT Result(E_FAIL);

	if( m_InputLinearData.IsConnected() && nullptr != m_InputLinearData.GetInputObjectInfo().getObject() )
	{
		Result = m_InputLinearData.GetInputObjectInfo().getObject()->getValues(rData);
	}

	return Result;
}

HRESULT SVLinearEdgeProcessingClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;
	
	SvIe::SVImageClass* pInputImage = m_InputImage.getInput<SvIe::SVImageClass>();
	if (nullptr != pInputImage)
	{
	  SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetOutputEdgeDistance( double& rValue )
{
	HRESULT l_hrOk = S_FALSE;

	std::vector<double> Data;

	rValue = 0.0;

	if( S_OK == GetInputLinearVectorData(Data) )
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
				case SvDef::SV_START_POSITION:
				{
					// Get measure line start position
					// Allways 0!

					rValue = 0.0;

					l_hrOk = S_OK;

					break;
				}

				case SvDef::SV_END_POSITION:
				{
					// Get measure line end position
					// Allways line length - 1, if length > 0!

					rValue = l_dEndOfLine;

					l_hrOk = S_OK;

					break;
				}
			
				case SvDef::SV_CENTER_POSITION:
				{
					// Get measure line center position
					// Allways ( line length / 2 ), if ( length / 2 ) > 0!
					rValue = l_dWidth / 2.0;

					l_hrOk = S_OK;

					break;
				}

				case SvDef::SV_OFFSET_POSITION:
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
					case SvDef::SV_FIRST_EDGE:
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

					case SvDef::SV_LAST_EDGE:
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

					case SvDef::SV_THIS_EDGE:
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
								case SvDef::SV_HEAD_TO_TAIL_DIRECTION:
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

								case SvDef::SV_TAIL_TO_HEAD_DIRECTION:
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

HRESULT SVLinearEdgeProcessingClass::GetPointFromDistance(double p_dDistance, SVPoint<double>& rPoint)
{
	RECT rect{0L, 0L, 0L, 0L};

	HRESULT l_hrOk{S_OK};
	rPoint.clear();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		rPoint.m_x = p_dDistance;
		rPoint.m_y = rect.bottom / 2;
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetEdgeOverlayFromDistance( double dDistance, SVExtentLineStruct &rLine )
{
	RECT rect {0L, 0L, 0L, 0L};
	HRESULT l_hrOk{S_OK};
	
	rLine.Initialize();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{

		rLine.m_bIsAngleValid = true;
		rLine.m_dAngle = 90.0;

		SVPoint<double> graphPoint{dDistance, static_cast<double> (rect.bottom / 2 - 10)};
		
		rLine.m_PointVector.emplace_back(graphPoint);

		graphPoint.m_x = dDistance;
		graphPoint.m_y = static_cast<double> (rect.bottom / 2 + 10);
		
		rLine.m_PointVector.emplace_back(graphPoint);

		rLine.m_dwColor = GetObjectColor();
	}

	return l_hrOk;

}

HRESULT SVLinearEdgeProcessingClass::GetOutputEdgePoint(SVPoint<double>& rPoint)
{
	RECT rect {0L, 0L, 0L, 0L};

	unsigned long l_ulUpper = 0;
	unsigned long l_ulLower = 0;

	HRESULT l_hrOk{S_OK};
	
	rPoint.clear();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) ||
		( S_OK != m_svLowerThresholdValueObject.GetValue( l_ulLower ) ) ||
		( S_OK != m_svUpperThresholdValueObject.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		double l_dDistance = 0.0;

		l_hrOk = GetOutputEdgeDistance( l_dDistance );

		if( S_OK == l_hrOk )
		{
			rPoint.m_x = l_dDistance;
			rPoint.m_y = rect.bottom / 2;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetThresholdBarsOverlay( SVExtentMultiLineStruct& rMultiLine )
{
	RECT rect {0L, 0L, 0L, 0L};

	unsigned long l_ulUpper = 0;
	unsigned long l_ulLower = 0;

	HRESULT l_hrOk{S_OK};
	rMultiLine.Initialize();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) ||
		( S_OK != m_svLowerThresholdValueObject.GetValue( l_ulLower ) ) ||
		( S_OK != m_svUpperThresholdValueObject.GetValue( l_ulUpper ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		SVExtentLineStruct l_svUpperLine;
		SVExtentLineStruct l_svLowerLine;

		double divisor = m_dwColorNumber;

		l_svUpperLine.m_bIsAngleValid = true;
		l_svUpperLine.m_dAngle = 0.0;
		l_svUpperLine.m_dwColor = m_cfThresholds;

		l_svLowerLine.m_bIsAngleValid = true;
		l_svLowerLine.m_dAngle = 0.0;
		l_svLowerLine.m_dwColor = m_cfThresholds;

		// Calc current Threshold bars...
		long lMaxThresholdValue = ( long ) ( l_ulUpper / divisor * ( ( double ) rect.bottom ) );
		long lMinThresholdValue = ( long ) ( l_ulLower / divisor * ( ( double ) rect.bottom ) );

		// MaxThresholdBar
		// Max left...
		SVPoint<double> graphPoint{0.0, static_cast<double> (rect.bottom - lMaxThresholdValue)};
		
		l_svUpperLine.m_PointVector.emplace_back(graphPoint);

		graphPoint.m_x = rect.right;

		l_svUpperLine.m_PointVector.emplace_back(graphPoint);

		// MinThresholdBar
		// Min left...
		graphPoint.m_x = 0;
		graphPoint.m_y = static_cast<double> (rect.bottom - lMinThresholdValue);
		
		l_svLowerLine.m_PointVector.emplace_back(graphPoint);

		graphPoint.m_x = rect.right;

		l_svLowerLine.m_PointVector.emplace_back(graphPoint);

		rMultiLine.m_svLineArray.emplace_back( l_svUpperLine );
		rMultiLine.m_svLineArray.emplace_back( l_svLowerLine );
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetHistogramOverlay( SVExtentLineStruct &rLine )
{
	RECT rect {0L, 0L, 0L, 0L};
	std::vector<double> Data;

	HRESULT l_hrOk{S_OK};
	rLine.Initialize();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) ||
		(S_OK != GetInputLinearVectorData( Data )) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		double divisor = m_dwColorNumber;

		rLine.m_dwColor = m_cfHistogram;

		for( size_t l = 0; l < Data.size(); l++ )
		{
			long pixel = static_cast<long> ( Data[l] / divisor * static_cast<double> (rect.bottom) );

			rLine.m_PointVector.emplace_back(static_cast<double> (l), static_cast<double> (rect.bottom - pixel));
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetEdgesOverlay( SVExtentMultiLineStruct &rMultiLine )
{
	RECT rect {0L, 0L, 0L, 0L};
	HRESULT l_hrOk{S_OK};
	rMultiLine.Initialize();

	std::vector<double> Edges;

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) ||
		 ( S_OK != m_svLinearEdges.GetArrayValues( Edges ) ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		long Direction( SvDef::SV_UNDEFINED_DIRECTION );

		m_svDirection.GetValue( Direction );

		SVExtentLineStruct EdgeLine;

		EdgeLine.m_bIsAngleValid = true;
		EdgeLine.m_dAngle = 90.0;
		EdgeLine.m_dwColor = m_cfEdges;

		for( size_t l = 0; l < Edges.size(); l++ )
		{
			double dX = Edges[l];

			if(SvDef::SV_UNDEFINED_DIRECTION == Direction)
			{
				dX = static_cast<int>(l);

				if( Edges[ l ] == 0 )
				{
					EdgeLine.m_dwColor = SvDef::DefaultBlackColor;
				}
				else if( Edges[ l ] == m_dwColorNumber - 1 )
				{
					EdgeLine.m_dwColor = SvDef::DefaultWhiteColor;
				}
				else
				{
					EdgeLine.m_dwColor = m_cfEdges;
				}
			}
			EdgeLine.m_PointVector.clear();

			SVPoint<double> graphPoint{dX, static_cast<double> (rect.bottom / 2 - 5)};
			
			EdgeLine.m_PointVector.emplace_back( graphPoint );

			graphPoint.m_y = static_cast<double> (rect.bottom / 2 + 5);
			
			EdgeLine.m_PointVector.emplace_back(graphPoint);

			rMultiLine.m_svLineArray.emplace_back( EdgeLine );
		}
	}

	return l_hrOk;
}

HRESULT SVLinearEdgeProcessingClass::GetSelectedEdgeOverlay( SVExtentLineStruct &rLine )
{
	RECT rect {0L, 0L, 0L, 0L};
	double l_dDistance = 0.0;

	HRESULT l_hrOk{S_OK};
	rLine.Initialize();

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if( nullptr == pAnalyzer ||
		S_OK != pAnalyzer->GetImageExtent().GetOutputRectangle( rect ) ||
		S_OK != GetOutputEdgeDistance( l_dDistance ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		SVPoint<double> graphPoint;

		rLine.m_bIsAngleValid = true;
		rLine.m_dAngle = 90.0;

		graphPoint.m_x = l_dDistance;
		graphPoint.m_y = rect.bottom / 2 - 10;
		
		rLine.m_PointVector.push_back( graphPoint );

		graphPoint.m_x = l_dDistance;
		graphPoint.m_y = rect.bottom / 2 + 10;
		
		rLine.m_PointVector.push_back( graphPoint );

		rLine.m_dwColor = GetObjectColor();
	}

	return l_hrOk;
}

void SVLinearEdgeProcessingClass::addOverlayFullResult(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	pGroup->set_detaillevel(SvPb::Level1);
	pGroup->set_name("LAnalyzer-Result");
	auto* pResultShape = pGroup->add_shapes();
	pResultShape->mutable_color()->set_value(m_cfEdges);
	auto* pResultMarker = pResultShape->mutable_markers();
	RECT rect{};
	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if (nullptr != pAnalyzer)
	{
		pAnalyzer->GetImageExtent().GetOutputRectangle(rect);
		pResultMarker->set_minvalue(rect.left);
		pResultMarker->set_maxvalue(rect.right);
	}
	pResultMarker->set_trpos(m_svLinearEdges.getTrPos() + 1);
	pResultMarker->set_sizetype(SvPb::Size::MidShort);

	if (isVertical)
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::x);
	}
	else
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::y);
	}
}

void SVLinearEdgeProcessingClass::addOverlayResultMarker(SvPb::OverlayShapeGroup& rGroup, bool isVertical) const
{
	auto* pResultShape = rGroup.add_shapes();
	pResultShape->mutable_color()->set_value(m_cfEdges);
	auto* pResultMarker = pResultShape->mutable_marker();
	RECT rect{};
	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
	if (nullptr != pAnalyzer)
	{
		pAnalyzer->GetImageExtent().GetOutputRectangle(rect);
		pResultMarker->set_minvalue(rect.left);
		pResultMarker->set_maxvalue(rect.right);
	}

	BOOL IsFixedEdgeMarker = false;
	if ((S_OK == m_svIsFixedEdgeMarker.GetValue(IsFixedEdgeMarker)) && IsFixedEdgeMarker)
	{
		long Position(0L);
		m_svPosition.GetValue(Position);
		// Get the selected fixed position...
		switch (Position)
		{
		case SvDef::SV_START_POSITION:
		{
			pResultMarker->mutable_value()->set_value(rect.left);
			break;
		}
		case SvDef::SV_END_POSITION:
		{
			pResultMarker->mutable_value()->set_value(rect.right);
			break;
		}

		case SvDef::SV_CENTER_POSITION:
		{
			pResultMarker->mutable_value()->set_value((rect.right - rect.left) / 2.0);
			break;
		}
		case SvDef::SV_OFFSET_POSITION:
		{
			double offset = 0.0;
			if ((S_OK == m_svPositionOffsetValue.GetValue(offset)) &&
				offset <= rect.right)
			{
				pResultMarker->mutable_value()->set_value(offset);
			}

			break;
		}
		}
	}
	else
	{
		auto* pSelectedPos = pResultMarker->mutable_selectedmarkerdata();
		long EdgeSelect(0);
		m_svEdgeSelect.GetValue(EdgeSelect);
		pSelectedPos->set_trpos_edgedata(m_svLinearEdges.getTrPos() + 1);
		switch (EdgeSelect)
		{
		case SvDef::SV_FIRST_EDGE:
			pSelectedPos->set_type(SvPb::SVOSelectedMarker_PosType_First);
			break;
		case SvDef::SV_LAST_EDGE:
			pSelectedPos->set_type(SvPb::SVOSelectedMarker_PosType_Last);
			break;
		case SvDef::SV_THIS_EDGE:
			pSelectedPos->set_type(SvPb::SVOSelectedMarker_PosType_This);
			double dEdgeSelect(0.0);
			m_svEdgeSelectThisValue.GetValue(dEdgeSelect);
			pSelectedPos->set_pos(dEdgeSelect);
			long direction(0);
			m_svDirection.GetValue(direction);
			pSelectedPos->set_fromlast(SvDef::SV_TAIL_TO_HEAD_DIRECTION == direction);
			break;
		}
	}

	pResultMarker->set_sizetype(SvPb::Size::MidShort);
	if (isVertical)
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::x);
	}
	else
	{
		pResultMarker->set_orientationtype(SvPb::Orientation::y);
	}
}

void SVLinearEdgeProcessingClass::addOverlayResultDetails(SvPb::Overlay& rOverlay, ResultType resultType, bool isVertical) const
{
	//add thresholdBar
	auto* pGroup = rOverlay.add_shapegroups();
	double color = 0;
	switch (resultType)
	{
	case ResultType::PixelCounting:
		pGroup->set_name("LAnalyzer-ResultsDetails-PixelCounting");
		color = SvDef::DefaultSubFunctionColor1;
		break;
	case ResultType::EdgeA:
		pGroup->set_name("LAnalyzer-ResultsDetails-EdgeA");
		color = SvDef::DefaultSubFunctionColor1;
		break;
	case ResultType::EdgeB:
		pGroup->set_name("LAnalyzer-ResultsDetails-EdgeB");
		color = SvDef::DefaultSubFunctionColor2;
		break;
	}
	pGroup->set_detaillevel(SvPb::Level2);
	auto* pThesholdLowerShape = pGroup->add_shapes();
	pThesholdLowerShape->mutable_color()->set_value(color);
	auto* pThresholdLowerMarker = pThesholdLowerShape->mutable_marker();
	pThresholdLowerMarker->set_minvalue(SvDef::cMin8BitPixelValue);
	pThresholdLowerMarker->set_maxvalue(SvDef::cMax8BitPixelValue);
	SvPb::setValueObject(m_svLowerThresholdValueObject, *pThresholdLowerMarker->mutable_value(), true);
	pThresholdLowerMarker->set_sizetype(SvPb::Size::Full);
	if (isVertical)
	{
		pThresholdLowerMarker->set_orientationtype(SvPb::Orientation::y);
	}
	else
	{
		pThresholdLowerMarker->set_orientationtype(SvPb::Orientation::x);
	}
	auto* pThesholdUpperShape = pGroup->add_shapes();
	pThesholdUpperShape->mutable_color()->set_value(color);
	auto* pThresholdUpperMarker = pThesholdUpperShape->mutable_marker();
	pThresholdUpperMarker->set_minvalue(SvDef::cMin8BitPixelValue);
	pThresholdUpperMarker->set_maxvalue(SvDef::cMax8BitPixelValue);
	SvPb::setValueObject(m_svUpperThresholdValueObject, *pThresholdUpperMarker->mutable_value(), true);
	pThresholdUpperMarker->set_sizetype(SvPb::Size::Full);
	if (isVertical)
	{
		pThresholdUpperMarker->set_orientationtype(SvPb::Orientation::y);
	}
	else
	{
		pThresholdUpperMarker->set_orientationtype(SvPb::Orientation::x);
	}

	// add result lines
	auto* pResultShape = pGroup->add_shapes();
	switch (resultType)
	{
	case ResultType::PixelCounting:
	{
		auto* pResultMarker = pResultShape->mutable_blockmarker();
		pResultMarker->set_pixelcountinganalyzerresultpos(m_svLinearEdges.getTrPos()+1);
		pResultMarker->add_colors(SvDef::DefaultBlackColor);
		pResultMarker->add_colors(SvDef::DefaultWhiteColor);
		if (isVertical)
		{
			pResultMarker->set_orientationtype(SvPb::Orientation::x);
		}
		else
		{
			pResultMarker->set_orientationtype(SvPb::Orientation::y);
		}
	}
	break;
	case ResultType::EdgeA:
	case ResultType::EdgeB:
	default:
	{
		pResultShape->mutable_color()->set_value(color);
		auto* pResultMarker = pResultShape->mutable_markers();

		RECT rect{};
		SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(GetAnalyzer());
		if (nullptr != pAnalyzer &&	S_OK == pAnalyzer->GetImageExtent().GetOutputRectangle(rect))
		{
			pResultMarker->set_minvalue(rect.left);
			pResultMarker->set_maxvalue(rect.right);
		}

		pResultMarker->set_trpos(m_svLinearEdges.getTrPos()+1);
		if (isVertical)
		{
			pResultMarker->set_orientationtype(SvPb::Orientation::x);
		}
		else
		{
			pResultMarker->set_orientationtype(SvPb::Orientation::y);
		}
		pResultMarker->set_sizetype(SvPb::Size::MidShort);
	}
	break;
	}
}

bool SVLinearEdgeProcessingClass::addGraphOverlay(SvPb::Overlay& rOverlay, bool isVertical)
{
	//add graph overlays
	auto* pValueObject = dynamic_cast<SvOi::IValueObject*>(m_InputLinearData.GetInputObjectInfo().getObject());
	if (m_InputLinearData.IsConnected() && nullptr != pValueObject)
	{
		auto* pGroup = rOverlay.add_shapegroups();
		pGroup->set_name("LAnalyzer-Graph");
		pGroup->set_detaillevel(SvPb::Level2);
		auto* pShape = pGroup->add_shapes();
		pShape->mutable_color()->set_value(m_cfHistogram);
		auto* pGraph = pShape->mutable_graph();
		if (isVertical)
		{
			pGraph->set_minvaluex(SvDef::cMin8BitPixelValue);
			pGraph->set_maxvaluex(SvDef::cMax8BitPixelValue);
			pGraph->set_trposx(pValueObject->getTrPos() + 1);
		}
		else
		{
			pGraph->set_minvaluey(SvDef::cMin8BitPixelValue);
			pGraph->set_maxvaluey(SvDef::cMax8BitPixelValue);
			pGraph->set_trposy(pValueObject->getTrPos() + 1);
		}
		return true;
	}
	return false;
}

void SVLinearEdgeProcessingClass::setDefaultValues(SvDef::SV_EDGECONTROL_POLARISATION_ENUM polarisationValue, SvDef::SV_EDGECONTROL_EDGESELECT_ENUM edgeSelectValue, double edgeSelectThisValue)
{
	m_svPolarisation.SetDefaultValue(polarisationValue, true);

	m_svEdgeSelect.SetDefaultValue(edgeSelectValue, true);
	m_svEdgeSelectThisValue.SetDefaultValue(edgeSelectThisValue, true);
}

bool SVLinearEdgeProcessingClass::UpdateThresholdValues(SVLinearEdgeProcessingClass::UpdateEnum updateState, const SvVol::LinkedValue& rThresholdSelected, const SvVol::LinkedValue& rThresholdPercentDiff,
	const SvVol::LinkedValue& rThresholdMinOffset, const SvVol::LinkedValue& rThresholdMaxOffset, byte& rThresholdValue, SvVol::SVDWordValueObjectClass& rThresholdValueObject)
{
	double dLower(0.0);
	switch (updateState)
	{
		case UpdateEnum::Selected:
			rThresholdSelected.getValue(dLower);
			break;
		case UpdateEnum::PercentDiff:
		{
			double percentDiff(0);
			HRESULT hTemp = rThresholdPercentDiff.getValue(percentDiff);
			if (S_OK == hTemp)
			{
				double dMax(0.0);
				if (S_OK != GetInputMaxThreshold(dMax))
				{
					return S_FALSE;
				}

				if (0 != percentDiff)
				{
					double dMin(0.0);
					if (S_OK != GetInputMinThreshold(dMin))
					{
						return S_FALSE;
					}

					dLower = dMax - ((dMax - dMin) * percentDiff/100.);
				}
				else
				{
					dLower = dMax;
				}
			}
			else
			{
				return false;
			}
			break;
		}
		case UpdateEnum::MinOffset:
		{
			double offset(0);
			HRESULT hTemp = rThresholdMinOffset.getValue(offset);
			if (S_OK == hTemp)
			{
				double dMin(0.0);
				if (S_OK != GetInputMinThreshold(dMin))
				{
					return S_FALSE;
				}

				dLower = dMin + offset;
			}
			else
			{
				return false;
			}
			break;
		}
		case UpdateEnum::MaxOffset:
		{
			double offset(0);
			HRESULT hTemp = rThresholdMaxOffset.getValue(offset);
			if (S_OK == hTemp)
			{
				double dMax(0.0);
				if (S_OK != GetInputMaxThreshold(dMax))
				{
					return false;
				}
				dLower = dMax - offset;
			}
			else
			{
				return false;
			}
			break;
		}
		case UpdateEnum::None:
			return true;
		default:
			return false;
	}

	if( dLower < 0.0 )
	{
		rThresholdValue = 0;
	}
	else if( 255.0 < dLower )
	{
		rThresholdValue = 255;
	}
	else
	{
		rThresholdValue = static_cast<byte>(dLower);
	}

	return (S_OK == rThresholdValueObject.SetValue(static_cast<DWORD> (rThresholdValue)));
}

HRESULT SVLinearEdgeProcessingClass::UpdateEdgeList()
{
	HRESULT Result( S_OK );

	std::vector<double> Data;
	std::vector<double> Edges;

	long Direction( 0L );
	long Polarisation( 0L );

	Result = GetInputLinearVectorData( Data );

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
			case SvDef::SV_HEAD_TO_TAIL_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < l_lCount - 1; ++l )
				{
					if( S_OK == IsEdge( Data[ l ], Data[ l + 1 ], m_upperThreshold, m_lowerThreshold, Polarisation ) )
					{
						if( Polarisation == SvDef::SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], m_upperThreshold, m_lowerThreshold, SvDef::SV_POSITIVE_POLARISATION, l_dDistance1 );
							bool l_bDistance2 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], m_upperThreshold, m_lowerThreshold, SvDef::SV_NEGATIVE_POLARISATION, l_dDistance2 );

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

							if( S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l + 1 ], m_upperThreshold, m_lowerThreshold, Polarisation, l_dDistance ) )
							{
								l_dDistance = l + l_dDistance;

								Edges.push_back( l_dDistance );
							}
						}
					}
				}

				break;
			}

			case SvDef::SV_TAIL_TO_HEAD_DIRECTION:
			{
				for( long l = l_lCount - 1; 0 < l; --l )
				{
					if( S_OK == IsEdge( Data[ l ], Data[ l - 1 ], m_upperThreshold, m_lowerThreshold, Polarisation ) )
					{
						if( Polarisation == SvDef::SV_ANY_POLARISATION )
						{
							double l_dDistance1 = 0.0;
							double l_dDistance2 = 0.0;

							bool l_bDistance1 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], m_upperThreshold, m_lowerThreshold, SvDef::SV_POSITIVE_POLARISATION, l_dDistance1 );
							bool l_bDistance2 = S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], m_upperThreshold, m_lowerThreshold, SvDef::SV_NEGATIVE_POLARISATION, l_dDistance2 );

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

							if( S_OK == CalculateSubPixelEdge( Data[ l ], Data[ l - 1 ], m_upperThreshold, m_lowerThreshold, Polarisation, l_dDistance ) )
							{
								l_dDistance = l - l_dDistance;

								Edges.push_back( l_dDistance );
							}
						}
					}
				}

				break;
			}

			case SvDef::SV_UNDEFINED_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < l_lCount; ++l )
				{
					double l_dValue = 0.0;

					if( S_OK == GetBlackWhiteEdgeValue( Data[ l ], m_upperThreshold, m_lowerThreshold, l_dValue ) )
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
		case SvDef::SV_POSITIVE_POLARISATION:
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
		case SvDef::SV_NEGATIVE_POLARISATION:
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
		case SvDef::SV_ANY_POLARISATION:
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
				case SvDef::SV_POSITIVE_POLARISATION:
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
				case SvDef::SV_NEGATIVE_POLARISATION:
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

} //namespace SvOp
