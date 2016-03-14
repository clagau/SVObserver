//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgeCountingAnalyzer
//* .File Name       : $Workfile:   SVLineEdgeCountingAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:57:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLineEdgeCountingAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEdgeCountingLineAnalyzerClass, SVEdgeCountingLineAnalyzerClassGuid );

SVEdgeCountingLineAnalyzerClass::SVEdgeCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								:SVSingleLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = TRUE;
	edgeControl.m_bIsEdgeA = TRUE;		// This is Edge A 

	init();
}

void SVEdgeCountingLineAnalyzerClass::init()
{
	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLineEdgeCountingAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &edgeCount, SVEdgeCountObjectGuid, IDS_OBJECTNAME_EDGE_COUNT, false, SVResetItemNone );

    //
	// Script object for edgeControl parameters.
	//
	RegisterEmbeddedObject( &StrEdgeControlParameters2, SVEdgeParameters2ObjectGuid, IDS_OBJECTNAME_EDGE_PARAMETERS2, false, SVResetItemNone );

 	// Set Edge Defaults
	m_voEdgeADirection.SetDefaultValue(SV_HEAD_TO_TAIL_DIRECTION,TRUE);
	m_voEdgeAPolarisation.SetDefaultValue(SV_ANY_POLARISATION,TRUE);
	m_voEdgeAEdgeSelect.SetDefaultValue(0L,TRUE);
	m_voEdgeAEdgeSelectThisValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeAIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeAPosition.SetDefaultValue(0L,TRUE);
	m_voEdgeAPositionOffsetValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeALowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_voEdgeAUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);

	// Set Embedded defaults
	edgeCount.SetDefaultValue( 0, TRUE );
	
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

	//
	// Place into persistance object. // not used anymore
	//
	StrEdgeControlParameters2.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrEdgeControlParameters2.SetDefaultValue( _T( "" ), FALSE );
}


SVEdgeCountingLineAnalyzerClass::~SVEdgeCountingLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVEdgeCountingLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	CString csTemp2;

	if( bOk = SVSingleLineAnalyzerClass::CreateObject( PCreateStructure ) )
	{
		//
		// The Edge parameters should be available now from the script..
		//
		StrEdgeControlParameters2.GetValue(csTemp2);
		edgeControl.ConvertStringToParams((LPCTSTR)csTemp2);
	}

	if( ! csTemp2.IsEmpty() )
	{
		m_voEdgeALowerThresholdValue.SetValue( 1L, edgeControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.SetValue( 1L, edgeControl.m_dwUpperThresholdValue);
		m_voEdgeADirection.SetValue( 1L, edgeControl.m_dwDirection);
		m_voEdgeAPolarisation.SetValue( 1L, edgeControl.m_dwPolarisation);
		m_voEdgeAEdgeSelect.SetValue( 1L, edgeControl.m_dwEdgeSelect);
		m_voEdgeAEdgeSelectThisValue.SetValue( 1L, edgeControl.m_dEdgeSelectThisValue);
		m_voEdgeAIsFixedEdgeMarker.SetValue( 1L, edgeControl.m_bIsFixedEdgeMarker);
		m_voEdgeAPosition.SetValue( 1L, edgeControl.m_dwPosition);
		m_voEdgeAPositionOffsetValue.SetValue( 1L, edgeControl.m_dPositionOffsetValue);
		StrEdgeControlParameters2.SetValue( 1L, _T( "" ) );
	}

	// Set / Reset Printable Flag
	edgeCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	StrEdgeControlParameters2.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

/*
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
*/
	isCreated = bOk;

	return bOk;
}

BOOL SVEdgeCountingLineAnalyzerClass::CloseObject()
{
	return SVSingleLineAnalyzerClass::CloseObject();
}

BOOL SVEdgeCountingLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVSingleLineAnalyzerClass::onRun( RRunStatus ) )
	{
		SVLineClass* pLine = getInputLine();
		ASSERT( pLine );

		pLine->mResultArray.clear();

		// Get Variable Threshold Values
		m_voEdgeALowerThresholdValue.GetValue(edgeControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.GetValue(edgeControl.m_dwUpperThresholdValue);
		m_voEdgeADirection.GetValue(edgeControl.m_dwDirection);
		m_voEdgeAPolarisation.GetValue(edgeControl.m_dwPolarisation);
		m_voEdgeAEdgeSelect.GetValue(edgeControl.m_dwEdgeSelect);
		m_voEdgeAEdgeSelectThisValue.GetValue(edgeControl.m_dEdgeSelectThisValue);
		m_voEdgeAIsFixedEdgeMarker.GetValue(edgeControl.m_bIsFixedEdgeMarker);
		m_voEdgeAPosition.GetValue(edgeControl.m_dwPosition);
		m_voEdgeAPositionOffsetValue.GetValue( edgeControl.m_dPositionOffsetValue);

		CalculateThresholdValues( edgeControl.m_dwUpperThresholdValue, edgeControl.m_dwLowerThresholdValue );

		// Calc edge list...
		if( edgeControl.CalculateEdgeList( pLine, m_bDisplayAnalyzeLine ) )
		{
			// Get edge count...
			long lEdgeCount = static_cast<long>(edgeControl.GetEdgeList().GetSize());

			edgeCount.SetValue( RRunStatus.m_lResultDataIndex, lEdgeCount );

			// Update for Edge Drawing...
			if (lEdgeCount)
			{
				pLine->mResultArray.resize( lEdgeCount );
				memcpy( &( pLine->mResultArray[ 0 ] ), edgeControl.GetEdgeList().GetData(), lEdgeCount * sizeof( DWORD ) );
			}
			return TRUE;
		}
		SetInvalid();
	}

	RRunStatus.SetInvalid();

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEdgeCountingLineAnalyzerClass::OnValidate()
{
	BOOL retVal = FALSE;
	
	// Validate our local needs
	if( NULL )
	{
		SetInvalid();
	}
	else
	{
		retVal = TRUE;

		// Validate our base class needs
		retVal = SVSingleLineAnalyzerClass::OnValidate() && retVal;

		// Set this object and all objects it owns, to invalid
		if( !retVal )
			SetInvalid();
	}
	return retVal;
}

HRESULT SVEdgeCountingLineAnalyzerClass::GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	return S_OK;
}

HRESULT SVEdgeCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_Status = S_OK;

	SVImageExtentClass l_svExtents;

	if ( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
	{
		COLORREF l_Color = 0;
		bool l_bIsAGageTool = true;
		double l_dHeight = 0.0;
		double l_dLineCorrect = 1.0;

		SVLineClass* pLine = getInputLine();

		statusColor.GetValue( l_Color );

		GetOverlayDrawlingAdjustments( l_bIsAGageTool, l_dHeight, l_dLineCorrect );

		// Build complete new set of draw objects for this figure...
		for( size_t i = 0; i < pLine->mResultArray.size(); ++i )
		{
			long l_lX = static_cast<long>(pLine->mResultArray[ i ] * l_dLineCorrect);

			SVExtentLineStruct l_Line;
			SVExtentPointStruct l_oGraphPoint;

			l_Line.m_bIsAngleValid = true;
			l_Line.m_dAngle = 90.0;

			l_oGraphPoint.m_dPositionX = l_lX;
			l_oGraphPoint.m_dPositionY = l_dHeight / 2 - 10;
			
			l_Line.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

			l_oGraphPoint.m_dPositionX = l_lX;
			l_oGraphPoint.m_dPositionY = l_dHeight / 2 + 10;
			
			l_Line.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

			l_Line.m_dwColor = l_Color;

			if( l_bIsAGageTool )
			{
				l_svExtents.TranslateFromOutputSpace( l_Line, l_Line );
			}

			p_MultiLine.m_svLineArray.Add( l_Line );
		}
	}

	return l_Status;
}

