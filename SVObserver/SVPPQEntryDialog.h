//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialog
//* .File Name       : $Workfile:   SVPPQEntryDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 20:11:56  $
//******************************************************************************

#ifndef SVPPQENTRYDIALOG_H
#define SVPPQENTRYDIALOG_H

#include "SVPPQEntryDialogDigInPage.h"
#include "SVPPQEntryDialogRemotePage.h"

class SVPPQObject;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCameraPageInfoStruct
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Type				Name				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :04.08.1999 RO 			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVCameraPageInfoStruct
{
	SVCameraPageInfoStruct()
	{
		Channel = 0;
	};

//******************************************************************************
// Data Element(s):
//******************************************************************************

	DWORD		  Channel;
};



class SVPPQEntryDialogPropertySheetClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogCameraPageClass
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogCameraPageClass : public CPropertyPage
{
//******************************************************************************
// Serialization Declaration(s):
//******************************************************************************

//******************************************************************************
// MFC Declarations:
//******************************************************************************

	DECLARE_DYNCREATE(SVPPQEntryDialogCameraPageClass)

//******************************************************************************
// Manual Generated Message Map Entries
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Message Map Entries
//******************************************************************************
protected:
	// Generated message map functions
	//{{AFX_MSG(SVPPQEntryDialogCameraPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

	friend class SVPPQEntryDialogPropertySheetClass;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPPQEntryDialogCameraPageClass();

//******************************************************************************
// Destructor(s):
//******************************************************************************

	~SVPPQEntryDialogCameraPageClass();

//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Process;
//******************************************************************************

//******************************************************************************
// Operation(s) Of Visual Representation:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogCameraPageClass)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//******************************************************************************
// Debug Operation(s):
//******************************************************************************

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:

// Dialog Data
	//{{AFX_DATA(SVPPQEntryDialogCameraPageClass)
	enum { IDD = IDD_PPQENTRY_CAMERA_PAGE };
	CListBox	selectedList;
	CListBox	availableList;
	CString		StrCurPos;
	//}}AFX_DATA


protected:
	SVPPQEntryDialogPropertySheetClass* m_pSheet;
	BOOL								m_bIsTaken;
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPPQEntryDialogPropertySheetClass
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVPPQEntryDialogPropertySheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPPQEntryDialogPropertySheetClass)

// Construction
public:
	SVPPQEntryDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVPPQEntryDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
protected:

	void addPages();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPPQEntryDialogPropertySheetClass)
	public:
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVPPQEntryDialogPropertySheetClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPPQEntryDialogPropertySheetClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVPPQObject *m_pPPQ;
	long		m_lCurrentPosition;
	BOOL		m_bIsTaken;

protected:

	SVPPQEntryDialogCameraPageClass		m_oCameraPage;
	SVPPQEntryDialogDigInPageClass		m_oDigitalPage;
	SVPPQEntryDialogRemotePageClass		m_oRemotePage;
};

#endif	// SVPPQENTRYDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPPQEntryDialog.h_v  $
 * 
 *    Rev 1.1   06 May 2013 20:11:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.6   16 Apr 2013 15:35:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:31:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.5   22 Apr 2003 14:07:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.4   20 Nov 2002 10:46:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * Many changes related to the new architecture
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.3   06 Sep 2000 16:34:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.2   Sep 16 1999 12:48:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new Digital Input Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.1   Aug 23 1999 13:55:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised to use new Digitizer Classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/