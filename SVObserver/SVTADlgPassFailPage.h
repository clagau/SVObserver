//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMFCControls\SVEditNumbers.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "RangeClassHelper.h"
#pragma endregion Includes

#pragma region Declarations
class SVToolClass;
#pragma endregion Declarations

class SVToolAdjustmentDialogPassFailPageClass : public CPropertyPage ,public SvOg::ISVPropertyPageDialog
{
#pragma region Constructor
public:
	// Standard constructor
	SVToolAdjustmentDialogPassFailPageClass( SVToolClass* PTool );
	SVToolAdjustmentDialogPassFailPageClass();
	// Standard destructor
	~SVToolAdjustmentDialogPassFailPageClass();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// ISVPropertyPageDialog
	virtual bool QueryAllowExit() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogPassFailPageClass)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(SVToolAdjustmentDialogPassFailPageClass)
	afx_msg void OnBnClickedFailHighIndirect();
	afx_msg void OnBnClickedWarnlHighIndirect();
	afx_msg void OnBnClickedWarnLowIndirect();
	afx_msg void OnBnClickedFailedLowIndirect();
	//{{AFX_MSG
#pragma region Protected Methods

#pragma region Privated Methods
	bool UpdateRangeValues();

	//************************************
	// Description:  Set the values from RangeHelper to the Dialog
	//************************************
	void SetDlgData();

	//************************************
	// Description:  Get the values from the Dialog and set them to the RangeHelper
	// Returns:  bool:  
	//************************************
	bool GetDlgData();

	//************************************
	/// Open an object selector and set the chosen value to the control.
	/// \param control [in] The control of the value
	/// \param fieldEnum [in] Enum of the value
	//************************************
	void setValuePerObjectSelector( CEdit& control, ERange fieldEnum);

	//************************************
	/// Show an object selector and return the name of the selection.
	/// \param name [in,out] Name of the object.
	/// \param fieldEnum [in] Enum of the value
	/// \returns bool true if a new object would selected.
	//************************************
	bool ShowObjectSelector(CString& name, ERange fieldEnum );
#pragma region Privated Methods

#pragma region Member Variables
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogPassFailPageClass)
	enum { IDD = IDD_TA_PASS_FAIL_DIALOG };
	CEdit m_EditFailHigh;
	CEdit m_EditWarnHigh;
	CEdit m_EditWarnLow;
	CEdit m_EditFailLow;
	CButton m_ButtonFailHigh;
	CButton m_ButtonWarnHigh;
	CButton m_ButtonWarnLow;
	CButton m_ButtonFailLow;
	CBitmap m_downArrowBitmap;
	//}}AFX_DATA

private:
	RangeClassHelper m_RangeHelper;
#pragma endregion Member Variables
};


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgPassFailPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:16:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Aug 2005 08:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:11:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Oct 1999 16:31:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnOk method.
 * Revised to use CEditNumbersFloat control to restrict
 * entries to doubles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 1999 19:59:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for new architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
