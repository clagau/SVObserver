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

#include "stdafx.h"
#include "SVLineEdgeCountingAnalyzer.h"
#include "SVImageToLineProject.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVTool.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLineEdgeCountingAnalyzer.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 14:57:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:04:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   11 Sep 2012 19:00:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed crash when there were no edges found.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   16 Aug 2012 18:55:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   01 Aug 2012 13:01:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   27 Jul 2012 08:55:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   18 Jul 2012 13:40:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   16 Jul 2012 12:49:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   10 Jul 2012 12:18:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   02 Jul 2012 17:15:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   16 Mar 2011 13:40:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   08 Dec 2010 13:02:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   01 Jun 2010 14:52:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   16 Dec 2009 14:03:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   21 Jun 2007 13:28:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   29 Aug 2005 16:20:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added onUpdateFigure to OnRun when m_bDisplayAnalyzerLine is true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   12 Aug 2005 13:51:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated class to fix issues with data synchrization.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   29 Jul 2005 12:42:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   21 Jun 2005 08:13:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   18 Mar 2005 09:16:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   17 Feb 2005 14:37:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   09 Dec 2003 12:19:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Adjusted where edge control parameters are read from value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   04 Dec 2003 12:59:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   18 Nov 2003 06:54:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the thresholds not updating.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   08 Oct 2003 16:30:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Oct 2003 10:58:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using value objects for the edge parameters instead of the string
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   22 Apr 2003 10:39:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   30 Jan 2003 12:07:48   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   10 Jan 2003 15:32:44   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   09 Dec 2002 19:32:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   19 Nov 2002 15:06:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   08 Nov 2001 14:26:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and onRun methods to calculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
