//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to select the table to show in the result table view 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comutil.h>
#include "SVOResource\resource.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvOg
{
	class ResultTableSelectionDlg : public CDialog
	{
	#pragma region Declarations
	public:
		enum { IDD = IDD_RESULT_TABLE_SELECTION };

		DECLARE_DYNAMIC(ResultTableSelectionDlg)

	#pragma endregion Declarations
	
	#pragma region Constructor
	public:
		/// The class constructor
		/// \param availableList [in] List of available table objects.
		/// \param selectedItem [in] Name-string of the selected item.
		/// \param pParent [in] the parent window for the dialog
		ResultTableSelectionDlg( const SvUl::NameGuidList& availableList, const SVString& selectedItem, CWnd* pParent = nullptr );

		virtual ~ResultTableSelectionDlg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		const SVGUID& getSelectedGuid() const { return m_selectedGuid; }
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
		SVGUID m_selectedGuid;
		SvMc::AvailableObjectListComboBox m_availableTableCB;
		SvUl::NameGuidList m_availableList;
		SVString m_selectedItem;
	#pragma endregion Member variables
	};
} //namespace SvOg
