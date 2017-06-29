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
#include "SVGlobal.h"
#include "SVOCore/SVImageClass.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearPixelCountingLineAnalyzerClass, SVLinearPixelCountingLineAnalyzerClassGuid );


SVLinearPixelCountingLineAnalyzerClass::SVLinearPixelCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								 :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = FALSE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = TRUE;

	init();
}

void SVLinearPixelCountingLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLinearPixelCountingAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	if( nullptr != l_pEdge )
	{
		l_pEdge->m_svDirection.SetDefaultValue( SV_UNDEFINED_DIRECTION, true );

		AddFriend( l_pEdge->GetUniqueObjectID() );
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
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Black Pixel Count Result...
	interfaceInfo.EmbeddedID = SVBlackPixelObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Black Pixel Count Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.m_ClassId = SVLongResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_BLACKPIXELCOUNT );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	// Declare Input Interface of White Pixel Count Result...
	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVWhitePixelObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the White Pixel Count Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.m_ClassId = SVLongResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_WHITEPIXELCOUNT );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );
}


SVLinearPixelCountingLineAnalyzerClass::~SVLinearPixelCountingLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearPixelCountingLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	blackPixelCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	whitePixelCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearPixelCountingLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

bool SVLinearPixelCountingLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	std::vector<double> Edges;

	// All inputs and outputs must be validated first
	bool Result = __super::onRun( rRunStatus, pErrorMessages );

	SVLinearEdgeProcessingClass *pEdge = GetEdgeA();

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
		SetInvalid();
	}

	return Result;
}

HRESULT SVLinearPixelCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}
