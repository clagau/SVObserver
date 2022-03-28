//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "NavigatorElement.h"
#pragma endregion Includes

#pragma region Declarations
class ToolSetView;
#pragma endregion Declarations


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

	void Rebuild(bool correctgrouping  );
	///Updates Images for valid invalid Tools. Valid and Invalid LoopTools have the same 
	///images because the valid status of looptool is not always actual.
	void RebuildImages();

	void SetSingleSelect();
	void SetMultipleSelect();

	void setObjectIds(uint32_t toolsetId, uint32_t inspectionId, bool CorrectGrouping);

	///Get navigator element pointers for the selected element in the ListCtrl
	std::vector<PtrNavigatorElement> GetSelectedNavigatorElements() const;

	///Get Navigator Element Pointer for the index in the ListCtrl
	PtrNavigatorElement GetNavigatorElement(int index) const;
	int GetNavigatorElementIndentation(int index) const;

	PtrNavigatorElement Get1stSelNavigatorElement() const;
	int Get1stSelIndex() const;
	uint32_t Get1stSelToolId() const;
	std::vector<uint32_t> GetAllSelectedToolIds() const;

	void selectTool(uint32_t toolId);
	
	///Select the last element in the list if no element was selected.
	void EnsureOneIsSelected();

	void GetSelectedItemScreenPosition(POINT& rPoint) const;
	void UndoSubSelectionsAllItems();

	void SaveScrollPos();
	void RestoreScrollPos();

	bool mayBeEdited() const;

	/// Display an error message box with the first error, if the tool has errors.
	/// \param toolId [in] Id of the tool.
	/// \returns bool Return true, if this method has displayed an message box, else false.
	bool displayErrorBox(uint32_t toolId) const;

	bool isToolValid(uint32_t tool) const;

protected:
	ToolSetView* GetView();
	const ToolSetView* GetView() const;
	///Images for valid and not valid LoopTools are the same 
	void CreateImageLists();

	int InsertElement(int itemNo, int indent, PtrNavigatorElement &rpNavigatorElement);

	///Insert Delimiter ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///ownerId is Id for the ownwer 
	int  InsertDelimiter(int itemNo, int indent, NavElementType type, uint32_t ownerId);

	///Insert all Subtools ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///id is id for the LoopTool
	int InsertSubTools(int itemNo, int indent, uint32_t id);

	void CollapseItem(int item);
	void ExpandItem(int item);

	void UndoSubSelections(int index);

	CImageList m_ImageList;
	int m_iNone;
	int m_iInvalid;
	int m_expandState;
	int m_fullParameterinML;
	int m_collapseState;
	int m_iTopIndex;
	int m_expandStateInvalid = 0;
	int m_collapseStateInvalid = 0;

	uint32_t m_ToolSetId{ SvDef::InvalidObjectId };
	uint32_t m_InspectionId{ SvDef::InvalidObjectId };
	std::vector<std::shared_ptr<NavigatorElement>> m_NavigatorElementVector;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


