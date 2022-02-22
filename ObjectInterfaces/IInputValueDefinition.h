//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for InputValueDefinition
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#include <string>
#pragma endregion Includes

namespace SvOi
{
	class  IInputValueDefinition
	{
	public:
		virtual ~IInputValueDefinition() = default;

		virtual long getVt() const = 0;
		virtual std::string getDisplayName() const = 0;
		virtual std::string getHelpText() const = 0;
		virtual std::string getGroup() const = 0;
		virtual int getDim() const = 0;
		virtual int  getLinkedValueIndex() const = 0;
		virtual SvPb::ExDllInterfaceType getType() const = 0;
		virtual const _variant_t& getDefaultValue() const = 0;
	};
}