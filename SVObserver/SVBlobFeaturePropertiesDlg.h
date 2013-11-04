//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobFeaturePropertiesDlg
//* .File Name       : $Workfile:   SVBlobFeaturePropertiesDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:16  $
//******************************************************************************

#if !defined(AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_)
#define AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVBlobFeaturePropertiesDlg : public CDialog
{
// Construction
public:
	SVBlobFeaturePropertiesDlg(CWnd* pParent = NULL);   // standard constructor

	CString m_strFeatureName;

// Dialog Data
	//{{AFX_DATA(SVBlobFeaturePropertiesDlg)
	enum { IDD = IDD_BLOB_FEATURE_PROPERTIES };
	double	m_dDefaultValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBlobFeaturePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBlobFeaturePropertiesDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBLOBFEATUREPROPERTIESDLG_H__84685960_AB6C_4332_B5BD_0E6A1587F0AB__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobFeaturePropertiesDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:45:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2005 13:10:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/