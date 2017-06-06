//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A child window with a tab-control for the Result Window.
//******************************************************************************

#pragma once
#pragma region Includes
#include "ResultTableListCtrl.h"
#include "ResultListCtrl.h"
#pragma endregion Includes

class ResultTabbedView : public CView
{
	DECLARE_DYNCREATE(ResultTabbedView)
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
protected:
	ResultTabbedView();
	virtual ~ResultTabbedView();
#pragma endregion Constructor

#pragma region Public Methods
public:
	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL GetParameters(SVObjectWriter& rWriter);
	BOOL SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent);
	BOOL CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent);
	void SetViewSize(CSize &Size);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	virtual void OnDraw(CDC* pDC) override;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
#pragma endregion Private Methods

#pragma region Member variables
private:
	CMFCTabCtrl m_TabCtrl;
	ResultListCtrl m_ResultList;
	ResultTableListCtrl m_ResultTableList;
	SVIPDoc* m_pIPDoc;
#pragma endregion Member variables
};
