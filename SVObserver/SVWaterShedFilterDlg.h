// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWatershedFilterDlg
// * .File Name       : $Workfile:   SVWaterShedFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 16:31:00  $
// ******************************************************************************

#if !defined(AFX_SVWATERSHEDFILTERDLG_H__INCLUDED_)
#define AFX_SVWATERSHEDFILTERDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVImageListClass.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVView.h"

class SVImageClass;
class SVToolAdjustmentDialogSheetClass;
class SVWatershedFilterClass;

struct SVInObjectInfoStruct;

/////////////////////////////////////////////////////////////////////////////
// SVWatershedFilterDlg dialog

class SVWatershedFilterDlg : public CDialog, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVWatershedFilterDlg();   // standard constructor

	SVWatershedFilterClass* GetWatershedFilter();

	virtual HRESULT SetInspectionData();
	
	SVToolAdjustmentDialogSheetClass*   m_pParent;
	// Input Image Related Variables
	SVImageClass*                       m_pCurrentSourceImage;
	SVImageListClass				    m_AvailableImages;

// Dialog Data
	//{{AFX_DATA(SVWatershedFilterDlg)
	enum { IDD = IDD_WATERSHED_FILTER };
	SVAvailableSourceImageListComboBoxClass	m_SVSourceImageCombo;
	BOOL	m_bUseBasin;
	BOOL	m_bSkipLastLevel;
	long	m_lMinVariation;
	BOOL	m_bUseWatershed;
	int		m_iEightWatershedLines;
	int		m_iMinFillBasin;
	int		m_iRegularWatershedLines;
	int		m_iVariationType;
	BOOL	m_bUseMarker;
	//}}AFX_DATA

	SVInObjectInfoStruct* m_pMarkerImageInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVWatershedFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVWatershedFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSETVarRadio();
	afx_msg void OnSelchangeSourceImageCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVWATERSHEDFILTERDLG_H__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVWaterShedFilterDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:31:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Aug 2005 08:46:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed a forward reference problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:53:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:01:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Watershed Filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/