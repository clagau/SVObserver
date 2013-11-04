//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistorySheet
//* .File Name       : $Workfile:   SVConditionalHistorySheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:02:12  $
//******************************************************************************

#ifndef SVCONDITIONALHISTORYSHEET_H_
#define SVCONDITIONALHISTORYSHEET_H_
#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVConditionalHistorySheet
#include "SVPropertySheetCancelImpl.h"
#include "SVExternalTool.h"
#include <map>

class SVInspectionProcess;

class SVConditionalHistorySheet : public CPropertySheet, public ISVCancel//, public SVPropertySheetCancelImpl
{
	DECLARE_DYNAMIC(SVConditionalHistorySheet)

// Construction
public:
	SVConditionalHistorySheet( LPCTSTR pszCaption, SVInspectionProcess* pInspection, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual ~SVConditionalHistorySheet();

	HRESULT CreatePages();

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

// Attributes
public:
	SVInspectionProcess* m_pInspection;

	CString    m_strInspection;
	CString    m_strMaxHistory;
	BOOL       m_bOverwrite;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalHistorySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();


	// Generated message map functions
protected:
	//{{AFX_MSG(SVConditionalHistorySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DestroyPages();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //#ifndef SVCONDITIONALHISTORYSHEET_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistorySheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:02:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/