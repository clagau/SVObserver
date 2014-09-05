//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDialog
//* .File Name       : $Workfile:   SVPictureDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   :     $Date:   05 Sep 2014 09:25:18  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVDisplayPicture.h"
#include <map>
#include "afxwin.h"
#include "afxcmn.h"
#pragma endregion Includes

class SVPictureDialog : public CDialog
{
	DECLARE_DYNAMIC(SVPictureDialog)

public:
#pragma region Constructor
	SVPictureDialog( long p_lTab, AppearanceTypes p_lAppearance = TopButtons );
	virtual ~SVPictureDialog();
#pragma endregion Constructor

	typedef std::map<unsigned int, HICON> IconMap;
	enum { IDD = IDD_PICT_DIALOG };
	
	HRESULT SetScrollBars();

	//************************************
	// Method:    SetPicture
	// Description:  Gives the PictureDisplay a picture to display.
	// Parameter: IPictureDisp* - pointer to the image to display
	// Parameter: COLORREF - color to be displayed in the area that is not filled by the image
	// Parameter: bool - flag that indicates the zoom factor should be recalculated based on the dimensions of p_Picture
	// Returns:   HRESULT
	//************************************
	HRESULT SetPicture( IPictureDisp* l_pPicture, COLORREF BackgroundColor, bool adjustZoom );

	//************************************
	// Method:    SetPictureWithROI
	// Description:  Gives the PictureDisplay a picture to display with overlay information (ROI).
	// Parameter: IPictureDisp* - pointer to the image to display
	// Parameter: COLORREF - color to be displayed in the area that is not filled by the image
	// Parameter: ISVROIList* - overlay information
	// Parameter: bool - flag that indicates the zoom factor should be recalculated based on the dimensions of p_Picture
	// Returns:   HRESULT
	//************************************
	HRESULT SetPictureWithROI( IPictureDisp* p_Picture, COLORREF BackgroundColor, ISVROIList* p_RoiList, bool adjustZoom );

	void SetAppearance( long l_lType );
	long GetAppearance() const;

	void SetButtonSpacing( long p_lSpacing );
	long GetButtonSpacing() const;

	void SetButtonJustify( long p_lJustify );
	long GetButtonJustify() const;

	void SetIsClearOverlayByNewImage( BOOL isValue );

	//************************************
	// Method:    CloneZoomHelper
	// Description:  Copies the the Value of m_zoom
	// Parameter: const ZoomHelper & zh
	// Returns:   void
	//************************************
	void CloneZoomHelper(const ZoomHelper& zh);

	long AddOverlay(const VariantParamMap& ParameterMap, bool refresh);
	long AddOverlayToGroup( long lGroup, const VariantParamMap& ParameterMap, bool refresh);
	HRESULT EditOverlay(long lHandle, const VariantParamMap& ParameterMap );

	HRESULT GetViewRect( long& pX1, long& pY1, long& pX2, long& pY2 ) const;

	HRESULT RemoveOverlay(long Handle);
	void RemoveAllOverlays();
	long DeleteDrawObject();

	long GetScrollBarWidth() const;
	void SetScrollBarWidth( long p_lWidth );
	double GetZoomFactor() const;
	void SetZoomFactor( double scaleFactor );

	HRESULT SetAllowEdit( long lHandle, long lEnable );
	HRESULT SelectDrawObject( long lHandle );
	void Resize( unsigned int nType, long x, long y, long Width, long Height );
	HRESULT GetObjectCoords( long lHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 );
	HRESULT GetObjectParameter( long lHandle, VariantParamMap& ParameterMap );

	HRESULT EnableSaveButton( long p_lEnable );

	//************************************
	// Method:    SetZoomSlider
	// Description:  Set the zoom slider control position 
	// according to ScaleFactor
	// Parameter: BOOL bRange  If true, the range of the slider will be set, too.
	// Returns:   void
	//************************************
	void SetZoomSlider(BOOL bRange );

	void SetScrollPosition(int orientation, int pos);
	int GetScrollPosition(int orientation) const;

	//************************************
	// Method:    CloneScrollPosition
	// Description:  Set the scroll Position to all other tabs 
	// Parameter: int orientation
	// Returns:   void SB_HOR SB_VERT or SB_BOTH
	//************************************
	void CloneScrollPosition(int orientation);

	void GetZoomRanges(double &min, double &max) const;

	//************************************
	// Method:    SetZoomFit
	// Description:  Set the maximum zoom factor the whole image can be shown
	// Parameter: void
	// Returns:   void
	//************************************
	void SetZoomFit();

	//************************************
	// Method:    IsScrollbarHidden
	// Description:  returns true if the scrollbars are hidden 
	// Returns:   BOOL
	//************************************
	BOOL IsScrollbarHidden();

	//************************************
	// Method:    AdjustSizeToImage
	// Description:  Adjust the size of the m_PictDisplay WND to the size of the image
	// Returns:   void
	//************************************
	void AdjustSizeToImage();

	//************************************
	// Method:    GetZoomHelper
	// Description:  Get const reference to zoomhelper
	// Returns:   const ZoomHelper&
	//************************************
	const ZoomHelper& GetZoomHelper();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadToolbarIcons();
	void PutIconsOnButtons();
	bool PutIconOnButton( unsigned int p_iBmpResource, unsigned int p_iButton );

	// Functions used to adjust display size...
	void PositionVerticalSlider( int LeftSpacing, int TopSpacing, int RightSpacing, int BottomSpacing, unsigned int ID );
	void PositionHorizontalSlider( int LeftSpacing, int TopSpacing, int RightSpacing, int BottomSpacing, unsigned int ID );
	void PositionPictureDisplay();
	void PositionButtons();
	void SetupWindow();
	void ToEditCtrl( unsigned int p_iResource, long p_lValue );

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelChangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedZoomPlus();
	afx_msg void OnBnClickedZoomMinus();
	afx_msg void OnBnClickedZoomFit();
	afx_msg void OnBnClickedZoomOne();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSaveButton();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	//************************************
	// Method:    ZoomToSliderPos
	// Description:  Set the zoom slider position to the actual zoom factor.
	// Returns:   void
	//************************************
	void ZoomToSliderPos();

	//************************************
	// Method:    GetPictureTab
	// Description:  GetPointer to the SVPictureTab Dialog
	// Returns:   SVPictureTabs*
	//************************************
	SVPictureTabs* GetPictureTab();

private:
	CButton m_ButtonZoomPlus;
	CButton m_ButtonZoomMinus;
	CButton m_ButtonZoomFit;
	CButton m_ButtonZoomOne;
	CButton m_ButtonSave;
	CStatic m_StaticZoom;
	int m_StaticZoomMaxTextLen;
	CSliderCtrl m_SliderCtrlZoom;

	SVDisplayPicture m_PictDisplay;	// <<<<****** Picture Class derived from CStatic *****//
	CScrollBar m_HorizScroll;
	CScrollBar m_VertScroll;
	int m_ButtonSize;
	AppearanceTypes m_eAppearance;
	long m_lButtonSpacing;
	long m_lButtonJustify;
	long m_lScrollBarWidth;
	bool m_bSaveButton;
	bool m_bZoomSlider;
	IconMap m_Icons;
	static const int SliderProp = 100;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDialog.h_v  $
 * 
 *    Rev 1.2   05 Sep 2014 09:25:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   added message handler OnHelpInfo. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Aug 2014 17:35:52   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   new Parameter adjustZoom in SetPicture
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Jun 2014 16:28:04   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 May 2011 08:28:40   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  141
 * SCR Title:  Fix External OCR Adjustment Page Memory Usage When Moving ROI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to only set icons on buttons once.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 May 2011 14:45:28   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  141
 * SCR Title:  Fix External OCR Adjustment Page Memory Usage When Moving ROI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct GDI resource leak
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Sep 2010 11:19:44   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  33
 * SCR Title:  Font Wizard Improvements
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetViewRect function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Mar 2010 09:58:16   sjones
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetPicture method to return HRESULT
 * Revised SetPictureWithROI method to return HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2010 10:55:36   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Objected Selected Event
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:20   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/