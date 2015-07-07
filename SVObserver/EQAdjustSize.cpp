//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Base class for Formulas for Tool Width,Height and Positions for ToolsSizeAdjust 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSize.h"
#include "SVTaskObject.h"
#include "SVEquationLibrary\SVEquationLex.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations







EQAdjustSize::EQAdjustSize( long subType, GUID resultGuid,  SVObjectClass* POwner, int StringResourceID )
	:SVEquationClass( POwner, StringResourceID )

{
	m_ResultGuid = resultGuid;
	m_SubType = subType;
	Init();
}



GUID& EQAdjustSize::GetResultGuid()
{
		return m_ResultGuid;

}

DWORD EQAdjustSize::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}

void EQAdjustSize::Init()
{
	enabled.SetDefaultValue( FALSE, TRUE );
	m_bUseOverlays = false;
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = m_SubType;
	RegisterEmbeddedObject( &m_result, m_ResultGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );
	// Set Embedded defaults
	static const double DefaultValue = 100.0;
	m_result.SetDefaultValue( DefaultValue, TRUE );
	// Set default inputs and outputs
	addDefaultInputObjects();
}



BOOL EQAdjustSize::CreateObject(SVObjectLevelCreateStruct *PCreateStruct)
{
	isCreated = SVEquationClass::CreateObject(PCreateStruct);

	// Set / Reset Printable Flag
	m_result.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return isCreated;
}


BOOL EQAdjustSize::OnValidate()
{
	BOOL retVal = TRUE;

	// validate our outputs
	retVal = SVEquationClass::OnValidate() && retVal;

	return retVal;
}


BOOL EQAdjustSize::onRun( SVRunStatusClass& RRunStatus )
{
	///This function is not called
	ASSERT(FALSE);
	return TRUE;
}

HRESULT EQAdjustSize::ResetObject()
{
	double value = 0.0;
	HRESULT hr = SVEquationClass::ResetObject();

	if( S_OK == hr )
	{
		if( HasCondition() && IsEnabled() )
		{
			value = yacc.equationResult;
			m_result.SetValue(1,value);	
		}
		
	}
	return hr;
}

