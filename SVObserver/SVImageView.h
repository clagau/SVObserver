//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageView
//* .File Name       : $Workfile:   SVImageView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Sep 2014 12:12:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVSystemLibrary/SVThreadWait.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/SVWinHandle.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVInfoStructs.h"
#include "SVOGui/ZoomHelper.h"
#include "SVOGui/ZoomHelperEx.h"
#include "SVLibrary/SVFileNameClass.h"
#pragma endregion Includes

struct IDirectDrawSurface7;

class SVToolClass;
class SVImageViewScroll;
class SVIPDoc;
class SVTaskObjectClass;
class SVObjectWriter;
class SVDrawContext;

// enums are parameter for function SetZoom
enum EZoom {
	EZoomMinus,
	EZoomPlus,
	EZoomFit,
	EZoomOne,
	EZoomValue };

class SVImageViewClass : public CView
{
	DECLARE_DYNCREATE( SVImageViewClass )

public:
	SVImageViewClass();
	virtual ~SVImageViewClass();

	SVIPDoc* GetIPDoc() const;

	void AttachToImage( const SVGUID& p_rImageId );
	void AttachToImage( LPCTSTR p_imageName );
	void DetachFromImage();

	const SVGUID& GetImageID() const;

	SVImageClass* GetImage();
	void GetImageRect( CRect &p_rect );

	BOOL GetParameters(SVObjectWriter& rWriter);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent );

	BOOL CheckParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent );

	HRESULT RecreateImageSurface();

	void ShowExtremeLUT( bool p_show = true );
	
	void TransformFromViewSpace( CPoint& p_point );

	BOOL GetObjectAtPoint( POINT p_point );

	//{{AFX_MSG(SVImageViewClass)
	afx_msg void OnLButtonDown( UINT p_nFlags, CPoint p_point );
	afx_msg void OnMouseMove( UINT p_nFlags, CPoint p_point );
	afx_msg void OnLButtonUp( UINT p_nFlags, CPoint p_point );
	afx_msg void OnLButtonDblClk( UINT p_nFlags, CPoint p_point );
	afx_msg void OnRButtonDblClk( UINT p_nFlags, CPoint p_point );
	afx_msg void OnCaptureChanged( CWnd* p_pWnd );
	afx_msg void OnNcMouseMove( UINT p_hitTest, CPoint p_point );
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu( CWnd* p_pWnd, CPoint p_point );
	afx_msg BOOL OnEraseBkgnd( CDC* p_pDC );
	afx_msg void OnZoomPlus();
	afx_msg void OnUpdateZoomPlus(CCmdUI *pCmdUI);
	afx_msg void OnZoomMinus();
	afx_msg void OnUpdateZoomMinus(CCmdUI *pCmdUI);
	afx_msg void OnZoomOne();
	afx_msg void OnUpdateZoomOne(CCmdUI *pCmdUI);
	afx_msg void OnZoomFit();
	afx_msg void OnZoomSliderMoved();
	afx_msg void OnUpdateZoomFit(CCmdUI *pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL( SVImageViewClass )
	public:
	virtual BOOL Create( LPCTSTR p_className, LPCTSTR p_windowName, DWORD p_style, const RECT& p_rect, CWnd* p_pParentWnd, UINT p_NID, CCreateContext* p_pContext = nullptr ) override;
	virtual void OnInitialUpdate() override;
	virtual BOOL OnCommand( WPARAM p_wParam, LPARAM p_lParam ) override;
	protected:
	virtual void OnDraw( CDC* p_pDC ) override;
	virtual void OnUpdate( CView* p_pSender, LPARAM p_lHint, CObject* p_pHint ) override;
	//}}AFX_VIRTUAL

public:
	//************************************
	// Method:    ImageIsEmpty
	// Description: true if no image is in this View
	// Returns:   bool
	//************************************
	bool ImageIsEmpty() const;

	//************************************
	// Method:    SetZoom
	// Description:  Set the zoom value according to the zoom parameter, which should be one of the new zoom values.
	// Parameter: EZoom zoom (EZoomMinus, EZoomPlus, EZoomFit, EZoomOne, EZoomValue)
	// Parameter: double value (only relevant for EZoomValue)
	// Parameter: bool when true, set the zoom slider in Mainframe
	// Returns:   double
	//************************************
	double SetZoom(EZoom zoom, double value = 1.0, bool bSetSlider = true);

	//************************************
	// Method:    SetZoomIndex
	// Description:  Set Zoom value according to the ezoom parameter, which should be one of the zoom steps which are the old Zoomvalue.
	// Parameter: EZoomMode ezoom ( SMALLEST,SMALL,NORMAL, LARGE, LARGEST, ZOOM_IN, ZOOM_OUT, ZOOM_VALUE) 
	// Parameter: unsigned int scaleIndex (only relevant for EZoomValue)
	// Parameter: bool when true, set the zoom slider in Mainframe
	// Returns:   bool
	//************************************
	bool SetZoomIndex( EZoomMode eZoom, unsigned int scaleIndex = 1 ,bool bSetSlider = true );

	//************************************
	// Method:    GetZoomHelper
	// Description:  returns a const reference to zoom helper
	// Returns:   const ZoomHelperEx&
	//************************************
	const ZoomHelperEx& GetZoomHelper() const;

	//************************************
	// Method:    IsZoomAllowed
	// Description:  Return  true if zooming is enabled
	// Returns:   bool
	//************************************
	bool IsZoomAllowed() const;

	//************************************
	// Method:    UpdateZoomToolbar
	// Description:  Updates the zoom toolbar but only when the view has the focus
	// Returns:   void
	//************************************
	void UpdateZoomToolbar();

	/// Saves either the current view (with or without overlays) or the underlying image to disk.
	/// The filepath used is chosen interactively by the user
	/// \param ViewOnly save the view if true, otherwise the underlying image
	/// \param showOverlays show overlays in saved image. ignored if ViewOnly is false
	void SaveViewOrImageToDisk(bool ViewOnly,bool showOverlays);

protected:
	void Initialize();
	bool GetScrollPosition( CPoint& p_point );
	bool SetScrollPosition( CPoint& p_point );

	bool SetScaleIndex( unsigned long p_scaleIndex );
	bool SetImageRect( CRect& p_rect );

	SVImageClass* GetImageByName( LPCTSTR ImageName ) const;

	HRESULT ShouldDraw( const SVExtentMultiLineStruct& p_rMultiLine );
	void DrawOverlay( SVDrawContext* PDrawContext, const SVExtentMultiLineStruct& p_rMultiLine );
	void UpdateOverlays( HDC p_hDC, long p_X, long p_Y );

	void SelectDisplayImage();

	void ReleaseImageSurface();

	HICON GetObjectCursor( POINT p_point );
	HICON GetObjectCursor( SVExtentLocationPropertyEnum p_svLocation, POINT p_point);

	HRESULT GetToolExtents(SVImageExtentClass& p_svToolExtents);

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump( CDumpContext& p_dc ) const override;
#endif

	SVBitmapInfo GetBitmapInfo() const;
	const unsigned char* GetBitmapBits() const;

	HRESULT UpdateImageSurfaces( const SVBitmapInfo& p_rBitmapInfo );
	HRESULT CopyBitsToSurface( const CRect& p_rSourceRect, const SVBitmapInfo& p_rBitmapInfo, const unsigned char* p_pBitmapBits );

	HRESULT BlitToScaledSurface( CRect& p_rSourceRect, CRect& p_rDestRect, LPCTSTR Filepath = nullptr, bool showOverlays = true);
	HRESULT BlitToPrimarySurface( CRect& p_rDestRect );
	HRESULT RecreateLostSurface();

	HRESULT GetRectInfo( CRect& p_rSourceRect, CRect& p_rDestRect );

	HRESULT UpdateBufferFromIPDoc();
	HRESULT UpdateSurface();
	HRESULT DisplaySurface();
	HRESULT NotifyIPDocDisplayComplete();

	//************************************
	// Method:    CalculateZoomFit
	// Description:  Calculate the m_ZoomFit parameter for the current view port and image sizes
	// Returns:   bool
	//************************************
	bool CalculateZoomFit();

#pragma region Member variables
private:
	SVByteVector m_ImageDIB;
	SVExtentMultiLineStructVector m_OverlayData;

	ZoomHelperEx m_ZoomHelper;

	SVGUID m_ImageId;
	SVString m_imageName;

	IDirectDrawSurface7* m_pDDImageSurface;
	IDirectDrawSurface7* m_pDDScaledImageSurface;

	bool m_showExtremeLUT;

	// JMS - Extent Objects

	SVTaskObjectClass* m_psvObject;

	SVExtentLocationPropertyEnum m_svLocation;

	SVExtentLocationPropertyEnum m_svMousePickLocation;

	// JMS - Extent Objects

	BOOL m_isPicked;

	BOOL m_mouseIsOverTool;
	CPoint m_mousePoint;
	CPoint m_lastMouseMovePoint;

	HBRUSH m_hWindowBackgroundColor;
	SVWinHandle<HICON> m_hActionIcon;

	int m_sourceImageWidth;
	int m_sourceImageHeight;
	int m_sourceBitCount;

	CRect m_LastRect;

	SVThreadWait m_ThreadWait;

	SVFileNameClass m_ViewOrImageFilename; /// filepath under which the last view or image was saved

#pragma endregion Member variables
};

