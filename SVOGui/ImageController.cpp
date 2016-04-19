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
#include "GuiCommands\GetImage.h"
#include "GuiCommands\GetAllowedImageList.h"
#include "GuiCommands\GetConnectedObjects.h"
#include "GuiCommands\ConnectToObject.h"
#include "GuiCommands\GetResultImage.h"
#include "GuiCommands\IsValid.h"
#include "GuiCommands\InspectionRunOnce.h"
#include "GuiCommands\GetObjectTypeInfo.h"
#include "GuiCommands\SaveImage.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\IToolSet.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "GuiController.h"
#include "BoundValue.h"
#include "ValuesAccessor.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
{
	ImageController::ImageController(const GUID& rInspectionID, const GUID& rTaskObjectID, SVObjectSubTypeEnum subType, bool bAllowColor)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_subType(subType)
	, m_bAllowColor(bAllowColor)
	{
	}

	HRESULT ImageController::Init()
	{
		return RetrieveAvailableImageList();
	}

	HRESULT ImageController::RetrieveAvailableImageList()
	{ 
		typedef GuiCmd::GetAllowedImageList Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_InspectionID, SVObjectTypeInfoStruct(SVImageObjectType, SVNotSetSubObjectType), m_TaskObjectID, m_subType, m_bAllowColor);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_availableList = commandPtr->AvailableObjects();
		}
		return hr;
	}

	const SvUl::NameGuidList& ImageController::GetAvailableImageList() const 
	{ 
		return m_availableList; 
	}

	SvUl::NameGuidList ImageController::GetResultImages() const
	{
		SvUl::NameGuidList list;

		typedef GuiCmd::GetResultImage Command;
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
		typedef GuiCmd::GetConnectedObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		GUID objectID = m_TaskObjectID;
		if (GUID_NULL != rInstanceID)
		{
			objectID = rInstanceID;
		}
		CommandPtr commandPtr = new Command(objectID, SVObjectTypeInfoStruct(SVImageObjectType, SVNotSetSubObjectType), maxImages);
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
		SVString m_name;
	public:
		ByName(const SVString& rName) : m_name(rName) {}
		bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
	};

	IPictureDisp* ImageController::GetImage(const SVString& name) const 
	{ 
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			return GetImage(it->second.ToGUID());
		}
		return nullptr; 
	}

	IPictureDisp* ImageController::GetImage(const GUID& imageID) const 
	{
		typedef GuiCmd::GetImage Command;
		typedef SVSharedPtr<Command> CommandPtr;
	
		CommandPtr commandPtr = new Command(imageID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_picture = commandPtr->Image();
		}
		return m_picture;
	}

	HRESULT ImageController::ConnectToImage(const SVString& inputName, const SVString& name, const GUID& rInstanceID) const 
	{ 
		HRESULT hr = E_INVALIDARG;
		typedef GuiCmd::ConnectToObject Command;
		typedef SVSharedPtr<Command> CommandPtr;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_availableList.begin(), m_availableList.end(), ByName(name));
		if (it != m_availableList.end())
		{
			GUID objectID = m_TaskObjectID;
			if (GUID_NULL != rInstanceID)
			{
				objectID = rInstanceID;
			}
			CommandPtr commandPtr = new Command(objectID, inputName, it->second, SVImageObjectType);
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		}	
		return hr; 
	}

	HRESULT ImageController::SaveImage(const SVString& rImageName, const SVString& rFilename)
	{
		HRESULT hr = E_INVALIDARG;
		typedef GuiCmd::SaveImage Command;
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
		typedef GuiCmd::IsValid Command;
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

	HRESULT ImageController::ToolRunOnce()
	{
		typedef GuiCmd::InspectionRunOnce Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_InspectionID, m_TaskObjectID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}

	SVObjectTypeInfoStruct ImageController::GetImageTypeInfo(const GUID& imageID) const
	{
		typedef GuiCmd::GetObjectTypeInfo Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(imageID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			return commandPtr->GetTypeInfo();
		}
		return SVObjectTypeInfoStruct();
	}
} /* namespace SVOGui */ } /* namespace Seidenader */