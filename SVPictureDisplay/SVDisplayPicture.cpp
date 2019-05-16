//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayPicture
//* .File Name       : $Workfile:   SVDisplayPicture.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   14 Aug 2014 17:35:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <cmath>
//Moved to precompiled header: #include <boost\shared_ptr.hpp>
#include "SVDisplayPicture.h"
#include "SVROIList.h"
#include "RectangleObject.h"
#include "LineObject.h"
#include "TextObject.h"
#include "ArrowObject.h"
#include "EllipseObject.h"
#include "SVPictureTabs.h"
#include "SVPictureDisplayCtrl.h"
#include "SymmetricTrapezoid.h"
#include "Doughnut.h"
#include "GraphObject.h"
#include "MarkerObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVDisplayPicture::SVDisplayPicture( long Tab )
	: m_ScrollPos( 0, 0 )
	, m_BackgroundColor( 0x00808080 )
	, m_lSelectedObject( 0 )
	, m_lLastSelectedObject( 0 )
	, m_pIPicture( nullptr )
	, m_memBitmap( nullptr )
	, m_lTab( Tab )
	, m_isClearOverlayByNewImage( TRUE )
{
	setImageSize(1, 1);
	m_ViewMaxRect.SetRect(0, 0, 100, 100);
	m_ViewFrame.cx = 0;
	m_ViewFrame.cy = 0;
	m_hHandCursor = ::LoadCursor( nullptr, MAKEINTRESOURCE(OCR_SIZEALL) );
	m_hSizeNWSE = LoadCursor( nullptr, MAKEINTRESOURCE(OCR_SIZENWSE) );
	m_hSizeNESW = LoadCursor( nullptr, MAKEINTRESOURCE(OCR_SIZENESW) );
	m_hSizeWE = LoadCursor( nullptr, MAKEINTRESOURCE(OCR_SIZEWE) );
	m_hSizeNS = LoadCursor( nullptr, MAKEINTRESOURCE(OCR_SIZENS) );
	m_hDefaultCursor = LoadCursor( nullptr, MAKEINTRESOURCE(OCR_NORMAL) );

	m_hCurrentCursor = m_hDefaultCursor;

	// Performance enhancement.

	HDC l_hdc = ::GetDC( ::GetDesktopWindow() );
	CDC l_dc;
	l_dc.Attach( l_hdc );
	CreateDCs( &l_dc );
	// Cleanup done by CDC class.
}

// Create DCs for painting...
void SVDisplayPicture::CreateDCs(CDC* dc)
{
	if( nullptr == m_memDC.m_hDC )
	{
		m_memDC.CreateCompatibleDC( dc ); // Memory DC ... Create when Pict display is created.
		m_BitmapDC.CreateCompatibleDC( dc ); // Memory DC ... Create when Pict display is created.
	}
}

SVDisplayPicture::~SVDisplayPicture()
{
	// Cleanup DCs used for painting.
	m_memDC.DeleteDC();
	m_BitmapDC.DeleteDC();

	if( nullptr != m_memBitmap )
	{
		::DeleteObject( m_memBitmap );
		m_memBitmap = nullptr;
	}
}

HBITMAP SVDisplayPicture::GetBitmapFromIPictureDisp() const
{
	HBITMAP l_hBitmap( nullptr );

	if ( nullptr != m_pIPicture.p )
	{
		IPicture* l_pPicture( nullptr );
		m_pIPicture.QueryInterface<IPicture>( &l_pPicture );

		if( l_pPicture)
		{
			HRESULT hr = l_pPicture->get_Handle( (OLE_HANDLE*)&l_hBitmap );
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			if (S_OK != hr)
			{
				TRACE(_T("GetBitmapFromIPictureDisp failed %08lx\n"), hr);
			}
#endif
			l_pPicture->Release();
		}
	}
	return l_hBitmap;
}

void SVDisplayPicture::Refresh()
{
	if (::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}


void SVDisplayPicture::SaveMaxRect()
{

	GetClientRect(&m_ViewMaxRect);
	CRect Orect;
	GetWindowRect(&Orect);
	m_ViewFrame.cx = Orect.Width() - m_ViewMaxRect.Width();
	m_ViewFrame.cy = Orect.Height() - m_ViewMaxRect.Height();
}

void SVDisplayPicture::AdjustSizeToImage()
{
	if( !HasBitmap() )
	{
		return;
	}

	long ViewRectSizeX = static_cast<long>( m_imageSize.cx * m_Zoom.GetZoom() );
	long ViewRectSizeY = static_cast<long>( m_imageSize.cy * m_Zoom.GetZoom() );

	long width = m_ViewMaxRect.Width() + m_ViewFrame.cx;
	long height = m_ViewMaxRect.Height() + m_ViewFrame.cy;

	if( ViewRectSizeX < m_ViewMaxRect.Width())
	{
		width = ViewRectSizeX + m_ViewFrame.cx; 
	}
	if( ViewRectSizeY < m_ViewMaxRect.Height())
	{
		height = ViewRectSizeY + m_ViewFrame.cy;
	}

	CRect crect;
	GetWindowRect(&crect);
	if(crect.Width() != width || crect.Height() != height )
	{
		SetWindowPos( nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER |SWP_NOZORDER );
	}
}

void SVDisplayPicture::DrawImage(CDC& dstDC, CDC& srcDC, const CRect& rect)
{
	dstDC.FillSolidRect( &rect, m_BackgroundColor );

	if( m_Zoom.IsOne() )
	{
		dstDC.BitBlt( 0, 
			0, 
			rect.Width(), 
			rect.Height(), 
			&srcDC, 
			m_ScrollPos.x, 
			m_ScrollPos.y, 
			SRCCOPY );
	}
	else
	{
		double zoom = m_Zoom.GetZoom();
		int OldStretchMode = dstDC.SetStretchBltMode( STRETCH_DELETESCANS );
		dstDC.StretchBlt( 0, 0, 
			rect.Width(), rect.Height(), 
			&srcDC, 
			m_ScrollPos.x, m_ScrollPos.y, 
			static_cast<int>( rect.Width()/zoom ), 
			static_cast<int>( rect.Height()/zoom ), 
			SRCCOPY );

		dstDC.SetStretchBltMode( OldStretchMode );
	}
}

void SVDisplayPicture::DrawOverlays(CDC& rDC)
{
	if( m_imageSize.cx > 1 && m_imageSize.cy > 1 )
	{
		double ZoomX = m_Zoom.GetZoom();
		double ZoomY = m_Zoom.GetZoom();

		DrawObjectList::iterator itt;
		for( itt = m_DrawObjects.begin(); itt != m_DrawObjects.end(); ++itt )
		{
			bool bSelected = false;
			if( itt->first == m_lLastSelectedObject )
			{
				bSelected = true;
			}
			itt->second.get()->Draw( m_ScrollPos, ZoomX, ZoomY, rDC, bSelected );
		}
	}
}

BEGIN_MESSAGE_MAP(SVDisplayPicture, CStatic)
	//{{AFX_MSG_MAP(SVDisplayPicture)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDisplayPicture message handlers

void SVDisplayPicture::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	HBITMAP hBitmap = GetBitmapFromIPictureDisp();
	if (nullptr != hBitmap)
	{
		CRect rect;	// Rect from IDC_PICT
		GetClientRect(&rect);

		HBITMAP l_OldBmp1 = (HBITMAP)::SelectObject( m_BitmapDC.m_hDC, hBitmap );

		HBITMAP l_OldBmp2 = (HBITMAP)::SelectObject( m_memDC.m_hDC, m_memBitmap );

		DrawImage(m_memDC, m_BitmapDC, rect);

		// Draw Overlays......
		m_memDC.SelectClipRgn(nullptr);	// set clip region to client rectangle.
		DrawOverlays(m_memDC);

		// blit to screen
		dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &m_memDC, 0, 0, SRCCOPY );

		::SelectObject( m_BitmapDC.m_hDC, l_OldBmp1 );
		::SelectObject( m_memDC.m_hDC, l_OldBmp2 );
	}
	else
	{	// Clear Background
		CRect rect;	// Rect from IDC_PICT
		GetClientRect(&rect);
		dc.FillSolidRect( &rect, m_BackgroundColor );
	}
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL SVDisplayPicture::OnEraseBkgnd(CDC* pDC)
{
	// Handled in OnPaint
	return true;
}

//  Set Picture should make a copy of the picture here.
HRESULT SVDisplayPicture::SetPicture( IPictureDisp* p_Picture, unsigned long BackgroundColor, bool AdjustZoom )
{
	HRESULT l_hr = S_OK;
	IPicture* l_pPicture( nullptr );
	if( nullptr != p_Picture )
	{
		// Rect from IDC_PICT this is the display area.
		CRect rect = m_ViewMaxRect;

		bool l_bFirstTime = false;
		// Delete the old bitmap.
		if( nullptr != m_memBitmap )
		{
			::DeleteObject( m_memBitmap );
			m_memBitmap = nullptr;
		}
		else
		{
			l_bFirstTime = true;
		}

		// Create compatible Bitmap here...Rather than in OnPaint to save time.
		CDC* pDC = GetDC();
		//@WARNING [gra][7.40][07.10.2016] We do not generate an event log if nullptr as this is an ocx which has not yet been linked with the SVMessage.dll
		if( nullptr != pDC )
		{
			m_memBitmap = ::CreateCompatibleBitmap( pDC->m_hDC, rect.Width(), rect.Height() );
			ReleaseDC( pDC );
		}

		p_Picture->QueryInterface( IID_IPicture, reinterpret_cast< void** >( &l_pPicture ) );

		HBITMAP l_hbm(nullptr);

		if( l_pPicture && S_OK == l_pPicture->get_Handle( reinterpret_cast< OLE_HANDLE* >( &l_hbm ) ) )
		{
			if (m_isClearOverlayByNewImage)
			{
				m_DrawObjects.clear();
			}
			if( nullptr == l_hbm )
			{
				ClearPicture();
			}
			else
			{
				m_pIPicture = p_Picture;

				BITMAP bitmap;
				if( ::GetObject( l_hbm, sizeof( bitmap ), &bitmap ) == 0 )
				{
					l_hr = E_InvalidBitmapHandle;

					setImageSize(0 , 0);
					CalculateZoomToFit();
				}
				else
				{
					setImageSize(bitmap.bmWidth, bitmap.bmHeight);
					CalculateZoomToFit();
				}
				m_BackgroundColor = BackgroundColor;

				if( AdjustZoom && (l_bFirstTime || m_Zoom.IsFit()) )
				{
					SetZoom(ZoomEnum::ZoomFitAll);
				}
				else if ( AdjustZoom && m_Zoom.IsOne() )
				{
					// Set Default Zoom here.
					SetZoom(ZoomEnum::ZoomNormal);
				}
			}
			l_pPicture->Release();
		}
		else
		{
			l_hr = E_InvalidPictureHandle;
		}
	}
	else
	{
		ClearPicture();
		l_hr = E_InvalidPictureHandle;
	}

	return l_hr;
}

void SVDisplayPicture::ClearPicture()
{
	m_pIPicture.Release();
	setImageSize(0, 0);
	m_Zoom.Clear();
}

HRESULT SVDisplayPicture::SetPictureWithROI( IPictureDisp* p_Picture, unsigned long BackgroundColor, ISVROIList* p_RoiList, bool AdjustZoom )
{
	HRESULT l_hr = S_OK;
	IPicture* l_pPicture( nullptr );

	m_DrawObjects.clear();

	if( nullptr != p_Picture )
	{
		// Rect from IDC_PICT this is the display area.
		CRect rect = m_ViewMaxRect;

		bool l_bFirstTime = false;
		// Delete the old bitmap.
		if( nullptr != m_memBitmap )
		{
			::DeleteObject( m_memBitmap );
			m_memBitmap = nullptr;
		}
		else
		{
			l_bFirstTime = true;
		}

		// Create compatible Bitmap here...Rather than in OnPaint to save time.
		CDC* dc = GetDC();
		m_memBitmap = ::CreateCompatibleBitmap( dc->m_hDC, rect.Width(), rect.Height() );
		ReleaseDC( dc );

		p_Picture->QueryInterface( IID_IPicture, reinterpret_cast< void** >( &l_pPicture ) );

		HBITMAP l_hbm(nullptr);

		if( l_pPicture && S_OK == l_pPicture->get_Handle( reinterpret_cast< OLE_HANDLE* >( &l_hbm ) ) )
		{
			if( nullptr == l_hbm )
			{
				ClearPicture();
			}
			else
			{
				m_pIPicture = p_Picture;

				BITMAP bitmap;
				if( ::GetObject( l_hbm, sizeof( bitmap ), &bitmap ) == 0 )
				{
					l_hr = E_InvalidBitmapHandle;
					setImageSize(0, 0);
					CalculateZoomToFit();
				}
				else
				{
					setImageSize(bitmap.bmWidth, bitmap.bmHeight);
					CalculateZoomToFit();
				}
				m_BackgroundColor = BackgroundColor;

				// Set DrawObjects
				CSVROIList* pList = CSVROIList::GetCppObjectFromDispatchPointer(p_RoiList);
				long lCount = pList->GetCount();

				// List is one based...
				for ( long i = 1; i <= lCount; ++i )
				{
					IDispatch* pDispatch = pList->GetItem(i);
					CSVROI* pRoi = CSVROI::GetCppObjectFromDispatchPointer(pDispatch);
					long parent = pRoi->GetParent();
					if (parent <= 0)
					{
						AddOverlayToGroup(parent, pRoi->GetParameterMap(), false);
					}
					else
					{
						AddOverlay(pRoi->GetParameterMap(), false);
					}
					pDispatch->Release();
				}
				if( AdjustZoom && (l_bFirstTime || m_Zoom.IsFit()) )
				{
					SetZoom(ZoomEnum::ZoomFitAll);
				}
				else if( AdjustZoom && m_Zoom.IsOne() )
				{
					SetZoom(ZoomEnum::ZoomNormal);
				}
			}
			l_pPicture->Release();
		}
		else
		{
			l_hr = E_InvalidPictureHandle;
		}
	}
	else
	{
		l_hr = E_InvalidPictureHandle;
		ClearPicture();
	}
	return l_hr;
}

// Get picture should also make a copy of the picture.
IPictureDisp* SVDisplayPicture::GetPicture() const
{
	IPictureDisp* pPictureDisp( nullptr );
	if ( nullptr == m_pIPicture.p )
	{
		m_pIPicture.QueryInterface<IPictureDisp>(&pPictureDisp);
	}
	return pPictureDisp;
}

void SVDisplayPicture::CalculateZoomToFit()
{
	if(!HasBitmap())
	{
		return;
	}
	if (::IsWindow(m_hWnd) && m_imageSize.cx > 0 && m_imageSize.cy > 0)
	{
		CRect rect = m_ViewMaxRect;

		CSize ViewSize(rect.Width(), rect.Height());
		m_Zoom.CalculateZoomFit(ZoomEnum::ZoomFitAll, m_imageSize, ViewSize);
	}
}

void SVDisplayPicture::SetZoom( ZoomEnum ZoomType )
{
	if(!HasBitmap())
	{
		return;
	}

	m_Zoom.SetZoomType(ZoomType);

	CloneZoomSettings();

	AdjustSizeToImage();

	Refresh();
}

void SVDisplayPicture::SetIsClearOverlayByNewImage( BOOL isValue )
{
	m_isClearOverlayByNewImage = isValue;
}

long SVDisplayPicture::AddOverlay(const VariantParamMap& ParameterMap, bool refresh)
{
	long Handle = -1; // BRW - Better to return a constant representing an invalid handle.
	try
	{
		std::shared_ptr<DrawObject> pOverlay = createOverlay(ParameterMap);
		Handle = CalculateNextHandle();
		m_DrawObjects.insert( std::make_pair( Handle, pOverlay ) );
		if (refresh)
		{
			Refresh();
		}
	}
	catch (...)
	{
		return -1; // BRW - Better to return a constant representing an invalid handle.
	}
	return Handle;
}

long SVDisplayPicture::AddOverlayToGroup(long lGroup, const VariantParamMap& ParameterMap, bool refresh)
{
	long handle = -1; // BRW - Better to return a constant representing an invalid handle.

	if( 0 != lGroup )
	{
		if( m_DrawObjects.find( lGroup ) != m_DrawObjects.end() )
		{
			// Existing Group....
			long lAllowEdit = m_DrawObjects[lGroup]->GetEditAllowed();
			std::shared_ptr<DrawObject> pOverlay = createOverlay(ParameterMap);
			pOverlay->SetEditAllowed(lAllowEdit);
			handle = CalculateNextHandle();
			m_DrawObjects[lGroup]->AddDrawObjectChild( handle, pOverlay );
		}
	}
	else
	{
		// new Group
		std::shared_ptr<DrawObject> pOverlay = createOverlay(ParameterMap);
		handle = CalculateNextHandle();
		m_DrawObjects.insert( std::make_pair( handle, pOverlay ) );
	}

	if (refresh)
	{
		Refresh();
	}
	return handle;
}

HRESULT SVDisplayPicture::EditOverlay(long lHandle, const VariantParamMap& ParameterMap)
{
	HRESULT hr = S_OK;
	bool bFound = false;
	DrawObjectList::const_iterator l_it = m_DrawObjects.begin();
	for( ; l_it != m_DrawObjects.end(); ++l_it )
	{
		if( l_it->first == lHandle )
		{
			bFound = true;
			break;
		}
		else
		{
			const DrawObjectList& l_ChildList = l_it->second->GetChildList();
			DrawObjectList::const_iterator l_itChild = l_ChildList.find( lHandle );
			if( l_itChild != l_ChildList.end() )
			{
				l_it = l_itChild;
				bFound = true;
				break;
			}
		}
	}

	if( bFound )
	{
		DrawObject* pDrawObject = ( l_it->second.get() );
		if( nullptr != pDrawObject )
		{
			pDrawObject->SetParameter(ParameterMap);
			Refresh();
		}
	}
	else
	{
		hr = E_InvalidOverlay; // Invalid handle
	}

	return hr;
}

HRESULT SVDisplayPicture::RemoveOverlay(long Handle)
{
	HRESULT l_hr = S_OK;
	bool bFound = false;
	for( DrawObjectList::iterator itter = m_DrawObjects.begin(); itter != m_DrawObjects.end(); ++itter )
	{
		if( itter->first == Handle )
		{
			m_DrawObjects.erase( itter );
			bFound = true;
			break;
		}
		else if( itter->second->RemoveChild( Handle ) )
		{
			bFound = true;
			break;
		}
	}

	if (bFound)
	{
		Refresh();
	}
	else
	{
		l_hr = E_InvalidOverlay; // Invalid Handle
	}
	return l_hr;
}

// Returns the handle of the object deleted.
long SVDisplayPicture::DeleteDrawObject()
{
	long l_lHandle = m_lLastSelectedObject;
	if( m_lLastSelectedObject > 0 )
	{
		if(m_DrawObjects.find(m_lLastSelectedObject) != m_DrawObjects.end() )
		{
			DrawObject* l_pObject = m_DrawObjects[m_lLastSelectedObject].get();
			long l_lAllow = l_pObject->GetEditAllowed();
			if( (l_lAllow & AllowDelete) == AllowDelete )		// Operator is allowed to delete the object....
			{
				RemoveOverlay( m_lLastSelectedObject );
				Refresh();
				m_lLastSelectedObject = 0;
			}
		}
	}
	return l_lHandle;
}

void SVDisplayPicture::RemoveAllOverlays()
{
	m_DrawObjects.clear();
	Refresh();
}

SVPictureTabs* SVDisplayPicture::GetPictureTab() const
{
	SVPictureTabs* ret = nullptr;
	CWnd* pParent1 = GetParent();
	CWnd* pParent2 = pParent1->GetParent();
	CWnd* pParent3 = pParent2->GetParent();
	ret = dynamic_cast<SVPictureTabs*>(pParent3);
	return ret;
}

void SVDisplayPicture::CloneZoomSettings()
{
	SVPictureTabs* pictureTab = GetPictureTab();
	if(pictureTab && pictureTab->GetZoomSimultaneous()) // BRW - Why do we care about GetZoomSimultaneous here?
	{
		pictureTab->CloneZoomSettings(m_lTab);
	}
}

void SVDisplayPicture::SetZoomFactor(double scaleFactor)
{
	if(!HasBitmap())
	{
		return;
	}

	m_Zoom.SetZoom(scaleFactor);
	CloneZoomSettings();

	AdjustSizeToImage();

	Refresh();
}

double SVDisplayPicture::GetZoomFactor() const
{
	return m_Zoom.GetZoom();
}

SIZE SVDisplayPicture::GetImageSize() const
{
	return m_imageSize;
}

void SVDisplayPicture::SetXOffset(long xOffset)
{
	m_ScrollPos.x = xOffset;
}

void SVDisplayPicture::SetYOffset(long yOffset)
{
	m_ScrollPos.y = yOffset;
}

bool SVDisplayPicture::Move( const CPoint &imagePoint, const CPoint &viewPoint )
{
	bool bSuccess = false;
	bool boundaryCheck = 0 != ( GetPictureTab()->GetBoundaryCheck() );

	if( m_DrawObjects.find(m_lSelectedObject) != m_DrawObjects.end() )
	{
		DrawObject* pObject = m_DrawObjects[m_lSelectedObject].get();

		long allow = pObject->GetEditAllowed();
		bool bAlMoveAndSize = 0 != (allow & AllowResizeAndMove);
		bool bAlMove = 0 != (allow & AllowMove);
		bool bAlSize = 0 != (allow & AllowResize);

		CPoint PointDiffImage = imagePoint - m_StartImagePoint;
		CPoint PointDiffView = viewPoint - m_StartViewPoint;

		CRect imageRect(0, 0, m_imageSize.cx, m_imageSize.cy);
		CRect viewRect;
		GetClientRect(&viewRect);
		CRect lBound;
		pObject->GetBoundingRect(lBound);

		switch( m_SelectType )
		{
		case HTTOP | HTX_INNER_EDGE: // fall through...
		case HTTOPLEFT | HTX_INNER_EDGE: // fall through...
		case HTLEFT | HTX_INNER_EDGE: // fall through...
		case HTBOTTOMLEFT | HTX_INNER_EDGE: // fall through...
		case HTBOTTOM | HTX_INNER_EDGE: // fall through...
		case HTBOTTOMRIGHT | HTX_INNER_EDGE: // fall through...
		case HTRIGHT | HTX_INNER_EDGE: // fall through...
		case HTTOPRIGHT | HTX_INNER_EDGE:
			{
				if(bAlSize)
				{
					bSuccess = pObject->Move( m_SelectType, PointDiffImage, PointDiffView);
				}
				break;
			}

		case HTTOPLEFT:
			{
				BOOL bMove = bAlMoveAndSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.left > lBound.left + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.left - lBound.left - PointDiffImage.x);
					}
					if (viewRect.left > lBound.left + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.left - lBound.left - PointDiffView.x);
					}
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.top > lBound.top + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.top - lBound.top - PointDiffImage.y);
					}
					if (viewRect.top > lBound.top + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.top - lBound.top - PointDiffView.y);
					}
				}
				if(bMove)
				{
					bSuccess = pObject->Move( HTTOPLEFT, PointDiffImage, PointDiffView );
				}

				break;
			}

		case HTTOP:
			{
				BOOL bMove = bAlMoveAndSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.top > lBound.top + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.top - lBound.top - PointDiffImage.y);
					}
					if (viewRect.top > lBound.top + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.top - lBound.top - PointDiffView.y);
					}
				}
				if( bMove )
				{
					bSuccess = pObject->Move( HTTOP, PointDiffImage, PointDiffView);
				}
				break;
			}

		case HTLEFT:
			{
				BOOL bMove = bAlMoveAndSize;

				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.left > lBound.left + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.left - lBound.left - PointDiffImage.x);
					}
					if (viewRect.left > lBound.left + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.left - lBound.left - PointDiffView.x);
					}
				}
				if( bMove )
				{
					bSuccess = pObject->Move( HTLEFT, PointDiffImage, PointDiffView );
				}
				break;
			}

		case HTTOPRIGHT:
			{
				BOOL bMove = bAlMoveAndSize;

				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.right < lBound.right + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.right - lBound.right - PointDiffImage.x);
					}
					if (viewRect.right < lBound.right + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.right - lBound.right - PointDiffView.x);
					}
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.top > lBound.top + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.top - lBound.top - PointDiffImage.y);
					}
					if (viewRect.top > lBound.top + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.top - lBound.top - PointDiffView.y);
					}
				}

				if(bMove )
				{
					bSuccess = pObject->Move( HTTOPRIGHT, PointDiffImage, PointDiffView );
				}

				break;
			}

		case HTBOTTOMLEFT:
			{
				BOOL bMove = bAlMoveAndSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.left > lBound.left + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.left - lBound.left - PointDiffImage.x);
					}
					if (viewRect.left > lBound.left + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.left - lBound.left - PointDiffView.x);
					}
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.bottom < lBound.bottom + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.bottom - lBound.bottom - PointDiffImage.y);
					}
					if (viewRect.bottom < lBound.bottom + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.bottom - lBound.bottom - PointDiffView.y);
					}
				}
				if( bMove )
				{
					bSuccess = pObject->Move( HTBOTTOMLEFT, PointDiffImage, PointDiffView );
				}

				break;
			}

		case HTBOTTOMRIGHT:
			{
				BOOL bMove = bAlSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.right < lBound.right + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.right - lBound.right - PointDiffImage.x);
					}
					if (viewRect.right < lBound.right + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.right - lBound.right - PointDiffView.x);
					}
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.bottom < lBound.bottom + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.bottom - lBound.bottom - PointDiffImage.y);
					}
					if (viewRect.bottom < lBound.bottom + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.bottom - lBound.bottom - PointDiffView.y);
					}
				}
				if(bMove)
				{
					bSuccess = pObject->Move( HTBOTTOMRIGHT, PointDiffImage, PointDiffView );
				}
				break;
			}

		case HTBOTTOM:
			{
				BOOL bMove =bAlSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.bottom < lBound.bottom + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.bottom - lBound.bottom - PointDiffImage.y);
					}
					if (viewRect.bottom < lBound.bottom + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.bottom - lBound.bottom - PointDiffView.y);
					}
				}

				if( bMove )
				{
					bSuccess = pObject->Move( HTBOTTOM, PointDiffImage, PointDiffView );
				}
				break;
			}

		case HTRIGHT:
			{
				BOOL bMove = bAlSize;
				if(bMove && boundaryCheck)
				{
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.right < lBound.right + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.right - lBound.right - PointDiffImage.x);
					}
					if (viewRect.right < lBound.right + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.right - lBound.right - PointDiffView.x);
					}
				}
				if(bMove )
				{
					bSuccess = pObject->Move( HTRIGHT, PointDiffImage, PointDiffView );
				}
				break;
			}

		case HTOBJECT:
			{
				BOOL bMove = bAlMove;

				if(bMove && boundaryCheck)
				{
					imageRect.NormalizeRect();
					//check if movement in the boundary, if not set PointDiff to the boundary.
					if (imageRect.left > lBound.left + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.left - lBound.left - PointDiffImage.x);
					}
					if (viewRect.left > lBound.left + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.left - lBound.left - PointDiffView.x);
					}
					if (imageRect.right < lBound.right + PointDiffImage.x)
					{
						PointDiffImage.x += (imageRect.right - lBound.right - PointDiffImage.x);
					}
					if (viewRect.right < lBound.right + PointDiffView.x)
					{
						PointDiffView.x += (viewRect.right - lBound.right - PointDiffView.x);
					}
					if (imageRect.top > lBound.top + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.top - lBound.top - PointDiffImage.y);
					}
					if (viewRect.top > lBound.top + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.top - lBound.top - PointDiffView.y);
					}
					if (imageRect.bottom < lBound.bottom + PointDiffImage.y)
					{
						PointDiffImage.y += (imageRect.bottom - lBound.bottom - PointDiffImage.y);
					}
					if (viewRect.bottom < lBound.bottom + PointDiffView.y)
					{
						PointDiffView.y += (viewRect.bottom - lBound.bottom - PointDiffView.y);
					}
				}
				if( bMove )
				{
					bSuccess = pObject->Move( HTOBJECT, PointDiffImage, PointDiffView );
				}
				break;
			}

		default:
			{
				break;
			}
		}

		if( bSuccess )
		{
			GetPictureDisplayCtrl()->ObjectMovedEx( m_lTab, m_lSelectedObject);
		}
	}

	return bSuccess;
}

void SVDisplayPicture::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint imagePoint;
	TranslateMouseToPicture( imagePoint, point );
	HTTYPE l_Type = HTNOWHERE;
	long Object=0;
	if( IsValidObjectAtPoint( l_Type, Object, imagePoint, point ) )
	{
		m_lSelectedObject = Object;
		m_StartImagePoint = imagePoint;
		m_StartViewPoint = point;
		SetCapture();
	}

	m_SelectType = l_Type;
	SelectCursor();

	if( Object > 0 ) // send Object Selected message to control
	{
		GetPictureDisplayCtrl()->SendMessage( OBJECT_SELECTED, m_lTab, m_lSelectedObject );
	}
	else
	{
		if( m_lLastSelectedObject > 0 )
		{
			//Fire Event with Handle == -1 to show deselection
			GetPictureDisplayCtrl()->SendMessage( OBJECT_SELECTED, m_lTab, -1 );
		}
	}

	m_lLastSelectedObject = Object;

	::SetCursor( m_hCurrentCursor );
	Refresh();
	CStatic::OnLButtonDown(nFlags, point);
	GetPictureDisplayCtrl()->SetFocus();
}

void SVDisplayPicture::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CPoint imagePoint;
	TranslateMouseToPicture( imagePoint, point );
	if( m_lSelectedObject )
	{
		Move( imagePoint, point );
		SVPictureDisplayCtrl* pCtrl = GetPictureDisplayCtrl();
		if(pCtrl)
		{
			pCtrl->ObjectChangedEx( m_lTab, m_lSelectedObject);
		}

		m_lSelectedObject = 0;
	}

	CStatic::OnLButtonUp(nFlags, point);
}

SVPictureDisplayCtrl* SVDisplayPicture::GetPictureDisplayCtrl() const
{
	CWnd* pParent1 = GetParent();
	CWnd* pParent2 = pParent1->GetParent();
	CWnd* pParent3 = pParent2->GetParent();
	CWnd* pParent4 = pParent3->GetParent();
	SVPictureDisplayCtrl* ret = dynamic_cast<SVPictureDisplayCtrl*>(pParent4);
	return ret;
}

void SVDisplayPicture::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint imagePoint;
	TranslateMouseToPicture( imagePoint, point );
	if( m_lSelectedObject )
	{
		if( Move( imagePoint, point ) )
		{
			m_StartImagePoint = imagePoint;
			m_StartViewPoint = point;
		}
		Refresh();
	}

	long Object = 0;
	if( m_lSelectedObject || IsValidObjectAtPoint( m_SelectType, Object, imagePoint, point ) )
	{
		SelectCursor();
		::SetCursor( m_hCurrentCursor );
	}

	GetPictureDisplayCtrl()->SendMessage( MOUSE_MOVEDIM, imagePoint.x, imagePoint.y );
	CStatic::OnMouseMove(nFlags, point);
}

void SVDisplayPicture::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	DrawObjectList::iterator it;
	for( it = m_DrawObjects.begin(); it != m_DrawObjects.end(); ++it )
	{
		it->second->setViewSize(CSize(cx, cy));
	}
}

void SVDisplayPicture::SelectCursor()
{
	switch ( 0xFF &  m_SelectType)
	{
	case HTTOPLEFT:
		{
			m_hCurrentCursor = m_hSizeNWSE;
			break;
		}
	case HTTOPRIGHT:
		{
			m_hCurrentCursor = m_hSizeNESW;
			break;
		}
	case HTBOTTOMLEFT:
		{
			m_hCurrentCursor = m_hSizeNESW;
			break;
		}
	case HTBOTTOMRIGHT:
		{
			m_hCurrentCursor = m_hSizeNWSE;
			break;
		}
	case HTOBJECT:
		{
			m_hCurrentCursor = m_hHandCursor;
			break;
		}
	case HTLEFT: // fall through...
	case HTRIGHT:
		{
			m_hCurrentCursor = m_hSizeWE;
			break;
		}
	case HTTOP: // fall through...
	case HTBOTTOM:
		{
			m_hCurrentCursor = m_hSizeNS;
			break;
		}
	default:
		{
			m_hCurrentCursor = m_hDefaultCursor;
			break;
		}
	}
}

bool SVDisplayPicture::HasBitmap() const
{
	return ( nullptr != m_memBitmap );
}

// The coordinates of the rectangle collection are checked against the parameter point.
// if there is an object at this point, then type is set based on where on the object
// is hit. The object handle is set into p_rlObject.
// 1 - TopLeft
// 2 - TopRight
// 3 - BottomLeft
// 4 - BottomRight
// 5 - Anywhere inside the object
bool SVDisplayPicture::IsValidObjectAtPoint( HTTYPE& p_rType, long& p_rlObject, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	// Iterate through Rectangles to find a valid Rectangle.
	bool bRet = false;
	long lFirstSelectedObject = 0;
	DrawObjectList::const_iterator l_it;
	for( l_it = m_DrawObjects.begin(); l_it != m_DrawObjects.end(); ++l_it )
	{
		if( l_it->second->IsValidObjectAtPoint( p_rType, imagePoint, viewPoint ) )
		{
			bRet = true;
			p_rlObject = l_it->first;
			if( lFirstSelectedObject == 0 )
			{
				lFirstSelectedObject = l_it->first;
			}

			if( m_lLastSelectedObject != p_rlObject )
			{
				if( p_rlObject > m_lLastSelectedObject )
					break;
			}
		}
	}
	if( m_lLastSelectedObject == p_rlObject )
	{
		l_it = m_DrawObjects.find( lFirstSelectedObject );
		if( l_it != m_DrawObjects.end() )
		{
			l_it->second->IsValidObjectAtPoint( p_rType, imagePoint, viewPoint );
			p_rlObject = lFirstSelectedObject;
		}
	}
	return bRet;
}

// Translates the in point to picture coordinates
void SVDisplayPicture::TranslateMouseToPicture( CPoint& OutPoint, const CPoint& InPoint ) const
{
	// Use zoom and offset to find the real picture point.

	if( m_Zoom.IsOne() )
	{
		OutPoint.x = InPoint.x + m_ScrollPos.x;
		OutPoint.y = InPoint.y + m_ScrollPos.y;
	}
	else
	{
		double zoom = m_Zoom.GetZoom();
		OutPoint.x = static_cast<int>( ( ( InPoint.x ) / zoom ) + m_ScrollPos.x );
		OutPoint.y = static_cast<int>( ( ( InPoint.y ) / zoom ) + m_ScrollPos.y );
	}
}

HRESULT SVDisplayPicture::SetAllowEdit( long lHandle, long lEnable )
{
	HRESULT l_hr = E_InvalidDrawObject;
	DrawObjectList::iterator l_it = m_DrawObjects.find( lHandle );
	if( l_it != m_DrawObjects.end() )
	{
		DrawObject* pObject = l_it->second.get();
		if( pObject )
		{
			l_hr = pObject->SetEditAllowed( lEnable );
		}
	}
	return l_hr;
}

HRESULT SVDisplayPicture::SelectDrawObject( long lHandle )
{
	HRESULT l_hr = E_InvalidDrawObject;
	DrawObjectList::iterator l_it = m_DrawObjects.find( lHandle );
	if( l_it != m_DrawObjects.end() )
	{
		m_lLastSelectedObject = lHandle;
		GetPictureDisplayCtrl()->SendMessage( OBJECT_SELECTED, m_lTab, m_lLastSelectedObject );
	}
	else
	{
		m_lLastSelectedObject = 0;
	}
	Refresh();
	return l_hr;
}

HRESULT SVDisplayPicture::GetObjectParameter( long lHandle, VariantParamMap& ParameterMap ) const
{
	HRESULT hr = S_FALSE;

	DrawObjectList::const_iterator it = m_DrawObjects.find( lHandle );
	if( it != m_DrawObjects.end() )
	{
		DrawObject* pObject = it->second.get();
		if( nullptr != pObject )
		{
			pObject->GetParameter(ParameterMap);
			hr = S_OK;
		}
	}

	return hr;
}

HRESULT SVDisplayPicture::GetObjectCoords( long lHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 ) const
{
	HRESULT l_hr = S_FALSE;
	DrawObjectList::const_iterator l_it = m_DrawObjects.find( lHandle );
	if( l_it != m_DrawObjects.end() )
	{
		DrawObject* pObject = l_it->second.get();
		RECT l_Rect = pObject->GetRectangle();
		rlX1 = l_Rect.left;
		rlY1 = l_Rect.top;
		rlX2 = l_Rect.right;
		rlY2 = l_Rect.bottom;
		l_hr = S_OK;
	}
	return l_hr;
}

HRESULT SVDisplayPicture::GetViewRect( long& pX1, long& pY1, long& pX2, long& pY2 ) const
{
	CRect rect; // Rect from IDC_PICT
	GetClientRect(&rect);

	CPoint topLeftPoint;
	CPoint bottomRightPoint;

	TranslateMouseToPicture( topLeftPoint, rect.TopLeft() );
	TranslateMouseToPicture( bottomRightPoint, rect.BottomRight() );
	pX1 = topLeftPoint.x;
	pY1 = topLeftPoint.y;
	pX2 = bottomRightPoint.x;
	pY2 = bottomRightPoint.y;
	return S_OK;
}

long SVDisplayPicture::GetTabHandler() const
{
	return m_lTab;
}

void SVDisplayPicture::GetZoomRanges(double &min, double &max) const
{
	min = m_Zoom.GetZoomMin();
	max = m_Zoom.GetZoomMax();
}

long SVDisplayPicture::CalculateNextHandle()
{
	SVPictureDisplayCtrl* pCtrl = GetPictureDisplayCtrl();
	if(pCtrl)
	{
		return pCtrl->CalculateNextHandle();
	}
	else
	{
		ASSERT(FALSE);
		return 0;
	}
}

void SVDisplayPicture::GetViewMaxRect(CRect& MaxRect) const
{
	MaxRect = m_ViewMaxRect;
}

std::shared_ptr<DrawObject> SVDisplayPicture::createOverlay( const VariantParamMap& ParameterMap )
{
	std::shared_ptr<DrawObject> pOverlay = nullptr;
	long overlayType = ParameterMap.at(P_Type).lVal;
	switch (overlayType)
	{
	case RectangleROI:
		pOverlay = std::shared_ptr<DrawObject>(new RectangleObject());
		break;
	case LineROI:
		pOverlay = std::shared_ptr<DrawObject>(new LineObject());
		break;
	case TextROI:
		pOverlay = std::shared_ptr<DrawObject>(new TextObject());
		break;
	case ArrowROI:
		pOverlay = std::shared_ptr<DrawObject>(new ArrowObject());
		break;
	case EllipseROI:
		pOverlay = std::shared_ptr<DrawObject>(new EllipseObject());
		break;
	case DoughnutROI:
		pOverlay = std::shared_ptr<DrawObject>(new Doughnut());
		break;
	case TrapezoidROI:
		pOverlay = std::shared_ptr<DrawObject>(new SymmetricTrapezoid());
		break;
	case GraphROI:
		pOverlay = std::shared_ptr<DrawObject>(new GraphObject());
		break;
	case MarkerROI:
		pOverlay = std::shared_ptr<DrawObject>(new MarkerObject());
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	pOverlay->SetParameter(ParameterMap);
	pOverlay->setImageSize(m_imageSize);
	CRect rect;	// Rect from IDC_PICT
	GetClientRect(&rect);
	pOverlay->setViewSize(CSize(rect.Width(), rect.Height()));
	return pOverlay;
}

void SVDisplayPicture::CloneZoomHelper(const ZoomHelper& zh)
{
	return m_Zoom.CloneSettings(zh);
}

const ZoomHelper& SVDisplayPicture::GetZoomHelper() const
{
	return const_cast<const ZoomHelper&>(m_Zoom);
}

void SVDisplayPicture::setImageSize( int x, int y )
{
	m_imageSize = CSize(x, y);
	DrawObjectList::iterator it;
	for( it = m_DrawObjects.begin(); it != m_DrawObjects.end(); ++it )
	{
		it->second->setImageSize(m_imageSize);
	}
}

