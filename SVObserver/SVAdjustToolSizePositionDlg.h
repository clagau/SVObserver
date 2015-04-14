//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVAdjustToolSizePositionDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   26 May 2014 10:49:56  $
//******************************************************************************

#ifndef SVADJUSTTOOLSIZEPOSITIONDLG_H
#define SVADJUSTTOOLSIZEPOSITIONDLG_H

#pragma region Includes
#include "SVUtilityLibrary/SVWinHandle.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "PropertyTree/PropTree.h"
#include "SVMFCControls\SVUpDownButton.h"
#pragma endregion Includes

#pragma region Declarations
class SVTaskObjectClass;
#pragma endregion Declarations

class SVAdjustToolSizePositionDlg : public CDialog
{
	DECLARE_DYNCREATE(SVAdjustToolSizePositionDlg)
#pragma region Constructor
public:
	SVAdjustToolSizePositionDlg(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, SVTaskObjectClass* pToolTask = nullptr);
	~SVAdjustToolSizePositionDlg();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAdjustToolSizePositionDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVAdjustToolSizePositionDlg)
#ifdef _WIN64
#define UINT_CUSTOM UINT_PTR
#else
#define UINT_CUSTOM UINT
#endif
	afx_msg LRESULT OnNotifyLButtonDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnNotifyLButtonUp( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer(UINT_CUSTOM nIDEvent);
	afx_msg void OnModeRadio();
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnBnClickedFullROI();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	HRESULT ButtonAction(SvMc::SVUpDownButton* pButton);
	HRESULT AdjustTool( SVExtentLocationPropertyEnum eAction, int dx, int dy );
	HRESULT AdjustToolAngle(double dDAngle);

	//************************************
	// Method:    BuildTreeFromExtents
	// Description: Build up the property tree from the extents.
	// Returns:   void
	//************************************
	void BuildTreeFromExtents();

	//************************************
	// Method:    FillTreeFromExtents
	// Description: Edit the entries of the property tree with the information from the extents.
	// Returns:   void
	//************************************
	void FillTreeFromExtents();

	//************************************
	// Method:    FillTreeFromExtents
	// Description: Edit or create the entries of the property tree with the information from the extents.
	// Parameter: SVRPropertyItem* pRoot The root of the tree.
	// Parameter: bool shouldCreate Define if the entries should be create new or only edit.
	// Returns:   void
	//************************************
	void FillTreeFromExtents( SVRPropertyItem* pRoot, bool shouldCreate );

	bool IsFullSize();

	//************************************
	// Method:    createIcons
	// Description: Create the icons for the arrow buttons.
	// Returns:   void
	//************************************
	void createIcons();
#pragma endregion Private Methods

#pragma region Member variables
private:
	enum
	{
		MODE_TOP_LEFT = 0,
		MODE_BOTTOM_RIGHT = 1,
		MODE_MOVE = 2,
		MODE_ROTATE = 3,
	};

	enum
	{
		TIMER_PAUSE  = 100,
		TIMER_REPEAT = 101,
	};

	// Dialog Data
	//{{AFX_DATA(SVAdjustToolSizePositionDlg)
	enum { IDD = IDD_ADJUST_TOOL_SIZE_POSITION_DIALOG };
	SvMc::SVUpDownButton	m_btnUp;
	SvMc::SVUpDownButton	m_btnRight;
	SvMc::SVUpDownButton	m_btnLeft;
	SvMc::SVUpDownButton	m_btnDown;
	int		m_iMode;
	//}}AFX_DATA

	SVImageExtentClass m_svExtents;
	SVImageExtentClass m_svOriginalExtents;
	SVTaskObjectClass* m_pToolTask;
	SVRPropTree m_Tree;
	const static int m_iPropertyFilter = SVExtentPropertyPositionsInput | SVExtentPropertyDimensionsInput;
	const static int ID_BASE = 1000;

	SVWinHandle<HICON> m_icoArrowUp;
	SVWinHandle<HICON> m_icoArrowDown;
	SVWinHandle<HICON> m_icoArrowLeft;
	SVWinHandle<HICON> m_icoArrowRight;
	SVWinHandle<HICON> m_icoArrowClockwise;
	SVWinHandle<HICON> m_icoArrowCounterclockwise;
	SvMc::SVUpDownButton* m_pButton;
	CString m_sTitle;
#pragma endregion Member variables
};	// end class SVAdjustToolSizePositionDlg
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAdjustToolSizePositionDlg.h_v  $
 * 
 *    Rev 1.5   26 May 2014 10:49:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   combine the three classes and made the changes like described in SCR894
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Oct 2013 13:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Sep 2013 16:16:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved WinHandle to SVUtilityLibrary.  Renamed WinHandle to SVWinHandle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 15:01:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Jun 2013 06:28:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added user message to prevent button clicked message from getting lost when loosing focus from PropTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Apr 2013 08:26:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dialog components to support Full Image button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:27:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 08:04:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with Adjust Tool position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Nov 2010 13:18:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 14:55:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Oct 2007 16:35:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DestroyWindow Function to free GDI Resources
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:08:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
