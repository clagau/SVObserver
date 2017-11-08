// ******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOProPosQueDlg
// * .File Name       : $Workfile:   SVOProPosQueDlg.cpp  $
// * ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 11:25:46  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOProPosQueDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVMFCControls\SVOSelectItemListDlg.h"
#include "SVOPropertyPageDlg.h"
#include "SVOCameraObj.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "TriggerInformation/SVOTriggerObj.h"
#include "SVOResource\ConstGlobalSvOr.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVOProPosQueDlg property page

IMPLEMENT_DYNCREATE(CSVOProPosQueDlg, CPropertyPage)

CSVOProPosQueDlg::CSVOProPosQueDlg() : CPropertyPage(CSVOProPosQueDlg::IDD)
{
	//{{AFX_DATA_INIT(CSVOProPosQueDlg)
	//}}AFX_DATA_INIT
    m_iVCHorzScroll = 0;
    m_bIsListSetup = FALSE;
}

CSVOProPosQueDlg::~CSVOProPosQueDlg()
{
}

void CSVOProPosQueDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOProPosQueDlg)
	DDX_Control(pDX, IDC_EDT_TRIGGER, m_ctlVTEdit);
	DDX_Control(pDX, IDC_LST_IP, m_ctlVIPList);
	DDX_Control(pDX, IDC_LST_VC, m_ctlVCList);
	DDX_Control(pDX, IDC_LST_PPQ_LIST, m_ctlPPQList);
	DDX_Control(pDX, IDC_BTN_REMOVE_VT, m_ctlRemoveVTbtn);
	DDX_Control(pDX, IDC_BTN_REMOVE_VI, m_ctlRemoveVIbtn);
	DDX_Control(pDX, IDC_BTN_REMOVE_VC, m_ctlRemoveVCbtn);
	DDX_Control(pDX, IDC_BTN_PROP_PPQ, m_ctlPropPPQbtn);
	DDX_Control(pDX, IDC_BTN_NEW_PPQ, m_ctlNewPPQbtn);
	DDX_Control(pDX, IDC_BTN_DELETE_PPQ, m_ctlDeletePPQbtn);
	DDX_Control(pDX, IDC_BTN_ADD_VT, m_ctlAddVTbtn);
	DDX_Control(pDX, IDC_BTN_ADD_VI, m_ctlAddVIbtn);
	DDX_Control(pDX, IDC_BTN_ADD_VC, m_ctlAddVCbtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSVOProPosQueDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSVOProPosQueDlg)
	ON_BN_CLICKED(IDC_BTN_ADD_VC, OnBtnAddVc)
	ON_BN_CLICKED(IDC_BTN_ADD_VI, OnBtnAddVi)
	ON_BN_CLICKED(IDC_BTN_ADD_VT, OnBtnAddVt)
	ON_BN_CLICKED(IDC_BTN_DELETE_PPQ, OnBtnDeletePpq)
	ON_BN_CLICKED(IDC_BTN_NEW_PPQ, OnBtnNewPpq)
	ON_BN_CLICKED(IDC_BTN_PROP_PPQ, OnBtnPropPpq)
	ON_BN_CLICKED(IDC_BTN_REMOVE_VC, OnBtnRemoveVc)
	ON_BN_CLICKED(IDC_BTN_REMOVE_VI, OnBtnRemoveVi)
	ON_BN_CLICKED(IDC_BTN_REMOVE_VT, OnBtnRemoveVt)
	ON_LBN_SELCHANGE(IDC_LST_PPQ_LIST, OnSelchangeLstPpqList)
	ON_LBN_DBLCLK(IDC_LST_PPQ_LIST, OnDblclkLstPpqList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVOProPosQueDlg message handlers

void CSVOProPosQueDlg::OnBtnAddVc() 
{
    int iCnt = m_pParent->GetCameraListCount();
    SVOCameraObjPtr pCameraObj;
    SVOPPQObjPtr pPPQObj( nullptr );
    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR)
    {
		CString PPQName;
		SvMc::CSVOSelectItemListDlg Dlg;
        
		m_ctlPPQList.GetText(iPPQ,PPQName);
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName);

        for (int i = 0; i < iCnt; i++)
        {
            pCameraObj = m_pParent->GetCameraObject(i);
            if ( m_ctlVCList.FindStringExact(-1,pCameraObj->GetCameraDisplayName().c_str()) == LB_ERR )
			{
                Dlg.AddListBoxItem(pCameraObj->GetCameraDisplayName().c_str());
			}
        }

        Dlg.SetDisplayName(_T("Attach Camera"));
        if( IDOK == Dlg.DoModal() )
        {
            if (!Dlg.m_sSelectItemListValue.IsEmpty())
            {
                if (m_ctlVCList.FindStringExact(-1,Dlg.m_sSelectItemListValue) == LB_ERR)
                {
                    m_ctlVCList.AddString(Dlg.m_sSelectItemListValue);
                    pPPQObj->AttachCameraToPPQ(Dlg.m_sSelectItemListValue);
                    m_pParent->SetModified(TRUE);
                    m_pParent->ItemChanged(PPQ_DLG,PPQName,ITEM_PPQ_ADD_CAM);
                }
            }
        }
    }
}

void CSVOProPosQueDlg::OnBtnAddVi() 
{
    int iCnt = m_pParent->GetInspectionListCount();
    SVOInspectionObjPtr pInspectionObj( nullptr );
	SVOPPQObjPtr pPPQObj( nullptr );

    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR )
    {
		SvMc::CSVOSelectItemListDlg Dlg;
        
		CString PPQName;
		std::string InspectName;
		std::string InspectLabel;
		m_ctlPPQList.GetText(iPPQ, PPQName);
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName );

        for (int i = 0; i < iCnt; i++)
        {
            pInspectionObj = m_pParent->GetInspectionObject(i);
			if( nullptr != pInspectionObj )
			{
	            InspectName = pInspectionObj->GetInspectionName();
		        InspectLabel = pInspectionObj->GetInspectionLabelName();
			}
            if ( !m_pParent->IsInspectUsed(InspectLabel.c_str()) )
            {
                Dlg.AddListBoxItem(InspectName.c_str());
            }
        }

        Dlg.SetDisplayName("Attach Inspection");

        if (IDOK == Dlg.DoModal() )
        {
            if (!Dlg.m_sSelectItemListValue.IsEmpty())
            {
                if (m_ctlVIPList.FindStringExact(-1, Dlg.m_sSelectItemListValue) == LB_ERR)
                {
                    m_ctlVIPList.AddString( Dlg.m_sSelectItemListValue );
                    m_pParent->AddUsedInspect(m_pParent->GetInspectionLabelFromName( Dlg.m_sSelectItemListValue ).c_str());
                
                    pPPQObj->AttachInspectionToPPQ(m_pParent->GetInspectionLabelFromName( Dlg.m_sSelectItemListValue ).c_str());
                    m_pParent->SetModified(TRUE);
                    m_pParent->ItemChanged(PPQ_DLG, Dlg.m_sSelectItemListValue, ITEM_PPQ_ADD_INS);
                    m_pParent->ItemChanged(PPQ_DLG, pPPQObj->GetPPQName().c_str(), ITEM_PPQ_ATTACH_INS);
                }
            }
        }
    }
}

void CSVOProPosQueDlg::OnBtnAddVt() 
{
    int iTrigCount = m_pParent->GetTriggerListCount();
    SvTi::SVOTriggerObjPtr pTriggerObj( nullptr );
	SVOPPQObjPtr pPPQObj( nullptr );
    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR )
    {       
		CString sPPQ;
		CString sCurrentTrig;
		SvMc::CSVOSelectItemListDlg Dlg;

        m_ctlPPQList.GetText(iPPQ,sPPQ);

        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ);

        m_ctlVTEdit.GetWindowText(sCurrentTrig);

        for (int i = 0; i < iTrigCount; i++)
        {
			std::string TriggerName;
            pTriggerObj = m_pParent->GetTriggerObject(i);
			if( nullptr != pTriggerObj )
			{
				TriggerName = pTriggerObj->GetTriggerDisplayName();
			}
            if (!m_pParent->IsTriggerUsed( TriggerName.c_str() ))
            {
                Dlg.AddListBoxItem( TriggerName.c_str() );
            }
        }

        Dlg.SetDisplayName("Attach Trigger");
        if ( IDOK == Dlg.DoModal() )
        {
            if (!Dlg.m_sSelectItemListValue.IsEmpty())
            {
                std::string NewTrigger = Dlg.m_sSelectItemListValue;
                m_ctlVTEdit.SetWindowText( NewTrigger.c_str() );
                pPPQObj->DetachTriggerFromPPQ();
                pPPQObj->AttachTriggerToPPQ( NewTrigger.c_str() );
                m_pParent->AddUsedTrigger( NewTrigger.c_str() );
                if (!NewTrigger.empty())
                {
                    if( NewTrigger != sCurrentTrig.GetString() )
                    {//find position of sCurrenTrig in list and remove it...
                        m_pParent->RemoveUsedTrigger(sCurrentTrig);
                        m_pParent->SetModified(TRUE);
                        m_pParent->ItemChanged(PPQ_DLG,sPPQ,ITEM_PPQ_ADD_TRG);
						m_ctlAddVTbtn.EnableWindow(FALSE);
						m_ctlRemoveVTbtn.EnableWindow(TRUE);
                    }
                }
            }
        }
    }
}

void CSVOProPosQueDlg::OnBtnDeletePpq() 
{
    int iCurSel = m_ctlPPQList.GetCurSel();
	SVOPPQObjPtr pPPQObj( nullptr );

    if (iCurSel != LB_ERR)
    {
		CString PPQName;
        m_ctlPPQList.GetText(iCurSel, PPQName);
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName ); 
         
        int iInsCnt = pPPQObj->GetAttachedInspectionCount();

        for (int i = 0; i < iInsCnt; i++)
        {
            std::string InspectionName = pPPQObj->GetAttachedInspection(i);
            m_pParent->ItemChanged(PPQ_DLG, m_pParent->GetInspectionNameFromLabel(InspectionName.c_str()).c_str(), ITEM_PPQ_DEL_INS);
        }

        m_pParent->RemovePPQFromList( PPQName );
        m_ctlPPQList.DeleteString(iCurSel);

        if (iCurSel > 0)
        {
            m_ctlPPQList.SetCurSel(iCurSel-1);
        }
        else
        {
            m_ctlPPQList.SetCurSel(0); //set to top of list
        }
        OnSelchangeLstPpqList();
        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(PPQ_DLG, PPQName, ITEM_ACTION_DELETE);
    }
}

void CSVOProPosQueDlg::OnBtnNewPpq() 
{
    std::string NewPPQ = m_pParent->GetNextPPQName();
    m_pParent->AddToPPQList( NewPPQ.c_str() );
    int iPos = m_ctlPPQList.AddString( NewPPQ.c_str() );
    m_ctlPPQList.SetCurSel(iPos);
    m_pParent->SetModified(TRUE);
    OnSelchangeLstPpqList();
    m_pParent->SetModified(TRUE);
    m_pParent->ItemChanged( PPQ_DLG, NewPPQ.c_str(), ITEM_ACTION_NEW);
}

void CSVOProPosQueDlg::OnBtnPropPpq() 
{
    bool l_bSrcImgProp;
    int iCurSel = m_ctlPPQList.GetCurSel();
    if ( iCurSel != LB_ERR )
    {
        CString PPQName;
        m_ctlPPQList.GetText(iCurSel,PPQName);
        SVOPPQObjPtr pPPQObj = m_pParent->GetPPQObjectByName(PPQName);
		if( nullptr != pPPQObj )
		{
			l_bSrcImgProp = pPPQObj->GetMaintainSourceImageProperty();
			CSVOPropertyPageDlg oDlg;
			SVOPPQObj& rTmpObj( oDlg.getPPQObject() );

			rTmpObj = *pPPQObj;
			oDlg.SetDlgPage(VIRTUAL_PPQ_DLG);
			oDlg.SetProductType(m_pParent->GetProductType());
			if (oDlg.DoModal() == IDOK)
			{
				*pPPQObj = rTmpObj;
				m_pParent->SetModified(TRUE);
				// check to see if MaintainSrcImg property is different
				if ( l_bSrcImgProp != pPPQObj->GetMaintainSourceImageProperty() )
				{
					// property value changed 
					m_pParent->ItemChanged( PPQ_DLG, PPQName, ITEM_PPQ_PROP_SRC_IMG );
				}
				//send down general message that properties have changed
				m_pParent->ItemChanged( PPQ_DLG, PPQName, ITEM_ACTION_PROP );
	        }
		}
    }
}

void CSVOProPosQueDlg::OnBtnRemoveVc() 
{
	SVOInspectionObjPtr pInspectionObj;
	SVOPPQObjPtr pPPQObj( nullptr );
    int iCurPPQ = m_ctlPPQList.GetCurSel();
    int iCurCam = m_ctlVCList.GetCurSel();

    if (iCurCam != LB_ERR)
    {
		CString PPQName;
		CString CameraName;
        m_ctlPPQList.GetText( iCurPPQ, PPQName );
        m_ctlVCList.GetText( iCurCam, CameraName );
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName );
        pPPQObj->DetachCameraFromPPQ( CameraName );
        m_ctlVCList.DeleteString(iCurCam);

		std::string InspectionName;
		long lSize;
		long l;

		lSize = pPPQObj->GetAttachedInspectionCount();
		for( l = 0; l < lSize; l++ )
		{
			InspectionName = pPPQObj->GetAttachedInspection( l );
			pInspectionObj = m_pParent->GetInspectionObjectByName( InspectionName.c_str() );
			if( nullptr != pInspectionObj && CameraName == pInspectionObj->GetToolsetImage().c_str() )
			{
				pInspectionObj->SetToolsetImage( _T("") );
			}// end if

		}// end for

        m_pParent->SetModified( true );
        m_pParent->ItemChanged( PPQ_DLG, PPQName, ITEM_PPQ_DEL_CAM);

    }// end if
}

void CSVOProPosQueDlg::OnBtnRemoveVi() 
{
	SVOPPQObjPtr pPPQObj( nullptr );
    int iCurPPQ = m_ctlPPQList.GetCurSel();
    int iCurIns = m_ctlVIPList.GetCurSel();

    if (iCurIns != LB_ERR)
    {
		CString PPQName;
		CString InspectorName;
		m_ctlVIPList.GetText( iCurIns, InspectorName );
        m_ctlPPQList.GetText( iCurPPQ, PPQName );
        m_ctlVIPList.DeleteString(iCurIns);
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName );
        pPPQObj->DetachInspectionFromPPQ(m_pParent->GetInspectionLabelFromName( InspectorName ).c_str());
        m_pParent->RemoveUsedInspect(m_pParent->GetInspectionLabelFromName( InspectorName ).c_str());
        m_pParent->SetModified( true );
        m_pParent->ItemChanged( PPQ_DLG, InspectorName, ITEM_PPQ_DEL_INS);
        if ( pPPQObj->GetAttachedInspectionCount() < 1 )
        {
            m_pParent->ItemChanged(PPQ_DLG,PPQName,ITEM_PPQ_DETACH_INS);
        }
    }
}

void CSVOProPosQueDlg::OnBtnRemoveVt() 
{
	SVOPPQObjPtr pPPQObj( nullptr );
    int iCurPPQ = m_ctlPPQList.GetCurSel();

    if ( iCurPPQ != LB_ERR )
    {        
		CString sCurrent;
		CString PPQName;
        m_ctlPPQList.GetText( iCurPPQ, PPQName );
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName );

        m_ctlVTEdit.GetWindowText(sCurrent);
        if (!sCurrent.IsEmpty())
        {
            m_pParent->RemoveUsedTrigger(sCurrent);
            m_ctlVTEdit.SetWindowText(_T(""));
            pPPQObj->DetachTriggerFromPPQ();
            m_pParent->SetModified( true );
            m_pParent->ItemChanged( PPQ_DLG, PPQName, ITEM_PPQ_DEL_TRG );
			m_ctlAddVTbtn.EnableWindow( true );
			m_ctlRemoveVTbtn.EnableWindow( false );
        }
    }
}

BOOL CSVOProPosQueDlg::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}

BOOL CSVOProPosQueDlg::OnSetActive() 
{
    if (m_bIsListSetup)
    {
        OnSelchangeLstPpqList();
    }
    
	return CPropertyPage::OnSetActive();
}

BOOL CSVOProPosQueDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    
    m_pParent = (CSVOConfigAssistantDlg*)GetParent()->GetParent();

	SetWindowContextHelpId(IDD+SvOr::HELPFILE_SVORESOURCE_OFFSET);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSVOProPosQueDlg::OnSelchangeLstPpqList() 
{
    int iCursel = m_ctlPPQList.GetCurSel();
	SVOPPQObjPtr pPPQObj( nullptr );
    m_ctlVIPList.ResetContent();
    m_ctlVCList.ResetContent();
    m_ctlVTEdit.SetWindowText(_T(""));

    if (iCursel != LB_ERR)
    {
		CString PPQName;
        m_ctlPPQList.GetText(iCursel, PPQName);
        pPPQObj = m_pParent->GetPPQObjectByName( PPQName );

        int iCam = pPPQObj->GetAttachedCameraCount();
        int iIns = pPPQObj->GetAttachedInspectionCount();

        for (int iC = 0; iC < iCam; iC++)
        {
			std::string CameraName( pPPQObj->GetAttachedCamera(iC) );
            if (m_pParent->IsCameraInList( CameraName.c_str() ))
            {
                m_ctlVCList.AddString( CameraName.c_str() );
            }
            else
            {
                pPPQObj->DetachCameraFromPPQ( CameraName.c_str() );
            }
        }
        m_ctlVCList.SetCurSel(0);

        for (int iI = 0; iI < iIns; iI++)
        {
			std::string InspectionName( pPPQObj->GetAttachedInspection(iI) );
            if (m_pParent->IsInspectionInList( InspectionName.c_str() ))
            {
                m_ctlVIPList.AddString(m_pParent->GetInspectionNameFromLabel( InspectionName.c_str() ).c_str());
            }
            else
            {
                pPPQObj->DetachInspectionFromPPQ( pPPQObj->GetAttachedInspection(iI).c_str() );
            }
        }
        m_ctlVIPList.SetCurSel(0);

		const std::string& rTriggerName( pPPQObj->GetAttachedTriggerName() );
        if (m_pParent->IsTriggerInList( rTriggerName.c_str() ))
        {
            m_ctlVTEdit.SetWindowText( rTriggerName.c_str() );
			m_ctlAddVTbtn.EnableWindow(FALSE);
			m_ctlRemoveVTbtn.EnableWindow(TRUE);
        }
        else
        {
            pPPQObj->DetachTriggerFromPPQ();
            m_ctlVTEdit.SetWindowText(_T(""));
			m_ctlAddVTbtn.EnableWindow(TRUE);
			m_ctlRemoveVTbtn.EnableWindow(FALSE);
        }
    }
}


void CSVOProPosQueDlg::SetupList()
{
    int iCount = m_pParent->GetPPQListCount();
    m_ctlVIPList.ResetContent();
    m_ctlVCList.ResetContent();
    m_ctlPPQList.ResetContent();
    m_ctlVTEdit.SetWindowText(_T(""));

    SVOPPQObjPtr pPPQObj( nullptr );

    for (int i = 0; i < iCount; i++)
    {
        pPPQObj = m_pParent->GetPPQObject(i);
        m_ctlPPQList.AddString( pPPQObj->GetPPQName().c_str() );
    }
     
    m_bIsListSetup = TRUE;
    m_ctlPPQList.SetCurSel(0); //set to first item in list...
                               //also call OnSelChange to reload other list...
    OnSelchangeLstPpqList();
}



void CSVOProPosQueDlg::OnDblclkLstPpqList() 
{
    OnBtnPropPpq();
}

BOOL CSVOProPosQueDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

