/// \copyright COPYRIGHT (c) 2023 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved
/// \file PassedWarnedFailedHelper.cpp
/// contains a mix-in class created to avoid code duplication which 
/// helps handle passed/warned/failed information that is required in several classes


#pragma region Includes
#include "stdafx.h"
#include "PassedWarnedFailedHelper.h"
#pragma endregion Includes

namespace SvVol
{

void PassedWarnedFailedHelper::registerPwfAndSetDefaults(SVObjectClass& rSvo)
{
	// Register Embedded Objects
	rSvo.RegisterEmbeddedObject(&m_Failed, SvPb::FailedEId, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone, false);
	rSvo.RegisterEmbeddedObject(&m_Warned, SvPb::WarnedEId, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone, false);
	rSvo.RegisterEmbeddedObject(&m_Passed, SvPb::PassedEId, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone, false);

	// Set Embedded defaults
	m_Passed.SetDefaultValue(BOOL(false), true);		// Default for Passed is FALSE !!!
	m_Passed.setSaveValueFlag(false);
	m_Passed.SetObjectAttributesAllowed(SvPb::ObjectAttributes::shortMode, SvOi::AddAttribute);
	m_Failed.SetDefaultValue(BOOL(true), true);			// Default for Failed is TRUE !!!
	m_Failed.setSaveValueFlag(false);
	m_Failed.SetObjectAttributesAllowed(SvPb::ObjectAttributes::shortMode, SvOi::AddAttribute);
	m_Warned.SetDefaultValue(BOOL(true), true);			// Default for Warned is TRUE !!!
	m_Warned.setSaveValueFlag(false);
}


void PassedWarnedFailedHelper::setPwfObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type)
{
	m_Passed.SetObjectAttributesAllowed(Attributes, Type);
	m_Failed.SetObjectAttributesAllowed(Attributes, Type);
	m_Warned.SetObjectAttributesAllowed(Attributes, Type);
}

void PassedWarnedFailedHelper::setWarnedFailedStatus(bool isWarn, bool isFail)
{
	m_Warned.SetValue(BOOL(isWarn));
	m_Failed.SetValue(BOOL(isFail));
	m_Passed.SetValue(BOOL(!isFail && !isWarn));
}

bool PassedWarnedFailedHelper::isFailed()
{
	BOOL RVal = true;
	m_Failed.GetValue(RVal);
	return(TRUE == RVal);
}

bool PassedWarnedFailedHelper::isWarned()
{
	BOOL RVal = true;
	m_Warned.GetValue(RVal);
	return(TRUE == RVal);
}

std::tuple<bool, bool, bool> PassedWarnedFailedHelper::GetPassedWarnedFailedStatus() const
{
	BOOL bPassed = true;
	BOOL bWarned = true;
	BOOL bFailed = true;

	m_Passed.GetValue(bPassed);
	m_Warned.GetValue(bWarned);
	m_Failed.GetValue(bFailed);

	return {bPassed == TRUE, bWarned == TRUE, bFailed == TRUE};
}

} //namespace SvVol
