//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to select the table to show in the result table view 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ResultTableSelectionDlg.h"
#include "TextDefinesSvOg.h"
#pragma endregion Includes

namespace SvOg
{
	#pragma region Declarations
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif

	BEGIN_MESSAGE_MAP(ResultTableSelectionDlg, CDialog)
	END_MESSAGE_MAP()

	IMPLEMENT_DYNAMIC(ResultTableSelectionDlg, CDialog)
	#pragma endregion Declarations

	#pragma region Constructor
	ResultTableSelectionDlg::ResultTableSelectionDlg( const SvUl::NameObjectIdList& availableList, const std::string& selectedItem, CWnd* pParent /*nullptr*/ )
	  : CDialog(ResultTableSelectionDlg::IDD, pParent)
		, m_availableList(availableList)
		, m_selectedItem(selectedItem)
	{

	}

	ResultTableSelectionDlg::~ResultTableSelectionDlg()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods

	#pragma endregion Public Methods

	#pragma region Private Methods
	void ResultTableSelectionDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_COLUMN_SELECT_CBOX, m_availableTableCB);
	}

	BOOL ResultTableSelectionDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_availableTableCB.Init(m_availableList, m_selectedItem, SvOg::Table_NoAvailable, SvOg::Table_NoSelected);

		UpdateData( false );
		return TRUE;
	}

	void ResultTableSelectionDlg::OnOK()
	{
		if( UpdateData( true ) )
		{
			m_selectedId = m_availableTableCB.getSelectedValue();
			CDialog::OnOK();
		}
	}
	#pragma endregion Private Methods
} //namespace SvOg