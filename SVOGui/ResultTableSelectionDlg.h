//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to select the table to show in the result table view 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comutil.h>
#include "SVOResource\resource.h"
#include "SVUtilityLibrary\NameObjectIdList.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvOg
{
	class ResultTableSelectionDlg : public CDialog
	{
	#pragma region Declarations
	public:
		enum { IDD = IDD_RESULT_TABLE_SELECTION };

	#pragma endregion Declarations
	
	#pragma region Constructor
	public:
		/// The class constructor
		/// \param availableList [in] List of available table objects.
		/// \param selectedItem [in] Name-string of the selected item.
		/// \param pParent [in] the parent window for the dialog
		ResultTableSelectionDlg( const SvUl::NameObjectIdList& availableList, const std::string& selectedItem, CWnd* pParent = nullptr );

		virtual ~ResultTableSelectionDlg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		uint32_t getSelectedId() const { return m_selectedId; }
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
	#pragma region Private Methods

	#pragma region Member variables
	private:
		uint32_t m_selectedId = SvDef::InvalidObjectId;
		SvMc::AvailableObjectListComboBox<uint32_t> m_availableTableCB;
		SvUl::NameObjectIdList m_availableList;
		std::string m_selectedItem;
	#pragma endregion Member variables
	};
} //namespace SvOg
