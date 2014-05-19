//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRange.cpp
//* .File Name       : $Workfile:   SVRange.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVRange.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

struct SVRangeClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SVDoubleValueObjectClass FailLow;
	SVDoubleValueObjectClass FailHigh;
	SVDoubleValueObjectClass WarnLow;
	SVDoubleValueObjectClass WarnHigh;
};


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVRangeClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVRangeClass, SVRangeClassGuid );


SVRangeClass::SVRangeClass( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectClass( POwner, StringResourceID )
{
	init();
}

void SVRangeClass::init()
{
	m_bUseOverlays = false;

	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &FailHigh, SVRangeClassFailHighObjectGuid, IDS_OBJECTNAME_FAIL_HIGH, false, SVResetItemNone );
	RegisterEmbeddedObject( &WarnHigh, SVRangeClassWarnHighObjectGuid, IDS_OBJECTNAME_WARN_HIGH, false, SVResetItemNone );
	RegisterEmbeddedObject( &FailLow, SVRangeClassFailLowObjectGuid, IDS_OBJECTNAME_FAIL_LOW, false, SVResetItemNone );
	RegisterEmbeddedObject( &WarnLow, SVRangeClassWarnLowObjectGuid, IDS_OBJECTNAME_WARN_LOW, false, SVResetItemNone );
	
	// Set Embedded defaults
	FailLow.SetDefaultValue( 0.0, TRUE );
	FailHigh.SetDefaultValue( 99999, TRUE );
	WarnLow.SetDefaultValue( 0.0, TRUE );
	WarnHigh.SetDefaultValue( 99999, TRUE );

	// Setup up the input
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "RangeValue" ) );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

BOOL SVRangeClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVTaskObjectClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	FailHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnHigh.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	FailLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	WarnLow.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	return isCreated;
}

BOOL SVRangeClass::OnValidate()
{
	BOOL bRetVal = SVTaskObjectClass::OnValidate();

		// check if input is valid
	if( bRetVal && inputObjectInfo.IsConnected() &&
		inputObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

/*	// inlined
BOOL SVRangeClass::getInputValue( double& RVal )
{
	if( inputObjectInfo.IsConnected &&
		inputObjectInfo.InputObjectInfo.PObject )
	{
		return ( static_cast <SVValueObjectClass*> (inputObjectInfo.InputObjectInfo.PObject) )->GetValue( RVal ) == S_OK;
	}
	return FALSE;
}
//*/

BOOL SVRangeClass::onRun(SVRunStatusClass& RRunStatus)
{
	double failLow,failHigh;
	double warnLow,warnHigh;

	double v;

	getInputValue(v);
	
	FailLow.GetValue( failLow );
	FailHigh.GetValue( failHigh );
	WarnLow.GetValue( warnLow );
	WarnHigh.GetValue( warnHigh );
			
	bool isFailed =  ( v < failLow || v > failHigh  );
  	bool isWarned = ( !isFailed && ( v < warnLow || v > warnHigh ) );

	if( isFailed )
		RRunStatus.SetFailed();
			
	if( isWarned )
		RRunStatus.SetWarned();
			
	if( !isFailed && !isWarned )
		RRunStatus.SetPassed();
			
	return TRUE;
}


DWORD_PTR SVRangeClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

// ISVCancel interface
bool SVRangeClass::CanCancel()
{
	return true;
}

HRESULT SVRangeClass::GetCancelData(SVCancelData*& rpCancelData)
{
	ASSERT(rpCancelData == NULL);
	SVRangeClassCancelData* pData = new SVRangeClassCancelData;
	rpCancelData = pData;

	pData->FailHigh = FailHigh;
	pData->FailLow = FailLow;
	pData->WarnHigh = WarnHigh;
	pData->WarnLow = WarnLow;

	return S_OK;
}

HRESULT SVRangeClass::SetCancelData(SVCancelData* pCancelData)
{
	SVRangeClassCancelData* pData = dynamic_cast<SVRangeClassCancelData*> (pCancelData);
	if ( pData )
	{
		FailHigh = pData->FailHigh;
		FailLow = pData->FailLow;
		WarnHigh = pData->WarnHigh;
		WarnLow = pData->WarnLow;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

BOOL SVRangeClass::getInputValue( double& RVal )
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
	{
		return ( static_cast <SVValueObjectClass*> (inputObjectInfo.GetInputObjectInfo().PObject) )->GetValue( RVal ) == S_OK;
	}
	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRange.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 11:56:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:03:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:38:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   12 Jul 2012 15:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   19 Jun 2012 14:12:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   01 Jun 2010 15:08:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Dec 2009 12:34:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   27 Mar 2006 09:16:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  542
 * SCR Title:  Change default value for Range Fail High and Warn High
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the default values for Warn High and Fail High to 9999 instead of being 0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   20 Mar 2006 13:57:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  547
 * SCR Title:  Fix Image overlay data
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed overlays issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   02 Sep 2005 14:33:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   29 Jul 2005 13:22:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   21 Jun 2005 08:27:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   17 Feb 2005 15:23:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   16 Feb 2005 15:10:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   12 Jul 2004 11:59:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made SVRangeClass implement ISVCancel
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Apr 2003 14:19:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   14 Mar 2003 17:42:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  282
 * SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed some small existing bugs dealing with: Range always reporting IsValid =  FALSE, Configure Analyzer available on context menu even when there isn't one, and setting ranges on Blob Analyzer results greater than 800,000,000,000.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   30 Jan 2003 16:17:30   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   03 Jan 2003 15:34:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added attributes to the WarnHigh, WarnLow, FailHigh, FailLow objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   09 Dec 2002 20:32:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:29:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Sep 1999 15:57:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. Moved SVRangeClass from SVResult.cpp
 *  and SVResult.h
 * Revised onRun Method to correct problem with testing
 * FailHigh/WarnHigh/WarnLow/FailLow values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
