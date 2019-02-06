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

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVWinHandle.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVMFCControls\SVUpDownButton.h"
#pragma endregion Includes

namespace SvIe
{
class SVTaskObjectClass;
}

class SVAdjustToolSizePositionDlg : public CDialog
{
	DECLARE_DYNCREATE(SVAdjustToolSizePositionDlg)
#pragma region Constructor
public:
	SVAdjustToolSizePositionDlg(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, SvIe::SVTaskObjectClass* pToolTask = nullptr);
	virtual ~SVAdjustToolSizePositionDlg();
#pragma endregion Constructor

#pragma region Protected Methods
protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAdjustToolSizePositionDlg)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVAdjustToolSizePositionDlg)
#define UINT_CUSTOM UINT_PTR

	afx_msg LRESULT OnNotifyLButtonDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnNotifyLButtonUp( WPARAM wParam, LPARAM lParam );
	afx_msg void OnTimer(UINT_CUSTOM nIDEvent);
	afx_msg void OnModeRadio();
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnBnClickedFullROI();
	afx_msg void OnBnClickedPropagate();
	afx_msg void OnBnClickedEditTool();
	

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	HRESULT ButtonAction(SvMc::SVUpDownButton* pButton);
	HRESULT AdjustTool( SvDef::SVExtentLocationPropertyEnum eAction, int dx, int dy );
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

	//************************************
	//! return true if full size button should be enabled 
	//! \returns bool
	//************************************	
	bool IsFullSizeAllowed();
	//************************************
	//! return True if propagate button  should be enabled 
	//! \returns bool
	//************************************
	bool UsePropagate();

	
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
	SvIe::SVTaskObjectClass* m_pToolTask;
	SVRPropTree m_Tree;
	const static int m_iPropertyFilter = SvDef::SVExtentPropertyPositionsInput | SvDef::SVExtentPropertyDimensionsInput;
	const static int ID_BASE = 1000;

	SVWinHandle<HICON> m_icoArrowUp;
	SVWinHandle<HICON> m_icoArrowDown;
	SVWinHandle<HICON> m_icoArrowLeft;
	SVWinHandle<HICON> m_icoArrowRight;
	SVWinHandle<HICON> m_icoArrowClockwise;
	SVWinHandle<HICON> m_icoArrowCounterclockwise;
	SvMc::SVUpDownButton* m_pButton;
	std::string m_Title;
#pragma endregion Member variables
};	// end class SVAdjustToolSizePositionDlg


