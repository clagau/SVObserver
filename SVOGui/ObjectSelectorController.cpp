//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ToolSetItemSelector.h"
#include "GuiCommands/GetObjectName.h"
#include "GuiCommands/GetPPQObjectName.h"
#include "GuiCommands/GetTaskObjectInstanceID.h"
#include "ObjectSelectorController.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg {

	#pragma region Constructor
	ObjectSelectorController::ObjectSelectorController(const SVGUID& rInspectionID, const SVGUID& rInstanceID) 
		: m_InspectionID(rInspectionID)
		, m_InstanceID(rInstanceID)
	{}

	ObjectSelectorController::~ObjectSelectorController()
	{}
	#pragma endregion Constructor

#pragma endregion Private Methods
	SVString ObjectSelectorController::GetInspectionName() const
	{
		SVString inspectionName;
		typedef SvCmd::GetObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			inspectionName = commandPtr->GetName();
		}
		return inspectionName;
	}

	SVString ObjectSelectorController::GetPPQName() const
	{
		SVString PPQName;
		typedef SvCmd::GetPPQObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(m_InspectionID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			PPQName = commandPtr->GetName();
		}
		return PPQName;
	}

	GUID ObjectSelectorController::GetToolSetGUID() const
	{
		GUID toolsetGUID = GUID_NULL;

		typedef SvCmd::GetTaskObjectInstanceID Command;
		typedef SVSharedPtr<Command> CommandPtr;

		SVObjectTypeInfoStruct info(SVToolSetObjectType);
		CommandPtr commandPtr = CommandPtr(new Command(m_InspectionID, info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			toolsetGUID = commandPtr->GetInstanceID();
		}

		return toolsetGUID;
	}
#pragma endregion Private Methods
} //namespace SvOg
