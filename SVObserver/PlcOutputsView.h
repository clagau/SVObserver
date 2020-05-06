//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file PlcOutputsView
/// This class is a list view for the PLC connected outputs
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVIODoc;

class PlcOutputsView : public CListView
{
	DECLARE_DYNCREATE(PlcOutputsView)
	DECLARE_MESSAGE_MAP()

	PlcOutputsView();
	virtual ~PlcOutputsView() = default;

protected:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

#pragma region Member variables
private:
	CListCtrl& m_rCtrl;
	CImageList m_ImageList;
	CImageList m_StateImageList;
	std::map<unsigned long, SVIOEntryHostStructPtr> m_Items;
#pragma endregion Member variables
};
