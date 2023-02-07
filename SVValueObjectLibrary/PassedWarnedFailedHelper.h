/// \copyright COPYRIGHT (c) 2023 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved
/// \file PassedWarnedFailedHelper.h
/// contains a mix-in class created to avoid code duplication which 
/// helps handle passed/warned/failed information that is required in several classes

#pragma once

#include "stdafx.h"
#include "SVBoolValueObjectClass.h"

namespace SvVol
{

struct PassedWarnedFailedHelper
{
	PassedWarnedFailedHelper() = default;
	~PassedWarnedFailedHelper() = default;

	void registerPwfAndSetDefaults(SVObjectClass& rSvo);
	void setPwfObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type);
	void setWarnedFailedStatus(bool isWarn, bool isFail); ///< also sets passed status derived from isWarn and isFail

	bool isFailed();
	bool isWarned();

	std::tuple<bool, bool, bool> GetPassedWarnedFailedStatus();

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	m_Failed;
};
} //namespace SvVol
