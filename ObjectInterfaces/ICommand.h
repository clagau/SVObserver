//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ICommand.h
/// All Rights Reserved
//*****************************************************************************
/// This is an interface to execute commands
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvOi
{
	class IInspectionProcess;
	class IObjectClass;
}

namespace SvPb
{
	class TreeItem;
}

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

void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IInspectionProcess* pInspection, SvOi::IObjectClass* pStartObject, SvPb::LinkedValueTypeEnum valueType, bool wholeArray = false, std::vector<SvOi::IObjectClass*> excludeSameLineageList = {});

} //namespace SvOi
