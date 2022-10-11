//******************************************************************************
/// COPYRIGHT (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for the analyzer
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvPb
{
class InspectionCmdResponse;
}

namespace SvOi
{
class IAnalyzer
{
public:
	virtual ~IAnalyzer() {}

	virtual SvPb::InspectionCmdResponse getAnalyzerExtentProperties() const = 0;
};
} //namespace SvOi