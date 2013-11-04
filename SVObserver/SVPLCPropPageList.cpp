//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropPageList
//* .File Name       : $Workfile:   SVPLCPropPageList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:27:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVPLCPropPageList.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVPLCValidateReasons.h"

IMPLEMENT_DYNAMIC(SVPLCPropPageList, CPropertyPage)

SVPLCPropPageList::SVPLCPropPageList(SVPLCPropSheet* pParent)
	: CPropertyPage(SVPLCPropPageList::IDD)
{
	m_pParent = pParent ;
	m_pEdit = NULL;
}

SVPLCPropPageList::~SVPLCPropPageList()
{
}

void SVPLCPropPageList::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_AddressList);
}


BEGIN_MESSAGE_MAP(SVPLCPropPageList, CPropertyPage)
//	ON_NOTIFY(HDN_ITEMCLICK, 0, &SVPLCPropPageList::OnHdnItemclickList1)
//	ON_WM_LBUTTONDOWN()
//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &SVPLCPropPageList::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// SVPLCPropPageList message handlers

BOOL SVPLCPropPageList::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowText( m_pParent->m_strCurrentPLCId );
	// Populate Block Starting Address List Box from m_pParent->m_ControlParameters.
	CHeaderCtrl* l_pHeader = m_AddressList.GetHeaderCtrl();
	int l_lHeaderCount = l_pHeader->GetItemCount();

	if( l_pHeader->GetItemCount() == 0 )
	{
		m_AddressList.InsertColumn(0,_T("Block Index"), LVCFMT_LEFT, 150);
		m_AddressList.InsertColumn( 1, _T("Address"), LVCFMT_LEFT, 150);
	}
	m_AddressList.DeleteAllItems();
	HRESULT l_hr = S_OK;
	_variant_t l_vTemp;
	long l_lTmpBlockCount = 1;
	l_hr = m_pParent->m_ControlMaterials.GetMaterial( CTAG_PLC_BLOCK_SIZE, l_vTemp );
	if( l_hr == S_OK )
	{
		l_lTmpBlockCount = l_vTemp;
	}
	
	for( long l_lIndex = 0; l_lIndex < l_lTmpBlockCount ; l_lIndex++ )
	{
		l_vTemp.Clear();
		SVString l_strTmp;
		l_strTmp.Format( "%s_%d", CTAG_PLC_BLOCK_START, l_lIndex + 1 );
		l_hr = m_pParent->m_ControlMaterials.GetMaterial( l_strTmp, l_vTemp );
		CString l_strAddress = l_vTemp;
		SVString l_strName;
		l_strName.Format( _T("Block %d"), l_lIndex  );
		m_AddressList.InsertItem( l_lIndex, l_strName.ToString() );
		if( l_strAddress.GetLength() > 0 )
		{
			m_AddressList.SetItemText( l_lIndex, 1, l_strAddress );
		}
		else
		{
			if( l_lIndex != 0 )
			m_AddressList.SetItemText( l_lIndex, 1, m_AddressList.GetItemText(0, 1 ) );
		}
	}
	m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH );

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL SVPLCPropPageList::OnWizardFinish()
{
	// TODO: Add your specialized code here and/or call the base class
	// Copy List back to m_pParent->m_ControlParameters.
//	SVPLCControlPar::VecStringArray l_tempAddresses;
	HRESULT l_hr = S_OK;
	for( int i = 0 ; i < m_AddressList.GetItemCount() ; i++ )
	{

		_variant_t l_vTemp( m_AddressList.GetItemText( i, 1 ) );
		SVString l_strTmp;
		l_strTmp.Format( "%s_%d", CTAG_PLC_BLOCK_START, i + 1 );
		l_hr = m_pParent->m_ControlMaterials.UpdateMaterial( l_strTmp, l_vTemp );
		ASSERT(l_hr == S_OK );
	}

	if( (l_hr = m_pParent->m_ValidateBlocks( m_pParent->m_strCurrentPLCId, m_pParent->m_ControlMaterials)) == S_OK ) // validate( SVMaterials )  function here.
	{
		return CPropertyPage::OnWizardFinish();
	}
	else
	{
		if( l_hr == OVERLAPPING_DATA )
		{
			AfxMessageBox( _T("Data is overlapping!"), MB_OK);
		}
		else
		if( l_hr == PAST_THE_END )
		{
			AfxMessageBox( _T("Data writes past the end of the table"), MB_OK);
		}
		else
		if( l_hr == DIFFERENT_TYPES )
		{
			AfxMessageBox( _T("Data Types do not match"), MB_OK );
		}
		return FALSE;
	}
}


LRESULT SVPLCPropPageList::OnWizardBack()
{

	SVPLCControlPar::VecStringArray l_tempAddresses;
	for( int i = 0 ; i < m_AddressList.GetItemCount() ; i++ )
	{
		l_tempAddresses.push_back( m_AddressList.GetItemText( i, 1 ) );
	}
//	m_pParent->m_ControlParameters.SetBlockAddresses( l_tempAddresses );

	m_pParent->SetWizardButtons( PSWIZB_NEXT );

	return CPropertyPage::OnWizardBack();
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropPageList.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:27:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Sep 2012 15:24:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Sep 2012 18:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:25:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 12:23:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Dec 2008 10:16:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Gray out the back button on the first property page of the plc properties wizard.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

