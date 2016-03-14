//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgePositionAnalyzer
//* .File Name       : $Workfile:   SVLineEdgePositionAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:03:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLineEdgePositionAnalyzer.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVEdgePositionLineAnalyzerClass, SVEdgePositionLineAnalyzerClassGuid );

SVEdgePositionLineAnalyzerClass::SVEdgePositionLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								:SVSingleLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = TRUE;
	m_bEnableEdgeSelect = TRUE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = TRUE;
	m_bEnableThreshold = TRUE;
	edgeControl.m_bIsEdgeA = TRUE;		// This is Edge A 

	init();
}

void SVEdgePositionLineAnalyzerClass::init()
{
	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLineEdgePositionAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &edge, SVEdgeAObjectGuid, IDS_OBJECTNAME_EDGE, false, SVResetItemNone );
	RegisterEmbeddedObject( &dpEdge, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE, false, SVResetItemNone );

    //
	// Script object for edgeControl parameters.
	//
	RegisterEmbeddedObject( &StrEdgeControlParameters1, SVEdgeParameters1ObjectGuid, IDS_OBJECTNAME_EDGE_PARAMETERS1, false, SVResetItemNone );

	// Set Edge Defaults
	m_voEdgeADirection.SetDefaultValue(SV_HEAD_TO_TAIL_DIRECTION,TRUE);
	m_voEdgeAPolarisation.SetDefaultValue(SV_ANY_POLARISATION,TRUE);
	m_voEdgeAEdgeSelect.SetDefaultValue(SV_FIRST_EDGE,TRUE);
	m_voEdgeAEdgeSelectThisValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeAIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeAPosition.SetDefaultValue(SV_START_POSITION,TRUE);
	m_voEdgeAPositionOffsetValue.SetDefaultValue(1.0,TRUE);		
	m_voEdgeALowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_voEdgeAUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	edge.SetDefaultValue( defaultPoint, TRUE );
	dpEdge.SetDefaultValue( defaultPoint, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Edge Result...
	interfaceInfo.EmbeddedID = SVEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Edge X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointXObjectType;
	resultClassInfo.ClassId = SVPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Add the EdgeB Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointYObjectType;
	resultClassInfo.ClassId = SVPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel Edge X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel Edge Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

    //
	// Place into persistance object.
	//
	StrEdgeControlParameters1.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrEdgeControlParameters1.SetDefaultValue( _T(""), FALSE );
}


SVEdgePositionLineAnalyzerClass::~SVEdgePositionLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVEdgePositionLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	CString csTemp1;

	if( bOk = SVSingleLineAnalyzerClass::CreateObject( PCreateStructure ) )
	{
		//
		// The Edge parameters should be available now from the script..
		//
		StrEdgeControlParameters1.GetValue(csTemp1);
	}

	if( ! csTemp1.IsEmpty() )
	{
		edgeControl.ConvertStringToParams((LPCTSTR)csTemp1);

		m_voEdgeALowerThresholdValue.SetValue( 1L, edgeControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.SetValue( 1L, edgeControl.m_dwUpperThresholdValue);
		m_voEdgeADirection.SetValue( 1L, edgeControl.m_dwDirection);
		m_voEdgeAPolarisation.SetValue( 1L, edgeControl.m_dwPolarisation);
		m_voEdgeAEdgeSelect.SetValue( 1L, edgeControl.m_dwEdgeSelect);
		m_voEdgeAEdgeSelectThisValue.SetValue( 1L, edgeControl.m_dEdgeSelectThisValue);
		m_voEdgeAIsFixedEdgeMarker.SetValue( 1L, edgeControl.m_bIsFixedEdgeMarker);
		m_voEdgeAPosition.SetValue( 1L, edgeControl.m_dwPosition);
		m_voEdgeAPositionOffsetValue.SetValue( 1L, edgeControl.m_dPositionOffsetValue);
		StrEdgeControlParameters1.SetValue( 1L, _T( "" ) );
	}

	// Set / Reset Printable Flag
	edge.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;	// Point
	dpEdge.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;	// Point
	StrEdgeControlParameters1.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

/*
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
*/
	isCreated = bOk;

	return bOk;
}

BOOL SVEdgePositionLineAnalyzerClass::CloseObject()
{
	return SVSingleLineAnalyzerClass::CloseObject();
}

BOOL SVEdgePositionLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVSingleLineAnalyzerClass::onRun( RRunStatus ) )
	{
		SVLineClass* pLine = getInputLine();
		ASSERT( pLine );

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
		
		// Calc edge lists...
		if( edgeControl.CalculateEdgeList( pLine, m_bDisplayAnalyzeLine ) ) 
		{
			// Calc current marker positions on the Line...
			double dEdge = 0.0;
			long lEdge = ( long ) edgeControl.CalculateCurrentEdgePosition( pLine );
			if( lEdge < 0 )
			{
				lEdge = 0;
				RRunStatus.SetFailed();
			}
			else
			{
				dEdge = edgeControl.CalculateSubPixelEdgePosition( pLine, lEdge );
			}

			SVExtentPointStruct l_svPoint1;
			SVExtentPointStruct l_svPoint2;

			GetRelativePositionPoints( l_svPoint1, l_svPoint2 );

			long l_lWidth = static_cast< long >( l_svPoint2.m_dPositionX ) - static_cast< long >( l_svPoint1.m_dPositionX );
			long l_lHeight = static_cast< long >( l_svPoint2.m_dPositionY ) - static_cast< long >( l_svPoint1.m_dPositionY );

			// Calc coordinates...
			SVDPointClass dpCoord;

			dpCoord.x = SVCalcLineDXValue( dEdge, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionX;
			dpCoord.y = SVCalcLineDYValue( dEdge, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionY;

			dpEdge.SetValue( RRunStatus.m_lResultDataIndex, dpCoord );

			POINT coord;
			//...edge
			coord.x = SVCalcLineXValue( lEdge, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionX );
			coord.y = SVCalcLineYValue( lEdge, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionY );
			edge.SetValue( RRunStatus.m_lResultDataIndex, coord );

			// Get edge result, if necessary...
			if( edgeControl.GetEdgeList().GetSize() > 0 )
			{
				if( m_bDisplayAnalyzeLine )
				{
					pLine->mResultArray.resize( edgeControl.GetEdgeList().GetSize() );
					memcpy( &( pLine->mResultArray[ 0 ] ), 
									edgeControl.GetEdgeList().GetData(), 
									edgeControl.GetEdgeList().GetSize() * sizeof( DWORD ) );
				}
				else
				{
					pLine->mResultArray.resize( 1 );

					pLine->mResultArray[0] = edgeControl.GetEdgeList()[0];
				}
			}
			else
			{
				pLine->mResultArray.clear();
			}

			return TRUE;
		}
		
		SetInvalid();
	}
	RRunStatus.SetInvalid();	
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate
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
BOOL SVEdgePositionLineAnalyzerClass::OnValidate()
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

