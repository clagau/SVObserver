//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "AuxiliaryExtentsController.h"
#include "GuiCommands\AreAuxiliaryExtentsAvailable.h"
#include "GuiCommands\GetAvailableAuxSourceImages.h"
#include "GuiCommands\GetAuxSourceImage.h"
#include "GuiCommands\SetAuxSourceImage.h"
#include "GuiCommands\InspectionRunOnce.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVObjectLibrary\SVClsids.h"
#pragma endregion Includes

namespace SvOg
{
	static LPCSTR UpdateAuxiliaryExtentsTag = "UpdateAuxiliaryExtents";

	AuxiliaryExtentsController::AuxiliaryExtentsController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_ImageController(rInspectionID, rTaskObjectID)
	, m_Values(BoundValues(rInspectionID, rTaskObjectID, boost::assign::map_list_of
			(UpdateAuxiliaryExtentsTag, SVUpdateAuxiliaryExtentsObjectGuid)))
	{
	}

	HRESULT AuxiliaryExtentsController::Init()
	{
		HRESULT hr = m_Values.Init();
		if (S_OK == hr)
		{
			hr = FindAuxSourceImages();
		}
		return hr;
	}

	bool AuxiliaryExtentsController::AreAuxiliaryExtentsAvailable() const
	{
		bool bRetVal = false;

		typedef SvCmd::AreAuxiliaryExtentsAvailable Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr(new Command(m_TaskObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			bRetVal = commandPtr->AuxExtentsAvailable();
		}
		return bRetVal;
	}

	HRESULT AuxiliaryExtentsController::Commit()
	{
		return m_Values.Commit();
	}

	bool AuxiliaryExtentsController::IsUpdateAuxExtentsEnabled() const
	{
		return m_Values.Get<bool>(UpdateAuxiliaryExtentsTag);
	}

	void AuxiliaryExtentsController::EnableAuxExtents(bool bEnable)
	{
		m_Values.Set<bool>(UpdateAuxiliaryExtentsTag, bEnable);
	}

	const SvUl::NameGuidList& AuxiliaryExtentsController::GetAvailableImageList() const
	{
		return m_auxSourceImages;
	}

	HRESULT AuxiliaryExtentsController::FindAuxSourceImages()
	{
		// Need to do pTool->GetSourceImages
		typedef SvCmd::GetAvailableAuxSourceImages Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr(new Command(m_TaskObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_auxSourceImages = commandPtr->AvailableImages();
		}
		return hr;
	}

	std::string AuxiliaryExtentsController::GetAuxSourceImageName() const
	{
		SvUl::NameGuidPair source = GetAuxSourceImage();
		return source.first;
	}

	class ByName
	{
		std::string m_name;
	public:
		ByName(const std::string& rName) : m_name(rName) {}
		bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
	};

	HRESULT AuxiliaryExtentsController::SetAuxSourceImage(const std::string& rName)
	{
		HRESULT hr = E_INVALIDARG;
		SVGUID imageID;
		SvUl::NameGuidList::const_iterator it = std::find_if(m_auxSourceImages.begin(), m_auxSourceImages.end(), ByName(rName));
		if (it != m_auxSourceImages.end())
		{
			imageID = it->second;
		}
		if (!imageID.empty())
		{
			typedef SvCmd::SetAuxSourceImage Command;
			typedef SVSharedPtr<Command> CommandPtr;
			CommandPtr commandPtr(new Command(m_TaskObjectID, imageID));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (SUCCEEDED(hr))
			{
				hr = RunOnce();
			}
		}
		return hr;
	}

	SvUl::NameGuidPair AuxiliaryExtentsController::GetAuxSourceImage() const
	{
		typedef SvCmd::GetAuxSourceImage Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr(new Command(m_TaskObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			return commandPtr->SourceImage();
		}
		return SvUl::NameGuidPair();
	}
	
	HRESULT AuxiliaryExtentsController::RunOnce()
	{
		typedef SvCmd::InspectionRunOnce Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_InspectionID, m_TaskObjectID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		return hr;
	}
} //namespace SvOg