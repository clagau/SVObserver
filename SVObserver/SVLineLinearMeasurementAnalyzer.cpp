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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLineLinearMeasurementAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:08:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   16 Aug 2012 18:55:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   18 Jul 2012 13:42:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   16 Jul 2012 12:49:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   10 Jul 2012 12:18:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   02 Jul 2012 17:15:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   30 Mar 2011 15:13:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.40 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use STL min/max functions instead of MFC macros.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   08 Dec 2010 13:02:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   01 Jun 2010 14:52:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   16 Dec 2009 14:03:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   03 Sep 2009 10:29:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   21 Jun 2007 13:28:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   29 Aug 2005 16:20:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added onUpdateFigure to OnRun when m_bDisplayAnalyzerLine is true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   12 Aug 2005 13:50:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated class to fix issues with data synchrization.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   29 Jul 2005 12:42:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   21 Jun 2005 08:13:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   17 Feb 2005 14:40:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   09 Dec 2003 12:19:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Adjusted where edge control parameters are read from value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   04 Dec 2003 13:00:18   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   18 Nov 2003 14:05:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with value objects not updating correctly
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   18 Nov 2003 07:49:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the edge data
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   13 Nov 2003 13:06:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     edgeB was being setup with edgeA parameters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   08 Oct 2003 16:30:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   08 Oct 2003 11:03:46   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using the value objects for the edge parameters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   22 Apr 2003 10:49:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   30 Jan 2003 12:07:44   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   10 Jan 2003 15:32:42   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   09 Dec 2002 19:32:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   19 Nov 2002 15:08:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   08 Nov 2001 14:36:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated init, onUpdateFigure and onRun methods to calculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   12 Oct 2001 08:48:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  232
   SCR Title:  Downloading a configuration from SVFocusNT sometimes causes a crash
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added try/catch block to the OnRun function. It also adds an event to the system event log
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   04 Oct 2001 15:48:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
