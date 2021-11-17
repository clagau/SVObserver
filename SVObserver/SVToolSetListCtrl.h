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
#include "NavigatorElement.h"
#pragma endregion Includes

#pragma region Declarations
class ToolSetView;
#pragma endregion Declarations


using NavigatorIndexAndToolId = std::pair<int, uint32_t>;

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
	void setObjectIds(uint32_t toolsetId, uint32_t inspectionId, bool CorrectGrouping);

	///Get Navigator Element Pointer for the Selected Element in the ListCtrl
	//pSelectedIndex gets the selected Index
	NavigatorIndexAndElementVector GetSelectedNavigatorIndexAndElementVector() const;

	///Get Navigator Element Pointer for the index in the ListCtrl
	PtrNavigatorElement GetNavigatorElement(int index) const;
	
	NavigatorIndexAndToolId Get1stSelIndexAndId() const;
	std::vector<uint32_t> GetAllSelectedToolIds() const;

	void SetSelectedToolId(uint32_t toolId);
	
	///Select the last element in the list if no Element was selected.
	void EnsureOneIsSelected();

	void GetSelectedItemScreenPosition(POINT& rPoint) const;

	void SaveScrollPos();
	void RestoreScrollPos();

	bool AllowedToEdit() const;

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

	int InsertElement(int itemNo, int Indend, PtrNavigatorElement &rpNavigatorElement);

	///Insert Delimiter ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///ownerId is Id for the ownwer 
	int  InsertDelimiter(int itemNo, int Indend, NavElementType type, uint32_t ownerId);

	///Insert all Subtools ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///id is id for the LoopTool
	int InsertSubTools(int itemNo, int indent, uint32_t id);

	void CollapseItem(int item);
	void ExpandItem(int item);

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


