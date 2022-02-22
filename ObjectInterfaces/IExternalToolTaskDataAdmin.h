//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for ExternalToolTaskData
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvOi
{
	class IInputValueDefinition;
	class IResultValueDefinition;
	//class IResultValue;
}

namespace SvOi
{
	class IExternalToolTaskDataAdmin
	{
	public:
		virtual ~IExternalToolTaskDataAdmin() {}

		virtual std::vector<std::shared_ptr<SvOi::IInputValueDefinition>> getInputValuesDefinition() const = 0;

		virtual long getNumInputValues() const = 0;
		virtual std::map<std::string, bool> getPropTreeState() const = 0;
		virtual void setPropTreeState(const std::map<std::string, bool>&)  = 0;

		virtual long getNumResultValues() const = 0;
		virtual std::vector<std::shared_ptr<SvOi::IResultValueDefinition>> getResultValuesDefinition() const = 0;

		virtual SvPb::GetTableResultsExternalToolResponse getTableResults() const = 0;
	};
}
