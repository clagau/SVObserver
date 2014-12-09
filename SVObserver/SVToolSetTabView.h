//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetTabView
//* .File Name       : $Workfile:   SVToolSetTabView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   09 Dec 2014 10:22:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVToolSetTreeCtrl.h"
#include "SVToolSetListCtrl.h"
#include "SVOutputInfoListTreeCtrl.h"
#pragma endregion Includes

#define ID_EDIT_LABEL_ENDS 40000
#define ID_RUN_ONCE		   40001

#pragma region Declarations
class SVIPDoc;
class SVToolClass;
class SVObjectWriter;
#pragma endregion Declarations

class SVToolSetTabViewClass : public CFormView
{
protected:
	DECLARE_DYNCREATE(SVToolSetTabViewClass)

public:
	//{{AFX_DATA(SVToolSetTabViewClass)
	enum { IDD = IDD_TOOLSETTAB_FORMVIEW };
	SVOutputInfoListTreeCtrlClass m_outputTreeCtrl;
	SVToolSetTreeCtrl m_toolSetTreeCtrl;
	SVToolSetListCtrl m_toolSetListCtrl;
	CTabCtrl m_toolSetTabCtrl;
	//}}AFX_DATA

	// Generated message map functions
	//{{AFX_MSG(SVToolSetTabViewClass)
	afx_msg void OnSelchangeToolsetTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkToolsetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedToolsetTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditToolsetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditToolsetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickToolsetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRightClickToolsetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabelEnds();
	afx_msg void OnSelectToolComment();
	afx_msg void OnSelectToolSetReference();
	afx_msg void OnSelectToolNormalize();
	afx_msg void OnEditToolName();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRunOnce();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	//{{AFX_VIRTUAL(SVToolSetTabViewClass)
public:
	virtual void OnInitialUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

public:
	SVIPDoc* GetIPDoc() const;
	BOOL GetParameters(SVObjectWriter& rWriter);

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
	BOOL CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);

	void SetViewSize(CSize &Size);
	bool IsLabelEditing() const;

	void SetSelectedTool(const SVGUID& rGuid);
	SVGUID GetSelectedTool() const;

	ToolListSelectionInfo GetToolListSelectionInfo() const;
	void HandleExpandCollapse(const CString& name, bool bCollapse);
	bool IsEndToolGroupAllowed() const;
	CString GetSelectedGroup() const;

protected:
	SVToolSetTabViewClass();           // protected constructor used by dynamic creation
	virtual ~SVToolSetTabViewClass();
	void ValidateLabelText(CString& newText);

	void RenameItem(int item, const CString& oldName, const CString& newName);

	void ToggleExpandCollapse(int item);
	bool IsToolsetListCtrlActive() const;

	bool CheckName(const CString& name) const;

	bool m_isLabeling;
	int m_labelingIndex;
	CString m_csLabelSaved;    // To restore label if necessary during editing.
	CString m_csLabelEdited;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetTabView.h_v  $
 * 
 *    Rev 1.6   09 Dec 2014 10:22:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed functions that support Operator Move.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Nov 2014 16:46:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  969
 * SCR Title:  SVObserver Should Check the Name of Group Tools When Naming Other Tools
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed IsGroupNameUnique method.
 * Added CheckName method.
 * Added ShowDuplicateNameMessage function.
 * Revised OnEndlabeleditToolsetList to check for unique name and prompt with Retry/Canel message box if not..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Aug 2014 16:18:36   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   blue frame around active panel new function OnSetFocus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jun 2014 18:32:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed FindLoneStartGroup method.
 * Revised IsEndGroupAllowed method to correct an issue with not finding the proper start/end group pairing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2014 16:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Sep 2013 14:51:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  854
 * SCR Title:  Add HotKey (F2) to Edit a Tool's Name
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added OnEditToolName method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 12:26:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   10 Jan 2013 16:21:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   02 Jan 2013 15:45:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnSelectToolLearn to OnSelectToolSetReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   07 Dec 2012 10:38:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new menu options for the new Shift Tool - Learn & Normalize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   04 Oct 2012 11:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   28 Sep 2012 14:49:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   25 Sep 2012 15:28:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12.1.1   02 Jan 2013 15:14:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnSelectToolLearn to OnSelectToolSetReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12.1.0   05 Dec 2012 12:27:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new menu options for the new Shift Tool - Learn & Normalize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   05 Jul 2012 14:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added context menu item to edit tool comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   02 Jul 2012 17:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   01 Jun 2010 11:10:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   27 Jun 2005 09:57:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed freeze set tab and access to freeze set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   17 Feb 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new Extents.
 * Added CheckParameters(
 * Removed GetViewSize(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Apr 2003 17:07:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   20 Nov 2002 14:42:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetParameters & GetParameters
 * Remove OLE Dispatch Map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Feb 2002 09:46:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  233
 * SCR Title:  Unable to select a tool to edit by double clicking when toolset is large on .5
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed ::OnClickToolsetList to not call SingleRunModeLoop. Instead it posts a message, ID_RUN_ONCE, and SingleRunModeLoop is called in its handler, OnRunOnce
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   Feb 21 2000 10:55:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to show panel control list (Freeze set).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   16 Feb 2000 16:52:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   07 Feb 2000 22:36:38   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added new Output List Tab, must be improved!
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 10:34:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added definition for method to implement 'right click' on tool set list control item.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:54:24   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 28 1999 17:33:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Make sure list item label editor ends when user 'pushes' other buttons or selects menu items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Sep 28 1999 14:40:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  31
 * SCR Title:  Integrate file manager/tracker.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Tool name 'label' edit needs to validate the user input, so a edit validation method was added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
