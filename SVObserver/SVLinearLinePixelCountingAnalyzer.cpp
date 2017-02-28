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
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLinearPixelCountingAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	if( nullptr != l_pEdge )
	{
		l_pEdge->m_svDirection.SetDefaultValue( SV_UNDEFINED_DIRECTION, true );

		AddFriend( l_pEdge->GetUniqueObjectID() );
	}

	// Register Embedded Objects
	RegisterEmbeddedObject( &blackPixelCount, SVBlackPixelObjectGuid, IDS_OBJECTNAME_BLACKPIXELCOUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &whitePixelCount, SVWhitePixelObjectGuid, IDS_OBJECTNAME_WHITEPIXELCOUNT, false, SVResetItemNone );

	// Set Embedded defaults
	blackPixelCount.SetDefaultValue( 0, true );
	whitePixelCount.SetDefaultValue( 0, true );

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
	blackPixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	whitePixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearPixelCountingLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

bool SVLinearPixelCountingLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	std::vector<double> l_svEdges;

	// All inputs and outputs must be validated first
	bool l_bOk = __super::onRun( RRunStatus, pErrorMessages );

	SVLinearEdgeProcessingClass *l_pEdge = GetEdgeA();

	if ( nullptr == l_pEdge || S_OK != l_pEdge->m_svLinearEdges.GetValues( RRunStatus.m_lResultDataIndex, l_svEdges ) )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	long l_lBlack = 0;
	long l_lWhite = 0;

	if( l_bOk )
	{
		for( size_t l = 0; l < l_svEdges.size(); l++ )
		{
			if( l_svEdges[ l ] == 0.0 )
			{
				l_lBlack++;
			}
			else if( l_svEdges[ l ] == l_pEdge->m_dwColorNumber - 1 )
			{
				l_lWhite++;
			}
		}
	}

	if ( S_OK != blackPixelCount.SetValue( RRunStatus.m_lResultDataIndex, l_lBlack )|| S_OK != whitePixelCount.SetValue( RRunStatus.m_lResultDataIndex, l_lWhite ) )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( ! l_bOk )
	{
		RRunStatus.SetInvalid();
		SetInvalid();
	}

	return l_bOk;
}

HRESULT SVLinearPixelCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}
