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

EQAdjustSize::EQAdjustSize(SVObjectSubTypeEnum subType, GUID resultGuid, SVObjectClass* POwner, int StringResourceID)
: SVEquationClass( POwner, StringResourceID )
{
	m_ResultGuid = resultGuid;
	m_SubType = subType;
	Init();
}

EQAdjustSize::~EQAdjustSize()
{
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
	enabled.SetDefaultValue( BOOL(false) );
	m_bUseOverlays = false;
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = m_SubType;
	RegisterEmbeddedObject( &m_result, m_ResultGuid, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone );
	// Set Embedded defaults
	static const double DefaultValue = 100.0;
	m_result.SetDefaultValue( DefaultValue );
	m_result.setSaveValueFlag(false);
	// Set default inputs and outputs
	addDefaultInputObjects();
}



bool EQAdjustSize::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	m_isCreated = SVEquationClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_result.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}


bool EQAdjustSize::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	///This function is not called
	return true;
}

bool EQAdjustSize::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVEquationClass::ResetObject(pErrorMessages);

	if( Result )
	{
		if( HasCondition() && IsEnabled() )
		{
			m_result.SetValue(getResult());
		}
		
	}
	return Result;
}

