//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ICommand.h
/// All Rights Reserved
//*****************************************************************************
/// This is an interface to execute commands
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

namespace SvOi
{
class ICommand
{
public:
	virtual ~ICommand() {};

	virtual unsigned long GetCommandType() const = 0;
	virtual HRESULT Execute() = 0;

protected:
	ICommand(){};
private:
	ICommand(const ICommand& rObject);
	const ICommand& operator=(const ICommand& rObject);
};

typedef std::shared_ptr<ICommand> ICommandPtr;
} //namespace SvOi
