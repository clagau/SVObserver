// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearPixelCountingLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLinePixelCountingAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:03:42  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLinePixelCountingAnalyzer.h"
#include "Definitions/LinearEdgeEnums.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVStatusLibrary/SVRunStatus.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLinearPixelCountingLineAnalyzerClass, SvPb::LinearPixelCountingLineAnalyzerClassId);


SVLinearPixelCountingLineAnalyzerClass::SVLinearPixelCountingLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
								 :SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearPixelCountingLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVLinearPixelCountingAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	if( nullptr != pEdge )
	{
		pEdge->m_svDirection.SetDefaultValue(SvDef::SV_UNDEFINED_DIRECTION, true );

		AddFriend( pEdge->GetUniqueObjectID() );
	}

	// Register Embedded Objects
	RegisterEmbeddedObject( &blackPixelCount, SVBlackPixelObjectGuid, IDS_OBJECTNAME_BLACKPIXELCOUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &whitePixelCount, SVWhitePixelObjectGuid, IDS_OBJECTNAME_WHITEPIXELCOUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	blackPixelCount.SetDefaultValue( 0, true );
	blackPixelCount.setSaveValueFlag(false);
	whitePixelCount.SetDefaultValue( 0, true );
	whitePixelCount.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Add the White Pixel Count Result...
	addScalarResultToAvailableChildren(SVBlackPixelObjectGuid, SvPb::SVResultLongObjectType, IDS_OBJECTNAME_BLACKPIXELCOUNT, IDS_RESULT_STRING);

	// Add the Black Pixel Count Result...
	addScalarResultToAvailableChildren(SVWhitePixelObjectGuid, SvPb::SVResultLongObjectType, IDS_OBJECTNAME_WHITEPIXELCOUNT, IDS_RESULT_STRING);
}


SVLinearPixelCountingLineAnalyzerClass::~SVLinearPixelCountingLineAnalyzerClass()
{
	SVLinearPixelCountingLineAnalyzerClass::CloseObject();
}


bool SVLinearPixelCountingLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	blackPixelCount.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	whitePixelCount.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

HRESULT SVLinearPixelCountingLineAnalyzerClass::GetSelectedEdgeOverlays(SVExtentMultiLineStruct &p_MultiLine)
{
	return S_OK;
}

bool SVLinearPixelCountingLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	std::vector<double> Edges;

	// All inputs and outputs must be validated first
	bool Result = __super::onRun( rRunStatus, pErrorMessages );

	SvOp::SVLinearEdgeProcessingClass *pEdge = GetEdgeA();

	if ( nullptr == pEdge || S_OK != pEdge->m_svLinearEdges.GetArrayValues( Edges ) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	long lBlack = 0;
	long lWhite = 0;

	if( Result )
	{
		for( size_t l = 0; l < Edges.size(); l++ )
		{
			if( Edges[ l ] == 0.0 )
			{
				lBlack++;
			}
			else if( Edges[ l ] == pEdge->m_dwColorNumber - 1 )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( !Result )
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}

void SVLinearPixelCountingLineAnalyzerClass::addEdgeOverlays(const SvIe::SVImageClass* pImage, SvPb::Overlay& rOverlay) const
{
	auto* pEgde = GetEdgeA();
	if (nullptr != pEgde)
	{
		pEgde->addOverlayGroups(pImage, rOverlay, SvOp::SVLinearEdgeProcessingClass::ResultType::PixelCounting);
	}
}
} //namespace SvAo