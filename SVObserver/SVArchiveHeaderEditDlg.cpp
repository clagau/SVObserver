//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveHeaderEditDlg
//* .File Name       : $Workfile:   SVArchiveHeaderEditDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2013 11:34:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

// SVArchiveHeaderEditDlg dialog

SVArchiveHeaderEditDlg::SVArchiveHeaderEditDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVArchiveHeaderEditDlg::IDD, pParent)
	,m_pEdit(nullptr)
	,m_PreviousClient()
{
}

SVArchiveHeaderEditDlg::~SVArchiveHeaderEditDlg()
{
	if( m_pEdit )
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = nullptr;
	}
}

bool SVArchiveHeaderEditDlg::SetValues( const SvDef::StringPairVector& p_astrValues)
{
	m_Strings.clear();
	m_Strings = p_astrValues;
	return true;
}

bool SVArchiveHeaderEditDlg::GetValues(SvDef::StringPairVector& p_astrValues) const
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
			SetWindowPos( nullptr, 0,0,rec.Width(), rec.Height(), SWP_NOMOVE | SWP_NOZORDER );
		}
		else
		{
			HeaderListChangeSize( cx, cy );
			Invalidate();
		}
	}
}

void SVArchiveHeaderEditDlg::OnLvnItemchangedStringListEdit(NMHDR*, LRESULT* pResult)
{
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

	m_HeaderListCtrl.InsertColumn(0, _T("Result Name"));
	m_HeaderListCtrl.InsertColumn(1, _T("Header Label"));

	int iItem = 0;
	for(const auto& pairValue : m_Strings)
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(calcObjectId(pairValue.first));
		if( l_pObject )
		{
			m_HeaderListCtrl.InsertItem(iItem, l_pObject->GetCompleteName().c_str() );
			m_HeaderListCtrl.SetItemText( iItem++, 1, pairValue.second.c_str());
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
		for(SvDef::StringPairVector::iterator it = m_Strings.begin() ; it != m_Strings.end(); ++it )
		{
			it->second = m_HeaderListCtrl.GetItemText(index, 1).GetString();
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


	pOkBtn->SetWindowPos(nullptr, b1x, b1y ,
		0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	pCancelBtn->SetWindowPos(nullptr, b2x, b2y ,
		0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	pStatic->SetWindowPos( nullptr, ListRec.left, T1y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	DisplaySelectedText();

	GetClientRect( m_PreviousClient ); // give us a reference for next size change.
}




void SVArchiveHeaderEditDlg::DisplaySelectedText(  )
{
	POSITION pos = m_HeaderListCtrl.GetFirstSelectedItemPosition();
	//! Double casting required to avoid warnings from 64 to 32 bit conversion
	int iSel =  static_cast<int> (reinterpret_cast<LONGLONG> (pos)) -1;
	if( iSel < 0 )
	{
		iSel = 0;
		m_HeaderListCtrl.SetItemState(0, NSTCIS_SELECTED, NSTCIS_SELECTED );
	}
	if( m_HeaderListCtrl.GetItemCount() > 0 )
	{
		std::string DisplayText = m_HeaderListCtrl.GetItemText(iSel,0).GetString();
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
		CSize textSize = dc->GetTextExtent(DisplayText.c_str());
		dc->SelectObject( pOldFont);

		CRect StaticRec;
		pText->GetWindowRect( StaticRec );
		pText->SetWindowPos( nullptr, 0, 0, newWidth, StaticRec.Height(), SWP_NOZORDER | SWP_NOMOVE | SWP_NOREPOSITION);
		if( textSize.cx < newWidth )
		{   // the string will fit.
			pText->SetWindowText( DisplayText.c_str() );
		}
		else
		{	// The string does not fit so we display a part of it.
			int newLength = static_cast<int>( DisplayText.size() * (double)newWidth /textSize.cx);
			std::string NewString = std::format( _T("{}..."), SvUl::Left( DisplayText, newLength-3).c_str() );
			pText->SetWindowText( NewString.c_str() );
		}
	}
}

