//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Window.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVOCore/SVIPResultData.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma region Declarations
class SVIPDoc;
#pragma endregion Declarations

class ResultListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE( ResultListCtrl )
	DECLARE_MESSAGE_MAP()

#pragma region Constructor
public:
	ResultListCtrl();
	virtual ~ResultListCtrl();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	/// The method updates the list
	void updateList(class SVIPDoc* pDoc);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	SVString CalcProcessesPerSecond(double p_LastTriggerDistance);
	void addColumnHeadings();
	void setColumnWidths();
#pragma endregion Private Methods

#pragma region Member variables
private:
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;

	CMenu m_ContextMenuItem;
	SVResultDefinitionDeque m_ResultDefinitions;
	SVIPResultData m_ResultData;
#pragma endregion Member variables
};

