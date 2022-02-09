//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayPicture
//* .File Name       : $Workfile:   SVDisplayPicture.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   14 Aug 2014 17:35:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVPictureDisplayIdl.h"
#include "DrawObject.h"
#include "SVOGui/ZoomHelper.h"
#pragma endregion Includes

#pragma region Declarations
#define OBJECT_CHANGED WM_USER + 1
#define OBJECT_DELETED WM_USER + 2
#define SAVE_BUTTON_PRESSED WM_USER + 3
#define OBJECT_MOVED WM_USER + 4
#define OBJECT_SELECTED WM_USER + 5
#define MOUSE_MOVEDIM WM_USER +6
#define TAB_CHANGED WM_USER + 7

enum DisplayPictureErrors
{
	E_InvalidBitmapHandle = -79002,
	E_InvalidPictureHandle = -79003,
	E_InvalidDrawObject = -79006,
	E_InvalidTab = -79012,
	E_ZoomOutOfRange = -79013,
	E_DuplicateTabName = -79014,
	E_InvalidOverlay = -79016
};

class SVPictureTabs;
class SVPictureDisplayCtrl;
#pragma endregion Declarations

class SVDisplayPicture : public CStatic
{
public:
#pragma region Constructor
	explicit SVDisplayPicture(long p_lTab);
	virtual ~SVDisplayPicture();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Method:    CloneZoomHelper
	// Description:  copies the zoom values to m_Zoom
	// Parameter: const ZoomHelper & zh
	// Returns:   void
	//************************************
	void CloneZoomHelper(const ZoomHelper& zh);

	//************************************
	// Method:    GetZoomHelper
	// Description:  returns a const reference to m_Zoom
	// Returns:   const ZoomHelper&
	//************************************
	const ZoomHelper& GetZoomHelper() const;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	void DrawImage(CDC& dstDC, CDC& srcDC, const CRect& rect);
	void DrawOverlays(CDC& rDC);
	SVPictureDisplayCtrl* GetPictureDisplayCtrl() const;
	void SelectCursor();
	void ClearPicture();

	//************************************
	// Method:    GetPictureTab
	// Description:  GetPointer to the SVPictureTab Dialog
	// Returns:   SVPictureTabs*
	//************************************
	SVPictureTabs* GetPictureTab() const;

	//************************************
	// Method:    CloneZoomSettings
	// Description:  Copies zoom parameter to all  SVDisplayPicture
	// when the property zoomsimultaneous is set
	// Returns:   void
	//************************************
	void CloneZoomSettings();

	// Generated message map functions
	//{{AFX_MSG(SVDisplayPicture)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion Protected Methods

#pragma region Public Methods
public:
	void Refresh();

	//************************************
	// Method:    SetPicture
	// Description:  Gives the PictureDisplay a picture to display.
	// Parameter: IPictureDisp* - pointer to the image to display
	// Parameter: COLORREF - color to be displayed in the area that is not filled by the image
	// Parameter: bool - flag that indicates the zoom factor should be recalculated based on the dimensions of p_Picture
	// Returns:   HRESULT
	//************************************
	HRESULT SetPicture( IPictureDisp* p_Picture, COLORREF BackgroundColor, bool AdjustZoom );

	//************************************
	// Method:    SetPictureWithROI
	// Description:  Gives the PictureDisplay a picture to display with overlay information (ROI).
	// Parameter: IPictureDisp* - pointer to the image to display
	// Parameter: COLORREF - color to be displayed in the area that is not filled by the image
	// Parameter: ISVROIList* - overlay information
	// Parameter: bool - flag that indicates the zoom factor should be recalculated based on the dimensions of p_Picture
	// Returns:   HRESULT
	//************************************
	HRESULT SetPictureWithROI( IPictureDisp* p_Picture, COLORREF BackgroundColor, ISVROIList* p_RoiList, bool AdjustZoom );

	void SetZoomFactor(double scaleFactor);

	double GetZoomFactor() const;

	//************************************
	// Method:    GetZoomRanges
	// Description:  Get min and Maximum of the scale factor
	// Parameter: float &min
	// Parameter: float &max
	// Returns:   void
	//************************************
	void GetZoomRanges(double &min, double &max) const;

	//************************************
	// Method:    GetTabHandler
	// Description:  get the tab handler connected with this picture 
	// Returns:   long
	//************************************
	long GetTabHandler() const;

	SIZE GetImageSize() const;

	//************************************
	// Method:    GetViewMaxRect
	// Description:  Gets the rectangle that is the maximum client view area
	// Parameter: CRect & MaxRect
	// Returns:   void
	//************************************
	void GetViewMaxRect(CRect& MaxRect) const;

	void SetXOffset(long xOffset);
	void SetYOffset(long yOffset);

	//************************************
	// Method:    CalculateZoomToFit
	// Description:  Calculate the new Zoom factor for Zoom to Fit
	//from the the new view size 
	// Returns:   void
	//************************************
	void CalculateZoomToFit();

	//************************************
	// Method:    SetZoom
	// Description:  Set the Zoom value
	// Parameter: ZoomEnum zoom:  ZoomToFit, ZoomOneToOne, ZoomPlus, ZoomMinus
	// Returns:   void
	//************************************
	void SetZoom(ZoomEnum zoom);

	void SetIsClearOverlayByNewImage( BOOL isValue );

	long AddOverlay(const VariantParamMap& ParameterMap, bool refresh);
	long AddOverlayToGroup(long lGroup, const VariantParamMap& ParameterMap, bool refresh);
	HRESULT EditOverlay(long lHandle, const VariantParamMap& ParameterMap);

	HRESULT SetAllowEdit( long lHandle, long lEnable );
	HRESULT SelectDrawObject( long lHandle );
	HRESULT RemoveOverlay(long Handle);
	long DeleteDrawObject();
	void RemoveAllOverlays();
	HRESULT GetObjectCoords( long rlHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 ) const;
	HRESULT GetObjectParameter( long lHandle, LongParamMap& ParameterMap ) const;
	HRESULT GetObjectParameter( long lHandle, VariantParamMap& ParameterMap ) const;
	HRESULT GetViewRect( long& pX1, long& pY1, long& pX2, long& pY2 ) const;
	IPictureDisp* GetPicture() const;

	//************************************
	// Method:    HasBitmap
	// Description:  True if a bitmap is loaded
	// Returns:   bool
	//************************************
	bool HasBitmap() const;

	//************************************
	// Method:    SaveMaxRect
	// Description:  Stores the size of the Window as maximum size
	// Returns:   void
	//************************************
	void SaveMaxRect();

	//************************************
	// Method:    AdjustSizeToImage
	// Description:  Adjust the size of the winow to the size of the image
	// Returns:   void
	//************************************
	void AdjustSizeToImage();
#pragma endregion Public Methods

#pragma region Private Methods
private:
	//************************************
	// Method:    CalculateNextHandle
	// Description:  returns an unambiguous long value
	// Parameter: void
	// Returns:   long
	//************************************
	long CalculateNextHandle();

	bool IsValidObjectAtPoint( HTTYPE& p_rType, long& p_rlObject, const CPoint& imagePoint, const CPoint& viewPoint ) const;
	void TranslateMouseToPicture( CPoint& OutPoint, const CPoint& InPoint ) const;
	bool Move( const CPoint &imagePoint, const CPoint &viewMovePoint );
	HBITMAP GetBitmapFromIPictureDisp() const;
	void CreateDCs(CDC* dc);
	std::shared_ptr<DrawObject> createOverlay( const VariantParamMap& ParameterMap );
	void setImageSize(int x, int y);
#pragma endregion Private Methods

#pragma region Member Variables
	HCURSOR m_hHandCursor;
	HCURSOR m_hSizeNWSE;
	HCURSOR m_hSizeNESW;
	HCURSOR m_hSizeNS;
	HCURSOR m_hSizeWE;
	HCURSOR m_hDefaultCursor;
	HCURSOR m_hCurrentCursor;
	CComPtr<IPictureDisp> m_pIPicture;
	CDC m_BitmapDC;
	CDC m_memDC;
	HBITMAP m_memBitmap;
	BOOL m_isClearOverlayByNewImage;
	CPoint m_ScrollPos;
	CSize m_imageSize;
	COLORREF m_BackgroundColor;

	DrawObjectList m_DrawObjects;
	long m_lLastSelectedObject;
	long m_lTab;

	// Edit Variables....
	HTTYPE m_SelectType;
	long m_lSelectedObject;
	CPoint m_StartImagePoint;
	CPoint m_StartViewPoint;

	ZoomHelper m_Zoom;

	//Maximum Size of the Client Area
	CRect m_ViewMaxRect;

	//Size of the frame around the Client area
	CSize m_ViewFrame;
#pragma endregion Member Variables
};

