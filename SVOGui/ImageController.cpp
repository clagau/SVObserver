//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the ImageController class for use by the GUI.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/bind.hpp>
#include "ImageController.h"
#include "DisplayHelper.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

namespace SvOg
{
	ImageController::ImageController(uint32_t inspectionID, uint32_t taskObjectID, SvPb::SVObjectSubTypeEnum ImageSubType, bool OnlyAboveImages)
	: m_InspectionID(inspectionID)
	, m_TaskObjectID(taskObjectID)
	, m_ImageSubType(ImageSubType)
	, m_OnlyAboveImages(OnlyAboveImages)
	{
	}

	HRESULT ImageController::Init()
	{
		return RetrieveAvailableImageList();
	}

	HRESULT ImageController::RetrieveAvailableImageList()
	{ 
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
		pRequest->set_objectid(m_InspectionID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pRequest->mutable_typeinfo()->set_subtype(m_ImageSubType);
		if (m_OnlyAboveImages)
		{
			pRequest->mutable_isbeforetoolmethod()->set_toolid(m_TaskObjectID);
		}

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			m_availableList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		}

		if (S_OK == hr)
		{
			requestCmd.Clear();
			responseCmd.Clear();
			auto* pGetSpecialImageListRequest = requestCmd.mutable_getspecialimagelistrequest();
			pGetSpecialImageListRequest->set_taskobjectid(m_TaskObjectID);

			hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getspecialimagelistresponse())
			{
				m_specialImageList.clear();
				m_specialImageList.reserve(responseCmd.getspecialimagelistresponse().specialimagenames().size());
				for (auto tmp : responseCmd.getspecialimagelistresponse().specialimagenames())
				{
					m_specialImageList.emplace_back(tmp);
				}
			}
		}
		return hr;
	}

	const SvUl::NameObjectIdList& ImageController::GetAvailableImageList() const 
	{ 
		return m_availableList; 
	}

	const SvDef::StringVector& ImageController::GetSpecialImageList() const
	{
		return m_specialImageList;
	}

	SvUl::NameObjectIdList ImageController::GetResultImages() const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pRequest->mutable_defaultplushidden();

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
		{
			return SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());
		}

		return {};
	}

	const SvUl::InputNameObjectIdPairList& ImageController::GetInputImageList(uint32_t instanceID, size_t maxImages) const
	{
		uint32_t objectID = m_TaskObjectID;
		if (SvDef::InvalidObjectId != instanceID)
		{
			objectID = instanceID;
		}
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getinputsrequest();
		pRequest->set_objectid(objectID);
		pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pRequest->set_maxrequested(static_cast<int32_t>(maxImages));
		pRequest->set_shouldexcludefirstobjectname(true); //used only for LinkedValue-names
		pRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType); //used only for LinkedValue-names

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getinputsresponse())
		{
			m_connectedList.clear();
			for (auto item : responseCmd.getinputsresponse().list())
			{
				SvUl::NameObjectIdPair tmp {item.objectname(), item.objectid()};
				m_connectedList[item.inputname()] = tmp;
			}
		}

		return m_connectedList;
	}

	class ByName
	{
		std::string m_name;
	public:
		explicit ByName(const std::string& rName) : m_name(rName) {}
		bool operator()(const SvUl::NameObjectIdPair& rVal) const { return rVal.first == m_name; }
		bool operator()(const std::string& rVal) const { return rVal == m_name; }
	};

	IPictureDisp* ImageController::GetImage(const std::string& name) const
	{
		long Width, Height;
		return GetImage(name, Width, Height);
	}

	IPictureDisp* ImageController::GetImage(const std::string& name, long& rWidth, long& rHeight) const
	{ 
		SvUl::NameObjectIdList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			return GetImage(it->second, rWidth, rHeight);
		}
		else
		{
			IPictureDisp* idisp {nullptr};
			SvDef::StringVector::const_iterator itVector = std::find_if(m_specialImageList.begin(), m_specialImageList.end(), ByName(name));
			if (itVector != m_specialImageList.end())
			{
				SvPb::InspectionCmdRequest requestCmd;
				SvPb::InspectionCmdResponse responseCmd;
				auto* pRequest = requestCmd.mutable_getimagerequest();
				pRequest->set_imagename(*itVector);
				pRequest->set_parentid(m_TaskObjectID);

				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
				if (S_OK == hr && responseCmd.has_getimageresponse() && 0 == responseCmd.getimageresponse().messages().messages().size())
				{
					DisplayHelper::convertPBImageToIPictureDisp(responseCmd.getimageresponse().imagedata(), rWidth, rHeight, &idisp);
				}
				else
				{
					assert(false);
					SvStl::MessageContainerVector messageList;
					if (responseCmd.has_getimageresponse())
					{
						messageList = SvPb::setMessageVectorFromMessagePB(responseCmd.getimageresponse().messages());
					}

					if (0 < messageList.size())
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Log);
						Exception.setMessage(messageList[0].getMessage());
					}
				}
			}
			return idisp;
		}
	}

	IPictureDisp* ImageController::GetImage(uint32_t imageID) const
	{
		long Width, Height;
		return GetImage(imageID, Width, Height);
	}

	IPictureDisp* ImageController::GetImage(uint32_t imageID, long& rWidth, long& rHeight) const
	{
		IPictureDisp* idisp {nullptr};
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getimagerequest();
		pRequest->set_imageid(imageID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getimageresponse() && 0 == responseCmd.getimageresponse().messages().messages().size())
		{
			DisplayHelper::convertPBImageToIPictureDisp(responseCmd.getimageresponse().imagedata(), rWidth, rHeight, &idisp);
		}
		else
		{
			assert(false);
			SvStl::MessageContainerVector messageList;
			if (responseCmd.has_getimageresponse())
			{
				messageList = SvPb::setMessageVectorFromMessagePB(responseCmd.getimageresponse().messages());
			}

			if (0 < messageList.size())
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(messageList[0].getMessage());
			}
		}

		return idisp;
	}

	HRESULT ImageController::ConnectToImage(const std::string& inputName, const std::string& name, uint32_t instanceID) const
	{ 
		HRESULT hr = E_INVALIDARG;
		uint32_t objectID = m_TaskObjectID;
		if (SvDef::InvalidObjectId != instanceID)
		{
			objectID = instanceID;
		}
		SvUl::NameObjectIdList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			SvPb::InspectionCmdRequest requestCmd;
			auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
			pRequest->set_objectid( objectID);
			pRequest->set_inputname(inputName);
			pRequest->set_newconnectedid(it->second);
			pRequest->set_objecttype(SvPb::SVImageObjectType);

			hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		}
		return hr; 
	}

	HRESULT ImageController::SaveImage(const std::string& rImageName, const std::string& rFilename)
	{
		HRESULT hr = E_INVALIDARG;
		SvUl::NameObjectIdList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(rImageName));
		if (it != m_availableList.end())
		{
			SvPb::InspectionCmdRequest requestCmd;
			auto* pRequest = requestCmd.mutable_saveimagerequest();
			pRequest->set_objectid(it->second);
			pRequest->set_imagename(rFilename);

			hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		}	
		return hr; 
	}

	bool ImageController::IsToolValid() const
	{
		bool bIsValid = false;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
		pRequest->set_objectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			bIsValid = responseCmd.getobjectparametersresponse().isvalid();
		}
		return bIsValid;
	}

	HRESULT ImageController::ResetTask(SvStl::MessageContainerVector& messages) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_resetobjectrequest();
		pRequest->set_objectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (hr == S_OK && responseCmd.standardresponse().has_errormessages())
		{
			messages = SvPb::setMessageVectorFromMessagePB(responseCmd.standardresponse().errormessages());
		}
		return hr;
	}

	HRESULT ImageController::ToolRunOnce()
	{
		return SvCmd::RunOnceSynchronous(m_InspectionID);
	}

	SvPb::SVImageTypeEnum ImageController::GetImageType(uint32_t imageID) const
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getimageinforequest();
		pRequest->set_imageid(imageID);

		SvPb::SVImageTypeEnum Result{ SvPb::SVImageTypeEnum::SVImageTypeUnknown };

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (hr == S_OK && responseCmd.has_getimageinforesponse())
		{
			Result = responseCmd.getimageinforesponse().imagetype();
		}
		
		return Result;
	}

	uint32_t getFirstResultImageId(const ImageController& rController)
	{
		uint32_t retID = SvDef::InvalidObjectId;
		const SvUl::NameObjectIdList& rImageList = rController.GetResultImages();
		if (0 < rImageList.size())
		{
			retID = rImageList[0].second;
		}
		return retID;
	}
} //namespace SvOg
