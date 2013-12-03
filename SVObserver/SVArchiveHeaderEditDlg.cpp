//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveHeaderEditDlg
//* .File Name       : $Workfile:   SVArchiveHeaderEditDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   11 Nov 2013 09:25:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVArchiveHeaderEditDlg.h"
#include "afxdialogex.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"


// SVArchiveHeaderEditDlg dialog

IMPLEMENT_DYNAMIC(SVArchiveHeaderEditDlg, CDialog)

SVArchiveHeaderEditDlg::SVArchiveHeaderEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVArchiveHeaderEditDlg::IDD, pParent)
	,m_pEdit(NULL)
	,m_PreviousClient()
{

}

SVArchiveHeaderEditDlg::~SVArchiveHeaderEditDlg()
{
	if( m_pEdit )
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

bool SVArchiveHeaderEditDlg::SetValues( const StringPairVect& p_astrValues)
{
	m_Strings.clear();
	m_Strings = p_astrValues;
	return true;
}

bool SVArchiveHeaderEditDlg::GetValues( StringPairVect& p_astrValues) const
{
	p_astrValues.clear();
	p_astrValues = m_Strings;
	return true;
}

void SVArchiveHeaderEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STRING_LIST_EDIT, m_HeaderListCtrl);

}

void SVArchiveHeaderEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if( ::IsWindow( m_HeaderListCtrl.m_hWnd))
	{
		CRect rec;
		bool bReset = false;
		GetWindowRect(rec);
		if( rec.Width() < m_OriginalRec.Width() )
		{
			rec.right = rec.left + m_OriginalRec.Width();
			bReset = true;
		}
		if(rec.Height() < m_OriginalRec.Height() )
		{
			rec.bottom = rec.top + m_OriginalRec.Height();
			bReset = true;
		}
		if( bReset)
		{
			SetWindowPos( NULL, 0,0,rec.Width(), rec.Height(), SWP_NOMOVE | SWP_NOZORDER );
		}
		else
		{
			HeaderListChangeSize( cx, cy );
			Invalidate();
		}
	}
}

void SVArchiveHeaderEditDlg::OnLvnItemchangedStringListEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	DisplaySelectedText();
	*pResult = 0;
}


BEGIN_MESSAGE_MAP(SVArchiveHeaderEditDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_STRING_LIST_EDIT, &SVArchiveHeaderEditDlg::OnLvnItemchangedStringListEdit)
END_MESSAGE_MAP()



BOOL SVArchiveHeaderEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetClientRect( m_PreviousClient );
	GetWindowRect( m_OriginalRec );

	RECT rec;
	m_HeaderListCtrl.GetClientRect(&rec);
	int iSize = (rec.right - rec.left - 1)/2;
	m_HeaderListCtrl.InsertColumn(0, _T("Result Name"));
	m_HeaderListCtrl.InsertColumn(1, _T("Header Label"));

	int iItem = 0;
	for(StringPairVect::const_iterator l_it = m_Strings.begin() ; l_it != m_Strings.end() ; ++l_it)
	{
		BSTR l_bstGUID = l_it->first.AllocSysString();
		SVGUID l_ObjGUID(l_bstGUID);
		::SysFreeString( l_bstGUID );
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(l_ObjGUID);
		if( l_pObject )
		{
			CString strObjectName = l_pObject->GetCompleteObjectName();
			m_HeaderListCtrl.InsertItem(iItem, strObjectName );
			m_HeaderListCtrl.SetItemText( iItem++, 1, l_it->second);
		}
	}

	CRect ClientRec;
	GetClientRect( ClientRec );
	HeaderListChangeSize(ClientRec.Width(), ClientRec.Height());
	DisplaySelectedText();
	return TRUE;  // return TRUE unless you set the focus to a control
}


void SVArchiveHeaderEditDlg::OnOK()
{
	int iCount = m_HeaderListCtrl.GetItemCount();
	if( iCount > 0 )
	{
		int index = 0;
		for( StringPairVect::iterator it = m_Strings.begin() ; it != m_Strings.end(); ++it )
		{
			char Text[128];
			m_HeaderListCtrl.GetItemText(index,1, Text, 128);
			it->second = Text;
			index++;
		}
	}
	CDialog::OnOK();
}

void SVArchiveHeaderEditDlg::HeaderListChangeSize(int cx, int cy)
{
	CRect ListRec;
	m_HeaderListCtrl.GetWindowRect( ListRec );
	ScreenToClient( ListRec );
	int iMarginAdjustX = ListRec.left*2;
	CRect OkBtnRec;
	CRect CanBtnRec;
	CRect StaticTextRec;
	CButton* pOkBtn = (CButton*)GetDlgItem(IDOK);
	CButton* pCancelBtn = (CButton*)GetDlgItem(IDCANCEL);
	CStatic* pStatic = (CStatic*)GetDlgItem( IDC_SELECTED_TEXT);

	pOkBtn->GetWindowRect( OkBtnRec );
	ScreenToClient( OkBtnRec );

	pCancelBtn->GetWindowRect( CanBtnRec );
	ScreenToClient( CanBtnRec );

	pStatic->GetWindowRect( StaticTextRec );
	ScreenToClient( StaticTextRec );
		
	// Place the Header List
	m_HeaderListCtrl.SetWindowPos(this, 0, 0, cx - iMarginAdjustX, cy - ListRec.top*2 - OkBtnRec.Height(), SWP_NOSENDCHANGING |SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE) ;
	int iSize = (cx - iMarginAdjustX - 4)/2;
	m_HeaderListCtrl.SetColumnWidth( 0, iSize );
	m_HeaderListCtrl.SetColumnWidth(1, iSize );

	// place cancel and ok btns.
	int b1x = (cx - m_PreviousClient.right) + OkBtnRec.left; //(OkBtnRec.right - OkBtnRec.Width()));
	int b1y = (cy - m_PreviousClient.bottom) + OkBtnRec.top; //(OkBtnRec.bottom - OkBtnRec.Height())) ;
	int b2x = (cx - m_PreviousClient.right) + CanBtnRec.left; // (CanBtnRec.right - CanBtnRec.Width()));
	int b2y = (cy - m_PreviousClient.bottom) + CanBtnRec.top; //(CanBtnRec.bottom - CanBtnRec.Height()));
	int T1y = (cy - m_PreviousClient.bottom) + StaticTextRec.top;;


	pOkBtn->SetWindowPos(NULL, b1x, b1y ,
		0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	pCancelBtn->SetWindowPos(NULL, b2x, b2y ,
		0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	pStatic->SetWindowPos( NULL, ListRec.left, T1y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	DisplaySelectedText();

	GetClientRect( m_PreviousClient ); // give us a reference for next size change.
}




void SVArchiveHeaderEditDlg::DisplaySelectedText(  )
{
	POSITION pos = m_HeaderListCtrl.GetFirstSelectedItemPosition();
	int iSel = reinterpret_cast<int>(pos)-1;
	if( iSel < 0 )
	{
		iSel = 0;
		HRESULT hr = m_HeaderListCtrl.SetItemState(0, NSTCIS_SELECTED, NSTCIS_SELECTED );
	}
	if( m_HeaderListCtrl.GetItemCount() > 0 )
	{
		CString strTmp = m_HeaderListCtrl.GetItemText(iSel,0);
		// get position of ok button
		CRect OkRec;
		CRect ListRec;
		m_HeaderListCtrl.GetWindowRect( ListRec);
		ScreenToClient( ListRec );
		GetDlgItem( IDOK )->GetWindowRect(OkRec);
		ScreenToClient(OkRec);
		// calculate how much room we have to place text
		// Fit Static Text box between the List box left and the Ok button left.
		int newWidth = OkRec.left - ListRec.left;

		// Get Text size in pixels ...
		CStatic* pText = static_cast<CStatic*>(GetDlgItem( IDC_SELECTED_TEXT ));
		CDC* dc = GetDC();
		CFont* pFont = pText->GetFont();
		CFont* pOldFont = dc->SelectObject( pFont);
		CSize textSize = dc->GetTextExtent(strTmp);
		dc->SelectObject( pOldFont);

		CRect StaticRec;
		pText->GetWindowRect( StaticRec );
		pText->SetWindowPos( NULL, 0, 0, newWidth, StaticRec.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOREPOSITION);
		if( textSize.cx < newWidth )
		{   // the string will fit.
			pText->SetWindowText( strTmp );
		}
		else
		{	// The string does not fit so we display a part of it.
			int newLength = static_cast<int>( strTmp.GetLength() * (double)newWidth /textSize.cx);
			CString strNewString;
			strNewString.Format( _T("%s..."), strTmp.Left(newLength-3));
			pText->SetWindowText( strNewString );
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVArchiveHeaderEditDlg.cpp_v  $
 * 
 *    Rev 1.0   11 Nov 2013 09:25:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dialog to edit archive tool headers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
