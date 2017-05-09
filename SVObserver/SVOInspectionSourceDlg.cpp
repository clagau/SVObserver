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
#include "SVMessage/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVInspectionProcess.h"
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
// CSVOInspectionSourceDlg property page

IMPLEMENT_DYNCREATE(CSVOInspectionSourceDlg, CPropertyPage)

CSVOInspectionSourceDlg::CSVOInspectionSourceDlg() : CPropertyPage(CSVOInspectionSourceDlg::IDD)
, m_Items( boost::bind( &( CListBox::GetItemData ), &m_ctlIPDlist, _1 ) , boost::bind( &( CListBox::SetItemData ), &m_ctlIPDlist, _1, _2 ) )
{
	//{{AFX_DATA_INIT(CSVOInspectionSourceDlg)
	m_sDeviceName = _T("");
	//}}AFX_DATA_INIT

	m_iIPDHorzScroll = 0;
	m_iVIHorzScroll = 0;
	m_iInspectionNextCount = 1;
}

CSVOInspectionSourceDlg::~CSVOInspectionSourceDlg()
{
}

void CSVOInspectionSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOInspectionSourceDlg)
	DDX_Control(pDX, IDC_LST_IPD_LIST, m_ctlIPDlist);
	DDX_Control(pDX, IDC_BTN_PROP_VI, m_ctlPropVIbtn);
	DDX_Control(pDX, IDC_BTN_DELETE_VI, m_ctlDeleteVIbtn);
	DDX_Control(pDX, IDC_BTN_ADD_IPD, m_ctlAddIPDbtn);
	DDX_Control(pDX, IDC_BTN_IMPORT_IPD, m_ctlImportIPDbtn);
	DDX_Control(pDX, IDC_BTN_EXPORT_IPD, m_ctlExportIPDbtn);
	//DDX_Text(pDX, IDC_EDT_APP_INSPECTION, m_sDeviceName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVOInspectionSourceDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSVOInspectionSourceDlg)
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
// CSVOInspectionSourceDlg message handlers

void CSVOInspectionSourceDlg::OnBtnAddIpd() 
{
	SVString NewInspection = m_pParent->GetNextInspectionName();
	SVString NewDisplayName = GetNextInspectionDisplayName(NewInspection);

	m_pParent->AddToInspectList( NewDisplayName.c_str(), NewInspection.c_str(), true);
	
	int iPos = m_ctlIPDlist.AddString( NewDisplayName.c_str() );
	m_Items.SetItemData(iPos, NewInspection.c_str() );
	m_ctlIPDlist.SetCurSel(iPos); 
	m_pParent->SetModified(true);
	m_pParent->ItemChanged(INSPECT_DLG, NewInspection.c_str(), ITEM_ACTION_NEW);
	m_ctlExportIPDbtn.EnableWindow(false);
}

void CSVOInspectionSourceDlg::OnBtnDeleteVi() 
{
	int iCursel = m_ctlIPDlist.GetCurSel();

	if (iCursel != LB_ERR)
	{
		SVString Label;
		CString InspectionName;
		m_ctlIPDlist.GetText(iCursel, InspectionName);
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);
		if( l_Iter != m_Items.end() )
		{
			Label = l_Iter->second;
		}

		m_ctlIPDlist.DeleteString(iCursel);
		m_pParent->RemoveInspectionFromList( Label.c_str() );

		if (iCursel > 0)
		{
			m_ctlIPDlist.SetCurSel(iCursel-1);
		}
		else
		{
			m_ctlIPDlist.SetCurSel(0);
		}
		m_pParent->SetModified( true );
		m_pParent->ItemChanged( INSPECT_DLG, InspectionName, ITEM_ACTION_DELETE);
		m_pParent->LastInspectionLabelDeleted( Label.c_str() );

		EnableDisableExport();
	}
}

void CSVOInspectionSourceDlg::EnableDisableExport()
{
	if ( !m_pParent->IsNewConfiguration() )
	{
		//check to see if the current inspection exists in the configuration
		int iCurInsp = m_ctlIPDlist.GetCurSel();
		if ( iCurInsp != LB_ERR )
		{
			SVString Name;
			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurInsp);
			if (l_Iter != m_Items.end() )
			{
				Name = l_Iter->second;
			}
			SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName( Name.c_str() );
			if( nullptr != pInspectionObj )
			{
				SVObjectClass *pObject;
				HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), pObject );
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

void CSVOInspectionSourceDlg::OnBtnPropVi() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();

	if ( iCurSel != LB_ERR )
	{
		SVString Label;

		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurSel);

		if( l_Iter != m_Items.end() )
		{
			Label = l_Iter->second;
		}

		SVOInspectionObjPtr pInpectionObj = m_pParent->GetInspectionObjectByName( Label.c_str() );
		if( nullptr != pInpectionObj )
		{
			// Get the PPQ that this inspection is attached to
			SVOPPQObjPtr pTempPPQ( nullptr );
			BOOL bFound;
			long hSize;
			long h;
			long lSize;
			long l;
		
			bFound = false;
			hSize = m_pParent->GetPPQListCount();
			for( h = 0; !bFound && h < hSize; h++ )
			{
				pTempPPQ = m_pParent->GetPPQObject( h );
				lSize = pTempPPQ->GetAttachedInspectionCount();
				for( l = 0; !bFound && l < lSize; l++ )
				{
					if( Label == pTempPPQ->GetAttachedInspection( l ) )
					{
						bFound = true;
					}
				}// end for
			}// end for

			CSVOPropertyPageDlg oDlg;
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
				m_pParent->ItemChanged( INSPECT_DLG, Label.c_str(), ITEM_ACTION_PROP);
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

void CSVOInspectionSourceDlg::OnBtnImportIpd() 
{
	// prompt for file 
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, AllInspectionExportFileExt, nullptr, dwFlags, AllInspectionExportFileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		SVString PathName = dlg.GetPathName();
   
		// Create new IP
		OnBtnAddIpd();
		int iCursel = m_ctlIPDlist.GetCurSel();
		if (iCursel != LB_ERR)
		{
			SVString Label;
			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);

			if( l_Iter != m_Items.end() )
			{
				Label = l_Iter->second;
			}
			
			SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(Label.c_str());
			
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
					SVString File;
					SVString App;

					::SVGetVersionString( App, TheSVObserverApp.getCurrentVersion() );
					::SVGetVersionString( File, l_VersionNumber );
					SVStringVector msgList;
					msgList.push_back(File);
					msgList.push_back(App);
					SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_56_INSPECTION_IMPORT_ERROR, SvOi::Tid_ImportInspectionError, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10008_ImportInspectionWrongVersion );

					OnBtnDeleteVi();
				}
			}
		}
	}
}

void CSVOInspectionSourceDlg::OnBtnExportIpd() 
{
	// Get Selected IP
	int iCursel = m_ctlIPDlist.GetCurSel();
	if (iCursel != LB_ERR)
	{
		SVString Label;
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);

		if( l_Iter != m_Items.end() )
		{
			Label = l_Iter->second;
		}

		SVOInspectionObjPtr pInspectionObj = m_pParent->GetInspectionObjectByName(Label.c_str());
		if( nullptr != pInspectionObj )
		{
			SVObjectClass* pObject( nullptr );
			HRESULT hr =  SVObjectManagerClass::Instance().GetObjectByDottedName(Label, pObject);
			SVInspectionProcess* pInspectionProcess = dynamic_cast<SVInspectionProcess*> (pObject);

			if (S_OK == hr && nullptr != pInspectionProcess)
			{
				bool IsColor = pInspectionProcess->IsColorCamera();
				LPCTSTR fileExt = (IsColor) ? ColorInspectionExportFileExt : InspectionExportFileExt;
				LPCTSTR fileFilters = (IsColor) ? ColorInspectionExportFileFilters : InspectionExportFileFilters;

				// prompt for path and file name
				DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
				bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
				SvMc::SVFileDialog dlg(false, bFullAccess, fileExt, Label.c_str(), dwFlags, fileFilters, this);
				if (dlg.DoModal() == IDOK)
				{
					SVString pathName = dlg.GetPathName();
			   
					// Create XML file
					hr = SVInspectionExporter::Export( pathName, Label, TheSVObserverApp.getCurrentVersion(), IsColor );
				}
			}
		}
	}
}

BOOL CSVOInspectionSourceDlg::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}

BOOL CSVOInspectionSourceDlg::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}

BOOL CSVOInspectionSourceDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_pParent = (CSVOConfigAssistantDlg*)GetParent()->GetParent();

	if ( m_pParent->IsNewConfiguration() )
	{
		m_ctlExportIPDbtn.EnableWindow(FALSE);
	}

	SetWindowContextHelpId(IDD+SvOr::HELPFILE_SVORESOURCE_OFFSET);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSVOInspectionSourceDlg::ClearList()
{
	m_Items.clear();
	m_ctlIPDlist.ResetContent();
}

void CSVOInspectionSourceDlg::SetupList()
{
	int iCount = m_pParent->GetInspectionListCount();
	ClearList();

	SVOInspectionObjPtr pInspectionObj( nullptr );

	for (int i = 0; i < iCount; i++)
	{
		pInspectionObj = m_pParent->GetInspectionObject(i);
		if( nullptr != pInspectionObj )
		{
			int iPos = m_ctlIPDlist.AddString(pInspectionObj->GetInspectionName().c_str());
			m_Items.SetItemData( iPos, pInspectionObj->GetInspectionLabelName() );
		}
	}
	m_ctlIPDlist.SetCurSel(0);
}

void CSVOInspectionSourceDlg::OnSelchangeLstIpdList() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();

	if (m_iCursel == iCurSel)
	{
		CString sCurrentTxt;
		m_iEditingSel = iCurSel;
		m_ctlIPDlist.GetText(iCurSel, sCurrentTxt);
		SvMc::CSVOEditorWnd *pEditor = new SvMc::CSVOEditorWnd( m_ctlIPDlist, SvO::SVEXCLUDECHARS_TOOL_IP_NAME );
		pEditor->Edit(iCurSel);
		pEditor->WaitForDoneEditing();
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurSel);

		SVString Label;
		if( l_Iter != m_Items.end() )
		{
			Label = l_Iter->second;
		}

		CString sNewTxt;
		m_ctlIPDlist.GetText(iCurSel, sNewTxt);
		if ( !m_pParent->CanInspectionNameBeUsed(sNewTxt) )
		{
			//Place MessageBox with error...
			m_ctlIPDlist.InsertString(m_iCursel, sCurrentTxt);
			m_Items.SetItemData( m_iCursel, Label );
			m_ctlIPDlist.DeleteString(m_iCursel+1);
		}
		else
		{
			m_pParent->RenameInspection( Label.c_str(), sNewTxt);
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

void CSVOInspectionSourceDlg::OnDblclkLstIpdList() 
{
	OnBtnPropVi();
}

void CSVOInspectionSourceDlg::OnDestroy() 
{
	CPropertyPage::OnDestroy();
}

SVString CSVOInspectionSourceDlg::GetNextInspectionDisplayName( const SVString& rLabeName )
{
	BOOL bFound = false;
	SVString Result;
	int iNum = atoi( SvUl_SF::Mid( rLabeName, 11).c_str() );

	while ( !bFound )
	{
		Result = SvUl_SF::Format( _T("%s%d"), _T("Inspection_"), iNum );
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

BOOL CSVOInspectionSourceDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

