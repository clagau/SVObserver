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

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
#include "SVObjectLibrary/SVObjectManagerClass.h"
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
#include "SVStatusLibrary/ExceptionManager.h"
#include "ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVOResource/ConstGlobalSvOr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR szInspectionExportFileFilters[] = _T("Inspection Export Files (*.bxp)|*.bxp||");
static LPCTSTR scInspectionExportFileExt = _T("bxp");
static TCHAR szColorInspectionExportFileFilters[] = _T("Color Inspection Export Files (*.cxp)|*.cxp||");
static LPCTSTR scColorInspectionExportFileExt = _T("cxp");

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
	CString sNewIns = m_pParent->GetNextInspectionName();
	CString sNewDisplayName = GetNextInspectionDisplayName(sNewIns);

	m_pParent->AddToInspectList(sNewDisplayName,sNewIns);
	
	int iPos = m_ctlIPDlist.AddString(sNewDisplayName);
	m_Items.SetItemData(iPos, sNewIns );
	m_ctlIPDlist.SetCurSel(iPos); 
	m_pParent->SetModified(TRUE);
	m_pParent->ItemChanged(INSPECT_DLG,sNewIns,ITEM_ACTION_NEW);
	m_ctlExportIPDbtn.EnableWindow(FALSE);
}

void CSVOInspectionSourceDlg::OnBtnDeleteVi() 
{
	CString sLabel;
	CString sTxt;
	int iCursel = m_ctlIPDlist.GetCurSel();

	if (iCursel != LB_ERR)
	{
		m_ctlIPDlist.GetText(iCursel,sTxt);
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);
		if( l_Iter != m_Items.end() )
		{
			sLabel = l_Iter->second;
		}

		m_ctlIPDlist.DeleteString(iCursel);
		m_pParent->RemoveInspectionFromList(sLabel);

		if (iCursel > 0)
		{
			m_ctlIPDlist.SetCurSel(iCursel-1);
		}
		else
		{
			m_ctlIPDlist.SetCurSel(0);
		}
		m_pParent->SetModified(TRUE);
		m_pParent->ItemChanged(INSPECT_DLG,sTxt,ITEM_ACTION_DELETE);
		m_pParent->LastInspectionLabelDeleted(sLabel);

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
			CString sName;
			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurInsp);
			if (l_Iter != m_Items.end() )
			{
				sName = l_Iter->second;
			}
			CSVOInspectionObj *pObj = m_pParent->GetInspectionObjectByName(sName);
			if (pObj)
			{
				SVObjectClass *pObject;
				SVString ipName = sName;
				HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName(ipName.c_str(), pObject);
				if ( hr == S_OK )
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
	CSVOInspectionObj TmpObj;
	int iCurSel = m_ctlIPDlist.GetCurSel();
	CString sTxt;
	CString sLabel;

	if ( iCurSel != LB_ERR )
	{
		m_ctlIPDlist.GetText(iCurSel,sTxt);

		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurSel);

		if( l_Iter != m_Items.end() )
		{
			sLabel = l_Iter->second;
		}

		CSVOInspectionObj *pObj = m_pParent->GetInspectionObjectByName(sLabel);
		TmpObj = *pObj;

		// Get the PPQ that this inspection is attached to
		CSVOPPQObj *pTempPPQ;
		BOOL bFound;
		long hSize;
		long h;
		long lSize;
		long l;
		
		bFound = FALSE;
		hSize = m_pParent->GetPPQListCount();
		for( h = 0; !bFound && h < hSize; h++ )
		{
			pTempPPQ = m_pParent->GetPPQObject( h );
			lSize = pTempPPQ->GetAttachedInspectionCount();
			for( l = 0; !bFound && l < lSize; l++ )
			{
				if( sLabel == pTempPPQ->GetAttachedInspection( l ) )
					bFound = TRUE;
			}// end for
		}// end for

		CSVOPropertyPageDlg oDlg;
		oDlg.SetInspectObject(&TmpObj);
		if ( bFound )
		{
			oDlg.SetPPQObject( pTempPPQ );
		}
		oDlg.SetDlgPage(VIRTUAL_INSPECT_DLG);
		oDlg.SetProductType(m_pParent->GetProductType());
		if (oDlg.DoModal() == IDOK)
		{
			*pObj = TmpObj;
			m_pParent->SetModified(TRUE);
			m_pParent->ItemChanged(INSPECT_DLG,sLabel,ITEM_ACTION_PROP);
		}	
	}		
}

static bool IsColorImport(LPCTSTR filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename, NULL, NULL, NULL, ext);
	// skip the '.'
	if (strcmp(&ext[1], scColorInspectionExportFileExt ) == 0)
	{
		bRetVal = true;
	}
	return bRetVal;
}

void CSVOInspectionSourceDlg::OnBtnImportIpd() 
{
	bool bColor = SVObjectManagerClass::Instance().m_bIsColorSVIM;
	LPCTSTR fileExt = (bColor) ? scColorInspectionExportFileExt : scInspectionExportFileExt;
	LPCTSTR fileFilters = (bColor) ? szColorInspectionExportFileFilters : szInspectionExportFileFilters;
	// prompt for file 
	DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
	SvMc::SVFileDialog dlg(true, bFullAccess, fileExt, NULL, dwFlags, fileFilters, this);
	dlg.m_ofn.lpstrTitle = _T("Select File");

	if (dlg.DoModal() == IDOK)
	{
		CString pathName = dlg.GetPathName();
		// check if color/mono and wrong system
		if (bColor && !IsColorImport(pathName))
		{
			// Show some message
			MessageBox(_T("Only Exported Color Inspections can be Imported into a Color SVIM."), _T("Import Error"), MB_ICONERROR | MB_OK);
			return;
		}
		if (!bColor && IsColorImport(pathName))
		{
			// Show some message
			MessageBox(_T("Exported Color Inspections cannot be Imported into a Non Color SVIM."), _T("Import Error"), MB_ICONERROR | MB_OK);
			//AfxMessageBox(_T("Exported Color Inspections cannot be Imported into a Non Color SVIM."));
			return;
		}
   
		// Create new IP
		OnBtnAddIpd();
		int iCursel = m_ctlIPDlist.GetCurSel();
		if (iCursel != LB_ERR)
		{
			CString sLabel;
			SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);

			if( l_Iter != m_Items.end() )
			{
				sLabel = l_Iter->second;
			}
			
			CSVOInspectionObj* pObj = m_pParent->GetInspectionObjectByName(sLabel);
			
			// Update EnableAuxillaryExtents and NewDisableMethod from Import file
			long l_NewDisableMethod = 0;
			long l_EnableAuxExtents = 0;
			unsigned long l_VersionNumber = 0;
			HRESULT hr = SVInspectionImporter::GetProperties(pathName, l_NewDisableMethod, l_EnableAuxExtents, l_VersionNumber);
			if (hr == S_OK)
			{
				bool shouldLoad = l_VersionNumber <= TheSVObserverApp.getCurrentVersion();
				if( shouldLoad )
				{
					// Save it for later, when we exit the dialog
					pObj->SetImportFilename(pathName);
					pObj->SetNewDisableMethod((l_NewDisableMethod) ? "Method 2" : "Method 1");
					pObj->SetEnableAuxiliaryExtent(l_EnableAuxExtents);
				}
				else
				{
					CString strText;
					CString strFile;
					CString strApp;

					::SVGetVersionString( strApp, TheSVObserverApp.getCurrentVersion() );
					::SVGetVersionString( strFile, l_VersionNumber );
					strText.Format(SvO::c_textImportInspectionError, strFile, strApp);
					SvStl::ExceptionMgr1 Exception( SvStl::ExpTypeEnum::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_56_INSPECTION_IMPORT_ERROR, strText, StdExceptionParams, Err_ImportInspectionWrongVersion_2008 );

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
		CString sLabel;
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCursel);

		if( l_Iter != m_Items.end() )
		{
			sLabel = l_Iter->second;
		}

		CSVOInspectionObj *pObj = m_pParent->GetInspectionObjectByName(sLabel);
		if (pObj)
		{
			SVString ipName = sLabel;// pObj->GetInspectionName();

			SVObjectClass* pObject;
			HRESULT hr =  SVObjectManagerClass::Instance().GetObjectByDottedName(ipName.c_str(), pObject);
			if (hr == S_OK && pObject)
			{
				bool bColor = SVObjectManagerClass::Instance().m_bIsColorSVIM;
				LPCTSTR fileExt = (bColor) ? scColorInspectionExportFileExt : scInspectionExportFileExt;
				LPCTSTR fileFilters = (bColor) ? szColorInspectionExportFileFilters : szInspectionExportFileFilters;

				// prompt for path and file name
				DWORD dwFlags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
				bool bFullAccess = TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_UNRESTRICTED_FILE_ACCESS);
				SvMc::SVFileDialog dlg(false, bFullAccess, fileExt, ipName.c_str(), dwFlags, fileFilters, this);
				if (dlg.DoModal() == IDOK)
				{
					CString pathName = dlg.GetPathName();
			   
					// Create XML file
					hr = SVInspectionExporter::Export( pathName, ipName, TheSVObserverApp.getCurrentVersion(), bColor );
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

	CSVOInspectionObj *pObj;

	for (int i = 0; i < iCount; i++)
	{
		pObj = m_pParent->GetInspectionObject(i);
		int iPos = m_ctlIPDlist.AddString(pObj->GetInspectionName());
		m_Items.SetItemData( iPos, pObj->GetInspectionLabelName() );
	}
	m_ctlIPDlist.SetCurSel(0);
}

void CSVOInspectionSourceDlg::OnSelchangeLstIpdList() 
{
	int iCurSel = m_ctlIPDlist.GetCurSel();
	CString sLabel;
	CString sNewTxt;
	CString sCurrentTxt;

	if (m_iCursel == iCurSel)
	{
		m_iEditingSel = iCurSel;
		m_ctlIPDlist.GetText(iCurSel,sCurrentTxt);
		SvMc::CSVOEditorWnd *pEditor = new SvMc::CSVOEditorWnd(&m_ctlIPDlist);
		pEditor->Edit(iCurSel);
		pEditor->WaitForDoneEditing();
		SVDataItemManager::const_iterator l_Iter = m_Items.GetItemData(iCurSel);

		if( l_Iter != m_Items.end() )
		{
			sLabel = l_Iter->second;
		}

		m_ctlIPDlist.GetText(iCurSel,sNewTxt);
		if ( !m_pParent->CanInspectionNameBeUsed(sNewTxt) )
		{
			//Place MessageBox with error...
			m_ctlIPDlist.InsertString(m_iCursel,sCurrentTxt);
			m_Items.SetItemData( m_iCursel, sLabel );
			m_ctlIPDlist.DeleteString(m_iCursel+1);
		}
		else
		{
			m_pParent->RenameInspection(sLabel,sNewTxt);
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

CString CSVOInspectionSourceDlg::GetNextInspectionDisplayName(CString sLabelName)
{
	BOOL bFound = FALSE;
	CString sInsDisplay = _T("Inspection_");
	CString sNewInsName;
	CString sNum = sLabelName.Mid(11);
	int iNum = atoi(sNum);

	while ( !bFound )
	{
		sNewInsName.Format("%s%d",sInsDisplay,iNum);
		if (m_ctlIPDlist.FindStringExact(-1,sNewInsName) != LB_ERR)
		{
			iNum++;
		}
		else
		{
			bFound = TRUE;
		}
	}

	return sNewInsName;
}

BOOL CSVOInspectionSourceDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, NULL, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOInspectionSourceDlg.cpp_v  $
 * 
 *    Rev 1.7   18 Sep 2014 13:40:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLibrary/SVFileDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 May 2014 11:19:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnHelpInfo to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Jan 2014 17:16:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jul 2013 10:16:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   made changes to enable/disable the export button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 May 2013 16:29:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 May 2013 09:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used new GetItemData method that combined the gui get and the data get into one method.  This method returns an iterator to the data container for extraction and updates(if non-const) of data element.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 May 2013 13:26:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 May 2013 11:02:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed GetItemData code in OnBtnPropVi
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:30:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Apr 2013 14:19:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:04:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Mar 2013 13:23:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnBtnExportIpd method to handle export a renamed inspection before the rename is applied.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Mar 2013 08:09:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added #incldue for SVObserver.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Mar 2013 16:42:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Inspection Import and Export buttons and associated logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2013 16:21:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Added new clear list method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Jan 2011 09:07:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to set the ContextHelp ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Nov 2003 13:52:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:06:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
