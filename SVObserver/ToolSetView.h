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
#pragma endregion Includes

#define ID_EDIT_LABEL_ENDS 40000
#define ID_RUN_ONCE        40001

#pragma region Declarations
class SVIPDoc;
class SVToolClass;
namespace SvOi
{
	class IObjectWriter;
}
#pragma endregion Declarations

class ToolSetView : public CFormView
{
#pragma region Constructor
	ToolSetView(); // protected constructor used by dynamic creation
	virtual ~ToolSetView();
#pragma endregion Constructor

public:
	SVIPDoc* GetIPDoc() const;
	void GetParameters(SvOi::IObjectWriter& rWriter);
	const SVToolGrouping& GetToolGroupings() const;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
	bool CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);

	void SetViewSize(CSize &Size);
	bool IsLabelEditing() const;

	void SetSelectedTool(const SVGUID& rGuid);
	SVGUID GetSelectedTool() const;

	ToolListSelectionInfo GetToolListSelectionInfo() const;
	void HandleExpandCollapse(const std::string& rName, bool bCollapse);
	bool IsEndToolGroupAllowed() const;

	SVToolSetListCtrl& getListCtrl() { return m_toolSetListCtrl; };
	//************************************
	/// Open TA-dialog for tools and comment-page for ToolGrouping.
	/// \returns true if opening is done, false if not done.
	//************************************
	bool enterSelectedEntry();

	/// Check if the current selected tool has errors. If no tool selected it will return false.
	/// \returns bool
	bool hasCurrentToolErrors();

	/// Display the first error of the current selected tool, if a tool is selected and it has an error.
	void displayFirstCurrentToolError();

	/// Return true, if adding of parameters of a tool from the active monitor list possible.
	/// \param ppqName [in] PPQ-name.
	/// \param rToolId [in] Tool ID.
	/// \returns bool
	bool isAddParameter2MonitorListPossible(LPCTSTR ppqName) const;
	/// Return true, if removing of parameters of a tool from the active monitor list possible.
	/// \param ppqName [in] PPQ-name.
	/// \param rToolId [in] Tool ID.
	/// \returns bool
	bool isRemoveParameter2MonitorListPossible(LPCTSTR ppqName) const;

	/// Return true, if all requested parameter of a tool in the active monitor list.
	/// \param ppqName [in] PPQ-name.
	/// \param rToolId [in] Tool ID.
	/// \returns bool
	bool areParametersInMonitorList(LPCTSTR ppqName, const SVGUID& rToolId) const;
	void addParameter2MonitorList(LPCTSTR ppqName);
	void removeParameter2MonitorList(LPCTSTR ppqName);

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

	void ValidateLabelText( std::string& rNewText );
	void RenameItem(int item, const std::string& rOldName, const std::string& rNewName);
	void ToggleExpandCollapse(int item);
	//************************************
	//! Searches the name in the list of all tool and group names case insensitive 
	//! \param rName [in ]
	//! \param lpExclude [in] if not null and the name is found (case sensitive)  this is ignored 
	//! \returns bool  bool true if the name is unique 
	//************************************
	bool CheckName(const std::string& rName, LPCTSTR lpExclude = nullptr) const;
	bool EditToolGroupingComment();

private:
#pragma region Private Methods
	bool ShowDuplicateNameMessage(const std::string& rName) const;
#pragma endregion Private Methods

	SVToolSetListCtrl m_toolSetListCtrl;

	bool m_isLabeling;
	int m_labelingIndex;
	std::string m_LabelSaved;    // To restore label if necessary during editing.
	std::string m_LabelEdited;

	std::string m_duplicateName;			//Store Name for DuplicatenameMessage
	bool m_showDuplicateNameMessage; //DuplictaNameMessage is shown after edeting the name 

};

