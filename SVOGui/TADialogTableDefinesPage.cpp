//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// \file TADialogTableDefinesPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet to defines the table in the Table-tool.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogTableDefinesPage.h"
#include "SVObjectLibrary\SVClsids.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "SVFormulaEditorSheet.h"
#include "FormulaController.h"
#include "GuiCommands\GetAvailableObjects.h"
#include "GuiCommands\ConstructAndInsertFriend.h"
#include "GuiCommands\DestroyFriend.h"
#include "GuiCommands\SetObjectName.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui {
	static const int HeaderSize = 1;
	static const int NameColumnSize = 150;
	static const int FormulaColumnSize = 500;
	static const int NameColumn = 0;
	static const int FormulaColumn = 1;
	const TCHAR* const EquationName = _T("Table Column");

	BEGIN_MESSAGE_MAP(TADialogTableDefinesPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADialogTableDefinesPage)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

#pragma region Constructor
	TADialogTableDefinesPage::TADialogTableDefinesPage( const GUID& rInspectionID, const GUID& rTaskObjectID ) 
		: CPropertyPage(TADialogTableDefinesPage::IDD)
		, m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
	{
	}

	TADialogTableDefinesPage::~TADialogTableDefinesPage()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool TADialogTableDefinesPage::QueryAllowExit()
	{
		HRESULT hResult = ValidateData();
		return (S_OK == hResult);
	}
#pragma endregion Public Methods

#pragma region Protected Methods
	void TADialogTableDefinesPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADialogTableDefinesPage)
		DDX_Control(pDX, IDC_GRID, m_Grid);
		//}}AFX_DATA_MAP
	}

	BOOL TADialogTableDefinesPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		initGridControl();
		FillGridControl();

		UpdateData( FALSE );

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
	}

	BOOL TADialogTableDefinesPage::OnKillActive()
	{
		HRESULT hResult = ValidateData();
		if (S_OK == hResult)
		{
			return CPropertyPage::OnKillActive();
		}
		else
		{
			return FALSE;
		}
	}

	void TADialogTableDefinesPage::OnBnClickedButtonRemove()
	{
		SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
		for( int i = Selection.GetMinRow(); i <= Selection.GetMaxRow(); i++ )
		{
			for( int j = Selection.GetMinCol(); j <= Selection.GetMaxCol(); j++ )
			{
				if(m_Grid.IsCellSelected( i, j ) )
				{
					typedef GuiCmd::DestroyFriend Command;
					typedef SVSharedPtr<Command> CommandPtr;
					CommandPtr commandPtr = new Command(m_TaskObjectID, m_gridList[i-1].second, GuiCmd::DestroyFriend::Flag_ResetInspection);
					SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
					HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					if (S_OK != hr)
					{
						SVStringArray msgList;
						msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
						SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_TableColumn_RemovingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
					}
					break;
				}
			}
		}
		FillGridControl();
	}

	void TADialogTableDefinesPage::OnBnClickedButtonAdd()
	{
		int number = static_cast<int>(m_gridList.size()+1);
		SVString name = SvUl_SF::Format(_T("%s %d"), EquationName, number);

		//search for unique name until one is found
		while ( !isTableNameUnique(name) )
		{
			number++;
			name = SvUl_SF::Format(_T("%s %d"), EquationName, number);
		}
		// Construct and Create the Filter Class Object
		typedef GuiCmd::ConstructAndInsertFriend Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_TaskObjectID, TableColumnEquationGuid, name.c_str());
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			SVStringArray msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_TableColumn_AddingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
		FillGridControl();
	}

	void TADialogTableDefinesPage::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		if (FormulaColumn == pItem->iColumn && 0 < pItem->iRow && m_gridList.size() >= pItem->iRow)
		{
			SVString strCaption = SvUl_SF::Format(_T("%s %s"), m_gridList[pItem->iRow-1].first.c_str(), _T("Formula"));

			SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, m_gridList[pItem->iRow-1].second.ToGUID(), strCaption.c_str());
			dlg.DoModal();
			FillGridControl();
		}
	}

	void TADialogTableDefinesPage::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		bool bAcceptChange = true;

		if (NameColumn == pItem->iColumn && 0 < pItem->iRow && m_gridList.size() >= pItem->iRow)
		{
			SVString newName = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			if ( isTableNameUnique(newName) )
			{
				typedef GuiCmd::SetObjectName Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr = new Command(m_gridList[pItem->iRow-1].second.ToGUID(), newName.c_str());
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK != hr)
				{
					bAcceptChange = false;
					SVStringArray msgList;
					msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_TableColumn_RenamingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
				else
				{
					m_gridList[pItem->iRow-1].first = newName;
				}
			}
			else
			{
				bAcceptChange = false;
				SVStringArray msgList;
				msgList.push_back(newName);
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_TableColumnName_NotUnique, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}

		*pResult = (bAcceptChange)? 0 : -1;
	}
#pragma endregion Protected Methods

#pragma region Private Methods
	HRESULT TADialogTableDefinesPage::ValidateData()
	{
		UpdateData(TRUE);
		HRESULT hResult = S_OK;
		return hResult;
	}

	void TADialogTableDefinesPage::initGridControl()
	{
		m_Grid.SetFixedRowCount( HeaderSize );
		m_Grid.SetFixedColumnCount( 0 );
		m_Grid.SetRowResize( FALSE );
		m_Grid.SetColumnResize( FALSE );
		m_Grid.AllowReorderColumn( false );
		m_Grid.EnableDragAndDrop( FALSE );
		m_Grid.SetEditable( true );
		m_Grid.SetColumnCount( 2 );
		m_Grid.SetColumnWidth( NameColumn, NameColumnSize );
		m_Grid.SetColumnWidth( FormulaColumn, FormulaColumnSize );
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.col = NameColumn;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
		Item.strText = "Name";
		m_Grid.SetItem( &Item );

		Item.col = FormulaColumn;
		Item.strText = "Formula";
		m_Grid.SetItem( &Item );
	}

	void TADialogTableDefinesPage::FillGridControl()
	{
		typedef GuiCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;
		m_gridList.clear();
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType, TableColumnEquationObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_gridList = commandPtr->AvailableObjects();
			m_Grid.SetRowCount( HeaderSize + static_cast<int>(m_gridList.size()) );
			SvGcl::GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
			Item.crBkClr = CLR_DEFAULT;
			Item.nFormat = DT_CENTER | DT_VCENTER;
			for(int i=0; i<m_gridList.size(); i++)
			{
				Item.row = i+HeaderSize;
				Item.col = NameColumn;
				Item.strText = m_gridList[i].first.c_str();
				m_Grid.SetItem( &Item );

				Item.col = FormulaColumn;
				FormulaController formulaController(m_InspectionID, m_TaskObjectID, m_gridList[i].second.ToGUID());
				Item.strText = formulaController.GetEquationText().c_str();
				m_Grid.SetItem( &Item );
				m_Grid.SetItemState(Item.row, FormulaColumn, m_Grid.GetItemState(Item.row,FormulaColumn) | GVIS_READONLY);
			}
		}
		m_Grid.Refresh();
	}

	bool TADialogTableDefinesPage::isTableNameUnique(const SVString& name)
	{
		bool bResult = true;
		for(int i=0; i<m_gridList.size(); i++)
		{
			if (m_gridList[i].first == name)
			{
				bResult = false;
			}
		}
		return bResult;
	}
#pragma endregion Private Mehods
}}