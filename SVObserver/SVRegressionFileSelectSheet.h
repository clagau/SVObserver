// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectSheet
// * .File Name       : $Workfile:   SVRegressionFileSelectSheet.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:56  $
// ******************************************************************************

#ifndef SVREGRESSIONFILESELECTSHEET_H
#define SVREGRESSIONFILESELECTSHEET_H

#include "SVRegressionTestStruct.h"
#include "SVVirtualCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet

class CSVRegressionFileSelectSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSVRegressionFileSelectSheet)

// Construction
public:
	CSVRegressionFileSelectSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSVRegressionFileSelectSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	BOOL ValidateMask(CString sMask);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionFileSelectSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();

// Implementation
public:
	virtual ~CSVRegressionFileSelectSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	void CreatePages(CList<RegressionTestStruct*,RegressionTestStruct*>*RegessionList,SVVirtualCameraPtrSet CameraList );


private: //methods
	CString MakeFileNameMask(CString csFileName);
	RegressionFileSelectCode ValidateList();
	long GetNumberOfFilesMatchingMask(CString sMask);
	void ClearRegressionList();
	void FillFileList();

private: //data members
	CList<CString, CString&> m_listMasks;
	CList<RegressionTestStruct*,RegressionTestStruct*> *m_pRegressionList;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRegressionFileSelectSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:41:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Sep 2011 13:13:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jul 2005 13:59:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code to save the current settings and will also save the regression file select path in the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
