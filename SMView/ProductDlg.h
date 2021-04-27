//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProductDlg.h
/// All Rights Reserved 
//*****************************************************************************
/// class derived from CDialog to Display results from getProduct getResult and get Failstatus
//******************************************************************************
#pragma once

#pragma region Includes
#include "resource.h"
#include "SVSharedMemoryLibrary/MLProduct.h"
#pragma endregion Includes

class ProductDlg : public CDialog
{
public:
	explicit ProductDlg(SvSml::MLProduct* pProduct, CWnd* pParent = NULL);
	explicit ProductDlg(SvSml::vecpProd *pFailstatus, CWnd* pParent = NULL);
	virtual ~ProductDlg();
	enum ECOL { trigger = 0, name, obj, var,size, data, ecolSize };
	static LPCTSTR ColHeader[ecolSize];
	static DWORD ColHeaderWidth[ecolSize];
	

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRODUCT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL AppendProduct(SvSml::MLProduct* pProduct, int& rfirstline);
	BOOL FillControl();
	DECLARE_MESSAGE_MAP()
public:
	SvSml::MLProduct* m_pProduct;
	SvSml::vecpProd *m_pFailstatus;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();

};
