//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorLIstDlg.cpp 
/// All Rights Reserved 
//*****************************************************************************

/// MonitorLIstDlg dialog  to Display Entries of one Monitorlist 
//******************************************************************************
#include "stdafx.h"
#include "RRSView.h"
#include "MonitorLIstDlg.h"
#include "RRSViewDlg.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"

IMPLEMENT_DYNAMIC(MonitorListDlg, CDialog)

LPCTSTR MonitorListDlg::ColHeader[] = { _T("List"),_T("Name"),_T("Type"), _T("Size")};
DWORD MonitorListDlg::ColHeaderWidth[] = {40,300,80, 80};

CString  Type2String(DWORD type)
{
	CString ret;
	switch (type)
	{
	case 0x11:
		ret = _T("Image");
		break;
	case 0x100:
		ret = _T("DWORD");
		break;
	case 0x101:
		ret = _T("long");
		break;

	case 0x102:
		ret = _T("double");
		break;
	case 0x103:
		ret = _T("bool");
		break;

	case 0x104:
		ret = _T("point");
		break;
	case 0x105:
		ret = _T("Byte");
		break;
	case 0x106:
		ret = _T("String");
		break;
	case 0x107:
		ret = _T("enum");
		break;
		/*
		SVDPointValueObjectType					= 0x00000108,
		SVInt64ValueObjectType					= 0x00000109,
		SVCharValueObjectType					= 0x0000010A,
		SVVariantValueObjectType				= 0x0000010B,
		DoubleSortValueObjectType				= 0x0000010C,*/
	default:
		ret.Format(_T("%X"), type);
		break;
	}
	
	return ret;
}

CString Size2String(DWORD size)
{
	CString ret;
	ret.Format(_T("%i"), size);
	return ret;
}

MonitorListDlg::MonitorListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MonitorListDlg::IDD, pParent)
{
}

MonitorListDlg::~MonitorListDlg()
{
}

void MonitorListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONITORLIST, m_ListCtrl);
}


void MonitorListDlg::FillControl()
{
	CRRSViewDlg *pView = dynamic_cast<CRRSViewDlg *>(AfxGetMainWnd());
	if(!pView)
	{
		return;
	}
	int sel  = pView->m_MonListsCtrl.GetSelectionMark();
	if(sel < 0 || sel >=  pView->m_MonListsCtrl.GetItemCount() )
	{	
		return;
	}
	CString MonitorListName = pView->m_MonListsCtrl.GetItemText(sel,0); 
	SvSml::MonitorListCpyMap::const_iterator  MoLiit = pView->m_monitorMap.find(MonitorListName.GetString());
	if(MoLiit == pView->m_monitorMap.end())
	{
		return ;
	}
	m_ListCtrl.DeleteAllItems();
	SvSml::MonitorEntryVector::const_iterator  Eit; 
	int item(0);
	for(Eit = MoLiit->second.prodItems.begin(); Eit != MoLiit->second.prodItems.end(); Eit++)
	{
		m_ListCtrl.InsertItem(item, _T("Prod"));
		m_ListCtrl.SetItemText(item,Name,Eit->name.c_str());
		m_ListCtrl.SetItemText(item,Type,Type2String( Eit->type) );
		m_ListCtrl.SetItemText(item,ECOL_Size,Size2String( Eit->size));
		item++;
	}
	for(Eit = MoLiit->second.rejctCond.begin(); Eit != MoLiit->second.rejctCond.end(); Eit++)
	{
		m_ListCtrl.InsertItem(item, _T("Rej"));
		m_ListCtrl.SetItemText(item,Name,Eit->name.c_str());
		m_ListCtrl.SetItemText(item,Type,Type2String( Eit->type) );
		m_ListCtrl.SetItemText(item,ECOL_Size,Size2String( Eit->size));
		item++;
	}
	for(Eit = MoLiit->second.failStats.begin(); Eit != MoLiit->second.failStats.end(); Eit++)
	{
		m_ListCtrl.InsertItem(item, _T("Fail"));
		m_ListCtrl.SetItemText(item,Name,Eit->name.c_str());
		m_ListCtrl.SetItemText(item,Type,Type2String( Eit->type) );
		m_ListCtrl.SetItemText(item,ECOL_Size,Size2String( Eit->size));
		item++;
	}
}


BOOL MonitorListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int c =0;c <= ECOL_Size; c++)
	{
		int ret =	m_ListCtrl.InsertColumn(c, ColHeader[c],LVCFMT_LEFT,ColHeaderWidth[c]);
	}
	FillControl();
	return TRUE;
}

BEGIN_MESSAGE_MAP(MonitorListDlg, CDialog)
END_MESSAGE_MAP()


