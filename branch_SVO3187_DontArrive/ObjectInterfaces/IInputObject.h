//******************************************************************************
/// COPYRIGHT (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for inputObject class
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
namespace SvDef
{
struct SVObjectTypeInfoStruct;
}
namespace SvPb
{
class GetInputDataResponse;
}
#pragma endregion Declarations

namespace SvOi
{
enum class InputAllowedMode
{
	Default,
	IsBeforeTool,
	DefaultPlusHidden,
	HasExtension
};

//this class is a interface. It should only have pure virtual public method and no member variables
class IInputObject
{
public:
	virtual ~IInputObject() {}

	virtual HRESULT checkAndSetInput(uint32_t connnectedInput) = 0;

	/// Return Parameter for the AvailableObject-command.
	/// \returns std::tuple<SvDef::SVObjectTypeInfoStruct, uint32_t, SvOi::InputAllowedMode> 1. info type, 2. startSearchId, 3. AllowedMode, 4. if AllowedMode == IsBeforeTool, it is the toolId, else it is InvalidObjectId
	virtual std::tuple<SvDef::SVObjectTypeInfoStruct, uint32_t, InputAllowedMode, uint32_t> getAvailableObjectParameter() const = 0;

	virtual SvPb::GetInputDataResponse getInputData(SvPb::SVObjectTypeEnum desiredFirstObjectTypeForConnectedName, bool excludeFirstObjectNameInConntectedName) const = 0;
};
} //namespace SvOi