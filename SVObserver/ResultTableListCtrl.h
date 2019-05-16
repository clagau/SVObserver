//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Table Window.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine\SVIPResultData.h"

#pragma endregion Includes

#pragma region Declarations
class SVIPDoc;
#pragma endregion Declarations


class ResultTableListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE(ResultTableListCtrl)
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
public:
	ResultTableListCtrl();
	virtual ~ResultTableListCtrl();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void setIPDoc(SVIPDoc* pDoc) { m_pDoc = pDoc; };
	/// The method updates the list
	void updateList();
#pragma endregion Public Methods

#pragma region Private Methods
private:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	void addColumnHeadings(const std::vector<SvIe::IPResultTableData>& rDataVec);
	void setColumnWidths();
#pragma endregion Private Methods

#pragma region Member variables
private:
	CMenu m_ContextMenuItem;						//Menu object for context menu

	std::vector <SvIe::IPResultTableData> m_ResultData;
	bool m_ColumnWidthSet = false;
	int m_ColumnCount = 0;
	mutable double m_UpdateTimeStamp = 0.0;
	SVIPDoc* m_pDoc = nullptr;
#pragma endregion Member variables
};
