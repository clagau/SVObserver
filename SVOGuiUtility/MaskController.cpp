//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MaskController.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "InspectionCommands\GetMaskData.h"
#include "InspectionCommands\SetMaskData.h"
#include "Definitions\TextDefineSvDef.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#pragma endregion Includes

namespace SvOgu
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

	RECT MaskController::GetResultRect() const
	{
		const SvUl::NameObjectIdList& rImageList = m_TaskImageController.GetResultImages();
		SvUl::NameObjectIdList::const_iterator it = rImageList.begin();
		if (it != rImageList.end())
		{
			SvPb::InspectionCmdRequest requestCmd;
			SvPb::InspectionCmdResponse responseCmd;
			auto* pRequest = requestCmd.mutable_getoutputrectanglerequest();
			pRequest->set_imageid(it->second);
			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getoutputrectangleresponse())
			{
				const auto& values = responseCmd.getoutputrectangleresponse();
				return { values.left(), values.top(), values.right(), values.bottom() };
			}
		}
		return {};
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
		HGLOBAL ret{ nullptr };
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>((SvOi::getObject(m_InspectionID)));
		if (nullptr == pInspection)
		{
			return ret;
		}
		
		SvCmd::GetMaskDataFunctor getMaskDataFunctor(m_maskOperatorID);
		std::packaged_task< HGLOBAL()> getMaskDataTask(std::move(getMaskDataFunctor));
		std::future<HGLOBAL>  getMaskDataFuture = getMaskDataTask.get_future();
		SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<std::packaged_task<HGLOBAL()>>>(std::move(getMaskDataTask));
		pInspection->SubmitCommand(pCommand);
		std::future_status status = getMaskDataFuture.wait_for(std::chrono::seconds{ 120});
		try
		{
			if (status == std::future_status::ready)
			{
				ret = getMaskDataFuture.get();
			}
		}
		catch (...)
		{
			ret = nullptr;
		}
		return ret;
		
	}

	bool MaskController::SetMaskData(HGLOBAL hGlobal)
	{
		bool ret{ false };
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>((SvOi::getObject(m_InspectionID)));
		if (nullptr == pInspection)
		{
			return ret;
		}
		SvCmd::SetMaskDataFunctor setMaskDataFunctor(m_maskOperatorID, hGlobal);
		std::packaged_task< HRESULT()> setMaskDataTask(std::move(setMaskDataFunctor));
		std::future<HRESULT>  setMaskDataFuture = setMaskDataTask.get_future();

		SvOi::ICommandPtr pCommand = std::make_shared<  SvOi::CTaskWrapper<std::packaged_task<HRESULT()>>>(std::move(setMaskDataTask));
		pInspection->SubmitCommand(pCommand);
		std::future_status status = setMaskDataFuture.wait_for(std::chrono::seconds{ 120});
		try
		{
			if (status == std::future_status::ready && S_OK == setMaskDataFuture.get())
			{
				ret = true;
			}
		}
		catch (...)
		{
			ret = false;
		}
		return ret;

		
	}
} //namespace SvOgu
