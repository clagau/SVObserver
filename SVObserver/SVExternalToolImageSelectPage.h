//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVExternalToolImageSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:20  $
//******************************************************************************

#if !defined(AFX_SVEXTERNALTOOLIMAGESELECT_H__7B89817A_8EF9_4C35_9F99_31F2551FBE02__INCLUDED_)
#define AFX_SVEXTERNALTOOLIMAGESELECT_H__7B89817A_8EF9_4C35_9F99_31F2551FBE02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVDlgImage.h"
#include "SVExternalToolTask.h"
#include "SVImageListClass.h"
#include "SVView.h"

class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolImageSelectPage dialog

class SVExternalToolImageSelectPage : public CPropertyPage
{
//	DECLARE_DYNCREATE(SVExternalToolImageSelectPage)

// Construction
public:
	SVExternalToolImageSelectPage(SVExternalToolDetailsSheet* pParent = NULL, int id = IDD );
	~SVExternalToolImageSelectPage();

	void Refresh();
// Dialog Data
	//{{AFX_DATA(SVExternalToolImageSelectPage)
	enum { IDD = IDD_EXTERNAL_TOOL_IMAGES };
	CListBox	m_lbImageList;
	SVDlgImageClass	m_ImageDisplay;
	SVAvailableSourceImageListComboBoxClass	m_cbAvailableImages;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolImageSelectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVExternalToolImageSelectPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeImage();
	afx_msg void OnSelchangeImageList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	SVExternalToolDetailsSheet*			m_pParentDialog;
	SVImageClass*						m_pCurrentSourceImage;
	SVInObjectInfoStruct*				m_pInputImageInfo[SVExternalToolTaskData::NUM_INPUT_IMAGES];
	int									m_iOldIndex;
	SVImageListClass				    m_AvailableImages;
	SVInputInfoListClass				m_ToolInputList;
	SVExternalTool*                     m_pTool;
	SVExternalToolTask*                 m_pTask;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVEXTERNALTOOLIMAGESELECT_H__7B89817A_8EF9_4C35_9F99_31F2551FBE02__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolImageSelectPage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:32:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:58:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/