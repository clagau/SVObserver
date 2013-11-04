//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDetailsSheet
//* .File Name       : $Workfile:   SVExternalToolDetailsSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:48  $
//******************************************************************************
#ifndef SVEXTERNALTOOLDETAILS_H
#define SVEXTERNALTOOLDETAILS_H

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolDetailsSheet
#include "SVPropertySheetCancelImpl.h"
#include "SVExternalTool.h"
#include <map>

class SVExternalToolTask;

class SVExternalToolDetailsSheet : public CPropertySheet, public ISVCancel//, public SVPropertySheetCancelImpl
{
	DECLARE_DYNAMIC(SVExternalToolDetailsSheet)

// Construction
public:
	SVExternalToolDetailsSheet( LPCTSTR pszCaption, SVExternalToolDlg* pParentWnd = NULL, UINT iSelectPage = 0 );
	HRESULT CreatePages();

	SVIPDoc* GetIPDoc() const;

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

// Attributes
public:
	SVExternalTool* m_pTool;
	SVExternalToolTask* m_pTask;
	SVExternalToolDlg* m_pSVExternalToolDlgParent;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolDetailsSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();

// Implementation
public:
	virtual ~SVExternalToolDetailsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVExternalToolDetailsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DestroyPages();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolDetailsSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:30:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2012 16:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
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
