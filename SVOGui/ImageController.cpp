//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the ImageController class for use by the GUI.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/bind.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "ImageController.h"
#include "InspectionCommands\GetImage.h"
#include "InspectionCommands\GetConnectedObjects.h"
#include "InspectionCommands\ConnectToObject.h"
#include "InspectionCommands\GetResultImage.h"
#include "InspectionCommands\IsValid.h"
#include "InspectionCommands\InspectionRunOnce.h"
#include "InspectionCommands\ResetObject.h"
#include "InspectionCommands\SaveImage.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\ISVImage.h"
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
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_InspectionID, SvDef::SVObjectTypeInfoStruct(SvDef::SVImageObjectType, m_ImageSubType), m_TaskObjectID, m_OnlyAboveImages);
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
		typedef SVSharedPtr<Command> CommandPtr;
	
		CommandPtr commandPtr = new Command(m_TaskObjectID);
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
		typedef SVSharedPtr<Command> CommandPtr;

		GUID objectID = m_TaskObjectID;
		if (GUID_NULL != rInstanceID)
		{
			objectID = rInstanceID;
		}
		CommandPtr commandPtr = new Command(objectID, SvDef::SVObjectTypeInfoStruct(SvDef::SVImageObjectType, SvDef::SVNotSetSubObjectType), maxImages);
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
			return GetImage(it->second.ToGUID());
		}
		else
		{
			SvDef::StringVector::const_iterator itVector = std::find_if(m_specialImageList.begin(), m_specialImageList.end(), ByName(name));
			if (itVector != m_specialImageList.end())
			{
				typedef SvCmd::GetImage Command;
				typedef SVSharedPtr<Command> CommandPtr;

				CommandPtr commandPtr = new Command(*itVector, m_TaskObjectID);
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
		typedef SVSharedPtr<Command> CommandPtr;
	
		CommandPtr commandPtr = new Command(rImageID);
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
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			GUID objectID = m_TaskObjectID;
			if (GUID_NULL != rInstanceID)
			{
				objectID = rInstanceID;
			}
			CommandPtr commandPtr = new Command(objectID, inputName, it->second, SvDef::SVImageObjectType);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}

	HRESULT ImageController::SaveImage(const std::string& rImageName, const std::string& rFilename)
	{
		HRESULT hr = E_INVALIDARG;
		typedef SvCmd::SaveImage Command;
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(rImageName));
		if (it != m_availableList.end())
		{
			CommandPtr commandPtr = new Command(it->second, rFilename);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}

	bool ImageController::IsToolValid() const
	{
		bool bIsValid = false;
		typedef SvCmd::IsValid Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_TaskObjectID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			bIsValid = commandPtr->isValid();
		}
		return bIsValid;
	}

	HRESULT ImageController::ResetTask(SvStl::MessageContainerVector& messages) const
	{
		bool bIsValid = false;
		typedef SvCmd::ResetObject Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_TaskObjectID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			messages = commandPtr->getErrorMessages();
		}
		return hr;
	}

	HRESULT ImageController::ToolRunOnce()
	{
		typedef SvCmd::InspectionRunOnce Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_InspectionID, m_TaskObjectID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
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
} //namespace SvOg
