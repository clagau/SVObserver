//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteInputsView
//* .File Name       : $Workfile:   SVDiscreteInputsView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVDiscreteInputsView : public CListView
{
	DECLARE_DYNCREATE(SVDiscreteInputsView)
	DECLARE_MESSAGE_MAP()

	SVDiscreteInputsView();
	virtual ~SVDiscreteInputsView() = default;

protected:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	CListCtrl& m_rCtrl;
	CImageList m_ImageList;
	CImageList m_StateImageList;
	std::map<unsigned long, SVIOEntryHostStructPtr> m_Items;
};

