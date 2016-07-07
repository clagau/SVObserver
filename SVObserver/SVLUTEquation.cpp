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

#pragma region Includes
#include "stdafx.h"
#include "SVLUTEquation.h"
#include "SVObserver.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#pragma endregion Includes

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
void SVLUTEquationClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVLUTEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	m_byteVectorResult.SetLegacyVectorObjectCompatibility();
	RegisterEmbeddedObject( &m_lutIndex, SVLUTIndexVariableObjectGuid, IDS_OBJECTNAME_LUTINDEXVARIABLE, false, SVResetItemNone  );
	RegisterEmbeddedObject( &m_byteVectorResult, SVLUTEquationResultObjectGuid, IDS_OBJECTNAME_LUTRESULT, false, SVResetItemNone  );
	RegisterEmbeddedObject( &m_isLUTFormulaClipped, SVLUTEquationClipFlagObjectGuid, IDS_OBJECTNAME_LUT_EQUATION_CLIP, false, SVResetItemTool  );

	// Set Embedded defaults
	m_byteVectorResult.SetDefaultValue( 0, TRUE );
	m_isLUTFormulaClipped.SetDefaultValue( TRUE, TRUE );

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
BOOL SVLUTEquationClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVEquationClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flag
	m_lutIndex.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_byteVectorResult.ObjectAttributesAllowedRef() &= ~(SV_PRINTABLE | SV_VIEWABLE);

	m_isCreated = bOk;

	return bOk;
}

HRESULT SVLUTEquationClass::ResetObject()
{
	HRESULT l_hrOk = SVEquationClass::ResetObject();

	// Resize to 256 entries...
	if( S_OK == l_hrOk && 256 != m_byteVectorResult.GetArraySize()  )
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
BOOL SVLUTEquationClass::SetDefaultFormula()
{
	// Get current complete name of LUT Index...
	CString strName = m_lutIndex.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType );

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

