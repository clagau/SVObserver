//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Table Window.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVResultList.h"
#include "SVOCore\SVIPResultData.h"
#include "SVUtilityLibrary/SVString.h"
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
	/// The method updates the list
	void updateList(class SVIPDoc* pDoc);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	void addColumnHeadings(const std::vector <IPResultTableData>& rDataVec);
	void setColumnWidths();
#pragma endregion Private Methods

#pragma region Member variables
private:
	CMenu m_ContextMenuItem;						//Menu object for context menu

	SVIPResultData m_ResultData;
	BOOL m_ColumnWidthSet;
	int m_ColumnCount;
	mutable double m_UpdateTimeStamp;
#pragma endregion Member variables
};
