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
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVMFCControls\SVUpDownButton.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
class SVAdjustToolSizePositionDlg : public CDialog
{
	DECLARE_DYNCREATE(SVAdjustToolSizePositionDlg)
#pragma region Constructor
public:
	SVAdjustToolSizePositionDlg(uint32_t inspectionId, uint32_t taskObjectId, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr);
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
	HRESULT AdjustTool( SvPb::SVExtentLocationPropertyEnum eAction, int dx, int dy );
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
	int		m_iMode = -1;
	//}}AFX_DATA

	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > m_originalExtents;
	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > m_extents;
	SvPb::SVExtentTranslationEnum m_translationType = SvPb::SVExtentTranslationNone;

	uint32_t m_ipId;
	uint32_t m_toolTaskId;
	SVRPropTree m_Tree;
	const static int m_iPropertyFilter = SvPb::SVExtentPropertyPositionsInput | SvPb::SVExtentPropertyDimensionsInput;
	const static int ID_BASE = 1000;

	SVWinHandle<HICON> m_icoArrowUp;
	SVWinHandle<HICON> m_icoArrowDown;
	SVWinHandle<HICON> m_icoArrowLeft;
	SVWinHandle<HICON> m_icoArrowRight;
	SVWinHandle<HICON> m_icoArrowClockwise;
	SVWinHandle<HICON> m_icoArrowCounterclockwise;
	SvMc::SVUpDownButton* m_pButton = nullptr;
	std::string m_Title;

	//bool m_isFullSizeAllowed = false;
	SvPb::EAutoSize m_autoSizeEnabled = SvPb::EAutoSize::EnableNone;

	ValueController m_values;
#pragma endregion Member variables
};	// end class SVAdjustToolSizePositionDlg
} //namespace SvOg