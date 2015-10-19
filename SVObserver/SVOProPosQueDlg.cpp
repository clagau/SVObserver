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

#include "stdafx.h"
#include "SVOProPosQueDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVMFCControls\SVOSelectItemListDlg.h"
#include "SVOPropertyPageDlg.h"
#include "SVOCameraObj.h"
#include "SVOPPQObj.h"
#include "SVOInspectionObj.h"
#include "SVOTriggerObj.h"
#include "SVOResource\ConstGlobalSvOr.h"

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
    CString sCameraName;
    SVOCameraObjPtr pCameraObj;
    SVOPPQObjPtr pPPQObj( nullptr );
    CString sPPQ;
    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR)
    {

		SvMc::CSVOSelectItemListDlg Dlg;
        
		m_ctlPPQList.GetText(iPPQ,sPPQ);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ);

        for (int i = 0; i < iCnt; i++)
        {
            pCameraObj = m_pParent->GetCameraObject(i);
            if ( m_ctlVCList.FindStringExact(-1,pCameraObj->GetCameraDisplayName()) == LB_ERR )
                Dlg.AddListBoxItem(pCameraObj->GetCameraDisplayName());
        }

        Dlg.SetDisplayName("Attach Camera");
        if( IDOK == Dlg.DoModal() )
        {
            if (!Dlg.m_sSelectItemListValue.IsEmpty())
            {
                if (m_ctlVCList.FindStringExact(-1,Dlg.m_sSelectItemListValue) == LB_ERR)
                {
                    m_ctlVCList.AddString(Dlg.m_sSelectItemListValue);
                    pPPQObj->AttachCameraToPPQ(Dlg.m_sSelectItemListValue);
                    m_pParent->SetModified(TRUE);
                    m_pParent->ItemChanged(PPQ_DLG,sPPQ,ITEM_PPQ_ADD_CAM);
                }
            }
        }
    }
}

void CSVOProPosQueDlg::OnBtnAddVi() 
{
    int iCnt = m_pParent->GetInspectionListCount();
    CString sInsname;
    SVOInspectionObjPtr pInspectionObj( nullptr );
	SVOPPQObjPtr pPPQObj( nullptr );
    CString sPPQ;

    CString sInspectName;
    CString sInspectLabel;

    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR )
    {
		SvMc::CSVOSelectItemListDlg Dlg;
        
		m_ctlPPQList.GetText(iPPQ,sPPQ);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ);

        for (int i = 0; i < iCnt; i++)
        {
            pInspectionObj = m_pParent->GetInspectionObject(i);
			if( nullptr != pInspectionObj )
			{
	            sInspectName = pInspectionObj->GetInspectionName();
		        sInspectLabel = pInspectionObj->GetInspectionLabelName();
			}
            if ( !m_pParent->IsInspectUsed(sInspectLabel) )
            {
                Dlg.AddListBoxItem(sInspectName);
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
                    m_pParent->AddUsedInspect(m_pParent->GetInspectionLabelFromName( Dlg.m_sSelectItemListValue ));
                
                    pPPQObj->AttachInspectionToPPQ(m_pParent->GetInspectionLabelFromName( Dlg.m_sSelectItemListValue ));
                    m_pParent->SetModified(TRUE);
                    m_pParent->ItemChanged(PPQ_DLG, Dlg.m_sSelectItemListValue,ITEM_PPQ_ADD_INS);
                    m_pParent->ItemChanged(PPQ_DLG,pPPQObj->GetPPQName(),ITEM_PPQ_ATTACH_INS);
                }
            }
        }
    }
}

void CSVOProPosQueDlg::OnBtnAddVt() 
{
    int iTrigCount = m_pParent->GetTriggerListCount();
    CString sTriggerName;
    SVOTriggerObjPtr pTriggerObj( nullptr );
	SVOPPQObjPtr pPPQObj( nullptr );
    CString sPPQ;
    int iPPQ = m_ctlPPQList.GetCurSel();

    if ( iPPQ != LB_ERR )
    {       
        CString sCurrentTrig;
		SvMc::CSVOSelectItemListDlg Dlg;

        m_ctlPPQList.GetText(iPPQ,sPPQ);

        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ);

        m_ctlVTEdit.GetWindowText(sCurrentTrig);

        for (int i = 0; i < iTrigCount; i++)
        {
			sTriggerName.Empty();
            pTriggerObj = m_pParent->GetTriggerObject(i);
			if( nullptr != pTriggerObj )
			{
				sTriggerName = pTriggerObj->GetTriggerDisplayName();
			}
            if (!m_pParent->IsTriggerUsed(sTriggerName))
            {
                Dlg.AddListBoxItem(sTriggerName);
            }
        }

        Dlg.SetDisplayName("Attach Trigger");
        if ( IDOK == Dlg.DoModal() )
        {
            if (!Dlg.m_sSelectItemListValue.IsEmpty())
            {
                CString sNewTrig = Dlg.m_sSelectItemListValue;
                m_ctlVTEdit.SetWindowText(sNewTrig);
                pPPQObj->DetachTriggerFromPPQ();
                pPPQObj->AttachTriggerToPPQ(sNewTrig);
                m_pParent->AddUsedTrigger(sNewTrig);
                if (!sNewTrig.IsEmpty())
                {
                    if (sNewTrig != sCurrentTrig)
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
    CString sPPQ;
	SVOPPQObjPtr pPPQObj( nullptr );

    if (iCurSel != LB_ERR)
    {
        m_ctlPPQList.GetText(iCurSel,sPPQ);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ); 
         
        int iInsCnt = pPPQObj->GetAttachedInspectionCount();

        for (int i = 0; i < iInsCnt; i++)
        {
            CString sInsName = pPPQObj->GetAttachedInspection(i);
            m_pParent->ItemChanged(PPQ_DLG,m_pParent->GetInspectionNameFromLabel(sInsName),ITEM_PPQ_DEL_INS);
        }


        m_pParent->RemovePPQFromList(sPPQ);
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
        m_pParent->ItemChanged(PPQ_DLG,sPPQ,ITEM_ACTION_DELETE);
    }
}

void CSVOProPosQueDlg::OnBtnNewPpq() 
{
    CString sNewPPQ = m_pParent->GetNextPPQName();
    m_pParent->AddToPPQList(sNewPPQ);
    int iPos = m_ctlPPQList.AddString(sNewPPQ);
    m_ctlPPQList.SetCurSel(iPos);
    m_pParent->SetModified(TRUE);
    OnSelchangeLstPpqList();
    m_pParent->SetModified(TRUE);
    m_pParent->ItemChanged(PPQ_DLG,sNewPPQ,ITEM_ACTION_NEW);
}

void CSVOProPosQueDlg::OnBtnPropPpq() 
{
    bool l_bSrcImgProp;
    int iCurSel = m_ctlPPQList.GetCurSel();
    if ( iCurSel != LB_ERR )
    {
        CString sTxt;
        m_ctlPPQList.GetText(iCurSel,sTxt);
        SVOPPQObjPtr pPPQObj = m_pParent->GetPPQObjectByName(sTxt);
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
					m_pParent->ItemChanged( PPQ_DLG, sTxt, ITEM_PPQ_PROP_SRC_IMG );
				}
				//send down general message that properties have changed
				m_pParent->ItemChanged( PPQ_DLG, sTxt, ITEM_ACTION_PROP );
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
    CString sPPQ;
    CString sCam;

    if (iCurCam != LB_ERR)
    {
        m_ctlPPQList.GetText(iCurPPQ,sPPQ);
        m_ctlVCList.GetText(iCurCam,sCam);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQ);
        pPPQObj->DetachCameraFromPPQ(sCam);
        m_ctlVCList.DeleteString(iCurCam);

		CString sInspect;
		long lSize;
		long l;

		lSize = pPPQObj->GetAttachedInspectionCount();
		for( l = 0; l < lSize; l++ )
		{
			sInspect = pPPQObj->GetAttachedInspection( l );
			pInspectionObj = m_pParent->GetInspectionObjectByName( sInspect );
			if( nullptr != pInspectionObj && sCam == pInspectionObj->GetToolsetImage() )
			{
				pInspectionObj->SetToolsetImage( _T("") );
			}// end if

		}// end for

        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(PPQ_DLG,sPPQ,ITEM_PPQ_DEL_CAM);

    }// end if
}

void CSVOProPosQueDlg::OnBtnRemoveVi() 
{
	SVOPPQObjPtr pPPQObj( nullptr );
    int iCurPPQ = m_ctlPPQList.GetCurSel();
    CString sPPQName;
    int iCurIns = m_ctlVIPList.GetCurSel();
    CString sInsName;

    if (iCurIns != LB_ERR)
    {
        m_ctlVIPList.GetText(iCurIns,sInsName);
        m_ctlPPQList.GetText(iCurPPQ,sPPQName);
        m_ctlVIPList.DeleteString(iCurIns);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQName);
        pPPQObj->DetachInspectionFromPPQ(m_pParent->GetInspectionLabelFromName(sInsName));
        m_pParent->RemoveUsedInspect(m_pParent->GetInspectionLabelFromName(sInsName));
        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(PPQ_DLG,sInsName,ITEM_PPQ_DEL_INS);
        if ( pPPQObj->GetAttachedInspectionCount() < 1 )
        {
            m_pParent->ItemChanged(PPQ_DLG,sPPQName,ITEM_PPQ_DETACH_INS);
        }

    }
}

void CSVOProPosQueDlg::OnBtnRemoveVt() 
{
    CString sCurrent;
    CString sCurPPQ;
	SVOPPQObjPtr pPPQObj( nullptr );
    int iCurPPQ = m_ctlPPQList.GetCurSel();

    if ( iCurPPQ != LB_ERR )
    {        
        m_ctlPPQList.GetText(iCurPPQ,sCurPPQ);
        pPPQObj = m_pParent->GetPPQObjectByName(sCurPPQ);

        m_ctlVTEdit.GetWindowText(sCurrent);
        if (!sCurrent.IsEmpty())
        {
            m_pParent->RemoveUsedTrigger(sCurrent);
            m_ctlVTEdit.SetWindowText(_T(""));
            pPPQObj->DetachTriggerFromPPQ();
            m_pParent->SetModified(TRUE);
            m_pParent->ItemChanged(PPQ_DLG,sCurPPQ,ITEM_PPQ_DEL_TRG);
			m_ctlAddVTbtn.EnableWindow(TRUE);
			m_ctlRemoveVTbtn.EnableWindow(FALSE);
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
    CString sPPQLabel;
	SVOPPQObjPtr pPPQObj( nullptr );
    m_ctlVIPList.ResetContent();
    m_ctlVCList.ResetContent();
    m_ctlVTEdit.SetWindowText(_T(""));

    if (iCursel != LB_ERR)
    {
        m_ctlPPQList.GetText(iCursel,sPPQLabel);
        pPPQObj = m_pParent->GetPPQObjectByName(sPPQLabel);

        int iCam = pPPQObj->GetAttachedCameraCount();
        int iIns = pPPQObj->GetAttachedInspectionCount();

        for (int iC = 0; iC < iCam; iC++)
        {
			const CString CameraName( pPPQObj->GetAttachedCamera(iC) );
            if (m_pParent->IsCameraInList(CameraName))
            {
                m_ctlVCList.AddString(CameraName);
            }
            else
            {
                pPPQObj->DetachCameraFromPPQ(CameraName);
            }
        }
        m_ctlVCList.SetCurSel(0);

        for (int iI = 0; iI < iIns; iI++)
        {
			const CString InspectionName( pPPQObj->GetAttachedInspection(iI) );
            if (m_pParent->IsInspectionInList( InspectionName ))
            {
                m_ctlVIPList.AddString(m_pParent->GetInspectionNameFromLabel( InspectionName ));
            }
            else
            {
                pPPQObj->DetachInspectionFromPPQ( pPPQObj->GetAttachedInspection(iI) );
            }
        }
        m_ctlVIPList.SetCurSel(0);

		const CString TriggerName( pPPQObj->GetAttachedTriggerName() );
        if (m_pParent->IsTriggerInList( TriggerName ))
        {
            m_ctlVTEdit.SetWindowText( TriggerName );
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
        m_ctlPPQList.AddString(pPPQObj->GetPPQName());
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
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, NULL, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOProPosQueDlg.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 11:25:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed OnHelpInfo sendMessage to use DWORD_PTR cast.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:11:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   26 Jan 2011 09:07:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to set the ContextHelp ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Nov 2010 11:08:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Sep 2008 17:06:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use corrected mis-spelled method (GetAttachedTriggerName)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Nov 2003 09:47:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to disable/enable the attach/detach trigger buttons.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Nov 2003 13:46:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Apr 2003 10:37:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  358
 * SCR Title:  Edit Configuration - Rules for Converting configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with dialogs not being deleted.  In attach camera, inspection, and trigger, the select dialog was created but never deleted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 13:27:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2003 13:27:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
