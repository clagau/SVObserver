//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MaskController.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "InspectionCommands\GetMaskData.h"
#include "InspectionCommands\SetMaskData.h"
#include "SVCommandLibrary\SVObjectSynchronousCommandTemplate.h"
#include "Definitions\TextDefineSvDef.h"
#pragma endregion Includes

namespace SvOg
{

	MaskController::MaskController(uint32_t inspectionID, uint32_t taskObjectID, uint32_t maskOperatorID)
	: m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	, m_maskOperatorID(maskOperatorID)
	, m_MaskImageController(inspectionID, maskOperatorID)
	, m_TaskImageController(inspectionID, taskObjectID)
	{
	}

	void MaskController::Init()
	{
		m_MaskImageController.Init();
		m_TaskImageController.Init();

		// Get Instance Id for the Mask Operator...
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
		pRequest->set_ownerid(m_maskOperatorID);
		pRequest->mutable_infostruct()->set_objecttype(SvPb::SVUnaryImageOperatorObjectType);
		pRequest->mutable_infostruct()->set_subtype(SvPb::SVShapeMaskHelperObjectType);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectidresponse())
		{
			m_ShapeMaskHelperID = responseCmd.getobjectidresponse().objectid();
		}
	}

	// must call init before calling this method
	uint32_t MaskController::GetInstanceID() const
	{
		return m_maskOperatorID;
	}

	// must call init before calling this method
	uint32_t MaskController::GetShapeMaskHelperID() const
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
		const SvUl::NameObjectIdList& rImageList = m_TaskImageController.GetResultImages();
		SvUl::NameObjectIdList::const_iterator it = rImageList.begin();
		if (it != rImageList.end())
		{
			return m_TaskImageController.GetImage(it->second);
		}
		return nullptr;
	}

	HRESULT MaskController::ImportMask(const std::string& filename)
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_importmaskrequest();
		pRequest->set_objectid(m_maskOperatorID);
		pRequest->set_filename(filename);

		return SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	}

	HRESULT MaskController::ExportMask(const std::string& filename)
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_exportmaskrequest();
		pRequest->set_objectid(m_maskOperatorID);
		pRequest->set_filename(filename);

		return SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
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
