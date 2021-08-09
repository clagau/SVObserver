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
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVOGui/ZoomHelper.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

struct IDirectDrawSurface7;

namespace SvIe
{
class SVImageClass;
}
namespace SvTo
{
class SVToolClass;
}
namespace SvOi
{
	class IObjectWriter;
}

class SVIPDoc;
class SVDrawContext;
class SVImageExtentClass;

struct ImageIdPair
{
	uint32_t m_objectId = SvDef::InvalidObjectId;
	uint32_t m_imageId = SvDef::InvalidObjectId;
};

class SVImageView : public CView
{
	DECLARE_DYNCREATE( SVImageView )

public:
	SVImageView();
	virtual ~SVImageView();

	SVIPDoc* GetIPDoc() const;

	void AttachToImage(uint32_t imageId);
	void AttachToImage( LPCTSTR imageName );
	void DetachFromImage();

	ImageIdPair GetImageID() const;

	SvIe::SVImageClass* GetImage();
	void GetImageRect( CRect &rect );

	void GetParameters(SvOi::IObjectWriter& rWriter);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool SetParameters( SVTreeType& tree, SVTreeType::SVBranchHandle parent );

	bool CheckParameters( SVTreeType& tree, SVTreeType::SVBranchHandle parent );

	HRESULT RecreateImageSurface();

	void ShowExtremeLUT( bool show = true );
	
	void TransformFromViewSpace( CPoint& point );

	BOOL GetObjectAtPoint( POINT point );

	//{{AFX_MSG(SVImageView)
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnCaptureChanged( CWnd* pWnd );
	afx_msg void OnNcMouseMove( UINT hitTest, CPoint point );
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint point );
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnZoomTypeChanged(UINT nId);
	afx_msg void OnUpdateZoomTypeChanged(CCmdUI* PCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL( SVImageView )
	public:
	virtual BOOL Create( LPCTSTR className, LPCTSTR windowName, DWORD style, const RECT& rect, CWnd* pParentWnd, UINT NID, CCreateContext* pContext = nullptr ) override;
	virtual void OnInitialUpdate() override;
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam ) override;
	protected:
	virtual void OnDraw( CDC* pDC ) override;
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) override;
	//}}AFX_VIRTUAL

public:
	//************************************
	// Method:    ImageIsEmpty
	// Description: true if no image is in this View
	// Returns:   bool
	//************************************
	bool ImageIsEmpty() const;

	//************************************
	// Method:    SetZoomValue
	// Description:  Set the zoom value according to the zoom parameter, which should be one of the new zoom values.
	// Parameter: double value
	// Parameter: bool when true, set the zoom slider in Mainframe
	// Returns:   double
	//************************************
	double SetZoomValue(double value = 1.0, bool bSetSlider = true);

	//************************************
	// Method:    SetZoom
	// Description:  Set Zoom value according to the ZoomType parameter, which should be one of the zoom steps which are the old Zoomvalue.
	// Parameter: ZoomEnum zoom type
	// Parameter: unsigned int scaleIndex (only relevant for EZoomValue)
	// Parameter: bool when true, set the zoom slider in Mainframe
	// Returns:   bool
	//************************************
	bool SetZoom(ZoomEnum ZoomType, unsigned int scaleIndex = 1 ,bool bSetSlider = true);

	//************************************
	// Method:    GetZoomHelper
	// Description:  returns a const reference to zoom helper
	// Returns:   const ZoomHelper&
	//************************************
	const ZoomHelper& GetZoomHelper() const;

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
	bool GetScrollPosition( CPoint& point );
	bool SetScrollPosition( CPoint& point );

	bool SetScaleIndex( unsigned long scaleIndex );
	bool SetImageRect( CRect& rect );

	HRESULT ShouldDraw( const SVExtentMultiLineStruct& rMultiLine );
	void DrawOverlay( SVDrawContext* PDrawContext, const SVExtentMultiLineStruct& rMultiLine );
	void UpdateOverlays( HDC hDC, long X, long Y );

	void SelectDisplayImage();

	void ReleaseImageSurface();

	HICON GetObjectCursor( POINT point );
	HICON GetObjectCursor( SvPb::SVExtentLocationPropertyEnum svLocation, POINT point);

	HRESULT GetToolExtents(SVImageExtentClass& svToolExtents);

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump( CDumpContext& dc ) const override;
#endif

	SVBitmapInfo GetBitmapInfo() const;
	const unsigned char* GetBitmapBits() const;

	HRESULT UpdateImageSurfaces( const SVBitmapInfo& rBitmapInfo );
	HRESULT CopyBitsToSurface( const CRect& rSourceRect, const SVBitmapInfo& rBitmapInfo, const unsigned char* pBitmapBits );

	HRESULT BlitToScaledSurface( CRect& rSourceRect, CRect& rDestRect, LPCTSTR Filepath = nullptr, bool showOverlays = true);
	HRESULT BlitToPrimarySurface( CRect& rDestRect );
	HRESULT RecreateLostSurface();

	HRESULT GetRectInfo( CRect& rSourceRect, CRect& rDestRect );

	HRESULT UpdateBufferFromIPDoc();
	HRESULT UpdateSurface();
	HRESULT DisplaySurface();
	HRESULT NotifyIPDocDisplayComplete();

	//************************************
	// Method:    CalculateZoomFit
	// Description:  Calculate the m_ZoomFit parameter for the current view port and image sizes
	// Returns:   bool
	//************************************
	bool CalculateZoomFit(ZoomEnum ZoomType);

	void DisplayAnalyzerResult(const SvDef::StringVector& rAnalyzerResults, SvPb::SVObjectSubTypeEnum analyzerType) const;

#pragma region Member variables
private:
	std::string m_ImageDIB;
	SVExtentMultiLineStructVector m_OverlayData;

	ZoomHelper m_ZoomHelper;

	ImageIdPair m_ImageIdPair;
	std::string m_imageName;

	IDirectDrawSurface7* m_pDDImageSurface;
	IDirectDrawSurface7* m_pDDScaledImageSurface;

	bool m_showExtremeLUT;

	// JMS - Extent Objects

	SvTo::SVToolClass* m_pTool = nullptr;

	SvPb::SVExtentLocationPropertyEnum m_svLocation;

	SvPb::SVExtentLocationPropertyEnum m_svMousePickLocation;

	// JMS - Extent Objects

	BOOL m_isPicked;

	BOOL m_mouseIsOverTool;
	CPoint m_mousePoint;
	CPoint m_lastMouseMovePoint;

	HBRUSH m_hWindowBackgroundColor{ nullptr };
	HICON m_hActionIcon{ nullptr };

	int m_sourceImageWidth;
	int m_sourceImageHeight;
	int m_sourceBitCount;

	CRect m_LastRect;

	SVFileNameClass m_ViewOrImageFilename; /// filepath under which the last view or image was saved

#pragma endregion Member variables
};

