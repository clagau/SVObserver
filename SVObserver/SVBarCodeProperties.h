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

#pragma once

#pragma region Includes
#include "SVBarCodeGeneralDialog.h"		// Added by ClassView
#include "SVBarCodeStringMatchDialog.h"	// Added by ClassView
#include "SVBarCodeAttributesDialog.h"	// Added by ClassView
#include "SVBarCodeDataMatrixDialog.h"	// Added by ClassView
#pragma endregion Includes

class SVBarCodeProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(SVBarCodeProperties)

public:
	SVBarCodeProperties(UINT nIDCaption = IDS_DIALOG_TITLE_BARCODEPROPERTIES, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~SVBarCodeProperties();

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
