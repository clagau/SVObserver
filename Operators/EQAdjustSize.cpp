//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Base class for Formulas for Tool Width,Height and Positions for ToolsSizeAdjust 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSize.h"
#include "InspectionEngine/SVTaskObject.h"
#include "Definitions/Color.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

EQAdjustSize::EQAdjustSize(SvPb::SVObjectSubTypeEnum subType, SvPb::EmbeddedIdEnum resultId, SVObjectClass* POwner, int StringResourceID)
: SVEquation( POwner, StringResourceID, true )
, m_ResultId(resultId)
, m_SubType(subType)
{
	Init();
}

EQAdjustSize::~EQAdjustSize()
{
}

SvPb::EmbeddedIdEnum EQAdjustSize::GetResultId()
{
	return m_ResultId;
}

DWORD EQAdjustSize::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}

void EQAdjustSize::Init()
{
	m_enabled.SetDefaultValue( BOOL(false) );
	m_bUseOverlays = false;
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_ObjectTypeInfo.m_SubType = m_SubType;
	RegisterEmbeddedObject( &m_result, m_ResultId, IDS_OBJECTNAME_RESULT, false, SvOi::SVResetItemNone, false );
	//@TODO[MEC][10.20][28.10.2021] Find a better default value! 
	constexpr double DefaultValue = 100.0; 
	m_result.SetDefaultValue( DefaultValue );
	m_result.setSaveValueFlag(false);
}



bool EQAdjustSize::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	m_isCreated = SVEquation::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_result.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}


bool EQAdjustSize::onRun( SvIe::RunStatus&, SvStl::MessageContainerVector* )
{
	///This function is not called
	return true;
}

bool EQAdjustSize::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = SVEquation::ResetObject(pErrorMessages);

	if( Result )
	{
		if( HasCondition() && IsEnabled() )
		{
			
			m_result.SetValue(getResult());
		}
		
	}
#ifdef TRACE_EQADJUSTSIZE 
	double val;
	m_result.GetValue(val);
	auto text = std::format("ResetObject  Result( {:d} )ret {:f}  Hascondition:{:d}; IsEnabled:{:d}   \n", Result, val, HasCondition() , IsEnabled());
	OutputDebugString(text.c_str());
#endif 
	return Result;
}

} //namespace SvOp
