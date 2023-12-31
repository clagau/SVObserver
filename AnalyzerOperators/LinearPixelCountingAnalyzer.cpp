// ******************************************************************************
// * COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearPixelCountingAnalyzer
// * .File Name       : $Workfile:   LinearPixelCountingAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:03:42  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinearPixelCountingAnalyzer.h"
#include "Definitions/LinearEdgeEnums.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "InspectionEngine/RunStatus.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( LinearPixelCountingAnalyzer, SvPb::LinearPixelCountingLineAnalyzerClassId);


LinearPixelCountingAnalyzer::LinearPixelCountingAnalyzer( SVObjectClass* POwner, int StringResourceID )
								 :LinearAnalyzer( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void LinearPixelCountingAnalyzer::init()
{
	// Identify our type
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearPixelCountingAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	if( nullptr != pEdge )
	{
		pEdge->setDirectionDefaultValue(SvDef::SV_UNDEFINED_DIRECTION);

		AddFriend( pEdge );
	}

	// Register Embedded Objects
	RegisterEmbeddedObject( &blackPixelCount, SvPb::BlackPixelEId, IDS_OBJECTNAME_BLACKPIXELCOUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject( &whitePixelCount, SvPb::WhitePixelEId, IDS_OBJECTNAME_WHITEPIXELCOUNT, false, SvOi::SVResetItemNone, false);
	RegisterEmbeddedObject(&m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone, true);

	// Set Embedded defaults
	blackPixelCount.SetDefaultValue( 0, true );
	blackPixelCount.setSaveValueFlag(false);
	whitePixelCount.SetDefaultValue( 0, true );
	whitePixelCount.setSaveValueFlag(false);
	
	// Add the White Pixel Count Result...
	addScalarResultToAvailableChildren(SvPb::BlackPixelEId, SvPb::SVResultLongObjectType, IDS_OBJECTNAME_BLACKPIXELCOUNT, IDS_RESULT_STRING);

	// Add the Black Pixel Count Result...
	addScalarResultToAvailableChildren(SvPb::WhitePixelEId, SvPb::SVResultLongObjectType, IDS_OBJECTNAME_WHITEPIXELCOUNT, IDS_RESULT_STRING);
}


LinearPixelCountingAnalyzer::~LinearPixelCountingAnalyzer()
{
	LinearPixelCountingAnalyzer::CloseObject();
}


bool LinearPixelCountingAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = LinearAnalyzer::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	blackPixelCount.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	whitePixelCount.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

HRESULT LinearPixelCountingAnalyzer::GetSelectedEdgeOverlays(SVExtentMultiLineStruct&) const
{
	return S_OK;
}

bool LinearPixelCountingAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	bool Result = __super::onRun( rRunStatus, pErrorMessages );

	SvOp::SVLinearEdgeProcessingClass *pEdge = GetEdgeA();

	if ( nullptr == pEdge )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	long lBlack = 0;
	long lWhite = 0;

	if( Result )
	{
		std::vector<double> Edges = pEdge->getLinearEdges();
		for( size_t l = 0; l < Edges.size(); l++ )
		{
			if( Edges[ l ] == 0.0 )
			{
				lBlack++;
			}
			else if( Edges[ l ] == pEdge->getColorNumber() - 1 )
			{
				lWhite++;
			}
		}
	}

	if ( S_OK != blackPixelCount.SetValue(lBlack)|| S_OK != whitePixelCount.SetValue(lWhite) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( !Result )
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}

void LinearPixelCountingAnalyzer::addOverlayResultDetails(SvPb::Overlay& rOverlay, bool isVertical) const
{
	auto* pEdge = GetEdgeA();
	if (nullptr != pEdge)
	{
		pEdge->addOverlayResultDetails(rOverlay, SvOp::SVLinearEdgeProcessingClass::ResultType::PixelCounting, isVertical);
	}
}
} //namespace SvAo