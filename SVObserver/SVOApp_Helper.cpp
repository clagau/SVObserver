//*****************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This file implemented the function for ISVOApp_Helper 
//*****************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVSetupDialogManager.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ISVOApp_Helper.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVMFCControls/ModelessMessageBox.h"
#pragma endregion Includes

#pragma region implementation of methods from ISVOApp_Helper
bool SvOi::isOkToEdit()
{
	return TheSVObserverApp.OkToEdit();
}

long SvOi::GetSourceImageDepth()
{
	return TheSVObserverApp.GetSourceImageDepth();
}

INT_PTR SvOi::OpenSVFileDialog(bool bOpenFileDialog, LPCTSTR Extention, std::string& FileName, DWORD Flags, LPCTSTR Filter, std::string& Path, LPCTSTR Title)
{
	bool FullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

	SvMc::SVFileDialog FileDlg(bOpenFileDialog,
		FullAccess,
		Extention,
		FileName.c_str(),
		Flags,
		Filter,
		nullptr);

	FileDlg.m_ofn.lpstrTitle = Title;
	FileDlg.m_ofn.lpstrInitialDir = Path.c_str();

	INT_PTR Result = FileDlg.DoModal();
	if (IDOK == Result)
	{
		FileName = FileDlg.GetFileName();
		Path = FileDlg.GetPathName();
	}
	return Result;
}

INT_PTR SvOi::OpenSVFolderDialog(std::string& Path, LPCTSTR Title)
{
	bool FullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

	SvMc::SVDlgFolder FolderDlg(FullAccess, Path.c_str());

	FolderDlg.InitDlgFolder(_T("Ok"), Title);
	INT_PTR Result = FolderDlg.DoModal();
	if (IDOK == Result)
	{
		Path = FolderDlg.GetPathName();
	}

	return Result;
}

void SvOi::CallModelessMessageBox(std::string &rMessage, HANDLE hCheckEvent)
{
	CModelessMsgBox* pBox = new CModelessMsgBox(rMessage, hCheckEvent);

	pBox->Create(CModelessMsgBox::IDD, nullptr);
}

HRESULT SvOi::SetupDialogManager(const SVGUID& rClassId, const SVGUID& rObjectId, HWND hWnd)
{
	return SVSetupDialogManager::Instance().SetupDialog(rClassId, rObjectId, CWnd::FromHandle(hWnd));
}
#pragma endregion implementation of methods from ISVOApp_Helper