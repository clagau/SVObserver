//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ToolSetView
//* .File Name       : $Workfile:   ToolSetView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   12 Feb 2015 13:47:08  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVToolSetListCtrl.h"
#include "SVToolGrouping.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#define ID_EDIT_LABEL_ENDS 40000
#define ID_RUN_ONCE        40001

#pragma region Declarations
class SVIPDoc;
class SVToolClass;
class SVObjectWriter;
#pragma endregion Declarations

class ToolSetView : public CFormView
{
#pragma region Constructor
	ToolSetView(); // protected constructor used by dynamic creation
	virtual ~ToolSetView();
#pragma endregion Constructor

public:
	SVIPDoc* GetIPDoc() const;
	BOOL GetParameters(SVObjectWriter& rWriter);
	const SVToolGrouping& GetToolGroupings() const;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
	BOOL CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);

	void SetViewSize(CSize &Size);
	bool IsLabelEditing() const;

	void SetSelectedTool(const SVGUID& rGuid);
	SVGUID GetSelectedTool() const;

	ToolListSelectionInfo GetToolListSelectionInfo() const;
	void HandleExpandCollapse(const SVString& rName, bool bCollapse);
	bool IsEndToolGroupAllowed() const;

	SVToolSetListCtrl& getListCtrl() { return m_toolSetListCtrl; };
	//************************************
	/// Open TA-dialog for tools and comment-page for ToolGrouping.
	/// \returns true if opening is done, false if not done.
	//************************************
	bool enterSelectedEntry();

protected:
	DECLARE_DYNCREATE(ToolSetView)
	enum { IDD = IDD_TOOLSET_VIEW };

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblClkToolSetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickToolSetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRightClickToolSetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabelEnds();
	afx_msg void OnSelectComment();
	afx_msg void OnSelectToolComment();
	afx_msg void OnSelectToolSetReference();
	afx_msg void OnSelectToolNormalize();
	afx_msg void OnEditToolName();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRunOnce();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	virtual void OnInitialUpdate() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

	void ValidateLabelText( SVString& rNewText );
	void RenameItem(int item, const SVString& rOldName, const SVString& rNewName);
	void ToggleExpandCollapse(int item);
	//************************************
	//! Searches the name in the list of all tool and group names case insensitive 
	//! \param rName [in ]
	//! \param lpExclude [in] if not null and the name is found (case sensitive)  this is ignored 
	//! \returns bool  bool true if the name is unique 
	//************************************
	bool CheckName(const SVString& rName, LPCTSTR lpExclude = nullptr) const;
	bool EditToolGroupingComment();

private:
#pragma region Private Methods
	bool ShowDuplicateNameMessage(const SVString& rName) const;
#pragma endregion Private Methods

	SVToolSetListCtrl m_toolSetListCtrl;

	bool m_isLabeling;
	int m_labelingIndex;
	SVString m_LabelSaved;    // To restore label if necessary during editing.
	SVString m_LabelEdited;

	SVString m_duplicateName;			//Store Name for DuplicatenameMessage
	bool m_showDuplicateNameMessage; //DuplictaNameMessage is shown after edeting the name 

};

