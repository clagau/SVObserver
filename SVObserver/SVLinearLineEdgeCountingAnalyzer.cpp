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
#include "SVTool.h"
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
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLinearEdgeCountingAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdge = new SVLinearEdgeAProcessingClass( this );

	AddFriend( l_pEdge->GetUniqueObjectID() );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svEdgeCount, SVEdgeCountObjectGuid, IDS_OBJECTNAME_EDGE_COUNT, false, SVResetItemNone );

	// Set Embedded defaults
	m_svEdgeCount.SetDefaultValue( 0, TRUE );
	
	// Set default inputs and outputs
	addDefaultInputObjects();

	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Edge Count Result...
	interfaceInfo.EmbeddedID = SVEdgeCountObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Edge Count Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.ClassId = SVLongResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE_COUNT );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
}


SVLinearEdgeCountingLineAnalyzerClass::~SVLinearEdgeCountingLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearEdgeCountingLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_svEdgeCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearEdgeCountingLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}


BOOL SVLinearEdgeCountingLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	BOOL l_bOk = SVLinearAnalyzerClass::onRun( RRunStatus );

	l_bOk = l_bOk && nullptr != GetEdgeA();

	if( l_bOk )
	{
		std::vector<double> l_svEdges;

		l_bOk = ( S_OK == GetEdgeA()->m_svLinearEdges.GetValues( RRunStatus.m_lResultDataIndex, l_svEdges ) );

		long l_lCount = static_cast<int>(l_svEdges.size());

		l_bOk = l_bOk && ( S_OK == m_svEdgeCount.SetValue( RRunStatus.m_lResultDataIndex, l_lCount ) );
	}

	if( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

BOOL SVLinearEdgeCountingLineAnalyzerClass::OnValidate()
{
	BOOL retVal = SVLinearAnalyzerClass::OnValidate();
		
	retVal = retVal && nullptr != GetEdgeA();

	if( !retVal )
		SetInvalid();

	return retVal;
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

