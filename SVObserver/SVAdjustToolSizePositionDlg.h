//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVAdjustToolSizePositionDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   24 Sep 2013 16:16:10  $
//******************************************************************************

#ifndef SVADJUSTTOOLSIZEPOSITIONDLG_H
#define SVADJUSTTOOLSIZEPOSITIONDLG_H

#include "SVUtilityLibrary/SVWinHandle.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "ISVCancel.h"


class SVObjectClass;
class SVImageViewClass;
class SVTaskObjectClass;



class SVAdjustToolSizePositionDlg : public CPropertySheet, public ISVCancel
{
	DECLARE_DYNAMIC(SVAdjustToolSizePositionDlg)

// Construction
public:
	SVAdjustToolSizePositionDlg( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, SVTaskObjectClass* pToolTask = NULL );
	virtual ~SVAdjustToolSizePositionDlg();
	HRESULT CreatePages();

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

// Attributes
public:
	SVImageExtentClass m_svExtents;
	SVImageExtentClass m_svOriginalExtents;
	SVTaskObjectClass* m_pToolTask;

// Operations
public:
	virtual void OnOK();
	virtual void OnCancel();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAdjustToolSizePositionDlg)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(SVAdjustToolSizePositionDlg)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DestroyPages();


};	// end class SVAdjustToolSizePositionDlg

#include "SVUpDownButton.h"

class SVAdjustToolSizePositionMainPage : public CPropertyPage
{
// Construction
public:
	SVAdjustToolSizePositionMainPage(const CString& sTitle=_T(""), SVAdjustToolSizePositionDlg* pParent=NULL);
	~SVAdjustToolSizePositionMainPage();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAdjustToolSizePositionMainPage)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(SVAdjustToolSizePositionMainPage)
	enum { IDD = IDD_ADJUST_TOOL_SIZE_POSITION_DIALOG };
	SVUpDownButton	m_btnUp;
	SVUpDownButton	m_btnRight;
	SVUpDownButton	m_btnLeft;
	SVUpDownButton	m_btnDown;
	int		m_iMode;
	CString	m_sWidthHeight;
	CString	m_sTopLeft;
	//}}AFX_DATA

	HRESULT ButtonAction(SVUpDownButton* pButton, UINT nFlags);
	HRESULT AdjustTool( SVExtentLocationPropertyEnum eAction, int dx, int dy );
	HRESULT AdjustToolAngle(double dDAngle);

	void UpdateExtentsDisplay();

	// Generated message map functions
	//{{AFX_MSG(SVAdjustToolSizePositionMainPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnModeRadio();
	virtual BOOL OnSetActive();
	//}}AFX_MSG
	void OnIdCancel();
	void OnIdOK();
	afx_msg LRESULT OnNotifyLButtonDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnNotifyLButtonUp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	SVAdjustToolSizePositionDlg*  m_pParentDialog;
	SVImageExtentClass*           m_pExtents;
	SVTaskObjectClass*			  m_pToolTask;

	SVWinHandle<HICON> m_icoArrowUp;
	SVWinHandle<HICON> m_icoArrowDown;
	SVWinHandle<HICON> m_icoArrowLeft;
	SVWinHandle<HICON> m_icoArrowRight;
	SVWinHandle<HICON> m_icoArrowClockwise;
	SVWinHandle<HICON> m_icoArrowCounterclockwise;

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
	SVUpDownButton* m_pButton;

	CString m_sTitle;

};	// end class SVAdjustToolSizePositionMainPage

#include "PropertyTree/PropTree.h"
// #include "afxwin.h"

class SVAdjustToolSizePositionPropertiesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVAdjustToolSizePositionPropertiesPage)

// Construction
public:
	SVAdjustToolSizePositionPropertiesPage(const CString& sTitle=_T(""), SVAdjustToolSizePositionDlg* pParent=NULL);
	~SVAdjustToolSizePositionPropertiesPage();

// Dialog Data
	//{{AFX_DATA(SVAdjustToolSizePositionPropertiesPage)
	enum { IDD = IDD_ADJUST_TOOL_SIZE_POSITION_PROPERTIES };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVAdjustToolSizePositionPropertiesPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	afx_msg LRESULT OnUserMessage( WPARAM wparam, LPARAM lparam ); 
	afx_msg void OnBnClickedFullROI();
	bool IsFullSize();

// Implementation
protected:

	SVRPropTree                     m_Tree;
	SVAdjustToolSizePositionDlg*    m_pParentDialog;
	CString                         m_sTitle;
	SVImageExtentClass*             m_pExtents;
	SVTaskObjectClass*				m_pToolTask;

	const static int                       m_iPropertyFilter;

	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);

	// Generated message map functions
	//{{AFX_MSG(SVAdjustToolSizePositionPropertiesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	void OnIdCancel();
	void OnIdOK();
	void FillTreeFromExtents();
	DECLARE_MESSAGE_MAP()

};	// end class SVAdjustToolSizePositionPropertiesPage

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAdjustToolSizePositionDlg.h_v  $
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
