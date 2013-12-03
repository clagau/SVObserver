//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCAddRemoveDlg
//* .File Name       : $Workfile:   SVPLCAddRemoveDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Nov 2013 12:44:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVPLCAddRemoveDlg.h"

IMPLEMENT_DYNAMIC(SVPLCAddRemoveDlg, CDialog)

SVPLCAddRemoveDlg::SVPLCAddRemoveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVPLCAddRemoveDlg::IDD, pParent)
{
}

SVPLCAddRemoveDlg::~SVPLCAddRemoveDlg()
{
}

void SVPLCAddRemoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_AvailableList);
	DDX_Control(pDX, IDC_USED_LIST, m_UsedList);
}


BEGIN_MESSAGE_MAP(SVPLCAddRemoveDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &SVPLCAddRemoveDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &SVPLCAddRemoveDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDOK, &SVPLCAddRemoveDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SVPLCAddRemoveDlg message handlers

void SVPLCAddRemoveDlg::OnBnClickedAddBtn()
{
	// TODO: Add your control notification handler code here
	int l_lAddSel = m_AvailableList.GetCurSel();
	if( l_lAddSel > -1 )
	{
		CString l_strPPQ;
		m_AvailableList.GetText( l_lAddSel, l_strPPQ );
		// Not Found..Add to list
		int l_iInsert = m_UsedList.AddString( l_strPPQ );
		m_UsedList.SetItemData( l_iInsert, m_AvailableList.GetItemData( l_lAddSel ) );
		m_AvailableList.DeleteString( l_lAddSel );
	}
}

void SVPLCAddRemoveDlg::OnBnClickedRemoveBtn()
{
	// TODO: Add your control notification handler code here
	int l_lRemoveSel = m_UsedList.GetCurSel();
	if( l_lRemoveSel > -1 )
	{
		CString l_strTmp;
		m_UsedList.GetText( l_lRemoveSel, l_strTmp );
		long l_iInsert = m_AvailableList.AddString( l_strTmp );
		m_AvailableList.SetItemData( l_iInsert, m_UsedList.GetItemData( l_lRemoveSel));
		m_UsedList.DeleteString( l_lRemoveSel );
	}
}

void SVPLCAddRemoveDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_astrSetupPPQs.clear();
	for( int i = 0 ; i < m_UsedList.GetCount() ; i++ )
	{
		CString l_strTmp;
		long l = static_cast<long>(m_UsedList.GetItemData( i ));
		m_astrSetupPPQs.push_back( m_astrAvailablePPQs[ l ]);
	}
	OnOK();
}

BOOL SVPLCAddRemoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Save the originals 
	m_astrOriginalPPQs = m_astrSetupPPQs;

	// Item Data is the available list index.
	for( size_t i = 0 ; i < m_astrAvailablePPQs.size() ; i++ )
	{
		if( std::find( m_astrSetupPPQs.begin(), m_astrSetupPPQs.end(), m_astrAvailablePPQs[i] ) == m_astrSetupPPQs.end() )
		{
			int l_iInsert = m_AvailableList.AddString( _T("PLC Transfer Block(" + m_astrAvailablePPQs[i] + _T(")")));
			m_AvailableList.SetItemData( l_iInsert, i );
		}
		else
		{
			int l_iInsert = m_UsedList.AddString( _T("PLC Transfer Block(" + m_astrAvailablePPQs[i] + _T(")")));
			m_UsedList.SetItemData( l_iInsert, i );
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// This function finds all items that are in the original list but not in the newlist and puts
// them in the outputlist.
HRESULT SVPLCAddRemoveDlg::GetRemoved( CStringVec& OutputList )
{
	OutputList.clear();
	for( size_t i = 0 ; i < m_astrOriginalPPQs.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_astrSetupPPQs.size() ; j++ )
		{
			if( m_astrOriginalPPQs[i] == m_astrSetupPPQs[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			OutputList.push_back( m_astrOriginalPPQs[i] );
		}
	}
	return S_OK;
}

// This function finds all items that are in the New list but not in the Original and puts
// them in the outputlist.
HRESULT SVPLCAddRemoveDlg::GetNewItems( CStringVec& OutputList )
{
	OutputList.clear();
	for( size_t i = 0 ; i < m_astrSetupPPQs.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_astrOriginalPPQs.size() ; j++ )
		{
			if( m_astrSetupPPQs[i] == m_astrOriginalPPQs[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			OutputList.push_back( m_astrSetupPPQs[i] );
		}
	}
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPLCAddRemoveDlg.cpp_v  $
 * 
 *    Rev 1.1   18 Nov 2013 12:44:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added static_casts to build without warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:23:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/