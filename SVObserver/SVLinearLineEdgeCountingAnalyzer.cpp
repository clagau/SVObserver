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
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVGlobal.h"
#include "SVOCore/SVTool.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearEdgeCountingLineAnalyzerClass, SVLinearEdgeCountingLineAnalyzerClassGuid );

SVLinearEdgeCountingLineAnalyzerClass::SVLinearEdgeCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								:SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = TRUE;

	init();
}

void SVLinearEdgeCountingLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLinearEdgeCountingAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	AddFriend( l_pEdge->GetUniqueObjectID() );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svEdgeCount, SVEdgeCountObjectGuid, IDS_OBJECTNAME_EDGE_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_svEdgeCount.SetDefaultValue( 0, TRUE );
	
	// Set default inputs and outputs
	addDefaultInputObjects();

	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Edge Count Result...
	interfaceInfo.EmbeddedID = SVEdgeCountObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Edge Count Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.m_ClassId = SVLongResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_EDGE_COUNT );
	SVString Title = SvUl_SF::LoadSVString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ");
	resultClassInfo.m_ClassName += Title.c_str();
	m_availableChildren.Add( resultClassInfo );
}

SVLinearEdgeCountingLineAnalyzerClass::~SVLinearEdgeCountingLineAnalyzerClass()
{
	CloseObject();
}

BOOL SVLinearEdgeCountingLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_svEdgeCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearEdgeCountingLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
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

		if ( S_OK == GetEdgeA()->m_svLinearEdges.GetArrayValues( Edges, rRunStatus.m_lResultDataIndex ) )
		{
			long lCount = static_cast<int>(Edges.size());
			if ( S_OK != m_svEdgeCount.SetValue( lCount, rRunStatus.m_lResultDataIndex ) )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( !Result )
	{
		SetInvalid();
		rRunStatus.SetInvalid();
	}

	return Result;
}

HRESULT SVLinearEdgeCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( S_OK == l_hrOk && nullptr != GetEdgeA() )
	{
		if( S_OK == GetEdgeA()->GetEdgesOverlay( p_MultiLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( p_MultiLine, p_MultiLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

