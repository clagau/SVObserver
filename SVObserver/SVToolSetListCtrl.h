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
#include "ObjectInterfaces/ObjectInfo.h"
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

	void Rebuild();
	///Updates Images for valid invalid Tools. Valid and Invalid LoopTools have the same 
	///images because the valid status of looptool is not always actual.
	void RebuildImages();

	void SetSingleSelect();
	void setObjectIds(const SVGUID& toolsetId, const SVGUID& inspectionId);

	///Get Navigator Element Pointer for the Selected Element in the ListCtrl
	//pSelectedIndex gets the selected Index
	PtrNavigatorElement GetSelectedNavigatorElement(int* pSelectedIndex = nullptr) const;

	///Get Navigator Element Pointer for the index in the ListCtrl
	PtrNavigatorElement GetNavigatorElement(int index) const;

	SVGUID GetSelectedTool() const;
	void SetSelectedTool(const SVGUID& rGuid);
	
	///Select the last element in the list if no Element was selected.
	void EnsureOneIsSelected();

	void GetSelectedItemScreenPosition(POINT& rPoint) const;

	void SaveScrollPos();
	void RestoreScrollPos();

	bool AllowedToEdit() const;

	/// Display an error message box with the first error, if the tool has errors.
	/// \param rGuid [in] Guid of the tool.
	/// \returns bool Return true, if this method has displayed an message box, else false.
	bool displayErrorBox(const SVGUID& rGuid) const;

	bool isToolValid(const SVGUID& tool) const;
protected:
	ToolSetView* GetView();
	const ToolSetView* GetView() const;
	///Images for valid and not valid LoopTools are the same 
	void CreateImageLists();

	int InsertElement(int itemNo, int Indend, PtrNavigatorElement &rpNavigatorElement);

	///Insert Delimiter ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///rOwnerGuid is Guid for the ownwer 
	int  InsertDelimiter(int itemNo, int Indend, NavElementType type, const GUID& rOwnerGuid);

	///Insert all Subtools ItemNo is firstIndex in Listcontrol
	/// indent is indentation 
	///rGuid is Guid for the LoopTool
	int InsertSubTools(int itemNo, int indent, const GUID& rGuid);

	void CollapseItem(int item);
	void ExpandItem(int item);

	CImageList m_ImageList;
	int m_iNone;
	int m_iInvalid;
	int m_expandState;
	int m_fullParameterinML;
	int m_collapseState;
	int m_iTopIndex;
	int m_expandStateLoopToolValid = 0;
	int m_collapseStateLoopToolValid = 0;
	int m_expandStateLoopToolInvalid = 0;
	int m_collapseStateLoopToolInvalid = 0;

	SVGUID m_ToolSetId;
	SVGUID m_InspectionId;
	std::vector<std::shared_ptr<NavigatorElement>> m_NavigatorElementVector;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


