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
#include "InspectionCommands\GetImage.h"
#include "InspectionCommands\GetConnectedObjects.h"
#include "InspectionCommands\ConnectToObject.h"
#include "InspectionCommands\GetResultImage.h"
#include "InspectionCommands\SaveImage.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVCommandLibrary\SVObjectSynchronousCommandTemplate.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ISVImage.h"
#include "InspectionCommands\CommandFunctionHelper.h"
#pragma endregion Includes

namespace SvOg
{
	ImageController::ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SvDef::SVObjectSubTypeEnum ImageSubType, bool OnlyAboveImages)
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
		typedef SvCmd::GetAllowedImageList Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr{ new Command(m_InspectionID, SvDef::SVObjectTypeInfoStruct(SvDef::SVImageObjectType, m_ImageSubType), m_TaskObjectID, m_OnlyAboveImages) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_availableList = commandPtr->AvailableObjects();
			m_specialImageList = commandPtr->AvailableSpecialImages();
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
		SvUl::NameGuidList list;

		typedef SvCmd::GetResultImage Command;
		typedef std::shared_ptr<Command> CommandPtr;
	
		CommandPtr commandPtr{ new Command(m_TaskObjectID) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			list = commandPtr->ResultImages();
		}
		return list;
	}

	const SvUl::InputNameGuidPairList& ImageController::GetConnectedImageList(const GUID& rInstanceID, int maxImages) const
	{
		typedef SvCmd::GetConnectedObjects Command;
		typedef std::shared_ptr<Command> CommandPtr;

		GUID objectID = m_TaskObjectID;
		if (GUID_NULL != rInstanceID)
		{
			objectID = rInstanceID;
		}
		CommandPtr commandPtr{ new Command(objectID, SvDef::SVObjectTypeInfoStruct(SvDef::SVImageObjectType, SvDef::SVNotSetSubObjectType), maxImages) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_connectedList = commandPtr->ConnectedObjects();
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
			SvDef::StringVector::const_iterator itVector = std::find_if(m_specialImageList.begin(), m_specialImageList.end(), ByName(name));
			if (itVector != m_specialImageList.end())
			{
				typedef SvCmd::GetImage Command;
				typedef std::shared_ptr<Command> CommandPtr;

				CommandPtr commandPtr{ new Command(*itVector, m_TaskObjectID) };
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					m_picture = commandPtr->Image();
					rWidth = commandPtr->getWidth();
					rHeight = commandPtr->getHeight();
				}
				return m_picture;
			}
		}
		return nullptr; 
	}

	IPictureDisp* ImageController::GetImage(const GUID& rImageID) const 
	{
		long Width, Height;
		return GetImage(rImageID, Width, Height);
	}

	IPictureDisp* ImageController::GetImage(const GUID& rImageID, long& rWidth, long& rHeight) const 
	{
		typedef SvCmd::GetImage Command;
		typedef std::shared_ptr<Command> CommandPtr;
	
		CommandPtr commandPtr{ new Command(rImageID) };
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_picture = commandPtr->Image();
			rWidth = commandPtr->getWidth();
			rHeight = commandPtr->getHeight();
		}
		return m_picture;
	}

	HRESULT ImageController::ConnectToImage(const std::string& inputName, const std::string& name, const GUID& rInstanceID) const 
	{ 
		HRESULT hr = E_INVALIDARG;
		typedef SvCmd::ConnectToObject Command;
		typedef std::shared_ptr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			GUID objectID = m_TaskObjectID;
			if (GUID_NULL != rInstanceID)
			{
				objectID = rInstanceID;
			}
			CommandPtr commandPtr{ new Command(objectID, inputName, it->second, SvDef::SVImageObjectType) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}

	HRESULT ImageController::SaveImage(const std::string& rImageName, const std::string& rFilename)
	{
		HRESULT hr = E_INVALIDARG;
		typedef SvCmd::SaveImage Command;
		typedef std::shared_ptr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(rImageName));
		if (it != m_availableList.end())
		{
			CommandPtr commandPtr{ new Command(it->second, rFilename) };
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}

	bool ImageController::IsToolValid() const
	{
		bool bIsValid = false;
		SvPb::InspectionCmdMsgs Request, Response; 
		SvPb::IsValidRequest* pIsValidRequest = Request.mutable_isvalidrequest();
		
		SvPb::SetGuidInProtoBytes(pIsValidRequest->mutable_objectid(), m_TaskObjectID);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
		if (S_OK == hr && Response.has_isvalidresponse())
		{
			bIsValid = Response.isvalidresponse().isvalid();
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
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
		if (hr == S_OK && responseMessage->has_messages())
		{
			messages = SvCmd::setMessageContainerFromMessagePB(responseMessage->messages());
		}
		return hr;
	}

	HRESULT ImageController::ToolRunOnce()
	{
		return SvCmd::RunOnceSynchronous(m_InspectionID, m_TaskObjectID);
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
