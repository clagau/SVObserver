//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonEntryDlg.cpp
/// All Rights Reserved 
//*****************************************************************************
/// MonitorEntryDialog displays detail for one Monitorentry
//******************************************************************************

#include "stdafx.h"
#include "MonEntryDlg.h"



IMPLEMENT_DYNAMIC(MonEntryDlg, CDialog)

MonEntryDlg::MonEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MonEntryDlg::IDD, pParent)
		, m_Name(_T(""))
		, m_InspectionStoreId(0)
		, m_ItemId(0)
		, m_Type(0)
		, m_SToreOffset(0)
		, m_Width(0)
		, m_Height(0)
		, m_Pitch(0)
		, m_Size(0)
{

}

MonEntryDlg::~MonEntryDlg()
{
}

void MonEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_INSPECTIONSTORE, m_InspectionStoreId);
	DDX_Text(pDX, IDC_ITEMID, m_ItemId);
	DDX_Text(pDX, IDC_TYPE, m_Type);
	DDX_Text(pDX, IDC_STOREOFFSET, m_SToreOffset);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_PITCH, m_Pitch);
	DDX_Text(pDX, IDC_MSIZE, m_Size);
}


BEGIN_MESSAGE_MAP(MonEntryDlg, CDialog)
	ON_STN_CLICKED(IDC_NAME, &MonEntryDlg::OnStnClickedName)
END_MESSAGE_MAP()


// MonEntryDlg message handlers


void MonEntryDlg::OnStnClickedName()
{
	// TODO: Add your control notification handler code here
}
