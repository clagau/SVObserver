//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.h
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVOResource/resource.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "GlobalSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#include "ObjectSelectorController.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui {
	class ObjectSelectorController
	{
#pragma region Constructor
	public:
		ObjectSelectorController(const SVGUID& rInspectionID, const SVGUID& rInstanceID = SV_GUID_NULL);
		virtual ~ObjectSelectorController();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		// Show the object selector and return the selection. 
		/// \param name [in,out] In: Defined which item should selected at begin. 
		///                     Out: If selector closed with OK-button, the selection will returned.
		/// \param Title [in] The title of the object selector.
		/// \param pParent [in] The parent control
		/// \param rInstanceId [in] 
		/// \returns bool True if selector was closed by OK-button.
		template <typename ToolsetSelector>
		bool Show(CString& name, const CString& Title, CWnd* pParent, const SVGUID& rInstanceId = SV_GUID_NULL );
#pragma endregion Public Methods

#pragma endregion Private Methods
		SVString GetInspectionName() const;

		SVString GetPPQName() const;

		GUID GetToolSetGUID() const;
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SVGUID m_InspectionID;
		SVGUID m_InstanceID;
#pragma region Member Variables
	};

}}

#pragma region Inline
#include "ObjectSelectorController.inl"
#pragma endregion Inline

namespace SvOg = Seidenader::SVOGui;