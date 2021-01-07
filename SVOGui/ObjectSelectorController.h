//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.h
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvOg
{
	class ObjectSelectorController
	{
#pragma region Constructor
	public:
		explicit ObjectSelectorController(uint32_t inspectionID, uint32_t instanceID = SvDef::InvalidObjectId, SvPb::ObjectAttributes objectattributes = SvPb::selectableForEquation);
		virtual ~ObjectSelectorController();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		// Show the object selector and return the selection. 
		/// \param rName [in,out] In: Defined which item should selected at begin. 
		///                     Out: If selector closed with OK-button, the selection will returned.
		/// \param Title [in] The title of the object selector.
		/// \param pParent [in] The parent control
		/// \returns bool True if selector was closed by OK-button.
		bool Show(std::string& rName, const std::string& rTitle, CWnd* pParent, SvPb::ObjectSelectorType type = SvPb::ObjectSelectorType::allValueObjects, SvPb::GetObjectSelectorItemsRequest::FilterCase filter = SvPb::GetObjectSelectorItemsRequest::FilterCase::kAttributesAllowed, uint32_t importantObjectForStopAtClosed = SvDef::InvalidObjectId);
		bool Show(std::string& rName, const std::string& rTitle, CWnd* pParent, SvPb::ObjectSelectorType type, const std::vector<uint32_t>& excludeSameLineageVector, uint32_t importantObjectForStopAtClosed = SvDef::InvalidObjectId);
		void setSearchAreas(const std::vector<SvPb::SearchArea>& rSearchAreas) { m_searchAreas = rSearchAreas; }
#pragma endregion Public Methods

#pragma endregion Private Methods
		bool Show(std::string& rName, const std::string& rTitle, CWnd* pParent, const SvPb::InspectionCmdRequest& rRequestCmd);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		std::vector<SvPb::SearchArea> m_searchAreas;
		SvPb::ObjectAttributes m_objectAttributes;
		uint32_t m_InspectionID;
		uint32_t m_InstanceID;
#pragma region Member Variables
	};
} //namespace SvOg
