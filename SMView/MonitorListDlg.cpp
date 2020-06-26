//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorLIstDlg.cpp 
/// All Rights Reserved 
//*****************************************************************************

/// MonitorLIstDlg dialog  to Display Entries of one Monitorlist 
//******************************************************************************
#include "stdafx.h"
#include "MonitorLIstDlg.h"
#include "SMViewDlg.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#include "MonEntryDlg.h"

IMPLEMENT_DYNAMIC(MonitorListDlg, CDialog)

LPCTSTR MonitorListDlg::ColHeader[] = { _T("List"),_T("Name"),_T("Type"), _T("Size")};
DWORD MonitorListDlg::ColHeaderWidth[] = {40,300,80, 80};

CString  Type2String(DWORD type)
{
	CString ret;
	switch (type)
	{
	case SvPb::SVImageObjectType:
		ret = _T("Image");
		break;
	
	default:
		ret.Format(_T("%X"), static_cast<int>(type));
		break;
	}
	
	return ret;
}

CString Size2String(DWORD size)
{
	CString ret;
	ret.Format(_T("%lu"), size);
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
	CSMViewDlg *pView = dynamic_cast<CSMViewDlg *>(AfxGetMainWnd());
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

	const SvSml:: MonitorListCpy*   pML= pView->m_MemReader.m_MLContainer.GetMonitorListCpyPointer(MonitorListName.GetString());

	//SvSml::MonitorListCpyMap::const_iterator  MoLiit = pView->m_monitorMap.find(MonitorListName.GetString());
	if(pML == nullptr)
	{
		return ;
	}
	m_ListCtrl.DeleteAllItems();
	SvSml::MonitorEntries::const_iterator  Eit; 
	int item(0); 
	LPCTSTR Typename[ SvSml::ListType::Count] = {_T("Prod"),_T("Image"), _T("Rej"), _T("Fail") };
	for(int list  =  0; list  < SvSml::ListType::Count; list++)
	{
		for(Eit = pML->m_MonitorEntries[list].begin(); Eit != pML->m_MonitorEntries[list].end(); ++Eit)
		{
			m_ListCtrl.InsertItem(item, Typename[list]);
			m_ListCtrl.SetItemText(item,Name,Eit->get()->name.c_str());
			m_ListCtrl.SetItemText(item,Type,Type2String( Eit->get()->data.ObjectType) );
			m_ListCtrl.SetItemText(item,ECOL_Size,Size2String(static_cast<DWORD>( Eit->get()->data.ByteSize)));
			item++;
		}
	}
	
}


BOOL MonitorListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int c =0;c <= ECOL_Size; c++)
	{
		/*int ret =	*/m_ListCtrl.InsertColumn(c, ColHeader[c],LVCFMT_LEFT,ColHeaderWidth[c]);
	}
	FillControl();
	return TRUE;
}

void MonitorListDlg::OnBnClickedButtonProperties()
{
	int sel  =  m_ListCtrl.GetSelectionMark();
	if(sel < 0 || sel >=  m_ListCtrl.GetItemCount() )
	{	
		return;
	}
	CString objectName = m_ListCtrl.GetItemText(sel,1); 
	std::string name = objectName.GetString();

	CSMViewDlg *pView = dynamic_cast<CSMViewDlg *>(AfxGetMainWnd());
	if(!pView)
	{
		return;
	}
	
	const SvSml::MonitorEntryPointer pMe   = pView->m_MemReader.m_MLContainer.GetMonitorEntryPointer( name);

	if(pMe.get())
	{

	MonEntryDlg ME_Dlg;
	ME_Dlg.m_Name = pMe->name.c_str();
	ME_Dlg.m_Height = (DWORD)pMe->data.sizeY;
	ME_Dlg.m_InspectionStoreId = pMe->data.m_inspectionStoreId;
	ME_Dlg.m_Pitch = (DWORD) pMe->data.Pitch;
	ME_Dlg.m_Type = pMe->data.ObjectType;
	ME_Dlg.m_Size = (DWORD) pMe->data.ByteSize;
	ME_Dlg.m_Width = (DWORD) pMe->data.sizeX;

	ME_Dlg.DoModal();
	
	}

	

}

BEGIN_MESSAGE_MAP(MonitorListDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_Properties, &MonitorListDlg::OnBnClickedButtonProperties)

END_MESSAGE_MAP()


