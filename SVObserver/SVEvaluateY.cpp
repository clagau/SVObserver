//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEvaluateY
//* .File Name       : $Workfile:   SVEvaluateY.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:29:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVEvaluateY.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVEvaluateYClass, SVEvaluateYClassGuid )

SVEvaluateYClass::SVEvaluateYClass( SVObjectClass* POwner, int StringResourceID )
				 :SVMathContainerClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.SubType = SVEvaluateYObjectType;

	// Identify our input type needs...

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputMathEnabled, SVOutputEvaluateYEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemNone );
	RegisterEmbeddedObject( &outputMathResult, SVOutputEvaluateYResultObjectGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	outputMathEnabled.SetDefaultValue( FALSE, TRUE );
	outputMathResult.SetDefaultValue( 0.0, TRUE );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVEvaluateYClass::~SVEvaluateYClass()
{
	CloseObject();
}

BOOL SVEvaluateYClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVMathContainerClass::CreateObject( PCreateStructure );

	// Reset Printable Flag
	outputMathResult.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	outputMathEnabled.ObjectAttributesAllowedRef()  |= SV_PRINTABLE;

	return isCreated;
}

BOOL SVEvaluateYClass::CloseObject()
{
	if( SVMathContainerClass::CloseObject() )
	{
		BOOL bRetVal = TRUE;
		return bRetVal;
	}
	return FALSE;
}


SVBoolValueObjectClass*	SVEvaluateYClass::getOutputMathEnabled()
{
	return &outputMathEnabled;
}

SVDoubleValueObjectClass* SVEvaluateYClass::getOutputMathResult()
{
	return &outputMathResult;
}

BOOL SVEvaluateYClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVBoolValueObjectClass*	pEnabled = getOutputMathEnabled();
	ASSERT( pEnabled );
	SVDoubleValueObjectClass* pResult = getOutputMathResult();
	ASSERT( pResult );

	BOOL bEnabled;
	if( S_OK != pEnabled->GetValue( bEnabled ) )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}

	if( ! bEnabled )
	{
		double result = 0.0;
		if( S_OK != pResult->SetValue( RRunStatus.m_lResultDataIndex, result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}
		return TRUE;
	}

	// All inputs and outputs must be validated first
	if( SVMathContainerClass::onRun( RRunStatus ) )
	{
		SVDoubleValueObjectClass* pInputResult = getInputMathResult();
		ASSERT( pInputResult );

		double result;
		if( S_OK != pInputResult->GetValue( result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		if( S_OK != pResult->SetValue( RRunStatus.m_lResultDataIndex, result ) )
		{
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}


DWORD SVEvaluateYClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;

	return( SVMathContainerClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEvaluateY.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:29:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   01 Jun 2010 14:19:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   16 Dec 2009 10:18:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   29 Jul 2005 12:32:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   21 Jun 2005 08:07:52   ebeyeler
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
 *    Rev 3.6   17 Feb 2005 14:04:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 14:06:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 09:32:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   29 Jan 2003 15:42:34   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage method to update image depth on a create all objects message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Dec 2002 19:19:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Nov 2002 12:51:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:31:32   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:14:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Y Value Equation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
