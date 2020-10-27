// DlgReportDefaultFile.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "AudidFilesDialog.h"
#include "SVUtilityLibrary/AudidFiles.h"
#include "SVUtilityLibrary/SHA256.h"
#include <atlwin.h>


namespace SvOg
{
	const LPCSTR AudidFilesDialog::HeaderNames[EColumncount] = { "Name", "Size","Date","Ignore","CalcHash", "HashValue" };
	const DWORD  AudidFilesDialog::HeaderWith[EColumncount] = { 120,60,120,40,40,200 };
	IMPLEMENT_DYNAMIC(AudidFilesDialog, CDialogEx)

		AudidFilesDialog::AudidFilesDialog(const std::vector< SvUl::AudidFile>& rList, DialogType t, CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_DLG_REPORT_FILE, pParent),
		m_DialogType(t)
	{
		m_AudidFiles.SetValues(rList);
		if (m_DialogType == WhiteList)
		{
			m_ContextMenu.LoadMenu(IDR_MENU_REPORT_WHITE);
		}
		else
		{
			m_ContextMenu.LoadMenu(IDR_MENU_REPORT);
		}
	}

	AudidFilesDialog::~AudidFilesDialog()
	{
		m_ContextMenu.DestroyMenu();
	}

	void AudidFilesDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_LIST_REPORT_FILE, m_ListCtrl);
	}


	BEGIN_MESSAGE_MAP(AudidFilesDialog, CDialogEx)
		ON_WM_SIZE()
		ON_WM_CONTEXTMENU()
		ON_COMMAND(ID_DEFAULT_CALCTRUE, &AudidFilesDialog::OnDefaultCalctrue)
		ON_COMMAND(ID_DEFAULT_CALCFALSE, &AudidFilesDialog::OnDefaultCalcfalse)
		ON_COMMAND(ID_DEFAULT_IGNORETRUE, &AudidFilesDialog::OnDefaultIgnoretrue)
		ON_COMMAND(ID_DEFAULT_IGNOREFALSE, &AudidFilesDialog::OnDefaultIgnorefalse)
		ON_COMMAND(ID_DEFAULT_CALCHASH, &AudidFilesDialog::OnDefaultCalchash)
		ON_BN_CLICKED(IDC_BUTTON_CALC, &AudidFilesDialog::OnBnClickedButtonCalc)
		ON_COMMAND(ID_DEFAULT_ADDFILE, &AudidFilesDialog::OnDefaultAddfile)
		ON_COMMAND(ID_DEFAULT_DELETEFILE, &AudidFilesDialog::OnDefaultDeletefile)
	END_MESSAGE_MAP()


	// DlgReportDefaultFile message handlers


	BOOL AudidFilesDialog::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		if (m_DialogType == WhiteList)
		{
			SetWindowText("SvUl::AudidFile WhiteList");
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

	void AudidFilesDialog::UpdateListctrl(bool rebuild)
	{

		if (rebuild)
		{
			m_ListCtrl.DeleteAllItems();
		}
		auto& rlist = m_AudidFiles.GetFiles();
		for (int i = 0; i < rlist.size(); i++)
		{
			SvUl::AudidFile& rAF = rlist[i];
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
	void AudidFilesDialog::LstCtrl2Data()
	{
		int Itemcount = m_ListCtrl.GetItemCount();
		auto& rlist = m_AudidFiles.GetFiles();
		for (int item = 0; item < Itemcount; item++)
		{

			char textbuffer[512];
			m_ListCtrl.GetItemText(item, EIgnore, textbuffer, 512);
			rlist[item].bignore = SvUl::AudidFile::String2Flag(textbuffer);


			m_ListCtrl.GetItemText(item, ECalcHash, textbuffer, 512);
			rlist[item].bhash = SvUl::AudidFile::String2Flag(textbuffer);

			m_ListCtrl.GetItemText(item, EHashValue, textbuffer, 512);
			rlist[item].hashvalue = textbuffer;


		}
	}

	void AudidFilesDialog::OnOK()
	{
		LstCtrl2Data();
		CDialogEx::OnOK();
	}


	void AudidFilesDialog::OnCancel()
	{
		// TODO: Add your specialized code here and/or call the base class

		CDialogEx::OnCancel();
	}


	void AudidFilesDialog::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);


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


	void AudidFilesDialog::OnContextMenu(CWnd*, CPoint point)
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


	void AudidFilesDialog::OnDefaultCalctrue()
	{
		IterateForSelected(ECalcHash, true);
	}


	void AudidFilesDialog::OnDefaultCalcfalse()
	{
		IterateForSelected(ECalcHash, false);
	}


	void AudidFilesDialog::OnDefaultIgnoretrue()
	{
		IterateForSelected(EIgnore, true);
	}


	void AudidFilesDialog::OnDefaultIgnorefalse()
	{
		IterateForSelected(EIgnore, false);
	}


	void AudidFilesDialog::OnDefaultCalchash()
	{
		IterateForSelected(EHashValue, false);
	}

	void AudidFilesDialog::IterateForSelected(columnType col, bool par)
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
					m_ListCtrl.SetItemText(nItem, EIgnore, SvUl::AudidFile::Flag2String(par).c_str());
					break;
				case ECalcHash:
					m_ListCtrl.SetItemText(nItem, ECalcHash, SvUl::AudidFile::Flag2String(par).c_str());
					break;
				case 	EHashValue:
				{
					SvUl::AudidFile& rAF = m_AudidFiles.GetFiles()[i];
					std::string hash;
					try
					{
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



	void AudidFilesDialog::OnBnClickedButtonCalc()
	{
		LstCtrl2Data();
		m_AudidFiles.CalculateSHA256();
		UpdateListctrl(false);

	}


	void AudidFilesDialog::OnDefaultAddfile()
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
			auto& files = m_AudidFiles.GetFiles();
			files.emplace_back(pathName.GetString());
			m_AudidFiles.UpdateList();
			UpdateListctrl(true);
		}
	}


	void AudidFilesDialog::OnDefaultDeletefile()
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
				if (index < (int)m_AudidFiles.GetFiles().size())
				{
					m_AudidFiles.GetFiles().erase(m_AudidFiles.GetFiles().begin() + index);
				}
			}
			m_AudidFiles.UpdateList();
			UpdateListctrl(true);
		}
	}
}