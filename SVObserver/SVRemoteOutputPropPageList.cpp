//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputPropPageList
//* .File Name       : $Workfile:   SVRemoteOutputPropPageList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:58:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVRemoteOutputPropPageList.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
//#include "SVDLLValidateReasons.h"

IMPLEMENT_DYNAMIC(SVRemoteOutputPropPageList, CPropertyPage)

SVRemoteOutputPropPageList::SVRemoteOutputPropPageList(SVRemoteOutputPropSheet* pParent)
	: CPropertyPage(SVRemoteOutputPropPageList::IDD)
{
	m_pParent = pParent ;
	m_pEdit = NULL;
}

SVRemoteOutputPropPageList::~SVRemoteOutputPropPageList()
{
}

void SVRemoteOutputPropPageList::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	// BRW - PLC has been deprecated.
	//DDX_Control(pDX, IDC_LIST1, m_AddressList);
}


BEGIN_MESSAGE_MAP(SVRemoteOutputPropPageList, CPropertyPage)
//	ON_NOTIFY(HDN_ITEMCLICK, 0, &SVRemoteOutputPropPageList::OnHdnItemclickList1)
//	ON_WM_LBUTTONDOWN()
//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &SVRemoteOutputPropPageList::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// SVRemoteOutputPropPageList message handlers

BOOL SVRemoteOutputPropPageList::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowText( m_pParent->m_strCurrentDLLId );
	// Populate Block Starting Address List Box from m_pParent->m_ControlParameters.
	//CHeaderCtrl* l_pHeader = m_AddressList.GetHeaderCtrl();
	//int l_lHeaderCount = l_pHeader->GetItemCount();

	//if( l_pHeader->GetItemCount() == 0 )
	//{
	//	m_AddressList.InsertColumn(0,_T("Desctiption"), LVCFMT_LEFT, 150);
	//	m_AddressList.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 150);
	//}
	//m_AddressList.DeleteAllItems();
	//HRESULT l_hr = S_OK;
	//_variant_t l_vTemp;
	//long l_lTmpBlockCount = 1;
//	l_hr = m_pParent->m_ControlMaterials.GetMaterial( CTAG_DLL_BLOCK_SIZE, l_vTemp );
//	if( l_hr == S_OK )
//	{
//		l_lTmpBlockCount = l_vTemp;
//	}
	
	m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH );

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL SVRemoteOutputPropPageList::OnWizardFinish()
{
	// TODO: Add your specialized code here and/or call the base class
	// Copy List back to m_pParent->m_ControlParameters.
//	SVDLLControlPar::VecStringArray l_tempAddresses;
	// BRW - PLC has been deprecated.
	//HRESULT l_hr = S_OK;
	//for( int i = 0 ; i < m_AddressList.GetItemCount() ; i++ )
	//{

		//_variant_t l_vTemp( m_AddressList.GetItemText( i, 1 ) );
		//SVString l_strTmp;
		//l_strTmp.Format( "%s_%d", CTAG_DLL_BLOCK_START, i + 1 );
		//l_hr = m_pParent->m_ControlMaterials.SetMaterial( l_strTmp, l_vTemp );
		//ASSERT(l_hr == S_OK );
	//}

	//if( (l_hr = m_pParent->m_ValidateBlocks( m_pParent->m_strCurrentDLLId, m_pParent->m_ControlMaterials)) == S_OK ) // validate( SVMaterials )  function here.
	//{
	//	return CPropertyPage::OnWizardFinish();
	//}
	//else
	//{
	//	if( l_hr == OVERLAPPING_DATA )
	//	{
	//		AfxMessageBox( _T("Data is overlapping!"), MB_OK);
	//	}
	//	else
	//	if( l_hr == PAST_THE_END )
	//	{
	//		AfxMessageBox( _T("Data writes past the end of the table"), MB_OK);
	//	}
	//	else
	//	if( l_hr == DIFFERENT_TYPES )
	//	{
	//		AfxMessageBox( _T("Data Types do not match"), MB_OK );
	//	}
	//	return FALSE;
	//}
	return CPropertyPage::OnWizardFinish();
}


LRESULT SVRemoteOutputPropPageList::OnWizardBack()
{

//	SVDLLControlPar::VecStringArray l_tempAddresses;
//	for( int i = 0 ; i < m_AddressList.GetItemCount() ; i++ )
//	{
//		l_tempAddresses.push_back( m_AddressList.GetItemText( i, 1 ) );
//	}
//	m_pParent->m_ControlParameters.SetBlockAddresses( l_tempAddresses );

	m_pParent->SetWizardButtons( PSWIZB_NEXT );

	return CPropertyPage::OnWizardBack();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteOutputPropPageList.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 07:58:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:52:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
