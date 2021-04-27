//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/29,2020/10/29 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFilesDialog.cpp
///  Dialog for config external files in audidtrail 
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "AuditFilesDialog.h"
#include "SVUtilityLibrary/AuditFiles.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "GridCtrlLibrary/GridCellCheck.h"
#pragma endregion Includes

namespace SvOg
{
	const LPCSTR AuditFilesDialog::HeaderNames[EColumncount] = { "Name", "Size","Time Stamp","Ignore","Hash","Time(µs)", "Hash Value" };
	const DWORD  AuditFilesDialog::HeaderWith[EColumncount] = { 160,80,80,40,40,60,200 };

		AuditFilesDialog::AuditFilesDialog(const std::vector< SvUl::AuditFile>& rList, DialogType t, CWnd* pParent /*=nullptr*/)
		: CDialog(IDD_DLG_REPORT_FILE, pParent),
		m_DialogType(t)
	{
		m_AuditFiles.SetValues(rList);
	}

	AuditFilesDialog::~AuditFilesDialog()
	{
		
	}

	void AuditFilesDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_FILEGRID, m_FileGrid);
	}


	BEGIN_MESSAGE_MAP(AuditFilesDialog, CDialog)
		ON_WM_SIZE()
		ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_FILEGRID, OnGridBeginEdit)
		ON_BN_CLICKED(IDC_BUTTON_ADD, &AuditFilesDialog::OnBnClickedButtonAdd)
		ON_BN_CLICKED(IDC_BUTTON_REMOVE, &AuditFilesDialog::OnBnClickedButtonRemove)
		ON_BN_CLICKED(IDHELP, &AuditFilesDialog::OnBnClickedButtonHelp)
		ON_COMMAND(ID_HELP, OnHelp)
		ON_NOTIFY(NM_CLICK, IDC_FILEGRID, OnGridClick)
	END_MESSAGE_MAP()


	// DlgReportDefaultFile message handlers

	void AuditFilesDialog::setResizeControls()
	{
		m_Resizer.Add(this, IDC_FILEGRID, SvMc::RESIZE_LOCKALL);
		m_Resizer.Add(this, IDHELP, SvMc::RESIZE_LOCKRIGHT | SvMc::RESIZE_LOCKBOTTOM);
		m_Resizer.Add(this, IDCANCEL, SvMc::RESIZE_LOCKRIGHT | SvMc::RESIZE_LOCKBOTTOM);
		m_Resizer.Add(this, IDOK, SvMc::RESIZE_LOCKRIGHT | SvMc::RESIZE_LOCKBOTTOM);
		m_Resizer.Add(this, IDC_BUTTON_REMOVE, SvMc::RESIZE_LOCKRIGHT | SvMc::RESIZE_LOCKBOTTOM);
		m_Resizer.Add(this, IDC_BUTTON_ADD, SvMc::RESIZE_LOCKRIGHT | SvMc::RESIZE_LOCKBOTTOM);

	}
	void AuditFilesDialog::InitFileGrid()
	{

		m_FileGrid.SetRedraw(FALSE);
		m_FileGrid.AutoSize(GVS_DATA);
		m_FileGrid.SetColumnCount(EColumncount);
		m_FileGrid.SetFixedRowSelection(TRUE);
		m_FileGrid.SetFixedRowCount(1);
		m_FileGrid.SetFixedColumnSelection(FALSE);
		m_FileGrid.AllowReorderColumn(false);
		//m_FileGrid.SetSingleRowSelection(true);

		//add header
		SvGcl::GV_ITEM Item;
		Item.mask = GVIF_TEXT;
		for (int col = 0; col < EColumncount; col++)
		{
			Item.row = 0;
			Item.col = col;
			Item.strText = HeaderNames[col];
			m_FileGrid.SetItem(&Item);
			m_FileGrid.SetColumnWidth(col, HeaderWith[col]);
		}

	}

	BOOL AuditFilesDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();
		// Remove Close Button
		ModifyStyle(WS_SYSMENU, 0, SWP_FRAMECHANGED);

		setResizeControls();
		CWnd* pRemovButton = GetDlgItem(IDC_BUTTON_REMOVE);
		CWnd* pAddButton = GetDlgItem(IDC_BUTTON_ADD);


		if (m_DialogType == WhiteList)
		{
			SetHelpID(static_cast<UINT> (ID_AUDITTRAIL_ADDITIONALFILES - SvOr::HELPFILE_ID_OFFSET));
			SetWindowText("Additional Files");

			if (pRemovButton)
			{
				pRemovButton->ShowWindow(SW_SHOW);
				pRemovButton->EnableWindow(TRUE);
			}
			if (pAddButton)
			{
				pAddButton->ShowWindow(SW_SHOW);
				pAddButton->EnableWindow(TRUE);
			}


		}
		else
		{
			SetWindowText("Default Files");
			SetHelpID(static_cast<UINT> (ID_AUDITTRAIL_DEFAULTFILES - SvOr::HELPFILE_ID_OFFSET));

			if (pRemovButton)
			{
				pRemovButton->ShowWindow(SW_HIDE);
				pRemovButton->EnableWindow(FALSE);
			}
			if (pAddButton)
			{
				pAddButton->ShowWindow(SW_HIDE);
				pAddButton->EnableWindow(FALSE);
			}
		}
		m_AuditFiles.CalculateSHA256();
		InitFileGrid();
	
		UpdateFileGrid();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void AuditFilesDialog::UpdateFileGrid()
	{
		m_FileGrid.SetRedraw(FALSE);
		auto& rlist = m_AuditFiles.GetFiles();
		
		m_FileGrid.SetRowCount(static_cast<int>(rlist.size()) + 1);
		for (int i = 0; i < static_cast<int>(rlist.size()); i++)
		{
			int row = i + 1;
			if (m_DialogType == WhiteList)
			{
				m_FileGrid.SetItemText(row, Ename, rlist[i].GetFullname().c_str());
			}
			else
			{
				m_FileGrid.SetItemText(row, Ename, rlist[i].GetFilename().c_str());
			}
			m_FileGrid.SetItemState(row, Ename, m_FileGrid.GetItemState(row, Ename) | GVIS_READONLY);

			m_FileGrid.SetItemText(row, Esize, rlist[i].GetFormatedSize().c_str());
			m_FileGrid.SetItemState(row, Esize, m_FileGrid.GetItemState(row, Esize) | GVIS_READONLY);

			m_FileGrid.SetItemText(row, Etimestamp, rlist[i].GetFormatedWriteDate().c_str());
			m_FileGrid.SetItemState(row, Etimestamp, m_FileGrid.GetItemState(row, Etimestamp) | GVIS_READONLY);

			using namespace SvGcl;
			m_FileGrid.SetCellType(row, EIgnore, RUNTIME_CLASS(GridCellCheck));
			m_FileGrid.SetItemState(row, EIgnore, m_FileGrid.GetItemState(row, EIgnore) & ~GVIS_READONLY);
			SvGcl::GridCellCheck* cell = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(row, EIgnore));
			if (nullptr != cell)
			{
				cell->SetCheck(rlist[i].bignore);
			}
			m_FileGrid.SetCellType(row, EHash, RUNTIME_CLASS(GridCellCheck));
			cell = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(row, EHash));
			if (nullptr != cell)
			{
				cell->SetCheck(rlist[i].bhash);
			}

			m_FileGrid.SetItemText(row, ETime, rlist[i].GetFormatedDuration().c_str());
			m_FileGrid.SetItemState(row, ETime, m_FileGrid.GetItemState(row, ETime) | GVIS_READONLY);

			m_FileGrid.SetItemText(row, EHashValue, rlist[i].GetHashvalue().c_str());
			m_FileGrid.SetItemState(row, EHashValue, m_FileGrid.GetItemState(row, EHashValue) | GVIS_READONLY);
		}
		m_FileGrid.SetRedraw(TRUE);

	}
	//Get checkboxes 
	void AuditFilesDialog::FileGrid2Data()
	{
		auto& rlist = m_AuditFiles.GetFiles();
		int ItemCount = static_cast<int>(rlist.size());

		if (m_FileGrid.GetRowCount() != ItemCount + 1)
		{
			assert(false);
			return;
		}
		for (int item = 0; item < ItemCount; item++)
		{
			int row = item + 1;
			auto* pCellIg = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(row, EIgnore));
			auto* pCellHs = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(row, EHash));

			rlist[item].bignore = pCellIg && pCellIg->GetCheck();
			rlist[item].bhash = pCellHs && pCellHs->GetCheck();

		}
	}


	void AuditFilesDialog::OnOK()
	{
		FileGrid2Data();
		CDialog::OnOK();
	}


	void AuditFilesDialog::OnCancel()
	{

		CDialog::OnCancel();
	}

	void AuditFilesDialog::OnGridBeginEdit(NMHDR* /*pNotifyStruct*/, LRESULT* pResult)
	{
		//editing is not allowed
		//SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;
		*pResult = -1;
	}

	void AuditFilesDialog::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);

		m_Resizer.Resize(this);
	}


	void AuditFilesDialog::OnBnClickedButtonAdd()
	{
		if (m_DialogType != WhiteList)
		{
			return;
		}

		// Create an Open dialog; the default file name extension is ".my".
		CFileDialog fileDlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);

		if (fileDlg.DoModal() == IDOK)
		{
			CString pathName = fileDlg.GetPathName();
			auto& files = m_AuditFiles.GetFiles();
			files.emplace_back(pathName.GetString());
			m_AuditFiles.Sort();
			m_AuditFiles.UpdateList();
			m_AuditFiles.CalculateSHA256();
			UpdateFileGrid();

		}
	}

	void AuditFilesDialog::OnBnClickedButtonRemove()
	{
		if (m_DialogType != WhiteList)
		{
			return;
		}
		//delelete all selected Entries
		std::vector<UINT> IndexToDelete;
		///
		auto selection = m_FileGrid.GetSelectedCellRange();
		for (int i = selection.GetMaxRow(); i >= selection.GetMinRow(); --i)
		{
			for (int j = selection.GetMinCol(); j <= selection.GetMaxCol(); ++j)
			{
				if (m_FileGrid.IsCellSelected(i, j))
				{
					int index = i - 1;
					if (index >= 0)
					{
						IndexToDelete.push_back(index);
					}

					break;
				}
			}

		}
		m_FileGrid.SetSelectedRange({});

		for (int i = (int)IndexToDelete.size() - 1; i >= 0; i--)
		{
			int  index = IndexToDelete[i];
			if (index < (int)m_AuditFiles.GetFiles().size())
			{
				m_AuditFiles.GetFiles().erase(m_AuditFiles.GetFiles().begin() + index);
			}
		}
		m_AuditFiles.UpdateList();
		UpdateFileGrid();
	}

	void AuditFilesDialog::OnBnClickedButtonHelp()
	{
		long lContextID = GetWindowContextHelpId();
		OnCommandHelp(0, lContextID);
	}
	void AuditFilesDialog::OnHelp()
	{
		AfxGetApp()->HtmlHelp(m_nIDHelp);
	}

	BOOL AuditFilesDialog::OnHelpInfo(HELPINFO* pHelpInfo)
	{
		pHelpInfo->iCtrlId = m_nIDHelp;
		AfxGetApp()->HtmlHelp(pHelpInfo->iCtrlId, HH_HELP_CONTEXT);
		return TRUE;
	}

	void AuditFilesDialog::OnGridClick(NMHDR* pNotifyStruct, LRESULT* /*pResult*/)
	{
		SvGcl::NM_GRIDVIEW* pItem = (SvGcl::NM_GRIDVIEW*)pNotifyStruct;

		switch (pItem->iColumn)
		{
		case EHash:
		case EIgnore:
		{
			bool hash{ true }, ignore{ false };
			SvGcl::GridCellCheck* cell = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(pItem->iRow, EIgnore));
			SvGcl::GridCellCheck* cellH = dynamic_cast<SvGcl::GridCellCheck*>(m_FileGrid.GetCell(pItem->iRow, EHash));

			if (cell && cellH)
			{
				hash = cellH->GetCheck();
				ignore = cell->GetCheck();
			}
			auto& rlist = m_AuditFiles.GetFiles();
			int index = pItem->iRow - 1;
			if (index >= 0 && index < rlist.size())
			{

				rlist[index].bignore = ignore;
				rlist[index].bhash = hash;
			}
			m_AuditFiles.CalculateSHA256(index);
			UpdateFileGrid();
		}
		break;
		default:
			break;

		}
	}


}