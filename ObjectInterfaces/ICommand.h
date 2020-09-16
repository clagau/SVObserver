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

template <class T>
class CTaskWrapper : public SvOi::ICommand
{
public:
	explicit CTaskWrapper(T&& task)
	{
		m_InspectionTask = std::move(task);
	}
	CTaskWrapper() = delete;
	virtual unsigned long GetCommandType() const override { return 1UL; }
	virtual HRESULT Execute() { m_InspectionTask(); return S_OK; }
	T m_InspectionTask;

};

} //namespace SvOi
