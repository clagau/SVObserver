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
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ISVImage.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

namespace SvOg
{
	ImageController::ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SvPb::SVObjectSubTypeEnum ImageSubType, bool OnlyAboveImages)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
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
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

		SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_InspectionID);
		pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pGetAvailableObjectsRequest->mutable_typeinfo()->set_subtype(m_ImageSubType);
		if (m_OnlyAboveImages)
		{
			SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_isbeforetoolmethod()->mutable_toolid(), m_TaskObjectID);
		}
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		if (S_OK == hr && response.has_getavailableobjectsresponse())
		{
			m_availableList = SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
		}

		if (S_OK == hr)
		{
			SvPb::InspectionCmdMsgs requestSpecial, responseSpecial;
			SvPb::GetSpecialImageListRequest* pGetSpecialImageListRequest = requestSpecial.mutable_getspecialimagelistrequest();

			SvPb::SetGuidInProtoBytes(pGetSpecialImageListRequest->mutable_taskobjectid(), m_TaskObjectID);
			hr = SvCmd::InspectionCommands(m_InspectionID, requestSpecial, &responseSpecial);
			if (S_OK == hr && responseSpecial.has_getspecialimagelistresponse())
			{
				m_specialImageList.clear();
				m_specialImageList.reserve(responseSpecial.getspecialimagelistresponse().specialimagenames().size());
				for (auto tmp : responseSpecial.getspecialimagelistresponse().specialimagenames())
				{
					m_specialImageList.emplace_back(tmp);
				}
			}
		}
		return hr;
	}

	const SvUl::NameGuidList& ImageController::GetAvailableImageList() const 
	{ 
		return m_availableList; 
	}

	const SvDef::StringVector& ImageController::GetSpecialImageList() const
	{
		return m_specialImageList;
	}

	SvUl::NameGuidList ImageController::GetResultImages() const
	{
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetAvailableObjectsRequest* pGetAvailableObjectsRequest = request.mutable_getavailableobjectsrequest();

		SvPb::SetGuidInProtoBytes(pGetAvailableObjectsRequest->mutable_objectid(), m_TaskObjectID);
		pGetAvailableObjectsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pGetAvailableObjectsRequest->mutable_defaultplushidden();
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		if (S_OK == hr && response.has_getavailableobjectsresponse())
		{
			return SvCmd::convertNameGuidList(response.getavailableobjectsresponse().list());
		}

		return {};
	}

	const SvUl::InputNameGuidPairList& ImageController::GetInputImageList(const GUID& rInstanceID, int maxImages) const
	{
		GUID objectID = m_TaskObjectID;
		if (GUID_NULL != rInstanceID)
		{
			objectID = rInstanceID;
		}
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetInputsRequest* pGetInputsRequest = request.mutable_getinputsrequest();
		SvPb::SetGuidInProtoBytes(pGetInputsRequest->mutable_objectid(), objectID);
		pGetInputsRequest->mutable_typeinfo()->set_objecttype(SvPb::SVImageObjectType);
		pGetInputsRequest->set_maxrequested(maxImages);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		SvUl::InputNameGuidPairList connectedList;
		if (S_OK == hr && response.has_getinputsresponse())
		{
			m_connectedList.clear();
			for (auto item : response.getinputsresponse().list())
			{
				SvUl::NameGuidPair tmp {item.objectname(), SvPb::GetGuidFromProtoBytes(item.objectid())};
				m_connectedList[item.inputname()] = tmp;
			}
		}

		return m_connectedList;
	}

	class ByName
	{
		std::string m_name;
	public:
		ByName(const std::string& rName) : m_name(rName) {}
		bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
		bool operator()(const std::string& rVal) const { return rVal == m_name; }
	};

	IPictureDisp* ImageController::GetImage(const std::string& name) const
	{
		long Width, Height;
		return GetImage(name, Width, Height);
	}

	IPictureDisp* ImageController::GetImage(const std::string& name, long& rWidth, long& rHeight) const
	{ 
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			return GetImage(it->second.ToGUID(), rWidth, rHeight);
		}
		else
		{
			IPictureDisp* idisp {nullptr};
			SvDef::StringVector::const_iterator itVector = std::find_if(m_specialImageList.begin(), m_specialImageList.end(), ByName(name));
			if (itVector != m_specialImageList.end())
			{
				SvPb::InspectionCmdMsgs request, response;
				SvPb::GetImageRequest* pGetImageRequest = request.mutable_getimagerequest();

				pGetImageRequest->set_imagename(*itVector);
				SvPb::SetGuidInProtoBytes(pGetImageRequest->mutable_parentid(), m_TaskObjectID);
				HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
				if (S_OK == hr && response.has_getimageresponse() && 0 == response.getimageresponse().messages().messages().size())
				{
					DisplayHelper::convertPBImageToIPictureDisp(response.getimageresponse().imagedata(), rWidth, rHeight, &idisp);
				}
				else
				{
					assert(false);
					SvStl::MessageContainerVector messageList;
					if (response.has_getimageresponse())
					{
						messageList = SvCmd::setMessageContainerFromMessagePB(response.getimageresponse().messages());
					}

					if (0 < messageList.size())
					{
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
						Exception.setMessage(messageList[0].getMessage());
					}
				}
			}
			return idisp;
		}
	}

	IPictureDisp* ImageController::GetImage(const GUID& rImageID) const 
	{
		long Width, Height;
		return GetImage(rImageID, Width, Height);
	}

	IPictureDisp* ImageController::GetImage(const GUID& rImageID, long& rWidth, long& rHeight) const 
	{
		IPictureDisp* idisp {nullptr};
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetImageRequest* pGetImageRequest = request.mutable_getimagerequest();

		SvPb::SetGuidInProtoBytes(pGetImageRequest->mutable_imageid(), rImageID);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		if (S_OK == hr && response.has_getimageresponse() && 0 == response.getimageresponse().messages().messages().size())
		{
			DisplayHelper::convertPBImageToIPictureDisp(response.getimageresponse().imagedata(), rWidth, rHeight, &idisp);
		}
		else
		{
			assert(false);
			SvStl::MessageContainerVector messageList;
			if (response.has_getimageresponse())
			{
				messageList = SvCmd::setMessageContainerFromMessagePB(response.getimageresponse().messages());
			}

			if (0 < messageList.size())
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(messageList[0].getMessage());
			}
		}

		return idisp;
	}

	HRESULT ImageController::ConnectToImage(const std::string& inputName, const std::string& name, const GUID& rInstanceID) const 
	{ 
		HRESULT hr = E_INVALIDARG;
		GUID objectID = m_TaskObjectID;
		if (GUID_NULL != rInstanceID)
		{
			objectID = rInstanceID;
		}
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			SvPb::InspectionCmdMsgs Request, Response;
			SvPb::ConnectToObjectRequest* pConnectToObjectRequest = Request.mutable_connecttoobjectrequest();

			SvPb::SetGuidInProtoBytes(pConnectToObjectRequest->mutable_objectid(), objectID);
			pConnectToObjectRequest->set_inputname(inputName);
			SvPb::SetGuidInProtoBytes(pConnectToObjectRequest->mutable_newconnectedid(), it->second);
			pConnectToObjectRequest->set_objecttype(SvPb::SVImageObjectType);
			hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
		}
		return hr; 
	}

	HRESULT ImageController::SaveImage(const std::string& rImageName, const std::string& rFilename)
	{
		HRESULT hr = E_INVALIDARG;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(rImageName));
		if (it != m_availableList.end())
		{
			SvPb::InspectionCmdMsgs Request, Response;
			SvPb::SaveImageRequest* pSaveImageRequest = Request.mutable_saveimagerequest();

			SvPb::SetGuidInProtoBytes(pSaveImageRequest->mutable_objectid(), it->second);
			pSaveImageRequest->set_imagename(rFilename);
			hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
		}	
		return hr; 
	}

	bool ImageController::IsToolValid() const
	{
		bool bIsValid = false;
		SvPb::InspectionCmdMsgs Request, Response; 
		SvPb::GetObjectParametersRequest* pIsValidRequest = Request.mutable_getobjectparametersrequest();
		
		SvPb::SetGuidInProtoBytes(pIsValidRequest->mutable_objectid(), m_TaskObjectID);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
		if (S_OK == hr && Response.has_getobjectparametersresponse())
		{
			bIsValid = Response.getobjectparametersresponse().isvalid();
		}
		return bIsValid;
	}

	HRESULT ImageController::ResetTask(SvStl::MessageContainerVector& messages) const
	{
		SvPb::InspectionCmdMsgs Request;
		SvPb::InspectionCmdMsgs Response;

		SvPb::ResetObjectRequest* requestMessage = Request.mutable_resetobjectrequest();
		SvPb::ResetObjectResponse* responseMessage = Response.mutable_resetobjectresponse();
		SvPb::SetGuidInProtoBytes(requestMessage->mutable_objectid(), m_TaskObjectID);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
		if (hr == S_OK && responseMessage->has_messages())
		{
			messages = SvCmd::setMessageContainerFromMessagePB(responseMessage->messages());
		}
		return hr;
	}

	HRESULT ImageController::ToolRunOnce()
	{
		return SvCmd::RunOnceSynchronous(m_InspectionID);
	}

	SvDef::SVImageTypeEnum ImageController::GetImageType(const GUID& rImageID) const
	{
		SvDef::SVImageTypeEnum Result{ SvDef::SVImageTypeEnum::SVImageTypeUnknown };
		SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*> (SvOi::getObject(rImageID));
		if (nullptr != pImage)
		{
			Result = pImage->GetImageType();
		}
		return Result;
	}

	SVGUID getFirstResultImageId(const ImageController& rController)
	{
		SVGUID retID = GUID_NULL;
		const SvUl::NameGuidList& rImageList = rController.GetResultImages();
		if (0 < rImageList.size())
		{
			retID = rImageList[0].second;
		}
		return retID;
	}
} //namespace SvOg
