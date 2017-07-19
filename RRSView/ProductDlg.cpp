//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProductDlg.cpp
/// All Rights Reserved 
//*****************************************************************************
/// class derived from CDialog to Display results from getProduct getResult and get Failstatus
//******************************************************************************

#include "stdafx.h"
#include "RRSView.h"
#include "ProductDlg.h"
#include "afxdialogex.h"
#include "SVSharedMemoryLibrary\MLProduct.h"

LPCTSTR ProductDlg::ColHeader[] = { _T("Trigger"),_T("Name"),_T("Obj"),_T("Var"), _T("Size"),_T("data") } ;
DWORD ProductDlg::ColHeaderWidth[] = { 40,400,40, 40,40,100 };

// ProductDlg dialog

IMPLEMENT_DYNAMIC(ProductDlg, CDialog)

ProductDlg::ProductDlg(SvSml::MLProduct* pProduct,  CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PRODUCT_DLG, pParent)
{
	m_pProduct = pProduct;
	m_pFailstatus = nullptr;
}

ProductDlg::ProductDlg(SvSml::productPointerVector* pFailstatus, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PRODUCT_DLG, pParent)
{
	m_pProduct = nullptr;
	m_pFailstatus = pFailstatus;
}

ProductDlg::~ProductDlg()
{
}

void ProductDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(ProductDlg, CDialog)
END_MESSAGE_MAP()


// ProductDlg message handlers


BOOL ProductDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_pProduct == nullptr && m_pFailstatus == nullptr)
		return TRUE;


	for (int c = 0; c < ecolSize; c++)
	{
		int ret = m_ListCtrl.InsertColumn(c, ColHeader[c], LVCFMT_LEFT, ColHeaderWidth[c]);
	}
	FillControl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL ProductDlg::FillControl()
{
	if (m_pProduct)
	{
		int line = 0;
		AppendProduct(m_pProduct, line);
	}
	else if (m_pFailstatus)
	{
		int line = 0;
		for (auto& it : *m_pFailstatus)
		{
			AppendProduct(it.get(), line);
		}
	}
	return true;
}
BOOL ProductDlg::AppendProduct(SvSml::MLProduct* pProduct,int& line  )
{
	
	if (!pProduct)
		return FALSE;
	CString csTrigger ;
	csTrigger.Format("%i", pProduct->m_trigger);
	int item = 0;
	CString csobj,csvar,csize;
	
	for (int item = 0; item < pProduct->m_dataEntries.size() && 
		item < pProduct->m_data.size(); item++)
	{
		csobj.Format("%i", pProduct->m_dataEntries[item].get()->data.ObjectType);
		csvar.Format("%i", pProduct->m_dataEntries[item].get()->data.variant_type); 
		csize.Format("%i", pProduct->m_dataEntries[item].get()->data.ByteSize);

		m_ListCtrl.InsertItem(line, csTrigger.GetString());
		m_ListCtrl.SetItemText(line, name, pProduct->m_dataEntries[item].get()->name.c_str());
		m_ListCtrl.SetItemText(line, obj, csobj.GetString());
		m_ListCtrl.SetItemText(line, var, csvar.GetString());
		m_ListCtrl.SetItemText(line, size, csize.GetString());
		m_ListCtrl.SetItemText(line++, data, pProduct->m_data[item].get()->c_str());
	}
		return TRUE;
}