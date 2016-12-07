//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetListCtrl
//* .File Name       : $Workfile:   SVToolSetListCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   19 Dec 2014 04:26:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
class ToolSetView;
#pragma endregion Declarations

class ToolListSelectionInfo // @TODO:  Move to its own file.
{
public:
	const int m_listIndex;			// selected index in the ListCtrl // @WARNING:  bad practice making members public
	const CString m_selection;		// text for current selection // @WARNING:  bad practice making members public

	ToolListSelectionInfo(int listIndex, const CString& selection)
	: m_listIndex(listIndex)
	, m_selection(selection) {}
};

class SVToolSetListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE(SVToolSetListCtrl)

public:
	//{{AFX_MSG(SVToolSetListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVToolSetListCtrl)
	//}}AFX_VIRTUAL
public:
	SVToolSetListCtrl();
	virtual ~SVToolSetListCtrl();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	void Rebuild();
	void RebuildImages();

	void SetSingleSelect();
	void setObjectIds(const SVGUID& toolsetId, const SVGUID& inspectionId);
	
	SVGUID getToolGuid(int index) const;
	SVGUID GetSelectedTool() const;
	void SetSelectedTool(const SVGUID& rGuid);

	ToolListSelectionInfo GetToolListSelectionInfo() const;
	void GetSelectedItemScreenPosition(POINT& rPoint) const;

	void SaveScrollPos();
	void RestoreScrollPos();

	bool AllowedToEdit() const;
	bool IsEndListDelimiter(const CString& text) const;
	bool IsEmptyStringPlaceHolder(const CString& text) const;

protected:
	ToolSetView* GetView();
	const ToolSetView* GetView() const;
	void CreateImageLists();
	int InsertStartGroup(int itemNo, const CString& startName, bool bCollapsed);
	int InsertEndGroup(int itemNo, const CString& endName, bool bCollapsed);
	int InsertTool(int itemNo, int listIndex, bool bCollapsed, int indent);
	void AddEndDelimiter();
	void InsertEmptyString(int itemNo);

	void CollapseItem(int item);
	void ExpandItem(int item);
	bool IsStartGrouping(int index, bool& bState) const;
	bool isToolValid(const SVGUID& tool) const;

	CImageList m_ImageList;

	int m_iNone;
	int m_iInvalid;
	int m_expandState;
	int m_collapseState;
	int m_iTopIndex;

	SvUl::NameGuidList m_taskList;
	SVGUID m_ToolSetId;
	SVGUID m_InspectionId;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


