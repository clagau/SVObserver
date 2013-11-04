//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionSheet
//* .File Name       : $Workfile:   SVDataDefinitionSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:07:34  $
//******************************************************************************
#ifndef SVDATADEFINITIONSHEET_H
#define SVDATADEFINITIONSHEET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVDataDefinitionSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVDataDefinitionSheet


#include "SVPropertySheetCancelImpl.h"

class SVInspectionProcess;

class SVDataDefinitionSheet : public CPropertySheet, public ISVCancel//, public SVPropertySheetCancelImpl
{
	DECLARE_DYNAMIC(SVDataDefinitionSheet)

// Construction
public:
	SVDataDefinitionSheet( LPCTSTR pszCaption, SVInspectionProcess* pInspection, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual ~SVDataDefinitionSheet();

	HRESULT CreatePages();

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

// Attributes
public:
	SVInspectionProcess* m_pInspection;

	CString    m_strInspection;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDataDefinitionSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();


	// Generated message map functions
protected:
	//{{AFX_MSG(SVDataDefinitionSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DestroyPages();

};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #ifndef SVDATADEFINITIONSHEET_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDataDefinitionSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:07:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Mar 2008 09:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Property Sheet for Data and image Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
