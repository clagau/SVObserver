//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTEquation
//* .File Name       : $Workfile:   SVLUTEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVLUTEquation.h"
#include "SVObserver.h"

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLUTEquationClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVLUTEquationClass, SVLUTEquationClassGuid );

SVLUTEquationClass::SVLUTEquationClass( SVObjectClass* POwner, int StringResourceID )
				   :SVEquationClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVLUTEquationClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVLUTEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	m_byteVectorResult.SetLegacyVectorObjectCompatibility();
	RegisterEmbeddedObject( &m_lutIndex, SVLUTIndexVariableObjectGuid, IDS_OBJECTNAME_LUTINDEXVARIABLE, false, SVResetItemNone  );
	RegisterEmbeddedObject( &m_byteVectorResult, SVLUTEquationResultObjectGuid, IDS_OBJECTNAME_LUTRESULT, false, SVResetItemNone  );
	RegisterEmbeddedObject( &m_isLUTFormulaClipped, SVLUTEquationClipFlagObjectGuid, IDS_OBJECTNAME_LUT_EQUATION_CLIP, false, SVResetItemTool  );

	// Set Embedded defaults
	m_byteVectorResult.SetDefaultValue( 0, TRUE );
	
	if ( TheSVObserverApp.getLoadingVersion() > 0x60A00 ) //0x60A00 is the version 6.10
	{  //the default value should be for new tools TRUE
		m_isLUTFormulaClipped.SetDefaultValue( TRUE, TRUE );
	}
	else
	{  //the default value should be for old configurations FALSE
		m_isLUTFormulaClipped.SetDefaultValue( FALSE, TRUE );
	}
	// NOTE: Vector Size Setting...
	// Set default vector size in CreateObject, since objectDepth must be set before!!!
	//m_byteVectorResult.SetSize( 256, TRUE );
	// Place another check/set vector size at the position you want to use the vector
	// i.e. in onRun(...) : 
	//	if( vector.GetSize() != targetSize  ) { vector.SetSize( targetSize ); }

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLUTEquationClass::~SVLUTEquationClass()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : Creates this object.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVEquationClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_lutIndex.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_byteVectorResult.ObjectAttributesAllowedRef() &= ~(SV_PRINTABLE | SV_VIEWABLE);

	isCreated = bOk;

	return bOk;
}

HRESULT SVLUTEquationClass::ResetObject()
{
	HRESULT l_hrOk = SVEquationClass::ResetObject();

	// Resize to 256 entries...
	if( l_hrOk == S_OK && m_byteVectorResult.GetArraySize() != 256 )
	{
		m_byteVectorResult.SetArraySize( 256 );
	}

	return l_hrOk;
}

BOOL SVLUTEquationClass::OnValidate()
{
	BOOL retVal = TRUE;
	
	// validate our outputs
	retVal = SVEquationClass::OnValidate() && retVal;

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetDefaultFormula
// -----------------------------------------------------------------------------
// .Description : Sets the default formula, which is in this case
//				: the LUT Index variable of this LUT Equation class.
//				:
//              : The LUT Index variable is running from 0 to 255, while  
//				: onRun(...) of this class is processed. 
//				: ( Refer to SVLUTEquationClass::onRun(...) ).
//				: 
//				: The result of this equation is then placed in the result vector
//				:
//				: NOTE
//				: Call this function only if you have an tool set ancestor!
//				: ( i.e. if your correlated tool is added to the tool set,
//				:   after the user clicked on 'Add XXXTool' ).
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :23.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTEquationClass::SetDefaultFormula()
{
	// Get current complete name of LUT Index...
	CString strName = m_lutIndex.GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType );

	CString strEmpty;
	if( ! strName.IsEmpty() )
	{
		// Set equation in quotes...
		CString strEquation;
		strEquation.Format( _T( "\"%s\"" ), strName );

		// Set equation...
		SetEquationText( strEquation );

		// Update symbol table and test...
		if( Test().bPassed )
		{
			return TRUE;
		}
		
		// something is wrong, flush equation...
		SetEquationText( strEmpty );
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this object.
//				: ( No routing! Call Run(...) to run and route! )
//				:
//              : Runs 256 times the equation and calculates the new LUT values,
//				: which are placed into the byte result vector.
//				: Use the m_lutIndex as variable in the equation, if you want to
//				: index the values. ( m_lutIndex is running from 0 to 255 )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVLUTEquationClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL   bRetVal   = FALSE;
	double dResult   = 0.0;
	long   lLUTIndex = 0;

	// Resize to 256 entries...
	if( m_byteVectorResult.GetArraySize() != 256 )
	{
		m_byteVectorResult.SetArraySize( 256 );
	}

	do
	{
		// Set current m_lutIndex...
		m_lutIndex.SetValue( RRunStatus.m_lResultDataIndex, lLUTIndex );

		// Run equation.. 
		bRetVal = SVEquationClass::onRun( RRunStatus );

		if( bRetVal && HasCondition() && IsEnabled() )
		{
			// Get equation result...
			dResult = yacc.equationResult;
		}
		else
			dResult = 0.0;

		//Clip result if required
		bool isClipped = false;
		m_isLUTFormulaClipped.GetValue(isClipped);
		if (isClipped)
		{
			if (UCHAR_MAX < dResult)
			{
				dResult = (double)UCHAR_MAX;
			}
			else if (0.0 > dResult)
			{
				dResult = 0.0;
			}
		}

		// Put result into LUT vector at m_lutIndex position...
		m_byteVectorResult.SetValue( RRunStatus.m_lResultDataIndex, lLUTIndex, ( BYTE ) dResult );

		// next index...
		++lLUTIndex;

	} while( bRetVal && lLUTIndex < 256 );
		 
	if( ! bRetVal )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLUTEquation.cpp_v  $
 * 
 *    Rev 1.2   31 Jan 2014 17:16:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2014 15:36:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  883
 * SCR Title:  Clip-mode for LUT formula mode
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed init method to register and initialize m_isLUTFormulaClipped.
 * Changed onRun method to support Clip Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:19:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Jun 2010 14:52:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   07 Oct 2009 14:40:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  673
 * SCR Title:  Change the print attribute on LUT Index on the LUT Operator to not printable
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   change so LUTIndex will not appear on the Configuration Print
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Aug 2005 13:53:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   turn off SV_VIEWABLE flag for LUT array
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Aug 2005 12:00:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   29 Jul 2005 13:07:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2005 08:14:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * equation uses bPassed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Feb 2005 14:44:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 10:58:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:37:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Nov 2002 15:29:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Mar 2000 17:33:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced SetDefaultFormulas() to set LUT index as default
 * formula.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:50:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides Formula calculation for LUT Vector with 256 elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
