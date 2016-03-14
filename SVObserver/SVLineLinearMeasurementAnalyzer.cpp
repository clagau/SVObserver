//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineLinearMeasurementAnalyzer
//* .File Name       : $Workfile:   SVLineLinearMeasurementAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:08:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLineLinearMeasurementAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVLine.h"
#include "SVTool.h"
#include "SVGageTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMeasurementLineAnalyzerClass, SVLinearMeasurementLineAnalyzerClassGuid );

SVLinearMeasurementLineAnalyzerClass::SVLinearMeasurementLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVDualLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = TRUE;
	m_bEnableEdgeSelect = TRUE;
	m_bEnablePolarisation = TRUE;
	m_bEnablePosition = TRUE;
	m_bEnableThreshold = TRUE;

	edgeAControl.m_bIsEdgeA = TRUE;		// This is Edge A 
	edgeBControl.m_bIsEdgeA = FALSE;	// This is Not Edge A

	init();
}

void SVLinearMeasurementLineAnalyzerClass::init()
{
	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLineLinearMeasurementAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &edgeA, SVEdgeAObjectGuid, IDS_OBJECTNAME_EDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &edgeB, SVEdgeBObjectGuid, IDS_OBJECTNAME_EDGE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &center, SVCenterObjectGuid, IDS_OBJECTNAME_CENTER, false, SVResetItemNone );
	RegisterEmbeddedObject( &width, SVWidthObjectGuid, IDS_OBJECTNAME_WIDTH, false, SVResetItemNone );

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SVResetItemNone );

    //
	// Script object for edgeControl parameters.
	//
	RegisterEmbeddedObject( &StrEdgeControlParameters3, SVEdgeParameters3ObjectGuid, IDS_OBJECTNAME_EDGE_PARAMETERS3, false, SVResetItemNone );
	RegisterEmbeddedObject( &StrEdgeControlParameters4, SVEdgeParameters4ObjectGuid, IDS_OBJECTNAME_EDGE_PARAMETERS4, false, SVResetItemNone );
	
	// Set Edge defaults
	m_voEdgeADirection.SetDefaultValue(SV_HEAD_TO_TAIL_DIRECTION,TRUE);
	m_voEdgeAPolarisation.SetDefaultValue(SV_ANY_POLARISATION,TRUE);
	m_voEdgeAEdgeSelectThisValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeAIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeAPositionOffsetValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeAEdgeSelect.SetDefaultValue(SV_FIRST_EDGE,TRUE);
	m_voEdgeAPosition.SetDefaultValue(SV_START_POSITION,TRUE);
	m_voEdgeALowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_voEdgeAUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);

	m_voEdgeBDirection.SetDefaultValue(SV_HEAD_TO_TAIL_DIRECTION,TRUE);
	m_voEdgeBPolarisation.SetDefaultValue(SV_ANY_POLARISATION,TRUE);
	m_voEdgeBEdgeSelectThisValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeBIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeBPositionOffsetValue.SetDefaultValue(1.0,TRUE);
	m_voEdgeBEdgeSelect.SetDefaultValue(SV_LAST_EDGE,TRUE);
	m_voEdgeBPosition.SetDefaultValue(SV_START_POSITION,TRUE);
	m_voEdgeBLowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_voEdgeBUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	edgeA.SetDefaultValue( defaultPoint, TRUE );
	edgeB.SetDefaultValue( defaultPoint, TRUE );
	center.SetDefaultValue( defaultPoint, TRUE );
	width.SetDefaultValue( 0, TRUE );

	mdpEdgeA.SetDefaultValue( defaultPoint, TRUE );
	mdpEdgeB.SetDefaultValue( defaultPoint, TRUE );
	mdpCenter.SetDefaultValue( defaultPoint, TRUE );
	mdWidth.SetDefaultValue( 0.0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Edge A Results...
	interfaceInfo.EmbeddedID = SVEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the EdgeA X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointXObjectType;
	resultClassInfo.ClassId = SVPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the EdgeA Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointYObjectType;
	resultClassInfo.ClassId = SVPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Edge B Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVEdgeBObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the EdgeB X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointXObjectType;
	resultClassInfo.ClassId = SVPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the EdgeB Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointYObjectType;
	resultClassInfo.ClassId = SVPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_EDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Center Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVCenterObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Center X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointXObjectType;
	resultClassInfo.ClassId = SVPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_CENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Center Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultPointYObjectType;
	resultClassInfo.ClassId = SVPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_CENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Width Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVWidthObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Width Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.ClassId = SVLongResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_WIDTH );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge A Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge B Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeBObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel EdgeB X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Center Results...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPCenterObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel Center X Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.ClassId = SVDPointXResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );
	// Add the Sub-pixel Center Y Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.ClassId = SVDPointYResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Width Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDWidthObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
	// Add the Sub-pixel Width Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_DWIDTH );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

    //
	// Place into persistance object.
	//
	StrEdgeControlParameters3.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrEdgeControlParameters3.SetDefaultValue( _T(""), FALSE );

	StrEdgeControlParameters4.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrEdgeControlParameters4.SetDefaultValue( _T(""), FALSE );
}


SVLinearMeasurementLineAnalyzerClass::~SVLinearMeasurementLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVLinearMeasurementLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	CString csTemp3;
	CString csTemp4;

	if( bOk = SVDualLineAnalyzerClass::CreateObject( PCreateStructure ) )
	{
		//
		// The Edge parameters should be available now from the script..
		//
		StrEdgeControlParameters3.GetValue(csTemp3);
		edgeAControl.ConvertStringToParams((LPCTSTR)csTemp3);

		StrEdgeControlParameters4.GetValue(csTemp4);
		edgeBControl.ConvertStringToParams((LPCTSTR)csTemp4);
	}

	if( ! csTemp3.IsEmpty() )
	{
		m_voEdgeALowerThresholdValue.SetValue( 1L, edgeAControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.SetValue( 1L, edgeAControl.m_dwUpperThresholdValue);
		m_voEdgeADirection.SetValue( 1L, edgeAControl.m_dwDirection);
		m_voEdgeAPolarisation.SetValue( 1L, edgeAControl.m_dwPolarisation);
		m_voEdgeAEdgeSelect.SetValue( 1L, edgeAControl.m_dwEdgeSelect);
		m_voEdgeAEdgeSelectThisValue.SetValue( 1L, edgeAControl.m_dEdgeSelectThisValue);
		m_voEdgeAIsFixedEdgeMarker.SetValue( 1L, edgeAControl.m_bIsFixedEdgeMarker);
		m_voEdgeAPosition.SetValue( 1L, edgeAControl.m_dwPosition);
		m_voEdgeAPositionOffsetValue.SetValue( 1L, edgeAControl.m_dPositionOffsetValue);
		StrEdgeControlParameters3.SetValue( 1L, _T( "" ) );
	}

	if( ! csTemp4.IsEmpty() )
	{
		m_voEdgeBLowerThresholdValue.SetValue( 1L, edgeBControl.m_dwLowerThresholdValue);
		m_voEdgeBUpperThresholdValue.SetValue( 1L, edgeBControl.m_dwUpperThresholdValue);
		m_voEdgeBDirection.SetValue( 1L, edgeBControl.m_dwDirection);
		m_voEdgeBPolarisation.SetValue( 1L, edgeBControl.m_dwPolarisation);
		m_voEdgeBEdgeSelect.SetValue( 1L, edgeBControl.m_dwEdgeSelect);
		m_voEdgeBEdgeSelectThisValue.SetValue( 1L, edgeBControl.m_dEdgeSelectThisValue);
		m_voEdgeBIsFixedEdgeMarker.SetValue( 1L, edgeBControl.m_bIsFixedEdgeMarker);
		m_voEdgeBPosition.SetValue( 1L, edgeBControl.m_dwPosition);
		m_voEdgeBPositionOffsetValue.SetValue( 1L, edgeBControl.m_dPositionOffsetValue);
		StrEdgeControlParameters4.SetValue( 1L, _T( "" ) );
	}

	// Set / Reset Printable Flag
	edgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	edgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	center.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	width.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	mdpEdgeA.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpEdgeB.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdpCenter.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	mdWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	StrEdgeControlParameters3.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	StrEdgeControlParameters4.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

/*
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;

	m_voEdgeBLowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_voEdgeBLowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeBUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeBUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
*/
	isCreated = bOk;

	return bOk;
}

BOOL SVLinearMeasurementLineAnalyzerClass::CloseObject()
{
	return SVDualLineAnalyzerClass::CloseObject();
}


BOOL SVLinearMeasurementLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	try
	{

	// All inputs and outputs must be validated first
	if( SVDualLineAnalyzerClass::onRun( RRunStatus ) )
	{
		SVLineClass* pLine = getInputLine();
		ASSERT( pLine );

		// Get Variable Threshold Values
		m_voEdgeALowerThresholdValue.GetValue(edgeAControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.GetValue(edgeAControl.m_dwUpperThresholdValue);
		m_voEdgeADirection.GetValue(edgeAControl.m_dwDirection);
		m_voEdgeAPolarisation.GetValue(edgeAControl.m_dwPolarisation);
		m_voEdgeAEdgeSelect.GetValue(edgeAControl.m_dwEdgeSelect);
		m_voEdgeAEdgeSelectThisValue.GetValue(edgeAControl.m_dEdgeSelectThisValue);
		m_voEdgeAIsFixedEdgeMarker.GetValue(edgeAControl.m_bIsFixedEdgeMarker);
		m_voEdgeAPosition.GetValue(edgeAControl.m_dwPosition);
		m_voEdgeAPositionOffsetValue.GetValue( edgeAControl.m_dPositionOffsetValue);

		m_voEdgeBLowerThresholdValue.GetValue(edgeBControl.m_dwLowerThresholdValue);
		m_voEdgeBUpperThresholdValue.GetValue(edgeBControl.m_dwUpperThresholdValue);
		m_voEdgeBDirection.GetValue(edgeBControl.m_dwDirection);
		m_voEdgeBPolarisation.GetValue(edgeBControl.m_dwPolarisation);
		m_voEdgeBEdgeSelect.GetValue(edgeBControl.m_dwEdgeSelect);
		m_voEdgeBEdgeSelectThisValue.GetValue(edgeBControl.m_dEdgeSelectThisValue);
		m_voEdgeBIsFixedEdgeMarker.GetValue(edgeBControl.m_bIsFixedEdgeMarker);
		m_voEdgeBPosition.GetValue(edgeBControl.m_dwPosition);
		m_voEdgeBPositionOffsetValue.GetValue( edgeBControl.m_dPositionOffsetValue);

		CalculateThresholdValues( edgeAControl.m_dwUpperThresholdValue, edgeAControl.m_dwLowerThresholdValue, 
								  edgeBControl.m_dwUpperThresholdValue, edgeBControl.m_dwLowerThresholdValue );

		// Calc edge lists...
		if( edgeAControl.CalculateEdgeList( pLine, m_bDisplayAnalyzeLine, m_bSetupEdgeA  ) &&
			edgeBControl.CalculateEdgeList( pLine, m_bDisplayAnalyzeLine, m_bSetupEdgeA ) 
		  )
		{
			// Calc current marker positions on the Line...
			double dEdgeA = 0.0;
			long lEdgeA = ( long ) edgeAControl.CalculateCurrentEdgePosition( pLine );
			if( lEdgeA < 0 )
			{
				lEdgeA = 0;	
				RRunStatus.SetFailed();
			}
			else
			{
				dEdgeA = edgeAControl.CalculateSubPixelEdgePosition( pLine, lEdgeA );
			}

			double dEdgeB = 0.0;
			long lEdgeB = ( long ) edgeBControl.CalculateCurrentEdgePosition( pLine );
			if( lEdgeB < 0 )
			{
				lEdgeB = 0;
				RRunStatus.SetFailed();
			}
			else
			{
				dEdgeB = edgeBControl.CalculateSubPixelEdgePosition( pLine, lEdgeB );
			}

			// Calc center ...
			long lCenter = abs( lEdgeB - lEdgeA ) / 2 + std::min( lEdgeB, lEdgeA );
			double dCenter = fabs( dEdgeB - dEdgeA ) / 2.0 + std::min( dEdgeB, dEdgeA );

			SVExtentPointStruct l_svPoint1;
			SVExtentPointStruct l_svPoint2;

			GetRelativePositionPoints( l_svPoint1, l_svPoint2 );

			long l_lWidth = static_cast< long >( l_svPoint2.m_dPositionX ) - static_cast< long >( l_svPoint1.m_dPositionX );
			long l_lHeight = static_cast< long >( l_svPoint2.m_dPositionY ) - static_cast< long >( l_svPoint1.m_dPositionY );

			// Calc coordinates...
			POINT coordA, coordB, coordCenter;
			//...edgeA
			coordA.x = SVCalcLineXValue( lEdgeA, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionX );
			coordA.y = SVCalcLineYValue( lEdgeA, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionY );
			edgeA.SetValue( RRunStatus.m_lResultDataIndex, coordA );
			//...edgeB
			coordB.x = SVCalcLineXValue( lEdgeB, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionX );
			coordB.y = SVCalcLineYValue( lEdgeB, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionY );
			edgeB.SetValue( RRunStatus.m_lResultDataIndex, coordB );
			//...center
			coordCenter.x = SVCalcLineXValue( lCenter, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionX );
			coordCenter.y = SVCalcLineYValue( lCenter, l_lWidth, l_lHeight ) + static_cast< long >( l_svPoint1.m_dPositionY );
			center.SetValue( RRunStatus.m_lResultDataIndex, coordCenter );
			// Calc width ...
			long lX = coordB.x - coordA.x;
			long lY = coordB.y - coordA.y;
			long lWidth = lX * lX + lY * lY;
			lWidth = lWidth > 0 ? (long)( sqrt( ( double ) lWidth ) ) : 0;
			width.SetValue( RRunStatus.m_lResultDataIndex, lWidth );

			// Calc Sub-pixel coordinates...
			SVDPointClass dpCoordA, dpCoordB, dpCoordCenter;
			//...Sub-pixel edgeA
			dpCoordA.x = SVCalcLineDXValue( dEdgeA, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionX;
			dpCoordA.y = SVCalcLineDYValue( dEdgeA, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionY;
			mdpEdgeA.SetValue( RRunStatus.m_lResultDataIndex, dpCoordA );
			//...Sub-pixel edgeB
			dpCoordB.x = SVCalcLineDXValue( dEdgeB, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionX;
			dpCoordB.y = SVCalcLineDYValue( dEdgeB, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionY;
			mdpEdgeB.SetValue( RRunStatus.m_lResultDataIndex, dpCoordB );
			//...Sub-pixel center
			dpCoordCenter.x = SVCalcLineDXValue( dCenter, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionX;
			dpCoordCenter.y = SVCalcLineDYValue( dCenter, l_lWidth, l_lHeight ) + l_svPoint1.m_dPositionY;
			mdpCenter.SetValue( RRunStatus.m_lResultDataIndex, dpCoordCenter );
			// Calc Sub-pixel width ...
			double dX = dpCoordB.x - dpCoordA.x;
			double dY = dpCoordB.y - dpCoordA.y;
			double dWidth = dX * dX + dY * dY;
			dWidth = ( dWidth > 0.0 ) ? sqrt( dWidth ) : 0.0;
			mdWidth.SetValue( RRunStatus.m_lResultDataIndex, dWidth );

			// Get edge result, if necessary...
			if( m_bDisplayAnalyzeLine )
			{
				if( m_bSetupEdgeA )
				{
					if( 0 < edgeAControl.GetEdgeList().GetSize() )
					{
						pLine->mResultArray.resize( edgeAControl.GetEdgeList().GetSize() );
						memcpy( &( pLine->mResultArray[ 0 ] ), 
										edgeAControl.GetEdgeList().GetData(), 
										edgeAControl.GetEdgeList().GetSize() * sizeof( DWORD ) );
					}
					else
					{
						pLine->mResultArray.clear();
					}
				}
				else
				{
					if( 0 < edgeBControl.GetEdgeList().GetSize() )
					{
						pLine->mResultArray.resize( edgeBControl.GetEdgeList().GetSize() );
						memcpy( &( pLine->mResultArray[ 0 ] ), 
										edgeBControl.GetEdgeList().GetData(), 
										edgeBControl.GetEdgeList().GetSize() * sizeof( DWORD ) );
					}
					else
					{
						pLine->mResultArray.clear();
					}
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

	}
	catch( ... )
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVF_UNHANDLED_EXCEPTION, nullptr, StdMessageParams );

		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -------------------	----------------------------------------------------------
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
BOOL SVLinearMeasurementLineAnalyzerClass::OnValidate()
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
		retVal = SVDualLineAnalyzerClass::OnValidate() && retVal;

		// Set this object and all objects it owns, to invalid
		if( !retVal )
			SetInvalid();
	}
	return retVal;
}

