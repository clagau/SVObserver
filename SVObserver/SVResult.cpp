//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResult.cpp
//* .File Name       : $Workfile:   SVResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   14 Jan 2015 16:43:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVResult.h"
#include "SVGlobal.h"
#include "SVRange.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL MACROS:
//******************************************************************************
//#define WIT_svWitFontParams(o) ((SVWitFontParams *) ((o)->val.data))
//#define WIT_svImVec(sv) ((WitImageVector *) &((sv)->imVec))
//#define WIT_svStrVec(sv) ((StringVector *) &((sv)->strVec))

struct SVResultClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SVResultClassCancelData() {m_pRangeData = NULL;}
	virtual ~SVResultClassCancelData() {if (m_pRangeData) delete m_pRangeData;}
	SVCancelData* m_pRangeData;
};

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVResultClass
//* Note(s)    : Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVResultClass, SVResultClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       :  SVResultClass
// -----------------------------------------------------------------------------
// .Description : Standard constructor
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :08.07.1998 RO			First Implementation
//	:19.01.1999 RO			Supplements for 3.0, Change Request ...
//							Calls now SVTaskObjectClass constructor instead of
//							SVObjectClass constructor
////////////////////////////////////////////////////////////////////////////////

SVResultClass::SVResultClass ( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVResultClass::init()
{
	m_bUseOverlays = false;

	// Identify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SVResetItemNone );
	RegisterEmbeddedObject( &warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SVResetItemNone );
	RegisterEmbeddedObject( &passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SVResetItemNone );

	// Set Embedded defaults
	passed.SetDefaultValue( FALSE, TRUE );			// Default for Passed is FALSE !!!
	failed.SetDefaultValue( TRUE, TRUE );			// Default for Failed is TRUE !!!
	warned.SetDefaultValue( TRUE, TRUE );			// Default for Warned is TRUE !!!

	// Set up the Default Inputs/Outputs
	addDefaultInputObjects();
}

SVResultClass::~SVResultClass()
{
}

BOOL SVResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = TRUE;
	}

	// Set / Reset Printable Flags
	failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

DWORD_PTR SVResultClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

BOOL SVResultClass::IsFailed()
{
	BOOL RVal = TRUE;
	failed.GetValue( RVal );
	return( RVal );
}

BOOL SVResultClass::IsWarned()
{
	BOOL RVal = TRUE;
	warned.GetValue( RVal );
	return( RVal );
}

BOOL SVResultClass::IsGood()
{
	BOOL RVal = TRUE;
	passed.GetValue( RVal );
	return( RVal );
}

SVRangeClass* SVResultClass::GetResultRange()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVRangeObjectType;
	SVRangeClass* pRange = reinterpret_cast<SVRangeClass*>(SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
	return pRange;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Draw
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:06.06.1998 RO          Supplement, look for sub results too
////////////////////////////////////////////////////////////////////////////////
CRect SVResultClass::Draw( HDC DC, CRect R )
{
	/* SEJ (OLD)
	CString strText;
	SVObjectClass* pObject = GetOwnerInfo().PObject;
	if( GetSize() <= 0 )
	{
		::SetTextColor( DC, SV_DEFAULT_INACTIVE_COLOR );
		if( pObject )
			strText = _T( "( " ) + pObject->GetName() + _T( ", No Result )" );
		else
			if( pOwnerTool )
				strText = _T( "( " ) + pOwnerTool->GetName() + _T( ", No Result )" );
			else
				strText = _T( "( Unknown Owner, No Result )" );

		::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
		
		R.top += SV_DEFAULT_TEXT_HEIGHT + 2;
	}
	else
	{
		// Store left position...
		long left = R.left;

		if( pOwnerTool && pOwnerTool->GetAnalyzer() &&
			pOwnerTool->GetAnalyzer()->GetResultObject() == this )
		{
			::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR );
			strText = pOwnerTool->GetName();
			::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
			R.top += SV_DEFAULT_TEXT_HEIGHT;

			// Indent
			R.left += SV_DEFAULT_TEXT_HEIGHT;
		}

		if( pObject )
		{
			::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR );
			strText = _T( "." ) + pObject->GetName();
			::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
			R.top += SV_DEFAULT_TEXT_HEIGHT;

			// Indent
			R.left += SV_DEFAULT_TEXT_HEIGHT;
		}

		// Draw all sub results...
		for( int i = 0; i < GetSize(); ++i )
			if( GetAt( i ) )
				R = GetAt( i )->Draw( DC, R );

		// Restore old left position...
		R.left = left;

		R.top += 2;
	}
	*/
	return R;
}

BOOL SVResultClass::Run( SVRunStatusClass& RRunStatus )
{
	SVValueObjectClass* pValue = getInput();
	
	long	resultSize = -1;
	if (nullptr != pValue)
	{
		resultSize = pValue->GetResultSize();
	}

	// This is testing to verify that a result was found.  Analyzers such as 
	// Blob, Character, and Barcode deal with variable numbers of valid 
	// results, and need to deal with the possibillity of not finding blobs,
	// characters, or getting a barcode read.
	if (0 == resultSize)
	{
		// no results to process.
		// if there are no results outside the range, we want the ResultObject
		// to pass.

		//make sure the statusColor is set correctly
		DWORD dwColor = RRunStatus.GetStatusColor();
		statusColor.SetValue(RRunStatus.m_lResultDataIndex, dwColor);

		passed.SetValue( RRunStatus.m_lResultDataIndex, true );
		failed.SetValue( RRunStatus.m_lResultDataIndex, false );
		warned.SetValue( RRunStatus.m_lResultDataIndex, false );
		return TRUE;
	}
	else
	{
		// valid results to process.
		if( SVTaskObjectListClass::Run(RRunStatus) )
		{
			//make sure statusColor is set correctly
			DWORD dwColor = RRunStatus.GetStatusColor();
			statusColor.SetValue(RRunStatus.m_lResultDataIndex, dwColor);

			// set our state according to the runStatus
			passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			return TRUE;
		}
	}

	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

SVValueObjectClass* SVResultClass::getInput()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast <SVValueObjectClass*> ( inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


BOOL SVResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVTaskObjectListClass::onRun( RRunStatus ) )
	{
		return TRUE;
	}

	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

bool SVResultClass::CanCancel()
{
	return true;
}

HRESULT SVResultClass::GetCancelData(SVCancelData*& rpCancelData)
{
	ASSERT(rpCancelData == NULL);
	SVResultClassCancelData* pData = new SVResultClassCancelData;
	rpCancelData = pData;

	if ( SVRangeClass* pRange = GetResultRange() )
	{
		pRange->GetCancelData( pData->m_pRangeData );
	}

	return S_OK;
}

HRESULT SVResultClass::SetCancelData(SVCancelData* pCancelData)
{
	SVResultClassCancelData* pData = dynamic_cast<SVResultClassCancelData*> (pCancelData);
	if ( pData )
	{
		if ( SVRangeClass* pRange = GetResultRange() )
		{
			return pRange->SetCancelData( pData->m_pRangeData );
		}
		else
		{
			return S_OK;
		}
	}
	else
	{
		return S_FALSE;
	}
}

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAnalyzeFeatureClass
//* Note(s)    : Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVAnalyzeFeatureClass, SVAnalyzeFeatureClassGuid );

SVAnalyzeFeatureClass::SVAnalyzeFeatureClass( LPCSTR ObjectName )
					  :SVTaskObjectClass( ObjectName )
{
	feature				= 0L;
	resultArray			= NULL;
	featureResultRange	= NULL;
	resultArraySize		= 0;
	failed = TRUE;
	warned = TRUE;

	resultString		= NULL;
}

SVAnalyzeFeatureClass::~SVAnalyzeFeatureClass()
{
	if( featureResultRange )
		delete( featureResultRange );

	if( resultArray )
		delete( resultArray );

	if( resultString )
		delete( resultString );

}

BOOL SVAnalyzeFeatureClass::Create( LPCSTR ObjectName, long F )
{
	ASSERT( ObjectName != _T( "" ) );


	if( ObjectName != _T( "" ))
	{
		SetName( ObjectName );
		feature = F;
		return TRUE;
	}
	return FALSE;
}

void SVAnalyzeFeatureClass::create( int MaxNumber, SVRangeClass* R )
{
	resultArray		= new double[MaxNumber];
	resultArraySize = MaxNumber;
	if( resultString )
		delete( resultString );
	resultString	= new char[ resultArraySize * 20 ];

	for( int i = 0; i < resultArraySize; ++i )
		resultArray[i] = 0.0;

	if( R == NULL )
		featureResultRange = new SVRangeClass( this ); //0.0, 0.0, 0.0, 0.0, GetName() );
	else
		featureResultRange = R;
}

BOOL SVAnalyzeFeatureClass::IsFailed()
{
	return failed;
}

BOOL SVAnalyzeFeatureClass::IsWarned()
{
	return warned;
}

void SVAnalyzeFeatureClass::setFeatureResultRange( SVRangeClass* R )
{
	if( featureResultRange != R )
	{
		delete( featureResultRange );
		featureResultRange = R;
	}
}

SVRangeClass* SVAnalyzeFeatureClass::getFeatureResultRange()
{
	return( featureResultRange );
}

double* SVAnalyzeFeatureClass::getResultArray()
{
	return resultArray;
}

long SVAnalyzeFeatureClass::getFeature()
{
	return feature;
}

TCHAR* SVAnalyzeFeatureClass::GetResultArrayString( int N )
{
	TCHAR num[20];

	if( resultString && N )
	{
		_stprintf( resultString, _T( "%.2f, " ), resultArray[ 0 ] );
		for( register int i = 1; i < N && i < resultArraySize; ++i )
		{
			_stprintf( num, _T( "%.2f, " ), resultArray[ i ] );
			_tcscat( resultString, num );		
		}
		resultString[ _tcslen( resultString ) - 2 ] = '\0';	// Delete last Comma!
		return( resultString );
	}

	return( _T( "\0" ) );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResult.cpp_v  $
 * 
 *    Rev 1.4   14 Jan 2015 16:43:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Removed unused classes SVAnalyzeFeatureListClass and SVRangeDialogClass (replaced by RangeXDialogClass).
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Jan 2015 17:47:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Fixed spelling in comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 12:40:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:03:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:55:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   04 Sep 2012 15:56:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   18 Jul 2012 14:25:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   02 Jul 2012 17:33:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   27 Jan 2011 11:54:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   08 Dec 2010 13:35:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   09 Nov 2010 16:30:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   05 Nov 2010 14:58:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove the un-necessary IP View Control from the application.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   01 Jun 2010 15:08:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   01 Jun 2010 11:03:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   16 Dec 2009 12:34:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   31 Dec 2008 11:18:56   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  644
   SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Result Dialog to show complete caption
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   20 Mar 2006 13:57:10   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  547
   SCR Title:  Fix Image overlay data
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed overlays issue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   11 Aug 2005 14:48:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added Input Request Marker for ActiveX Call.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   09 Aug 2005 08:09:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   29 Jul 2005 13:22:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   21 Jun 2005 08:30:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   17 Feb 2005 15:25:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   16 Feb 2005 15:10:46   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   12 Jul 2004 11:58:38   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     made SVResultClass implement ISVCancel
   cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 14:55:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   18 Mar 2003 16:57:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  282
   SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified limits on the range dialog to 17000000. The old limits didn't make a difference. MIL sometimes still fails.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   14 Mar 2003 17:42:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  282
   SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed some small existing bugs dealing with: Range always reporting IsValid =  FALSE, Configure Analyzer available on context menu even when there isn't one, and setting ranges on Blob Analyzer results greater than 800,000,000,000.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   30 Jan 2003 16:17:30   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated processMessage and CreateObject methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   09 Dec 2002 20:02:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 11:12:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed references to System
   GetIPDocument
           Changed method of access
   Changed ValueObject semantics to work with buckets
   Changed TheObjectManager -> TheSVObjectManager
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   19 Apr 2001 19:17:58   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   19 Apr 2001 15:29:06   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Feb 03 2000 15:26:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  100
   SCR Title:  Negative Range Values Not Allowed
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Change range checking of Fail High, Warn High, Warn Low, and Fail Low to include negative numbers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:31:26   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Nov 09 1999 10:57:20   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove dead code from SVResult and associated files.
   SVAvailableFeatureListClass was removed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Oct 27 1999 15:14:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fix bug in Build Reference which resulted in a Tool failure as a result of a bug in the translation of 'child' images.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   27 Sep 1999 16:23:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved SVRangeClass to SVRange.cpp and SVRange.h
   Revised Constructor initialization not to create SVRangeClass.
   Revised onRun Method.
   Added Run Method to properly handle updating the
   pas/fail/warn status in the result class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   Sep 23 1999 14:04:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed obsolete Serialize() code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   Sep 23 1999 10:15:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed Obsolete classes and functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   23 Sep 1999 09:37:14   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     SVAnalyzeFeatureClass::Create () was changed to allow for a zero based feature index.
   
   SVAvailableFeatureListClass::SVAvailableFeatureListClass ()
   was changed to remove hard coded name and direct references to MIL names (predefines).
    
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 10 1999 14:25:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed old SVBlobAnalyzerClass and SVBlobAnalyzerDialog code into separate files.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   03 Sep 1999 14:31:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised CreateObject to call immediate base class CreateObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   02 Sep 1999 08:39:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised due to no requirement for sub results.
   Revised Run Logic.
   Removed code for resultColor 
   (color now lives in SVObjectClass)
   Revised to call immediate base class CreateObject.
   Removed override of SetObjectDepth. 
   (handled in SVObjectClass)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   30 Aug 1999 19:49:44   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Introduced Input Interface Handling.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   30 Aug 1999 18:57:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Run method to use SVRunStatusClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   27 Aug 1999 18:57:10   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Working on PixelAnalyzerResultClass.
   
   Changing White Pixel Result references to generic Pixel Result references.
   
   
   
   Split SVWhitePixelAnalyzerResultClass off into a separate file, and renamed it to reflect a more generic functionallity.
   SVPixelResultAnalyzer.cpp and SVPixelResultAnalyzer.h
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   27 Aug 1999 12:59:58   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Added Range Dialog as Setup Dialog to SVResult Base Class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Aug 25 1999 22:50:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  Steve Jones
   Change Description:  
     Revised Validate method to be OnValidate method.
    ( OnValidate - local scope validation).
   Added Validate method (Routes to all owned objects)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   25 Aug 1999 11:48:02   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Upgrading White Pixel Analyzer to new i/o object interface. 
   Changing White Pixel Analyzer to more generic Pixel Analyzer.
   
   
   
   Changed 
   SVWhitePixelAnalyzeResultClass to
   SVPixelAnalyzeResultClass
   and
   SVWhitePixelAnalyzeResultClassGuid to 
   SVPixelAnalyzeResultClassGuid
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   Aug 23 1999 18:23:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Split out SVResultListClass for better modularity.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/