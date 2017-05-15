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
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "GuiCommands\GetErrorMessageList.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg {
	static const int cHeaderSize = 1;
	static const int cNameColumnSize = 150;
	static const int cFormulaColumnSize = 500;
	static const int cNameColumn = 0;
	static const int cFormulaColumn = 1;
	static const TCHAR* const cEquationName = _T("Table Column");
	static const TCHAR* const cHeaderName_NameColumn = _T("Column Name");
	static const TCHAR* const cHeaderName_FormulaColumn = _T("Formula");

	BEGIN_MESSAGE_MAP(TADialogTableDefinesPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADialogTableDefinesPage)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
		ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
		ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
		ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
		ON_COMMAND( ID_ADD_COLUMN, OnBnClickedButtonAdd )
		ON_COMMAND( ID_REMOVE_COLUMNS, OnBnClickedButtonRemove )
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
		ValidateData();
		return true;
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
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL TADialogTableDefinesPage::OnKillActive()
	{
		ValidateData();
		return CPropertyPage::OnKillActive();
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
					typedef SvCmd::DestroyFriend Command;
					typedef SVSharedPtr<Command> CommandPtr;
					CommandPtr commandPtr = new Command(m_TaskObjectID, m_gridList[i-1].second, SvCmd::DestroyFriend::Flag_ResetInspection);
					SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
					HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					if (S_OK != hr)
					{
						SVStringVector msgList;
						msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_RemovingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
					}
					break;
				}
			}
		}
		FillGridControl();
	}

	void TADialogTableDefinesPage::OnBnClickedButtonAdd()
	{
		SVGUID addPreGuid = SV_GUID_NULL;
		SvGcl::CCellRange Selection = m_Grid.GetSelectedCellRange();
		if ( Selection.GetMinRow() == Selection.GetMaxRow() && 0 < Selection.GetMinRow() )
		{
			addPreGuid = m_gridList[Selection.GetMinRow()-1].second;
		}
		int number = static_cast<int>(m_gridList.size()+1);
		SVString name = SvUl_SF::Format(_T("%s %d"), cEquationName, number);

		//search for unique name until one is found
		while ( !isTableNameUnique(name) )
		{
			number++;
			name = SvUl_SF::Format(_T("%s %d"), cEquationName, number);
		}
		// Construct and Create the Filter Class Object
		typedef SvCmd::ConstructAndInsertFriend Command;
		typedef SVSharedPtr<Command> CommandPtr;
		CommandPtr commandPtr = new Command(m_TaskObjectID, TableColumnEquationGuid, name.c_str(), addPreGuid);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			SVStringVector msgList;
			msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_AddingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
		ValidateData(); //validate the new line (this does a reset and add the column to the tableObject)
		FillGridControl();
	}

	void TADialogTableDefinesPage::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		if (cFormulaColumn == pItem->iColumn && 0 < pItem->iRow && m_gridList.size() >= pItem->iRow)
		{
			ValidateData(); //validate the new line (this does a reset and add the column to the tableObject)
			SVString strCaption = SvUl_SF::Format(_T("%s %s"), m_gridList[pItem->iRow-1].first.c_str(), _T("Formula"));

			SvOg::SVFormulaEditorSheetClass dlg(m_InspectionID, m_TaskObjectID, m_gridList[pItem->iRow-1].second.ToGUID(), strCaption.c_str());
			dlg.DoModal();
			FillGridControl();
		}
	}

	void TADialogTableDefinesPage::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;

		CPoint p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(&p);
			showContextMenu( p );
		}
	}

	void TADialogTableDefinesPage::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*) pNotifyStruct;
		bool bAcceptChange = true;

		if (cNameColumn == pItem->iColumn && 0 < pItem->iRow && m_gridList.size() >= pItem->iRow)
		{
			SVString newName = m_Grid.GetCell(pItem->iRow, pItem->iColumn)->GetText();
			if (!newName.empty())
			{
				if ( isTableNameUnique(newName) )
				{
					typedef SvCmd::SetObjectName Command;
					typedef SVSharedPtr<Command> CommandPtr;
					CommandPtr commandPtr = new Command(m_gridList[pItem->iRow-1].second.ToGUID(), newName.c_str());
					SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
					HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					if (S_OK != hr)
					{
						bAcceptChange = false;
						SVStringVector msgList;
						msgList.push_back(SvUl_SF::Format(_T("%d"), hr));
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumn_RenamingFailed, msgList, SvStl::SourceFileParams(StdMessageParams) );
					}
					else
					{
						m_gridList[pItem->iRow-1].first = newName;
					}
				}
				else
				{
					bAcceptChange = false;
					SVStringVector msgList;
					msgList.push_back(newName);
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_NotUnique, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}
			else
			{
				bAcceptChange = false;
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_TableColumnName_Empty, SvStl::SourceFileParams(StdMessageParams) );
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
		// Do a reset of the Tool
		typedef SVSharedPtr<SvCmd::ResetObject> ResetObjectCommandPtr;
		ResetObjectCommandPtr commandPtr(new SvCmd::ResetObject(m_InspectionID));
		SVObjectSynchronousCommandTemplate<ResetObjectCommandPtr> cmd(m_InspectionID, commandPtr);

		hResult = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hResult)
		{
			typedef SVSharedPtr<SvCmd::GetErrorMessageList> GetErrorMessageListCommandPtr;
			GetErrorMessageListCommandPtr errorCommandPtr(new SvCmd::GetErrorMessageList(m_TaskObjectID));
			SVObjectSynchronousCommandTemplate<GetErrorMessageListCommandPtr> errorCmd(m_InspectionID, errorCommandPtr);
			errorCmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			SvStl::MessageContainerVector errorMessageList = errorCommandPtr->GetMessageList();
			if (0 < errorMessageList.size())
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( errorMessageList[0].getMessage() );
			}
		}
		return hResult;
	}

	void TADialogTableDefinesPage::initGridControl()
	{
		m_Grid.SetFixedRowCount( cHeaderSize );
		m_Grid.SetFixedColumnCount( 0 );
		m_Grid.SetRowResize( FALSE );
		m_Grid.SetColumnResize( FALSE );
		m_Grid.AllowReorderColumn( false );
		m_Grid.EnableDragAndDrop( FALSE );
		m_Grid.SetEditable( true );
		m_Grid.SetColumnCount( 2 );
		m_Grid.SetColumnWidth( cNameColumn, cNameColumnSize );
		m_Grid.SetColumnWidth( cFormulaColumn, cFormulaColumnSize );
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
		Item.row = 0;
		Item.col = cNameColumn;
		Item.crBkClr = CLR_DEFAULT;
		Item.nFormat = DT_LEFT | DT_VCENTER | DT_WORDBREAK;
		Item.strText = cHeaderName_NameColumn;
		m_Grid.SetItem( &Item );

		Item.col = cFormulaColumn;
		Item.strText = cHeaderName_FormulaColumn;
		m_Grid.SetItem( &Item );
	}

	void TADialogTableDefinesPage::FillGridControl()
	{
		typedef SvCmd::GetAvailableObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;
		m_gridList.clear();
		CommandPtr commandPtr = new Command(m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType, TableColumnEquationObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_gridList = commandPtr->AvailableObjects();
			m_Grid.SetRowCount( cHeaderSize + static_cast<int>(m_gridList.size()) );
			SvGcl::GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_BKCLR;
			Item.crBkClr = CLR_DEFAULT;
			Item.nFormat = DT_LEFT | DT_VCENTER;
			for(int i=0; i<m_gridList.size(); i++)
			{
				Item.row = i+cHeaderSize;
				Item.col = cNameColumn;
				Item.strText = m_gridList[i].first.c_str();
				m_Grid.SetItem( &Item );

				Item.col = cFormulaColumn;
				FormulaController formulaController(m_InspectionID, m_TaskObjectID, m_gridList[i].second.ToGUID());
				Item.strText = formulaController.GetEquationText().c_str();
				m_Grid.SetItem( &Item );
				m_Grid.SetItemState(Item.row, cFormulaColumn, m_Grid.GetItemState(Item.row,cFormulaColumn) | GVIS_READONLY);
			}
		}
		m_Grid.Refresh();
		if (c_maxTableColumn > m_gridList.size())
		{
			GetDlgItem( IDC_BUTTON_ADD )->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem( IDC_BUTTON_ADD )->EnableWindow(FALSE);
		}
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

	void TADialogTableDefinesPage::showContextMenu( CPoint point )
	{
		CMenu menu;
		CMenu* pPopupMenu;

		if( menu.LoadMenu( IDR_TABLE_DEFINE_MOUSE_MENU ) )
		{
			if( pPopupMenu = menu.GetSubMenu( 0 ) )
			{
				ClientToScreen( &point );
				pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
			}
		}
	}
#pragma endregion Private Mehods
} //namespace SvOg
