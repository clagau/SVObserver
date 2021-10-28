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
#include "SVSecurity/SVSecurityManager.h"
#include "SVSetupDialogManager.h"
#include "SVMessage\SVMessage.h"
#include "ObjectInterfaces\ISVOApp_Helper.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVMFCControls/ModelessMessageBox.h"
#include "SVOGui/SVLicenseMgrModelessDlg.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

#pragma region implementation of methods from ISVOApp_Helper
bool SvOi::isOkToEdit()
{
	return TheSVObserverApp().OkToEdit();
}

DWORD  SvOi::getLoadingVersion()
{
	return TheSVObserverApp().getLoadingVersion();
}

DWORD  SvOi::getCurrentVersion()
{
	return TheSVObserverApp().getCurrentVersion();
}

INT_PTR SvOi::OpenSVFileDialog(bool bOpenFileDialog, LPCTSTR Extention, std::string& FileName, DWORD Flags, LPCTSTR Filter, std::string& Path, LPCTSTR Title)
{
	bool FullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

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
	bool FullAccess = TheSecurityManager().SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);

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
	ModelessMessageBox* pBox = new ModelessMessageBox(rMessage, hCheckEvent);

	pBox->Create(ModelessMessageBox::IDD, nullptr);
}

HRESULT SvOi::SetupDialogManager(SvPb::ClassIdEnum classId, uint32_t objectId, HWND hWnd)
{
	return SVSetupDialogManager::Instance().SetupDialog(classId, objectId, CWnd::FromHandle(hWnd));
}

void SvOi::showLicenseManagerDialog(const std::string& rMessage, const std::set<std::string>& rList, HANDLE hCheckEvent)
{
	SVLicenseMgrModelessDlg::Show(rMessage, rList, hCheckEvent);
}

void SvOi::destroyLicenseManagerDialog()
{
	SVLicenseMgrModelessDlg::Destroy();
}

long SvOi::getGigePacketSize()
{
	return TheSVObserverApp().getGigePacketSize();
}

void SvOi::modifyAcquisitionDevice(LPCTSTR acquisitionName, const SVDeviceParamCollection* pParams)
{
	SVConfigurationObject* pConfig = nullptr;
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (nullptr != pConfig)
	{
		pConfig->ModifyAcquisitionDevice(acquisitionName, pParams);
	}
}
#pragma endregion implementation of methods from ISVOApp_Helper