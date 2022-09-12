//******************************************************************************
/// COPYRIGHT (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for parameter task class
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

#pragma region Declarations
namespace SvPb
{
class InspectionCmdResponse;
class AddParameterAndUseItRequest;
class CheckParameterNamesRequest;
}
#pragma endregion Declarations

namespace SvOi
{
//this class is a interface. It should only have pure virtual public method and no member variables
class IParameterTask
{
public:
	virtual ~IParameterTask() {}

	/// Add a new Parameter and use it for the known dependencies
	virtual SvPb::InspectionCmdResponse addParameterAndUseIt(const SvPb::AddParameterAndUseItRequest& rRequest) = 0;

	/// Check if the parameter names are unique. 
	// To have a complete check, all input/result names must be added in this command, because the current parameter will be ignored.
	virtual SvPb::InspectionCmdResponse checkParameterNames(const SvPb::CheckParameterNamesRequest& rRequest) = 0;
};
} //namespace SvOi
