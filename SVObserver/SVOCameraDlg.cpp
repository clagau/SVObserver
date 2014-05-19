//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCameraDlg
//* .File Name       : $Workfile:   SVOCameraDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 10:25:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVOCameraDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOPropertyPageDlg.h"
#include "SVOCameraObj.h"
#include "SVOInspectionObj.h"
#include "SVImageProcessingClass.h"
#include "SVAcquisitionClass.h"
#include "SVHardwareManifest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSVOCameraDlg::CSVOCameraDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CSVOCameraDlg::IDD)
{
	//{{AFX_DATA_INIT(CSVOCameraDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_bNewConfig = FALSE;
    m_bModified = FALSE;
}

void CSVOCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOCameraDlg)
	DDX_Control(pDX, IDC_CAMERA_MANAGER, m_btnCameraManager);
	DDX_Control(pDX, IDC_LST_CAMERA, m_ctlCameraList);
	DDX_Control(pDX, IDC_BTN_PROP_VC, m_btnVCProp);
	DDX_Control(pDX, IDC_BTN_NEW_VC, m_btnNew);
	DDX_Control(pDX, IDC_BTN_DELETE_VC, m_btnDelete);
	DDX_Control(pDX, IDC_BTN_ADVANCED, m_btnAdvancedProp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVOCameraDlg, CDialog)
	//{{AFX_MSG_MAP(CSVOCameraDlg)
	ON_LBN_SELCHANGE(IDC_LST_CAMERA, OnSelchangeLstCamera)
	ON_BN_CLICKED(IDC_BTN_ADVANCED, OnBtnAdvanced)
	ON_BN_CLICKED(IDC_BTN_DELETE_VC, OnBtnDeleteVc)
	ON_BN_CLICKED(IDC_BTN_NEW_VC, OnBtnNewVc)
	ON_BN_CLICKED(IDC_BTN_PROP_VC, OnBtnPropVc)
	ON_LBN_DBLCLK(IDC_LST_CAMERA, OnDblclkLstCamera)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CAMERA_MANAGER, OnCameraManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVOCameraDlg message handlers

BOOL CSVOCameraDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    m_pParent = (CSVOConfigAssistantDlg*)GetParent()->GetParent();
    m_bNewConfig = m_pParent->GetNewConfigFlag();
	
	if ( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
	{
		// if camera file is specified
		if ( !m_pParent->GetNewConfigFlag() )
		{
			m_btnAdvancedProp.EnableWindow(TRUE);
		}
	}

	if( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() )
		&& SVHardwareManifest::IsDigitalSVIM( TheSVObserverApp.GetSVIMType() ))
	{
		m_btnCameraManager.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnCameraManager.ShowWindow(SW_HIDE);
	}

	SetWindowContextHelpId(IDD+0x20000);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSVOCameraDlg::OnSelchangeLstCamera() 
{
	int iCurSel = m_ctlCameraList.GetCurSel();
	CSVOCameraObj *pObj;
	CString sTxt;

	if ( iCurSel >= 0 )
	{
		m_ctlCameraList.GetText(iCurSel,sTxt);
		pObj = m_pParent->GetCameraObjectByName(sTxt);
		if( pObj != NULL )
		{
			if ( !SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
			{
				if ( pObj->IsFileAcquisition() )
				{
					m_btnAdvancedProp.EnableWindow(TRUE);
				}
				else
				{
					m_btnAdvancedProp.EnableWindow(FALSE);
				}
			}
		}
	}
}

void CSVOCameraDlg::OnBtnAdvanced() 
{
    CSVOCameraObj TmpObj;
    int iCurSel = m_ctlCameraList.GetCurSel();
    CString sTxt;
    m_ctlCameraList.GetText(iCurSel,sTxt);
    CSVOCameraObj *pObj = m_pParent->GetCameraObjectByName(sTxt);
    TmpObj = *pObj;
    CSVOPropertyPageDlg oDlg;
    oDlg.SetCameraObject(&TmpObj);
    oDlg.SetDlgPage(VIRTUAL_CAMERA_ADV);
    oDlg.SetProductType(m_pParent->GetProductType());
	oDlg.SetConfigAssistantDlg(m_pParent);
    if (oDlg.DoModal() == IDOK)
    {
        *pObj = TmpObj;
        m_pParent->SetModified(TRUE);
		if (TmpObj.IsFileAcquisition())
		{
			m_pParent->ItemChanged(CAMERA_DLG,pObj->GetCameraDisplayName(),ITEM_ACTION_PROP);
		}
    }	
}

void CSVOCameraDlg::OnBtnDeleteVc() 
{
	int iCursel = m_ctlCameraList.GetCurSel();
    if (iCursel != LB_ERR)
    {
        CString sTxt;
        m_ctlCameraList.GetText(iCursel,sTxt);
        m_ctlCameraList.DeleteString(iCursel);
        m_pParent->RemoveCameraFromList(sTxt);

		CSVOInspectionObj *pInspect;
		CString sInspect;
		long lSize;
		long l;

		lSize = m_pParent->GetInspectionListCount();
		for( l = 0; l < lSize; l++ )
		{
			pInspect = m_pParent->GetInspectionObject( l );
			if( sTxt == pInspect->GetToolsetImage() )
			{
				pInspect->SetToolsetImage( _T("") );
			}
		}

        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(CAMERA_DLG,sTxt,ITEM_ACTION_DELETE);
    }
    if (iCursel > 0)
    {
        m_ctlCameraList.SetCurSel(iCursel-1);
    }
    else
    {
        m_ctlCameraList.SetCurSel(0);
    }

    if (m_ctlCameraList.GetCount() < m_pParent->GetAllowedNumberOfDigs())
    {
        m_btnNew.EnableWindow(TRUE);
    }
}

void CSVOCameraDlg::OnBtnNewVc() 
{
	CString sNewCamera = m_pParent->GetNextCameraName();
    int iDig  = m_pParent->GetNextCameraNumber() - 1;
    
    m_pParent->AddToCameraList(sNewCamera,iDig);
    CSVOCameraObj *pObj = m_pParent->GetCameraObjectByName(sNewCamera);

    if ( m_pParent->GetProductType() == SVIM_PRODUCT_RGB_MONO )
    {
        iDig = 0;
        int iBands = m_pParent->GetNextCameraNumber() - 1;
        pObj->SetDigNumber(iDig);
        pObj->SetBandNumber(iBands);        
    }

    int iPos = m_ctlCameraList.AddString(sNewCamera);
    m_ctlCameraList.SetCurSel(iPos);
    m_pParent->SetModified(TRUE);
    m_pParent->ItemChanged(CAMERA_DLG,sNewCamera,ITEM_ACTION_NEW);

    if ( m_ctlCameraList.GetCount() >= m_pParent->GetAllowedNumberOfDigs() )
    {
        m_btnNew.EnableWindow(FALSE);
    }	
}

void CSVOCameraDlg::OnBtnPropVc() 
{
    CSVOCameraObj TmpObj;
    int iCurSel = m_ctlCameraList.GetCurSel();
    CString sTxt;
    if (iCurSel != LB_ERR)
    {
        m_ctlCameraList.GetText(iCurSel,sTxt);
        CSVOCameraObj *pObj = m_pParent->GetCameraObjectByName(sTxt);
		if( pObj == NULL )
		{
			ASSERT(FALSE);
			return;
		}
		TmpObj = *pObj;
        CSVOPropertyPageDlg oDlg;
        oDlg.SetCameraObject(&TmpObj);
        oDlg.SetDlgPage(VIRTUAL_CAMERA_DLG);
		oDlg.SetProductType( m_pParent->GetProductType() );
        if (oDlg.DoModal() == IDOK)
        {
			if (TmpObj.IsFileAcquisition())
			{
				*pObj = TmpObj;
				m_pParent->SetModified(TRUE);
				m_btnAdvancedProp.EnableWindow(true);
				m_pParent->ItemChanged(CAMERA_DLG,pObj->GetCameraDisplayName(),ITEM_ACTION_PROP);
			}
			else
			{
				if (pObj->IsFileAcquisition() || !TmpObj.GetCameraFile().IsEmpty())
				{
					*pObj = TmpObj;
					m_pParent->SetModified(TRUE);
					m_pParent->ItemChanged(CAMERA_DLG,pObj->GetCameraDisplayName(),ITEM_ACTION_PROP);
					m_btnAdvancedProp.EnableWindow(FALSE);
				
					if ( (m_bNewConfig) && (!m_bModified) )
					{
						m_bModified = TRUE;
						if (m_pParent->GetCameraListCount() > 1)
						{
							if (AfxMessageBox("Would you like to use this camera file for all camera choices?",MB_YESNO) == IDYES)
							{
								SetCameraPropForAll(pObj->GetCameraDisplayName());
							}
						}
					}
					if ( m_pParent->GetProductType() == SVIM_PRODUCT_RGB_MONO )
					{
						SetCameraPropForAll( pObj->GetCameraDisplayName() );
					}

					if ( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
					{
						m_btnAdvancedProp.EnableWindow(TRUE);
					
						// when the camera file changes, load the camera file parameters into the device (so it's in sync with the Virtual Camera)
						CString strDigName = m_pParent->BuildDigName( pObj );
						SVAcquisitionClassPtr psvDevice;
						if ( S_OK == SVImageProcessingClass::Instance().GetAcquisitionDevice( strDigName, psvDevice ) )
						{
							SVFileNameArrayClass svFiles;
							SVFileNameClass svFile;
							CString strFileName = pObj->GetCameraFile();
							svFile.SetFullFileName( strFileName );
							svFiles.Add( svFile );
							psvDevice->LoadFiles( svFiles );
						}
					}
				}
				else
				{
					if ( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
					{
						m_btnAdvancedProp.EnableWindow(FALSE);
					}
				}
			}
        }
    }
}

void CSVOCameraDlg::SetupList()
{
    m_ctlCameraList.ResetContent();
    int iCamCnt = m_pParent->GetCameraListCount();
    CSVOCameraObj *pObj;

    for (int i = 0; i < iCamCnt; i++)
    {
        pObj = m_pParent->GetCameraObject(i);
        int iPos = m_ctlCameraList.AddString(pObj->GetCameraDisplayName());
        m_ctlCameraList.SetItemData(iPos,pObj->GetDigNumber());
    }
    m_ctlCameraList.SetCurSel(0); // set to top of lsit...
    if ( iCamCnt >= m_pParent->GetAllowedNumberOfDigs() )
    {
        m_btnNew.EnableWindow(FALSE);
    }
    else
    {
        m_btnNew.EnableWindow(TRUE);
    }
	OnSelchangeLstCamera();
}

void CSVOCameraDlg::OnDblclkLstCamera() 
{
	OnBtnPropVc();	
}

void CSVOCameraDlg::SetCameraPropForAll(CString sCurrentCamera)
{
    int iCamCnt = m_pParent->GetCameraListCount();
    CSVOCameraObj *pObj;
    CString sFileName;

    pObj = m_pParent->GetCameraObjectByName(sCurrentCamera);
    sFileName = pObj->GetCameraFile();

    for (int i = 0; i < iCamCnt; i++)
    {
        pObj = m_pParent->GetCameraObject(i);

        if (sCurrentCamera != pObj->GetCameraDisplayName())
        {
            pObj->SetCameraFile(sFileName);
			pObj->SetCameraFileChanged();
            m_pParent->ItemChanged(CAMERA_DLG,pObj->GetCameraDisplayName(),ITEM_ACTION_PROP);
        }
    }
}

BOOL CSVOCameraDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, NULL, reinterpret_cast<DWORD_PTR>(pHelpInfo) );
	return TRUE;
}

void CSVOCameraDlg::OnCameraManager() 
{
	TheSVObserverApp.DisplayCameraManager(m_pParent->GetProductType());
	m_pParent->ItemChanged(CAMERA_DLG, _T(""), ITEM_ACTION_REFRESH);
}

void CSVOCameraDlg::DisableAdvanceBtn()
{
	m_btnAdvancedProp.EnableWindow(FALSE);
}

void CSVOCameraDlg::ShowCameraDlgBtn( int nCmdShow )
{
	m_btnCameraManager.ShowWindow( nCmdShow );
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCameraDlg.cpp_v  $
 * 
 *    Rev 1.4   15 May 2014 10:25:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed OnHelpInfo signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Dec 2013 11:03:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  870
 * SCR Title:  Fix Various Issues in the Edit Configuration Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to show or hide the camera manager button based on system type and configuration type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2013 13:56:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  851
 * SCR Title:  Fix bugs in the Configuration Assistant dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check for NULL to prevent crash.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jun 2013 15:26:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   21 May 2013 13:11:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  828
 * SCR Title:  Camera Binning
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnBtnPropVc to send the camera parameters to the device when the camera file changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:45:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   16 Jan 2013 16:14:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated camera look-up functionality and const correctness of methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   22 Mar 2011 08:01:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   17 Mar 2011 11:08:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  717
 * SCR Title:  Fix issues with advanced button and file acquisition on analog systems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue when going back to hardware for the camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Mar 2011 09:13:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  717
 * SCR Title:  Fix issues with advanced button and file acquisition on analog systems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with advanced button with camera using file acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   26 Jan 2011 09:07:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to set the ContextHelp ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   04 Nov 2010 14:08:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   15 Feb 2010 12:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Jun 2009 08:52:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnCameraManager method to call TheSVObserverApp.DisplayCameraManager using the product type as a parameter for the call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   03 Jun 2009 09:56:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused include files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 Oct 2008 14:24:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnBtnAdvanced to notify parent of Item changed for FileAcquisition 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Sep 2008 14:36:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnBtnPropVc to support FileAcquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Nov 2005 16:06:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to handle new SVIM X1 product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Nov 2005 08:35:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  520
 * SCR Title:  Fix select camera ROI dialog crash
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Format 7 ROI crash (load camera files on selection in new configuration)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Dec 2003 09:37:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method for disabling the advanced button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Nov 2003 13:46:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Nov 2003 09:26:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved OnCameraManager functionality to SVObserverApp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Nov 2003 16:27:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added code to allow disconnecting and reconnecting buffers in preparation for unplugging cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Oct 2003 08:29:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SV1394CameraManagerDlg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Oct 2003 13:17:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes to edit configuration to track camera file changes so that the default light reference will be read from the camera file only initially or when changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 10:39:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 12:29:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Mar 2003 07:12:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed SVIM_RGB_BOARD to SVIM_RGB_MONO_BOARD to match new enumeration in SVObserverEnums.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Feb 2003 15:03:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  326
 * SCR Title:  Camera files are not changing correctly on RGB_MONO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   when changing camera files for any camera on an RGB Mono, will change them all.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
