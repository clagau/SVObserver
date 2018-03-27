//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MaskController.h"
#include "InspectionCommands\GetInstanceIDByTypeInfo.h"
#include "InspectionCommands\ImportMask.h"
#include "InspectionCommands\ExportMask.h"
#include "InspectionCommands\GetMaskData.h"
#include "InspectionCommands\SetMaskData.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "Definitions\TextDefineSvDef.h"
#pragma endregion Includes

namespace SvOg
{

	MaskController::MaskController(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& maskOperatorID)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_maskOperatorID(maskOperatorID)
	, m_MaskImageController(rInspectionID, maskOperatorID)
	, m_TaskImageController(rInspectionID, rTaskObjectID)
	{
	}

	void MaskController::Init()
	{
		m_MaskImageController.Init();
		m_TaskImageController.Init();

		// Get Instance GUID for the Mask Operator...
		typedef SvCmd::GetInstanceIDByTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;
	
		SvDef::SVObjectTypeInfoStruct ObjectInfo(SvDef::SVUnaryImageOperatorObjectType, SvDef::SVShapeMaskHelperObjectType);
		CommandPtr commandPtr = CommandPtr{ new Command(m_maskOperatorID, ObjectInfo) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmdShapeHelper(m_InspectionID, commandPtr);
		HRESULT Result = cmdShapeHelper.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == Result)
		{
			m_ShapeMaskHelperID = commandPtr->GetInstanceID();
		}
	}

	// must call init before calling this method
	const GUID& MaskController::GetInstanceID() const
	{
		return m_maskOperatorID;
	}

	// must call init before calling this method
	const GUID& MaskController::GetShapeMaskHelperID() const
	{
		return m_ShapeMaskHelperID;
	}

	IPictureDisp* MaskController::GetReferenceImage() const
	{
		return m_MaskImageController.GetImage(SvDef::ReferenceImageName);
	}

	IPictureDisp* MaskController::GetMaskImage() const
	{
		return m_MaskImageController.GetImage(SvDef::MaskImageName);
	}

	IPictureDisp* MaskController::GetResultImage() const
	{
		const SvUl::NameGuidList& rImageList = m_TaskImageController.GetResultImages();
		SvUl::NameGuidList::const_iterator it = rImageList.begin();
		if (it != rImageList.end())
		{
			return m_TaskImageController.GetImage(it->second.ToGUID());
		}
		return nullptr;
	}

	HRESULT MaskController::ImportMask(const std::string& filename)
	{
		typedef SvCmd::ImportMask Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr{ new Command(m_maskOperatorID, filename) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	HRESULT MaskController::ExportMask(const std::string& filename)
	{
		typedef SvCmd::ExportMask Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr{ new Command(m_maskOperatorID, filename) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	HGLOBAL MaskController::GetMaskData() const
	{
		typedef SvCmd::GetMaskData Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr{ new Command(m_maskOperatorID) };
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
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr{ new Command(m_maskOperatorID, hGlobal) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return (S_OK == hr) ? true : false;
	}
} //namespace SvOg
