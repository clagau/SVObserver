//******************************************************************************
/// \copyright (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//******************************************************************************
/// A child window with a tab-control for the Result Window.
//******************************************************************************

#pragma once
#pragma region Includes
#include "ResultTableListCtrl.h"
#include "ResultListCtrl.h"
#include "ResultTabControl.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace SvOi
{
class IObjectWriter;
}

class ResultTabbedView : public CView
{
	DECLARE_DYNCREATE(ResultTabbedView)
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
protected:
	ResultTabbedView() = default;
	virtual ~ResultTabbedView() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	typedef SvXml::SVXMLMaterialsTree SVTreeType;
	


	void GetParameters(SvOi::IObjectWriter& rWriter);
	bool SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent);
	bool CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle hParent);
	void SetViewSize(CSize &Size);
	void tableIdChanged() { m_customTabCtrl.tableIdChanged(); };
	void initializeResultTabControl() { m_customTabCtrl.initializeResultTabControl(); };
#pragma endregion Public Methods

#pragma region Private Methods
private:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual void OnDraw(CDC* pDC) override;
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) override;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
#pragma endregion Private Methods

#pragma region Member variables
private:
	ResultTabControl m_customTabCtrl;
	ResultListCtrl m_ResultList;
	
	ResultTableListCtrl m_ResultTableList;
	SVIPDoc* m_pIPDoc {nullptr};
#pragma endregion Member variables
};
