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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLinePixelCountingAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:17:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   16 Aug 2012 18:55:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   01 Aug 2012 13:01:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   27 Jul 2012 08:55:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   18 Jul 2012 13:42:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   16 Jul 2012 12:52:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   10 Jul 2012 12:18:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   02 Jul 2012 17:17:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   16 Mar 2011 13:40:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   08 Dec 2010 13:02:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   01 Jun 2010 14:52:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   16 Dec 2009 14:03:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   03 Sep 2009 10:29:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   29 Aug 2005 16:20:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added onUpdateFigure to OnRun when m_bDisplayAnalyzerLine is true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   09 Aug 2005 09:51:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   29 Jul 2005 12:42:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   21 Jun 2005 08:14:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   18 Mar 2005 09:15:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   17 Feb 2005 14:41:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   09 Dec 2003 12:20:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Adjusted where edge control parameters are read from value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   04 Dec 2003 13:07:58   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   08 Oct 2003 16:30:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   08 Oct 2003 11:06:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using the value objects for the edge parameters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   22 Apr 2003 10:48:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   30 Jan 2003 12:07:42   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   10 Jan 2003 15:32:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   09 Dec 2002 19:33:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   19 Nov 2002 15:18:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Feb 2002 13:59:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  276
   SCR Title:  Gage Tool Pixel Counting Analyzer doesn't save thresholds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     reverted to some code from previous version to correct bug added by adding persistence for GageTool - Pixel Analyzers.
   Added conditional code depending on GageTool or ProfileTool. These changes were mainly in ::onRun and ::SetupDialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   17 Jan 2002 14:34:42   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  276
   SCR Title:  Gage Tool Pixel Counting Analyzer doesn't save thresholds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added a SVEdgeControl and a SVStringValueObjectClass member to enable the persistence of the threshold values. Modified the entire class to use these new members.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   08 Nov 2001 14:43:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and onRun methods to calculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   04 Oct 2001 16:17:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/

