//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressDialog
//* .File Name       : $Workfile:   SVParserProgressDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Jun 2014 11:57:24  $
//******************************************************************************

#ifndef SVPARSERPROGRESSDIALOG_H_INCLUDED_
#define SVPARSERPROGRESSDIALOG_H_INCLUDED_

#include <map>
#include "SVContainerLibrary/SVVector.h"
#include "SVProgressDialog.h"
#include "SVSystemLibrary/SVLockableClass.h"
#include "SVUtilityLibrary\SVSharedPtr.h"

class SVObjectScriptParserClass;

typedef SVSharedPtr<CProgressCtrl> ProgressCtrlSharedPtr;
typedef SVSharedPtr<CStatic> StaticSharedPtr;

struct SVParserProgressControlStruct
{
	BOOL bValidate;
	GUID OwnerGuid;
	SVObjectScriptParserClass* pParser;
	ProgressCtrlSharedPtr pProgressCtrl;
	StaticSharedPtr pStaticTextCtrl;
	CString Text;
	int TotalSize;

	SVParserProgressControlStruct()
	{
		bValidate = FALSE;
		OwnerGuid = GUID_NULL;
		pParser = NULL;
		pProgressCtrl = NULL;
		pStaticTextCtrl = NULL;
		TotalSize = 0;
	}
};

typedef std::map< int, SVParserProgressControlStruct > SVParserControlList;

class SVParserProgressDialog : public SVProgressDialog
{
// Construction
public:
	SVParserProgressDialog(LPCTSTR title, CWnd* pParent = NULL);   // standard constructor
	virtual ~SVParserProgressDialog();

	BOOL AddParser( unsigned long parserHandle, SVObjectScriptParserClass* pParser );
	
	CProgressCtrl* GetProgressControl( unsigned long parserHandle );
	CStatic* GetTextControl( unsigned long parserHandle );
	bool CheckComplete();

protected:
	SVParserProgressControlStruct& GetParserControl(unsigned long parserHandle);
	SVParserProgressControlStruct& GetFirstAvailableParserControl();

	void resizeAndMoveDialog( CRect& RRect, int cnt );
	void addScrollBar( int visibleWidth, int maxWidth );

protected:
	SVParserControlList m_parserControlList;

// msvEndDialogLock - When the parsing threads end, they all call
// OnEndProgressDialog (). OnEndProgressDialog () checks to see whether or
// not all other threads are terminated, if so then the call does "final"
// clean up.  More than one thread can call this function, and a race 
// condition can exist where multiple threads are doing final clean up.
// This lock variable is used by OnEndProgressDialog () to ensure that only 
// one thread at a time may use it.
	SvSyl::SVLockableClass      msvEndDialogLock;

	// Implementation
protected:
	void SetProgressLocations();

	// Generated message map functions
	//{{AFX_MSG(SVParserProgressDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateProgress( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndProgressDialog( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	int m_originalWidth;
	BOOL m_scrollBarShown;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVParserProgressDialog.h_v  $
 * 
 *    Rev 1.1   25 Jun 2014 11:57:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  910
 * SCR Title:  Fix memory leaks for Software Trigger Dialog and Parser Progress Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the CProgressCtrl and the CStatic to be a shared pointer so they get cleaned up correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:18:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.8   04 Sep 2012 15:50:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.7   08 Dec 2010 13:25:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.6   08 Nov 2010 14:46:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.5   16 Dec 2009 12:25:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.4   16 Feb 2005 15:01:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.3   22 Apr 2003 13:37:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.2   09 Aug 2001 07:47:58   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added "bValidate" member to SVParserProgressControlStruct
 * and revised creation of the structure.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.1   07 Aug 2001 15:11:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  208
 * SCR Title:  Improve configuration load time
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed memory leak of CWnd objects.
 * Changed IPDs to load in serial instead of in parallel
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.0   12 Apr 2000 14:25:06   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  125
 * SCR Title:  Make OCR tool load and go online without match string files present
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVLockableClass      msvEndDialogLock; 
 * was added to the SVParserProgressDialog class and implimented in SVParserProgressDialog::OnEndProgressDialog (), to control execution so that only one thread could execute OnEndProgressDialog () at a time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Nov 1999 09:20:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added ScrollBars and Dynamic Sizing.
 * Added Support for Multiple Columns.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Nov 1999 11:36:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * Dialog Class to show the progress of multiple
 * SVObjectScriptParser objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
