//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for ResultValueDefinition
//******************************************************************************

#pragma once

#pragma region Includes
#include <string>
#pragma endregion Includes

namespace SvOi
{
	class  IResultValueDefinition
	{
	public:
		virtual ~IResultValueDefinition() = default;

		virtual long getVT() const = 0;
		virtual std::string getDisplayName() const = 0;
		virtual std::string getHelpText() const = 0;
		virtual std::string getGroup() const = 0;
		virtual bool UseDisplayNames() const = 0;
	};
}