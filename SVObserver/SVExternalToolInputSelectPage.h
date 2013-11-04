//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolInputSelectPage
//* .File Name       : $Workfile:   SVExternalToolInputSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:38  $
//******************************************************************************

#ifndef SVEXTERNALTOOLINPUTSELECTPAGE_H
#define SVEXTERNALTOOLINPUTSELECTPAGE_H

#include "PropertyTree/PropTree.h"

class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;
class SVObjectClass;

class SVExternalToolInputSelectPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(SVExternalToolInputSelectPage)

// Construction
public:
	SVExternalToolInputSelectPage(const CString& sTitle, SVExternalToolDetailsSheet* pParent = NULL, int id = IDD );
	~SVExternalToolInputSelectPage();

	CString							m_sGroupName;
	bool							m_bTabbed;

// Dialog Data
	//{{AFX_DATA(SVExternalToolInputSelectPage)
	enum { IDD = IDD_EXTERNAL_TOOL_INPUTS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolInputSelectPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int SelectObject(CString& rsSelectedName, SVRPropertyItem* pItem);
	SVObjectClass* FindObject(SVRPropertyItem* pItem);
	HRESULT ValidateItem(SVRPropertyItem* pItem);
	int GetItemIndex(SVRPropertyItem* pItem);

	SVExternalToolDetailsSheet*		m_pParentDialog;
	SVExternalToolTask*				m_pTask;
	SVExternalTool*					m_pTool;

	enum
	{
		ID_BASE = 100,
	};

	SVRPropTree	m_Tree;


	// Generated message map functions
	//{{AFX_MSG(SVExternalToolInputSelectPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult);
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolInputSelectPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:32:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:52:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Feb 2012 10:08:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  753
 * SCR Title:  Fix input selector for the External Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with the input selector. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 10:18:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
