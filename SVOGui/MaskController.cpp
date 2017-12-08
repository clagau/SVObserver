//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MaskController.h"
#include "InspectionCommands\GetInstanceIDByTypeInfo.h"
#include "InspectionCommands\GetReferenceImage.h"
#include "InspectionCommands\GetMaskImage.h"
#include "InspectionCommands\ImportMask.h"
#include "InspectionCommands\ExportMask.h"
#include "InspectionCommands\GetMaskData.h"
#include "InspectionCommands\SetMaskData.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace SvOg
{

	MaskController::MaskController(const GUID& rInspectionID, const GUID& rTaskObjectID)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	{
	}

	void MaskController::Init()
	{
		// Get Instance GUID for the Mask Operator...
		typedef SvCmd::GetInstanceIDByTypeInfo Command;
		typedef SVSharedPtr<Command> CommandPtr;
	
		SvDef::SVObjectTypeInfoStruct info(SvDef::SVUnaryImageOperatorObjectType, SvDef::SVUserMaskOperatorObjectType);
	
		CommandPtr commandPtr = CommandPtr(new Command(m_TaskObjectID, info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_maskOperatorID = commandPtr->GetInstanceID();
		}
	}

	// must call init before calling this method
	const GUID& MaskController::GetInstanceID() const
	{
		return m_maskOperatorID;
	}

	IPictureDisp* MaskController::GetReferenceImage() const
	{
		typedef SvCmd::GetReferenceImage Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			return commandPtr->Image();
		}
		return nullptr;
	}

	IPictureDisp* MaskController::GetMaskImage() const
	{
		typedef SvCmd::GetMaskImage Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			return commandPtr->Image();
		}
		return nullptr;
	}

	HRESULT MaskController::ImportMask(const std::string& filename)
	{
		typedef SvCmd::ImportMask Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID, filename);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	HRESULT MaskController::ExportMask(const std::string& filename)
	{
		typedef SvCmd::ExportMask Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID, filename);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	HGLOBAL MaskController::GetMaskData() const
	{
		typedef SvCmd::GetMaskData Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			return commandPtr->GetDataHandle();
		}
		return nullptr;
	}

	bool MaskController::SetMaskData(HGLOBAL hGlobal)
	{
		typedef SvCmd::SetMaskData Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_maskOperatorID, hGlobal);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return (S_OK == hr) ? true : false;
	}
} //namespace SvOg
