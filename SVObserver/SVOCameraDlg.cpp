//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraDlg
//* .File Name       : $Workfile:   SVOCameraDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 10:25:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVObserver.h"
#include "SVOCameraDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOPropertyPageDlg.h"
#include "SVOCameraObj.h"
#include "SVOInspectionObj.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/AcquisitionName.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVOCameraDlg::SVOCameraDlg(CWnd* /*=nullptr*/)
	: CPropertyPage(SVOCameraDlg::IDD), m_pParent(nullptr)
{
}

void SVOCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCameraDlg)
	DDX_Control(pDX, IDC_CAMERA_MANAGER, m_btnCameraManager);
	DDX_Control(pDX, IDC_LST_CAMERA, m_ctlCameraList);
	DDX_Control(pDX, IDC_BTN_PROP_VC, m_btnVCProp);
	DDX_Control(pDX, IDC_BTN_NEW_VC, m_btnNew);
	DDX_Control(pDX, IDC_BTN_DELETE_VC, m_btnDelete);
	DDX_Control(pDX, IDC_BTN_ADVANCED, m_btnAdvancedProp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVOCameraDlg, CDialog)
	//{{AFX_MSG_MAP(SVOCameraDlg)
	ON_LBN_SELCHANGE(IDC_LST_CAMERA, OnSelchangeLstCamera)
	ON_BN_CLICKED(IDC_BTN_ADVANCED, OnBtnAdvanced)
	ON_UPDATE_COMMAND_UI(IDC_BTN_ADVANCED, OnUpdateAdvancedBtn)
	ON_BN_CLICKED(IDC_BTN_DELETE_VC, OnBtnDeleteVc)
	ON_BN_CLICKED(IDC_BTN_NEW_VC, OnBtnNewVc)
	ON_BN_CLICKED(IDC_BTN_PROP_VC, OnBtnPropVc)
	ON_LBN_DBLCLK(IDC_LST_CAMERA, OnDblclkLstCamera)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CAMERA_MANAGER, OnCameraManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVOCameraDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    m_pParent = (SVOConfigAssistantDlg*)GetParent()->GetParent();
    m_bNewConfig = m_pParent->GetNewConfigFlag();
	
	if( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() )
		&& SVHardwareManifest::IsDigitalSVIM( TheSVObserverApp().GetSVIMType() ))
	{
		m_btnCameraManager.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnCameraManager.ShowWindow(SW_HIDE);
	}

	SetWindowContextHelpId(IDD+SvOr::HELPFILE_DLG_IDD_OFFSET);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOCameraDlg::OnUpdateAdvancedBtn( CCmdUI* pCmdUI )
{
	bool Enabled( true );

	int iCurSel = m_ctlCameraList.GetCurSel();
	SVOCameraObjPtr pCameraObj;
	CString sTxt;

	if ( iCurSel >= 0 )
	{
		m_ctlCameraList.GetText(iCurSel,sTxt);
		pCameraObj = m_pParent->GetCameraObjectByName(sTxt);
		if( nullptr != pCameraObj )
		{
			if ( !pCameraObj->IsFileAcquisition() )
			{
				if ( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
				{
					if( pCameraObj->GetCameraFile().empty() )
					{
						Enabled = false;
					}
				}
				else
				{
					Enabled = false;
				}
			}
		}
	}
	pCmdUI->Enable( Enabled );
}

void SVOCameraDlg::OnSelchangeLstCamera() 
{
	UpdateDialogControls( this, false );
}

void SVOCameraDlg::OnBtnAdvanced() 
{
    int iCurSel = m_ctlCameraList.GetCurSel();
    CString sTxt;
    m_ctlCameraList.GetText(iCurSel,sTxt);
    SVOCameraObjPtr pCameraObj = m_pParent->GetCameraObjectByName(sTxt);
	if( nullptr != pCameraObj )
	{
		SVOPropertyPageDlg oDlg;
		SVOCameraObj& rTmpObj( oDlg.getCameraObject() );

		rTmpObj = *pCameraObj;
		oDlg.SetDlgPage(VIRTUAL_CAMERA_ADV);
		oDlg.SetProductType(m_pParent->GetProductType());
		oDlg.SetConfigAssistantDlg(m_pParent);
		if (oDlg.DoModal() == IDOK)
		{
			*pCameraObj = rTmpObj;
			m_pParent->SetModified(TRUE);
			if (pCameraObj->IsFileAcquisition())
			{
				m_pParent->ItemChanged(CAMERA_DLG, pCameraObj->GetCameraDisplayName().c_str(), ITEM_ACTION_PROP);
			}
			m_pParent->ItemChanged(CAMERA_DLG, pCameraObj->GetCameraDisplayName().c_str(), ITEM_ACTION_SAVE);
		}	
	}
}

void SVOCameraDlg::OnBtnDeleteVc() 
{
	int iCursel = m_ctlCameraList.GetCurSel();
    if (iCursel != LB_ERR)
    {
        CString CameraName;
        m_ctlCameraList.GetText(iCursel,CameraName);
        m_ctlCameraList.DeleteString(iCursel);
        m_pParent->RemoveCameraFromList(CameraName);

		SVOInspectionObjPtr pInspectionObj( nullptr );
		long lSize;

		lSize = m_pParent->GetInspectionListCount();
		for(long l = 0; l < lSize; l++ )
		{
			pInspectionObj = m_pParent->GetInspectionObject( l );
			if( nullptr != pInspectionObj && CameraName.GetString() == pInspectionObj->GetToolsetImage() )
			{
				pInspectionObj->SetToolsetImage( _T("") );
			}
		}

        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(CAMERA_DLG,CameraName,ITEM_ACTION_DELETE);
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

void SVOCameraDlg::OnBtnNewVc() 
{
	std::string NewCamera = m_pParent->GetNextCameraName();
    int Dig  = m_pParent->GetNextCameraNumber() - 1;
	int CameraID( Dig );
    
    m_pParent->AddToCameraList( NewCamera.c_str(), Dig, CameraID);

    int iPos = m_ctlCameraList.AddString( NewCamera.c_str() );
    m_ctlCameraList.SetCurSel(iPos);
    m_pParent->SetModified(true);
    m_pParent->ItemChanged( CAMERA_DLG, NewCamera.c_str(), ITEM_ACTION_NEW);

    if ( m_ctlCameraList.GetCount() >= m_pParent->GetAllowedNumberOfDigs() )
    {
        m_btnNew.EnableWindow(false);
    }	
}

void SVOCameraDlg::OnBtnPropVc() 
{
    int iCurSel = m_ctlCameraList.GetCurSel();
    CString CameraName;
    if (iCurSel != LB_ERR)
    {
        m_ctlCameraList.GetText(iCurSel, CameraName);
        SVOCameraObjPtr pCameraObj = m_pParent->GetCameraObjectByName( CameraName );
		if( nullptr == pCameraObj )
		{
			assert(FALSE);
			return;
		}
        SVOPropertyPageDlg oDlg;
		SVOCameraObj& rTmpObj( oDlg.getCameraObject() );

        rTmpObj = *pCameraObj;
        oDlg.SetDlgPage(VIRTUAL_CAMERA_DLG);
		oDlg.SetProductType( m_pParent->GetProductType() );
        if (oDlg.DoModal() == IDOK)
        {
			if (rTmpObj.IsFileAcquisition())
			{
				*pCameraObj = rTmpObj;
				m_pParent->SetModified(true);
				m_pParent->ItemChanged(CAMERA_DLG, pCameraObj->GetCameraDisplayName().c_str(), ITEM_ACTION_PROP);
				std::string DigName = SvUl::getAcquisitionName(pCameraObj->GetDigNumber(), pCameraObj->IsFileAcquisition());
				SvIe::SVDigitizerProcessingClass::Instance().SetDigitizerColor( DigName.c_str(), pCameraObj->IsColor() );
			}
			else
			{
				if (pCameraObj->IsFileAcquisition() || !rTmpObj.GetCameraFile().empty() || pCameraObj->IsColor() != rTmpObj.IsColor() )
				{
					*pCameraObj = rTmpObj;
					m_pParent->SetModified(true);
					m_pParent->ItemChanged( CAMERA_DLG, pCameraObj->GetCameraDisplayName().c_str(), ITEM_ACTION_PROP);
					m_btnAdvancedProp.EnableWindow(false);
				
					if ( (m_bNewConfig) && (!m_bModified) )
					{
						//When more than one camera and the camera file is not empty ask if the same setting for all cameras
						if( 1 < m_pParent->GetCameraListCount() && !pCameraObj->GetCameraFile().empty() )
						{
							m_bModified = true;
							SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
							INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_Camera_UseQuestion, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
							if (IDYES == result)
							{
								SetCameraPropForAll(pCameraObj->GetCameraDisplayName().c_str());
							}
						}
					}

					if ( SVHardwareManifest::IsDigitalSVIM( m_pParent->GetProductType() ) )
					{
						// when the camera file changes, load the camera file parameters into the device (so it's in sync with the Virtual Camera)
						int Digitizer = SvIe::SVDigitizerProcessingClass::Instance().getDigitizerID( pCameraObj->GetCameraID() );
						pCameraObj->SetDigNumber( Digitizer );
						std::string DigName = SvUl::getAcquisitionName(pCameraObj->GetDigNumber(), pCameraObj->IsFileAcquisition());
						SvIe::SVDigitizerProcessingClass::Instance().SetDigitizerColor( DigName.c_str(), pCameraObj->IsColor() );
						SvIe::SVAcquisitionClassPtr psvDevice(SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(DigName.c_str()));
						if (nullptr != psvDevice)
						{
							SVFileNameArrayClass svFiles;
							SVFileNameClass svFile;
							svFile.SetFullFileName(pCameraObj->GetCameraFile().c_str());
							svFiles.push_back(svFile);
							if (false == pCameraObj->GetSequenceCameraFile().empty())
							{
								svFile.SetFullFileName(pCameraObj->GetSequenceCameraFile().c_str());
								svFiles.push_back(svFile);
							}
							if (S_OK != psvDevice->LoadFiles(svFiles))
							{
								SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
								Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Config_CameraFileInvalid, {pCameraObj->GetCameraFile()}, SvStl::SourceFileParams(StdMessageParams));
								pCameraObj->SetCameraFile(std::string());
								m_pParent->ItemChanged(CAMERA_DLG, pCameraObj->GetCameraDisplayName().c_str(), ITEM_ACTION_PROP);
							}
						}
					}
				}
			}
			UpdateDialogControls( this, false );
        }
    }
}

void SVOCameraDlg::SetupList()
{
    m_ctlCameraList.ResetContent();
    int iCamCnt = m_pParent->GetCameraListCount();
    SVOCameraObjPtr pObj;

    for (int i = 0; i < iCamCnt; i++)
    {
        pObj = m_pParent->GetCameraObject(i);
        int iPos = m_ctlCameraList.AddString( pObj->GetCameraDisplayName().c_str() );
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

void SVOCameraDlg::OnDblclkLstCamera() 
{
	OnBtnPropVc();	
}

void SVOCameraDlg::SetCameraPropForAll( LPCTSTR CurrentCamera )
{
    int iCamCnt = m_pParent->GetCameraListCount();
    SVOCameraObjPtr pObj;
    std::string FileName;
	bool isColorCamera( false );

    pObj = m_pParent->GetCameraObjectByName( CurrentCamera );
    FileName = pObj->GetCameraFile();
	isColorCamera = pObj->IsColor();

    for (int i = 0; i < iCamCnt; i++)
    {
        pObj = m_pParent->GetCameraObject(i);

        if ( CurrentCamera != pObj->GetCameraDisplayName() )
        {
            pObj->SetCameraFile( FileName );
			pObj->SetIsColor( isColorCamera );
			pObj->SetCameraFileChanged();
            m_pParent->ItemChanged( CAMERA_DLG, pObj->GetCameraDisplayName().c_str(), ITEM_ACTION_PROP);
        }
    }
}

BOOL SVOCameraDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );
	return TRUE;
}

void SVOCameraDlg::OnCameraManager() 
{
	TheSVObserverApp().DisplayCameraManager(m_pParent->GetProductType());
	m_pParent->ItemChanged(CAMERA_DLG, _T(""), ITEM_ACTION_REFRESH);
}


