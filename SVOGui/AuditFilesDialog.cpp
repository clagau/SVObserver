//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/29,2020/10/29 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file AuditFilesDialog.cpp
///  Dialog for config external files in audidtrail 
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "AuditFilesDialog.h"
#include "SVUtilityLibrary/AuditFiles.h"
#include "SVUtilityLibrary/SHA256.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes

namespace SvOg
{
	const LPCSTR AuditFilesDialog::HeaderNames[EColumncount] = { "Name", "Size","Date","Ignore","CalcHash", "HashValue" };
	const DWORD  AuditFilesDialog::HeaderWith[EColumncount] = { 120,60,120,40,40,200 };
	IMPLEMENT_DYNAMIC(AuditFilesDialog, CDialog)

		AuditFilesDialog::AuditFilesDialog(const std::vector< SvUl::AuditFile>& rList, DialogType t, CWnd* pParent /*=nullptr*/)
		: CDialog(IDD_DLG_REPORT_FILE, pParent),
		m_DialogType(t)
	{
		m_AuditFiles.SetValues(rList);
		if (m_DialogType == WhiteList)
		{
			m_ContextMenu.LoadMenu(IDR_MENU_REPORT_WHITE);
		}
		else
		{
			m_ContextMenu.LoadMenu(IDR_MENU_REPORT);
		}
	}

	AuditFilesDialog::~AuditFilesDialog()
	{
		m_ContextMenu.DestroyMenu();
	}

	void AuditFilesDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_LIST_REPORT_FILE, m_ListCtrl);
	}


	BEGIN_MESSAGE_MAP(AuditFilesDialog, CDialog)
		ON_WM_SIZE()
		ON_WM_CONTEXTMENU()
		ON_COMMAND(ID_DEFAULT_CALCTRUE, &AuditFilesDialog::OnDefaultCalctrue)
		ON_COMMAND(ID_DEFAULT_CALCFALSE, &AuditFilesDialog::OnDefaultCalcfalse)
		ON_COMMAND(ID_DEFAULT_IGNORETRUE, &AuditFilesDialog::OnDefaultIgnoretrue)
		ON_COMMAND(ID_DEFAULT_IGNOREFALSE, &AuditFilesDialog::OnDefaultIgnorefalse)
		ON_COMMAND(ID_DEFAULT_CALCHASH, &AuditFilesDialog::OnDefaultCalchash)
		ON_BN_CLICKED(IDC_BUTTON_CALC, &AuditFilesDialog::OnBnClickedButtonCalc)
		ON_COMMAND(ID_DEFAULT_ADDFILE, &AuditFilesDialog::OnDefaultAddfile)
		ON_COMMAND(ID_DEFAULT_DELETEFILE, &AuditFilesDialog::OnDefaultDeletefile)
	END_MESSAGE_MAP()


	// DlgReportDefaultFile message handlers


	BOOL AuditFilesDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();
		
		if (m_DialogType == WhiteList)
		{
			SetHelpID(static_cast<UINT> (ID_CONFIGREPORT_ADDITIONALFILES - SvOr::HELPFILE_ID_OFFSET));
			SetWindowText("Additional Report Files");
		}
		else
		{
			SetWindowText("Default Report Files");
			SetHelpID(static_cast<UINT> (ID_CONFIGREPORT_DEFAULTFILES - SvOr::HELPFILE_ID_OFFSET));
		}

		for (int i = 0; i < EColumncount; i++)
		{
			int WidthFactor = 1;
			if (i == 0 && (m_DialogType == WhiteList))
			{
				WidthFactor = 2;// double with of first column because of fuulpath
			}

			m_ListCtrl.InsertColumn(i, HeaderNames[i], LVCFMT_LEFT, HeaderWith[i], i);
		}
		UpdateListctrl(true);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void AuditFilesDialog::UpdateListctrl(bool rebuild)
	{

		if (rebuild)
		{
			m_ListCtrl.DeleteAllItems();
		}
		auto& rlist = m_AuditFiles.GetFiles();
		for (int i = 0; i < rlist.size(); i++)
		{
			SvUl::AuditFile& rAF = rlist[i];
			if (rebuild)
			{
				if (m_DialogType == WhiteList)
				{
					m_ListCtrl.InsertItem(LVIF_TEXT | LVIF_PARAM, i, rAF.GetFullname().c_str(), 0, 0, 0, i);
				}
				else
				{
					m_ListCtrl.InsertItem(LVIF_TEXT | LVIF_PARAM, i, rAF.GetFilename().c_str(), 0, 0, 0, i);
				}
			}
			else
			{
				if (m_DialogType == WhiteList)
				{
					m_ListCtrl.SetItemText(i, Ename, rAF.GetFullname().c_str());
				}
				else
				{
					m_ListCtrl.SetItemText(i, Ename, rAF.GetFilename().c_str());
				}
			}
			//Esize, Edate, EIgnore, ECalcHash, EColumncount
			m_ListCtrl.SetItemText(i, Edate, rAF.GetFormatedWriteDate().c_str());
			m_ListCtrl.SetItemText(i, Esize, rAF.GetFormatedSize().c_str());
			m_ListCtrl.SetItemText(i, EIgnore, rAF.GetFormatedIgnoreFlag().c_str());
			m_ListCtrl.SetItemText(i, ECalcHash, rAF.GetFormatedHashFlag().c_str());
			m_ListCtrl.SetItemText(i, EHashValue, rAF.GetHashvalue().c_str());
		}
	}
	void AuditFilesDialog::LstCtrl2Data()
	{
		int Itemcount = m_ListCtrl.GetItemCount();
		auto& rlist = m_AuditFiles.GetFiles();
		for (int item = 0; item < Itemcount; item++)
		{

			char textbuffer[512];
			m_ListCtrl.GetItemText(item, EIgnore, textbuffer, 512);
			rlist[item].bignore = SvUl::AuditFile::String2Flag(textbuffer);


			m_ListCtrl.GetItemText(item, ECalcHash, textbuffer, 512);
			rlist[item].bhash = SvUl::AuditFile::String2Flag(textbuffer);

			m_ListCtrl.GetItemText(item, EHashValue, textbuffer, 512);
			rlist[item].hashvalue = textbuffer;


		}
	}

	void AuditFilesDialog::OnOK()
	{
		LstCtrl2Data();
		CDialog::OnOK();
	}


	void AuditFilesDialog::OnCancel()
	{
		// TODO: Add your specialized code here and/or call the base class

		CDialog::OnCancel();
	}


	void AuditFilesDialog::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);


		int fs = 10;
		int bh = 20;
		int bw = 50;
		CWnd* pOK = GetDlgItem(IDOK);
		CWnd* pC = GetDlgItem(IDCANCEL);
		CWnd* pCalc = GetDlgItem(IDC_BUTTON_CALC);
		RECT okR;
		if (pOK && ::IsWindow(pOK->GetSafeHwnd()))
		{
			pOK->GetWindowRect(&okR);
			bw = okR.right - okR.left;
			bh = okR.bottom - okR.top;
			pOK->MoveWindow(cx - 2 * fs - 2 * bw, cy - fs - bh, bw, bh);;
		}
		if (pC && ::IsWindow(pC->GetSafeHwnd()))
		{

			pC->MoveWindow(cx - fs - bw, cy - fs - bh, bw, bh);;
		}
		if (pCalc && ::IsWindow(pCalc->GetSafeHwnd()))
		{

			pCalc->MoveWindow(cx - 3 * fs - 3 * bw, cy - fs - bh, bw, bh);;
		}



		int gap = 3 * fs + bh;

		if (::IsWindow(m_ListCtrl.GetSafeHwnd()))
		{
			m_ListCtrl.MoveWindow(fs, fs, cx - fs, cy - gap);

		}



	}


	void AuditFilesDialog::OnContextMenu(CWnd*, CPoint point)
	{
		{
			CMenu* pPopupMenu = m_ContextMenu.GetSubMenu(0);
			assert(nullptr != pPopupMenu);
			CWnd* pOwner = this;
			while (pOwner->GetStyle() & WS_CHILD)
			{
				pOwner = pOwner->GetParent();
			}
			pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pOwner);
		}
	}


	void AuditFilesDialog::OnDefaultCalctrue()
	{
		IterateForSelected(ECalcHash, true);
	}


	void AuditFilesDialog::OnDefaultCalcfalse()
	{
		IterateForSelected(ECalcHash, false);
	}


	void AuditFilesDialog::OnDefaultIgnoretrue()
	{
		IterateForSelected(EIgnore, true);
	}


	void AuditFilesDialog::OnDefaultIgnorefalse()
	{
		IterateForSelected(EIgnore, false);
	}


	void AuditFilesDialog::OnDefaultCalchash()
	{
		IterateForSelected(EHashValue, false);
	}

	void AuditFilesDialog::IterateForSelected(columnType col, bool par)
	{
		int uSelectedCount = m_ListCtrl.GetSelectedCount();


		// Update all of the selected items.
		if (uSelectedCount > 0)
		{
			for (int i = 0, nItem = -1; i < uSelectedCount; i++)
			{

				
				nItem = m_ListCtrl.GetNextItem(nItem, LVNI_SELECTED);
				ASSERT(nItem != -1);
				switch (col)
				{

				case EIgnore:
					m_ListCtrl.SetItemText(nItem, EIgnore, SvUl::AuditFile::Flag2String(par).c_str());
					break;
				case ECalcHash:
					m_ListCtrl.SetItemText(nItem, ECalcHash, SvUl::AuditFile::Flag2String(par).c_str());
					break;
				case 	EHashValue:
				{
					std::string hash;
					try
					{
						SvUl::AuditFile& rAF = m_AuditFiles.GetFiles()[i];
						hash = SvUl::SHA256(rAF.GetFullname().c_str());
					}
					catch (const std::exception& e)
					{
						hash = e.what();;
					}
					m_ListCtrl.SetItemText(nItem, EHashValue, hash.c_str());
				}
				}


			}
		}
	}



	void AuditFilesDialog::OnBnClickedButtonCalc()
	{
		LstCtrl2Data();
		m_AuditFiles.CalculateSHA256();
		UpdateListctrl(false);

	}


	void AuditFilesDialog::OnDefaultAddfile()
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
			m_AuditFiles.UpdateList();
			UpdateListctrl(true);
		}
	}


	void AuditFilesDialog::OnDefaultDeletefile()
	{
		if (m_DialogType != WhiteList)
		{
			return;
		}
		//delelete all selected Entries
		std::vector<UINT> IndexToDelete;

		int uSelectedCount = m_ListCtrl.GetSelectedCount();
		if (uSelectedCount > 0)
		{
			for (int i = 0, nItem = -1; i < uSelectedCount; i++)
			{
				nItem = m_ListCtrl.GetNextItem(nItem, LVNI_SELECTED);
				if (nItem != -1)
				{
					IndexToDelete.push_back(nItem);
				}
			}
			for (int i = (int)IndexToDelete.size() - 1; i >= 0; i--)
			{
				int  index = IndexToDelete[i];
				if (index < (int)m_AuditFiles.GetFiles().size())
				{
					m_AuditFiles.GetFiles().erase(m_AuditFiles.GetFiles().begin() + index);
				}
			}
			m_AuditFiles.UpdateList();
			UpdateListctrl(true);
		}
	}
}