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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLineEdgePositionAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:03:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   16 Aug 2012 18:55:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   18 Jul 2012 13:40:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   16 Jul 2012 12:49:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   10 Jul 2012 12:18:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   02 Jul 2012 17:15:54   jspila
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
   
      Rev 3.27   08 Dec 2010 13:02:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   01 Jun 2010 14:52:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   16 Dec 2009 14:03:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   03 Sep 2009 10:29:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   21 Jun 2007 13:28:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   29 Aug 2005 16:20:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added onUpdateFigure to OnRun when m_bDisplayAnalyzerLine is true.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   12 Aug 2005 13:51:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated class to fix issues with data synchrization.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   29 Jul 2005 12:42:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   21 Jun 2005 08:13:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   18 Mar 2005 09:16:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   09 Mar 2005 10:35:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Change Calculatlion method to draw gage tool edges.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   17 Feb 2005 14:37:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   09 Dec 2003 12:19:18   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Adjusted where edge control parameters are read from value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   04 Dec 2003 13:00:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   18 Nov 2003 06:54:58   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the thresholds not updating.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   13 Nov 2003 07:45:12   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems - was updating the wrong data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   08 Oct 2003 16:30:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Oct 2003 10:59:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using the value objects to store the edge data
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   22 Apr 2003 10:39:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   30 Jan 2003 12:07:44   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   10 Jan 2003 15:32:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   09 Dec 2002 19:32:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   19 Nov 2002 15:07:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   08 Nov 2001 14:30:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated init, onUpdateFigure and onRun methods to calculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
