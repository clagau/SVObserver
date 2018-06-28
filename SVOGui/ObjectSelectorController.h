//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.h
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVGUID.h"
#include "SVOResource/resource.h"
#include "InspectionCommands/GetSelectorList.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOg
{
	class ObjectSelectorController
	{
#pragma region Constructor
	public:
		ObjectSelectorController(const SVGUID& rInspectionID, const SVGUID& rInstanceID = GUID_NULL);
		virtual ~ObjectSelectorController();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		// Show the object selector and return the selection. 
		/// \param rName [in,out] In: Defined which item should selected at begin. 
		///                     Out: If selector closed with OK-button, the selection will returned.
		/// \param Title [in] The title of the object selector.
		/// \param pParent [in] The parent control
		/// \param rInstanceId [in] 
		/// \param rInstanceId [in] 
		/// \returns bool True if selector was closed by OK-button.
		bool Show( std::string& rName, const std::string& rTitle, CWnd* pParent, const SVGUID& rInstanceId = GUID_NULL, SvCmd::SelectorFilterTypeEnum FilterType = SvCmd::SelectorFilterTypeEnum::AttributesAllowedFilterType);
#pragma endregion Public Methods

#pragma endregion Private Methods
		std::string GetInspectionName() const;

		std::string GetPPQName() const;

		GUID GetToolSetGUID() const;
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SVGUID m_InspectionID;
		SVGUID m_InstanceID;
#pragma region Member Variables
	};
} //namespace SvOg
