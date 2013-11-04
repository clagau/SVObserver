//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCPropPageStart
//* .File Name       : $Workfile:   SVPLCPropPageStart.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:27:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVPLCPropPageStart.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"

IMPLEMENT_DYNAMIC(SVPLCPropPageStart, CPropertyPage)

SVPLCPropPageStart::SVPLCPropPageStart( SVPLCPropSheet* pParent /*=NULL*/ )
	: CPropertyPage( SVPLCPropPageStart::IDD )
	, m_lBlockCount(1)
	, m_lQueueSize(0)
{
	m_pParent = pParent;

}

SVPLCPropPageStart::~SVPLCPropPageStart()
{
}

void SVPLCPropPageStart::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BLOCK_SIZE_EDIT, m_lBlockCount);
	DDX_Text(pDX, IDC_QUEUE_SIZE_EDIT, m_lQueueSize);
}


BEGIN_MESSAGE_MAP(SVPLCPropPageStart, CPropertyPage)
END_MESSAGE_MAP()


// SVPLCPropPageStart message handlers

BOOL SVPLCPropPageStart::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowText( m_pParent->m_strCurrentPLCId );
	_variant_t l_vVariant;
	m_pParent->m_ControlMaterials.GetMaterial( CTAG_PLC_BLOCK_SIZE, l_vVariant );
	m_lBlockCount = l_vVariant;

	m_pParent->m_ControlMaterials.GetMaterial( CTAG_PLC_QUEUE_SIZE, l_vVariant );
	m_lQueueSize = l_vVariant;
	UpdateData( FALSE );
	m_pParent->SetWizardButtons(PSWIZB_NEXT );

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT SVPLCPropPageStart::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	if( m_lBlockCount < 1 || m_lBlockCount > 30 )
	{
		AfxMessageBox( "Block count must be a number from 1 to 30.");
		return -1;
	}
	if( m_lQueueSize < 1 || m_lQueueSize > 30 )
	{
		AfxMessageBox( "Queue size must be a number from 1 to 30.");
		return -1;
	}
	_variant_t l_vTemp;

	l_vTemp = m_lBlockCount;
	m_pParent->m_ControlMaterials.UpdateMaterial( CTAG_PLC_BLOCK_SIZE, l_vTemp );
	l_vTemp.Clear();

	l_vTemp = m_lQueueSize;
	m_pParent->m_ControlMaterials.UpdateMaterial( CTAG_PLC_QUEUE_SIZE, l_vTemp );
	l_vTemp.Clear();

	m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH );
	if( m_pParent->GetPage(1)->m_hWnd ) // Re-initialize dialog
	{
		if( m_pParent->GetPage(1)->IsWindowEnabled() )
		{
			m_pParent->GetPage(1)->OnInitDialog();
		}
	}
	return CPropertyPage::OnWizardNext();
}

BOOL SVPLCPropPageStart::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
    {
		if(pMsg->wParam== VK_DOWN )
		{
			pMsg->wParam = VK_TAB;
		}
		if(pMsg->wParam== VK_UP )
		{
			pMsg->wParam = VK_SHIFT ;
			PostMessage( WM_KEYDOWN, VK_TAB );
			PostMessage( WM_KEYUP, VK_TAB );
		}
		if( GetFocus()->GetDlgCtrlID() != PSWIZB_NEXT && pMsg->wParam == VK_RETURN )
		{
			pMsg->wParam = VK_TAB;
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCPropPageStart.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:27:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Sep 2012 15:24:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Sep 2012 18:29:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Dec 2010 11:28:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 12:25:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jan 2009 08:54:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed behavior of return key in PLC Edit Dialogs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2009 13:05:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added limits to Queue size and Block size
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
 *    Rev 1.0   14 Nov 2008 14:09:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

