//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinePixelCountingAnalyzer
//* .File Name       : $Workfile:   SVLinePixelCountingAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:17:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVLinePixelCountingAnalyzer.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVImageToLineProject.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVImageClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVPixelCountingLineAnalyzerClass, SVPixelCountingLineAnalyzerClassGuid );

SVPixelCountingLineAnalyzerClass::SVPixelCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								 :SVSingleLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = FALSE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = TRUE;
	edgeControl.m_bIsEdgeA = TRUE;

	init();
}

void SVPixelCountingLineAnalyzerClass::init()
{
	m_bDisplayAnalyzeEdge = FALSE;

	// Identify our type
	outObjectInfo.ObjectTypeInfo.SubType = SVLinePixelCountingAnalyzerObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &blackPixelCount, SVBlackPixelObjectGuid, IDS_OBJECTNAME_BLACKPIXELCOUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &whitePixelCount, SVWhitePixelObjectGuid, IDS_OBJECTNAME_WHITEPIXELCOUNT, false, SVResetItemNone );

	//
	// Script object for edgeControl parameters.
	//
	RegisterEmbeddedObject( &StrEdgeControlParameters1,  SVEdgeParameters1ObjectGuid, IDS_OBJECTNAME_EDGE_PARAMETERS1, false, SVResetItemNone );
  
	// Set Edge Defaults
	m_voEdgeALowerThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_LOWER_TRESH,TRUE);
	m_voEdgeAUpperThresholdValue.SetDefaultValue(SV_DEFAULT_TOOL_UPPER_TRESH,TRUE);
	m_voEdgeADirection.SetDefaultValue(SV_UNDEFINED_DIRECTION,TRUE);

	// Set Embedded defaults
	blackPixelCount.SetDefaultValue( 0, TRUE );
	whitePixelCount.SetDefaultValue( 0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Black Pixel Count Result...
	interfaceInfo.EmbeddedID = SVBlackPixelObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the Black Pixel Count Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.ClassId = SVLongResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_BLACKPIXELCOUNT );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

	// Declare Input Interface of White Pixel Count Result...
	resultClassInfo.DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVWhitePixelObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Add the White Pixel Count Result...
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
	resultClassInfo.ClassId = SVLongResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_WHITEPIXELCOUNT );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	availableChildren.Add( resultClassInfo );

    //
	// Place into persistance object.
	//
	StrEdgeControlParameters1.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	StrEdgeControlParameters1.SetDefaultValue( _T( "" ), FALSE );

	m_bDisplayBWAnalyzeLine = FALSE;
}


SVPixelCountingLineAnalyzerClass::~SVPixelCountingLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVPixelCountingLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	CString csTemp1;

	if( bOk = SVSingleLineAnalyzerClass::CreateObject( PCreateStructure ) )
	{
		SVLineClass* pLine  = getInputLine();
		if( pLine )
		{
			pixelHistogramArray.SetSize( (int)(pLine->m_dwColorNumber) );

			//
			// The Edge parameters should be available now from the script..
			//
			StrEdgeControlParameters1.GetValue(csTemp1);
			edgeControl.ConvertStringToParams((LPCTSTR)csTemp1);
		}
	}

	if( ! csTemp1.IsEmpty() )
	{
		m_voEdgeALowerThresholdValue.SetValue( 1L, edgeControl.m_dwLowerThresholdValue);
		m_voEdgeAUpperThresholdValue.SetValue( 1L, edgeControl.m_dwUpperThresholdValue);		
		m_voEdgeADirection.SetValue( 1L, edgeControl.m_dwDirection);
		StrEdgeControlParameters1.SetValue( 1L, _T( "" ) );
	}

	// Set / Reset Printable Flag
	blackPixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	whitePixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	StrEdgeControlParameters1.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() &= ~SV_EXTENT_OBJECT;
	m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

BOOL SVPixelCountingLineAnalyzerClass::CloseObject()
{
	return SVSingleLineAnalyzerClass::CloseObject();
}

BOOL SVPixelCountingLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVSingleLineAnalyzerClass::onRun( RRunStatus ) )
	{
		SVLineClass* pLine = getInputLine();
		ASSERT( pLine );

		// Get Variable Threshold Values
		DWORD dwUpper, dwLower;
		m_voEdgeALowerThresholdValue.GetValue(dwLower);
		m_voEdgeAUpperThresholdValue.GetValue(dwUpper);

		CalculateThresholdValues( dwUpper, dwLower );

		pLine->m_dwDisplayedMaxThresholdBarValue = dwUpper;
		pLine->m_dwDisplayedMinThresholdBarValue = dwLower;

		// Reset Histogram buffer to zero...
		pixelHistogramArray.SetSize( (int)(pLine->m_dwColorNumber) );
		BYTE* pHist = ( BYTE* ) pixelHistogramArray.GetData();
		if( pHist && pixelHistogramArray.GetSize() == pLine->m_dwColorNumber && pixelHistogramArray.GetSize() > 0 )
		{
			memset( pHist, 0, pixelHistogramArray.GetSize() * sizeof( DWORD ) );
		}
		else
		{
			SetInvalid();
			RRunStatus.SetInvalid();

			return FALSE;		
		}

		DWORD dwResult;
		const DWORD dwBlack = 0;
		const DWORD dwWhite = ( DWORD ) pLine->m_dwColorNumber - 1;

		pLine->mResultArray.clear();

		for( long l = 0; l < pLine->mlLineArrayLength; ++l )
		{
			double dResult;

			// Get pixel value, regarding the pixel depth...
			pLine->mNormalizedArray.GetAt( l, (void *)(&dResult) );

			dwResult = (DWORD)(dResult);
	
			// Binarizing...
			if( dwLower <= dwUpper )
				dwResult = ( dwLower <= dwResult && dwResult <= dwUpper) ? dwWhite : dwBlack;
			else
				dwResult = ( dwUpper <= dwResult && dwResult <= dwLower ) ? dwBlack : dwWhite;

			// Put result data to the line result buffer, if necessary for displaying...
			if( m_bDisplayBWAnalyzeLine )
				pLine->mResultArray.push_back( dwResult );

			// increment pixel value correlated histogram entry...
			++ pixelHistogramArray[ dwResult ];
		}

		// Set Black/White Pixel Count...
		long lBlackCount = pixelHistogramArray[ dwBlack ];
		long lWhiteCount = pixelHistogramArray[ dwWhite ];
		blackPixelCount.SetValue( RRunStatus.m_lResultDataIndex, lBlackCount );
		whitePixelCount.SetValue( RRunStatus.m_lResultDataIndex, lWhiteCount );

		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}

HRESULT SVPixelCountingLineAnalyzerClass::GetEdgesOverlay( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_Status = S_OK;

	SVImageExtentClass l_svExtents;

	if ( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
	{
		bool l_bIsAGageTool = true;
		double l_dHeight = 0.0;
		double l_dLineCorrect = 1.0;

		SVLineClass* pLine = getInputLine();

		GetOverlayDrawlingAdjustments( l_bIsAGageTool, l_dHeight, l_dLineCorrect );

		if( m_bDisplayBWAnalyzeLine && pLine != NULL )
		{
			// Build complete new set of draw objects for this figure...
			for( size_t i = 0; i < pLine->mResultArray.size(); ++i )
			{
				long l_lX = static_cast<long>(i * l_dLineCorrect);

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

				if( pLine->mResultArray[ i ] == 0 )
				{
					l_Line.m_dwColor = SV_DEFAULT_BLACK_COLOR;
				}
				else
				{
					l_Line.m_dwColor = SV_DEFAULT_WHITE_COLOR;
				}

				if( l_bIsAGageTool )
				{
					l_svExtents.TranslateFromOutputSpace( l_Line, l_Line );
				}

				p_MultiLine.m_svLineArray.Add( l_Line );
			}
		}
	}

	return l_Status;
}

HRESULT SVPixelCountingLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	return S_OK;
}

