//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgeCountingAnalyzer
//* .File Name       : $Workfile:   SVLinearLineEdgeCountingAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 12:05:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineEdgeCountingAnalyzer.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
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

SV_IMPLEMENT_CLASS( SVLinearEdgeCountingLineAnalyzerClass, SvPb::LinearEdgeCountingLineAnalyzerClassId);

SVLinearEdgeCountingLineAnalyzerClass::SVLinearEdgeCountingLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
								:SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = true;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearEdgeCountingLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgeCountingAnalyzerObjectType;

	SvOp::SVLinearEdgeProcessingClass *pEdge = new SvOp::SVLinearEdgeAProcessingClass( this );

	AddFriend( pEdge->getObjectId() );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svEdgeCount, SvPb::EdgeCountEId, IDS_OBJECTNAME_EDGE_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svShowAllEdgeAOverlays, SvPb::ShowAllEdgeAOverlaysEId, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_svEdgeCount.SetDefaultValue( 0 );
	m_svEdgeCount.setSaveValueFlag(false);
	
	// Set default inputs and outputs
	addDefaultInputObjects();

	// Add the Edge Count Result...
	addScalarResultToAvailableChildren(SvPb::EdgeCountEId, SvPb::SVResultLongObjectType, IDS_OBJECTNAME_EDGE_COUNT, IDS_RESULT_STRING);
}

SVLinearEdgeCountingLineAnalyzerClass::~SVLinearEdgeCountingLineAnalyzerClass()
{
	SVLinearEdgeCountingLineAnalyzerClass::CloseObject();
}

bool SVLinearEdgeCountingLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_svEdgeCount.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearEdgeCountingLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages);
}

bool SVLinearEdgeCountingLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	bool Result = SVLinearAnalyzerClass::onRun( rRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages);

	if( Result )
	{
		std::vector<double> Edges;

		if ( S_OK == GetEdgeA()->m_svLinearEdges.GetArrayValues( Edges ) )
		{
			long lCount = static_cast<int>(Edges.size());
			if ( S_OK != m_svEdgeCount.SetValue(lCount) )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( !Result )
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}

HRESULT SVLinearEdgeCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	if(nullptr != GetEdgeA())
	{
		if( S_OK == GetEdgeA()->GetEdgesOverlay( p_MultiLine ) )
		{
			GetImageExtent().TranslateFromOutputSpace( p_MultiLine, p_MultiLine );

			return S_OK;
		}
	}

	return E_FAIL;
}

void SVLinearEdgeCountingLineAnalyzerClass::addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical) const
{
	if (nullptr != GetEdgeA())
	{
		GetEdgeA()->addOverlayFullResult(rOverlay, isVertical);
	}
}

} //namespace SvAo