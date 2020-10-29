//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionSourceDlg
//* .File Name       : $Workfile:   SVOInspectionSourceDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   18 Sep 2014 13:40:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost\config.hpp>
//Moved to precompiled header: #include <boost\bind.hpp>
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOInspectionSourceDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVMFCControls\SVOEditorWnd.h"
#include "SVOPropertyPageDlg.h"
#include "SVOInspectionObj.h"
#include "SVOPPQObj.h"
#include "SVInspectionExporter.h"
#include "SVInspectionImporter.h"
#include "SVObserver.h"
#include "SVMFCControls/SVFileDialog.h"
#include "SVGlobal.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "Definitions/GlobalConst.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVInspectionProcess.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR InspectionExportFileFilters = _T("Inspection Export Files (*.bxp)|*.bxp||");
static LPCTSTR InspectionExportFileExt = _T("bxp");
static LPCTSTR ColorInspectionExportFileFilters = _T("Color Inspection Export Files (*.cxp)|*.cxp||");
static LPCTSTR ColorInspectionExportFileExt = _T("cxp");
static LPCTSTR AllInspectionExportFileFilters = _T("All Inspection Export Files (*.bxp;*.cxp)|*.bxp;*.cxp||");
static LPCTSTR AllInspectionExportFileExt = _T("bxp;cxp");

/////////////////////////////////////////////////////////////////////////////
// SVOInspectionSourceDlg property page

IMPLEMENT_DYNCREATE(SVOInspectionSourceDlg, CPropertyPage)

SVOInspectionSourceDlg::SVOInspectionSourceDlg() : CPropertyPage(SVOInspectionSourceDlg::IDD), m_pParent(nullptr)
{
}

void SVOInspectionSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOInspectionSourceDlg)
	DDX_Control(pDX, IDC_LST_IPD_LIST, m_ctlIPDlist);
	DDX_Control(pDX, IDC_BTN_PROP_VI, m_ctlPropVIbtn);
	DDX_Control(pDX, IDC_BTN_DELETE_VI, m_ctlDeleteVIbtn);
	DDX_Control(pDX, IDC_BTN_ADD_IPD, m_ctlAddIPDbtn);
	DDX_Control(pDX, IDC_BTN_IMPORT_IPD, m_ctlImportIPDbtn);
	DDX_Control(pDX, IDC_BTN_EXPORT_IPD, m_ctlExportIPDbtn);
	//DDX_Text(pDX, IDC_EDT_APP_INSPECTION, m_sDeviceName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVOInspectionSourceDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOInspectionSourceDlg)
	ON_BN_CLICKED(IDC_BTN_ADD_IPD, OnBtnAddIpd)
	ON_BN_CLICKED(IDC_BTN_DELETE_VI, OnBtnDeleteVi)
	ON_BN_CLICKED(IDC_BTN_PROP_VI, OnBtnPropVi)
	ON_LBN_SELCHANGE(IDC_LST_IPD_LIST, OnSelchangeLstIpdList)
	ON_LBN_DBLCLK(IDC_LST_IPD_LIST, OnDblclkLstIpdList)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BTN_IMPORT_IPD, OnBtnImportIpd)
	ON_BN_CLICKED(IDC_BTN_EXPORT_IPD, OnBtnExportIpd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOInspectionSourceDlg message handlers

void SVOInspectionSourceDlg::OnBtnAddIpd() 
{
	std::string NewInspection = m_pParent->GetNextInspectionName();
	std::string NewDisplayName = GetNextInspectionDisplayName(NewInspection);

	m_pParent->AddToInspectionList( NewDisplayName.c_str(), true);
	
	int iPos = m_ctlIPDlist.AddString( NewDisplayName.c_str() );
	m_ctlIPDlist.SetCurSel(iPos); 
	m_pParent->SetModified(true);
	m_pParent->ItemChanged(INSPECT_DLG, NewInspection.c_str(), ITEM_ACTION_NEW);
	m_ctlExportIPDbtn.EnableWindow(false);
}

void SVOInspectionSourceDlg::OnBtnDeleteVi() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();

	if (iCurSel != LB_ERR)
	{
		CString InspectionName;
		m_ctlIPDlist.GetText(iCurSel, InspectionName);

		m_ctlIPDlist.DeleteString(iCurSel);
		m_pParent->RemoveInspectionFromList(InspectionName.GetString());

		if (iCurSel > 0)
		{
			m_ctlIPDlist.SetCurSel(iCurSel-1);
		}
		else
		{
			m_ctlIPDlist.SetCurSel(0);
		}
		m_pParent->SetModified( true );
		m_pParent->ItemChanged( INSPECT_DLG, InspectionName, ITEM_ACTION_DELETE);
		m_pParent->LastInspectionLabelDeleted(InspectionName.GetString());

		EnableDisableExport();
	}
}

void SVOInspectionSourceDlg::EnableDisableExport()
{
	if ( !m_pParent->IsNewConfiguration() )
	{
		//check to see if the current inspection exists in the configuration
		int iCurSel = m_ctlIPDlist.GetCurSel();
		if ( iCurSel != LB_ERR )
		{
			CString InspectionName;
			m_ctlIPDlist.GetText(iCurSel, InspectionName);
			SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(InspectionName.GetString());
			if( nullptr != pInspectionObj )
			{
				SVObjectClass *pObject;
				HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName(InspectionName.GetString(), pObject );
				if ( S_OK == hr )
				{
					m_ctlExportIPDbtn.EnableWindow(TRUE);
				}
				else
				{
					m_ctlExportIPDbtn.EnableWindow(FALSE);
				}
			}
		}
	}
}

void SVOInspectionSourceDlg::OnBtnPropVi() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();

	if ( iCurSel != LB_ERR )
	{
		CString InspectionName;
		m_ctlIPDlist.GetText(iCurSel, InspectionName);

		SVOInspectionObjPtr pInpectionObj = m_pParent->GetInspectionObjectByName(InspectionName.GetString());
		if( nullptr != pInpectionObj )
		{
			// Get the PPQ that this inspection is attached to
			SVOPPQObjPtr pTempPPQ( nullptr );
			bool bFound{false};
			for(long i = 0; !bFound && i < m_pParent->GetPPQListCount(); ++i)
			{
				pTempPPQ = m_pParent->GetPPQObject(i);
				long lSize = pTempPPQ->GetAttachedInspectionCount();
				for(long l = 0; !bFound && l < lSize; l++ )
				{
					if(InspectionName.GetString() == pTempPPQ->GetAttachedInspection( l ) )
					{
						bFound = true;
					}
				}// end for
			}// end for

			SVOPropertyPageDlg oDlg;
			SVOInspectionObj& rTmpObj( oDlg.getInspectObject() );

			rTmpObj = *pInpectionObj;
			if ( bFound && nullptr != pTempPPQ )
			{
				oDlg.getPPQObject() = *pTempPPQ;
			}
			oDlg.SetDlgPage(VIRTUAL_INSPECT_DLG);
			oDlg.SetProductType(m_pParent->GetProductType());
			if (oDlg.DoModal() == IDOK)
			{
				*pInpectionObj = rTmpObj;
				m_pParent->SetModified(TRUE);
				m_pParent->ItemChanged( INSPECT_DLG, InspectionName.GetString(), ITEM_ACTION_PROP);
			}
		}
	}		
}

static bool IsColorImport(LPCTSTR filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename, nullptr, nullptr, nullptr, ext);
	// skip the '.'
	if (strcmp(&ext[1], ColorInspectionExportFileExt ) == 0)
	{
		bRetVal = true;
	}
	return bRetVal;
}

void SVOInspectionSourceDlg::OnBtnImportIpd() 
{
	// prompt for file 
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, AllInspectionExportFileExt, nullptr, dwFlags, AllInspectionExportFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		std::string PathName = dlg.GetPathName();
   
		// Create new IP
		OnBtnAddIpd();
		int iCurSel = m_ctlIPDlist.GetCurSel();
		if (iCurSel != LB_ERR)
		{
			CString InspectionName;
			m_ctlIPDlist.GetText(iCurSel, InspectionName);
			SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(InspectionName.GetString());
			
			// Update EnableAuxillaryExtents and NewDisableMethod from Import file
			long l_NewDisableMethod = 0;
			long l_EnableAuxExtents = 0;
			unsigned long l_VersionNumber = 0;
			HRESULT hr = SVInspectionImporter::GetProperties(PathName, l_NewDisableMethod, l_EnableAuxExtents, l_VersionNumber);
			if (S_OK == hr && nullptr != pInspectionObj )
			{
				bool shouldLoad = l_VersionNumber <= TheSVObserverApp.getCurrentVersion();
				if( shouldLoad )
				{
					// Save it for later, when we exit the dialog
					pInspectionObj->SetImportFilename( PathName.c_str() );
					pInspectionObj->SetNewDisableMethod((l_NewDisableMethod) ? "Method 2" : "Method 1");
					pInspectionObj->SetEnableAuxiliaryExtent(l_EnableAuxExtents);
					pInspectionObj->SetColor( IsColorImport(PathName.c_str()) );

				}
				else
				{
					std::string File;
					std::string App;

					::SVGetVersionString( App, TheSVObserverApp.getCurrentVersion() );
					::SVGetVersionString( File, l_VersionNumber );
					SvDef::StringVector msgList;
					msgList.push_back(File);
					msgList.push_back(App);
					SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Exception.setMessage( SVMSG_SVO_56_INSPECTION_IMPORT_ERROR, SvStl::Tid_ImportInspectionError, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10008_ImportInspectionWrongVersion );

					OnBtnDeleteVi();
				}
			}
		}
	}
}

void SVOInspectionSourceDlg::OnBtnExportIpd() 
{
	// Get Selected IP
	int iCurSel = m_ctlIPDlist.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		CString InspectionName;
		m_ctlIPDlist.GetText(iCurSel, InspectionName);
		SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(InspectionName.GetString());
		std::string Name(InspectionName.GetString());
		if( nullptr != pInspectionObj )
		{
			SVObjectClass* pObject( nullptr );
			HRESULT hr =  SVObjectManagerClass::Instance().GetObjectByDottedName(Name, pObject);
			SVInspectionProcess* pInspectionProcess = dynamic_cast<SVInspectionProcess*> (pObject);

			if (S_OK == hr && nullptr != pInspectionProcess)
			{
				bool IsColor = pInspectionProcess->IsColorCamera();
				LPCTSTR fileExt = (IsColor) ? ColorInspectionExportFileExt : InspectionExportFileExt;
				LPCTSTR fileFilters = (IsColor) ? ColorInspectionExportFileFilters : InspectionExportFileFilters;

				// prompt for path and file name
				DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
				bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
				SvMc::SVFileDialog dlg(false, bFullAccess, fileExt, InspectionName.GetString(), dwFlags, fileFilters, this);
				if (dlg.DoModal() == IDOK)
				{
					std::string pathName = dlg.GetPathName();
			   
					// Create XML file
					SVInspectionExporter::Export( pathName, Name, TheSVObserverApp.getCurrentVersion(), IsColor );
				}
			}
		}
	}
}

BOOL SVOInspectionSourceDlg::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}

BOOL SVOInspectionSourceDlg::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL SVOInspectionSourceDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_pParent = dynamic_cast<SVOConfigAssistantDlg*> (GetParent()->GetParent());

	if ( m_pParent->IsNewConfiguration() )
	{
		m_ctlExportIPDbtn.EnableWindow(FALSE);
	}

	SetWindowContextHelpId(IDD+SvOr::HELPFILE_DLG_IDD_OFFSET);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOInspectionSourceDlg::ClearList()
{
	m_ctlIPDlist.ResetContent();
}

void SVOInspectionSourceDlg::SetupList()
{
	int iCount = m_pParent->GetInspectionListCount();
	ClearList();

	SVOInspectionObjPtr pInspectionObj( nullptr );

	for (int i = 0; i < iCount; i++)
	{
		pInspectionObj = m_pParent->GetInspectionObject(i);
		if( nullptr != pInspectionObj )
		{
			m_ctlIPDlist.AddString(pInspectionObj->GetInspectionName().c_str());
		}
	}
	m_ctlIPDlist.SetCurSel(0);
}

void SVOInspectionSourceDlg::OnSelchangeLstIpdList() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();

	if (m_iCursel == iCurSel)
	{
		CString sCurrentTxt;
		m_ctlIPDlist.GetText(iCurSel, sCurrentTxt);
		SvMc::CSVOEditorWnd *pEditor = new SvMc::CSVOEditorWnd( m_ctlIPDlist, SvDef::cExcludeCharsToolIpName );
		pEditor->Edit(iCurSel);
		pEditor->WaitForDoneEditing();

		CString sNewTxt;
		m_ctlIPDlist.GetText(iCurSel, sNewTxt);

		bool invalidName = m_pParent->IsOriginalInspectionInList(sNewTxt);
		if (invalidName)
		{
			SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(sCurrentTxt.GetString());
			if (nullptr != pInspectionObj)
			{
				//The new name can be the original name
				if (sNewTxt == pInspectionObj->GetOriginalName().c_str())
				{
					invalidName = false;
				}
			}
		}
		invalidName |= m_pParent->IsInspectionInList(sNewTxt) || sNewTxt.IsEmpty();
		if (invalidName)
		{
			//Place MessageBox with error...
			m_ctlIPDlist.InsertString(m_iCursel, sCurrentTxt);
			m_ctlIPDlist.DeleteString(m_iCursel+1);
		}
		else
		{
			m_pParent->RenameInspection(sCurrentTxt.GetString(), sNewTxt);
			m_pParent->SetModified(TRUE);
		}
	}
	else
	{
		m_iCursel = iCurSel;
	}

	if ( m_iCursel != LB_ERR )
	{
		m_ctlIPDlist.SetCurSel(m_iCursel);
	}

	EnableDisableExport();
}

void SVOInspectionSourceDlg::OnDblclkLstIpdList() 
{
	OnBtnPropVi();
}

void SVOInspectionSourceDlg::OnDestroy() 
{
	CPropertyPage::OnDestroy();
}

std::string SVOInspectionSourceDlg::GetNextInspectionDisplayName( const std::string& rLabeName )
{
	BOOL bFound = false;
	std::string Result;
	int iNum = atoi( SvUl::Mid( rLabeName, 11).c_str() );

	while ( !bFound )
	{
		Result = SvUl::Format( _T("%s%d"), _T("Inspection_"), iNum );
		if( LB_ERR != m_ctlIPDlist.FindStringExact( -1, Result.c_str() ) )
		{
			iNum++;
		}
		else
		{
			bFound = true;
		}
	}

	return Result;
}

BOOL SVOInspectionSourceDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

