//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeProperties
//* .File Name       : $Workfile:   SVBarCodeProperties.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:42:40  $
//******************************************************************************

#if !defined(AFX_SVBARCODEPROPERTIES_H__AF5FDD6C_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)
#define AFX_SVBARCODEPROPERTIES_H__AF5FDD6C_7BE0_11D3_A7D0_00106F010A93__INCLUDED_

#include "SVBarCodeGeneralDialog.h"		// Added by ClassView
#include "SVBarCodeStringMatchDialog.h"	// Added by ClassView
#include "SVBarCodeAttributesDialog.h"	// Added by ClassView
#include "SVBarCodeDataMatrixDialog.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVBarCodeProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(SVBarCodeProperties)

// Construction
public:
	SVBarCodeProperties(UINT nIDCaption = IDS_DIALOG_TITLE_BARCODEPROPERTIES, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBarCodeProperties)
	//}}AFX_VIRTUAL

// Implementation
public:
	SVBarCodeDataMatrixDialog m_dlgBarCodeDataMatrix;
	SVBarCodeAttributesDialog m_dlgBarCodeAttributes;
	SVBarCodeStringMatchDialog m_dlgBarCodeStringMatch;
	SVBarCodeGeneralDialog m_dlgBarCodeGeneral;
	virtual ~SVBarCodeProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVBarCodeProperties)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBARCODEPROPERTIES_H__AF5FDD6C_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBarCodeProperties.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:42:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 16:54:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/