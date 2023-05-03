//*****************************************************************************
/// \copyright (c) 2016,2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TaTableSourcePage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to select a table as source.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource\resource.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVUtilityLibrary\NameObjectIdList.h"
#pragma endregion Includes

namespace SvOg
{
	class TATableSourcePage : public CPropertyPage//, protected ImageController
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TaTableSourcePage)
		enum { IDD = IDD_TA_TABLE_SOURCE_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		TATableSourcePage(uint32_t inspectionId, uint32_t taskObjectId, int id = IDD);
		virtual ~TATableSourcePage();
#pragma endregion Constructor

#pragma region Public Methods
	public:

#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TaTableSourcePage)
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		//}}AFX_VIRTUAL

		//{{AFX_MSG(TaTableSourcePage)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeCombo1();
		//}}AFX_MSG
#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		/// Retrieve the current available list of tables
		/// \returns HRESULT
		HRESULT RetrieveAvailableList();

		/// Connect a table object to the input.
		/// \param id [in] The embedded id of the input.
		/// \param name [in] Name of the object from the available list.
		/// \returns HRESULT
		HRESULT ConnectToObject(SvPb::EmbeddedIdEnum id, const std::string& name);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvMc::AvailableObjectListComboBox<uint32_t> m_availableSourceTableCB;
		
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		SvUl::NameObjectIdList m_availableList;
#pragma endregion Member Variables
	};
} //namespace SvOg


