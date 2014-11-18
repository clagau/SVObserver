//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerDialog
//* .File Name       : $Workfile:   SVBlobAnalyzerDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Nov 2014 05:09:04  $
//******************************************************************************

#ifndef __SVBLOBANALYZERDIALOG__INCLUDED
#define __SVBLOBANALYZERDIALOG__INCLUDED

#include "SVBlobAnalyzerFeatureListBox.h"
#include "SVBlobAnalyzer.h"

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
class SVToolClass;

class SVBlobAnalyzeFeatureDialogClass : public CDialog, public SVTaskObjectInterfaceClass
{
public:
protected:
// Generated message map functions
	//{{AFX_MSG(SVBlobAnalyzeFeatureDialogClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonSetRange();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSetSortBtn();
	afx_msg void OnAscending();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeList2();
	afx_msg void OnNbrOfBlobs();
	afx_msg void OnExcludeBlob();
	afx_msg void OnBtnFillBlobs();
	afx_msg void OnFillBlobs();
	afx_msg void OnBlobColor();
	afx_msg void OnChangeEditMaxNbrBlobs();
	afx_msg void OnKillFocusMaxNbrBlobs();
	afx_msg void OnButtonSetFeatureProperties();
	afx_msg void OnChangeEditMaxBlobDataArraySize(); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
// Standard constructor
	SVBlobAnalyzeFeatureDialogClass( SVToolClass* PTool, SVBlobAnalyzerClass* Analyzer, SVIPDoc* p_pIPDoc, CWnd* pParent = NULL );

// Standard destructor
	~SVBlobAnalyzeFeatureDialogClass();

	virtual HRESULT SetInspectionData();

	void EnableButtons();

	//{{AFX_VIRTUAL(SVBlobAnalyzeFeatureDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVBlobAnalyzeFeatureDialogClass)
	enum { IDD = IDD_BA_FEATURE_DIALOG };
	CEdit	m_ebMaxBlobDataArraySize;
	CEdit	m_ctlMaxNbrBlobs;
	CButton	m_chkFillBlob;
	CButton	m_btnFillBlobs;
	CComboBox m_cbBlobColor;
	SVBlobAnalyzeFeatureListBoxClass	m_lbSelectedFeatures;
	SVBlobAnalyzeFeatureListBoxClass	m_lbAvailableFeatures;
	CString	msvSortFeatureEdt;
	BOOL	msvAscending;
	BOOL	m_bExclude;
	long	m_lMaxNumberBlobs;
	long	m_lMaxBlobDataArraySize;
	//}}AFX_DATA


/*- msvszOriginalFeaturesEnabled ---------------------------------------------------*/
/*- This holds the original list of enabled features so that the list can be        */
/*- restored if the operator presses cancel. ---------------------------------------*/
	TCHAR                           msvszOriginalFeaturesEnabled 
	                                    [SV_NUMBER_OF_BLOB_FEATURES + 1]; // Null Terminated

	SVErrorClass                    msvError;

	SVToolClass*                    msvpTool;

	CString script;
	CString alias;
	long    m_colorBlobEnum;

protected:
	SVIPDoc* m_pIPDoc;
	SVBlobAnalyzerClass* m_pCurrentAnalyzer;
};


#endif //__SVBLOBANALYZERDIALOG__INCLUDED

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBlobAnalyzerDialog.h_v  $
 * 
 *    Rev 1.2   18 Nov 2014 05:09:04   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Rename m_isBlackBlobs into m_colorBlobEnum and change the type from bool to long.
 * Use m_colorBlobEnumValue from analyzer class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Nov 2014 07:03:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add blob color combobox and method OnBlobColor
 * add m_isBlackBlobs
 * move parameter init from Constructor body to header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:43:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   02 Jul 2012 16:36:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   09 Feb 2012 09:09:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  751
 * SCR Title:  Fix validation of desired maximum number of processed blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed validation for Desired Max Number of Processed Blobs.  will now validate when leaving the field.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   08 Aug 2005 15:30:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   28 Jul 2005 13:07:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   19 Jul 2005 16:10:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add message handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Jul 2005 14:38:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented support for Max Blob Data Array Size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   04 Dec 2003 12:58:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code for Max Number of Blobs.  The value is now setable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   06 Nov 2003 07:40:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the layout of the dialog and changed logic based on if fill blobs is turned on
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   29 Oct 2003 14:06:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new button on dialog for FillBlobs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Apr 2003 17:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   27 Apr 2000 08:37:26   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  131
 * SCR Title:  Bug fixes for blob analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added a handler function for the check box 'Exclude Failed Blobs'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2000 18:29:30   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBlobAnalyzeFeatureDialogClass definition was changed to replace
 *     CString csOriginalFeaturesEnabled; with
 *     TCHAR  msvszOriginalFeaturesEnabled;
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   14 Apr 2000 10:10:02   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  123, 122
 * SCR Title:  Add Grahpical Interface to show all blobs found
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Added an option to exclude or include failed blobs.
 * Added a button to Set Range for Number of Blobs Found.
 * Made the features 'Box X Max', 'Box X Min', 'Box Y Max' and 'Box Y Min' non removable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 16 2000 16:17:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  107
 * SCR Title:  Add More Blob Analyzer Features
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed code to add 26 more Blob features that SVObserver can calculate using the MIL 6.0 library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:05:44   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Nov 17 1999 10:27:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Save the location of the dialog between 'uses'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Oct 1999 13:54:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to correct problem with sort ascending checkbox.
 * Added logic to Hide/Show Sort Ascending, Set Sort, and
 * Range button based on features selected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Oct 1999 08:50:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added CheckBox for Sort Ascending
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 17:25:40   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Sep 1999 22:06:18   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Mostly commented out and stubbed code for allowing SVBlobAnalyzerClass to compile and run.  Needs work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
