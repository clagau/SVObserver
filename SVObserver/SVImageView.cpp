//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageView
//* .File Name       : $Workfile:   SVImageView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   09 Dec 2014 09:20:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageView.h"
#include "SVDirectX.h"
#include "SVDisplayImageSelect.h"
#include "SVDrawObject.h"
#include "SVImageViewScroll.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "SVObserverOuttakes.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVPatResultDlgClass.h"
#include "SVSetupDialogManager.h"
#include "SVMainFrm.h"
#include "SVIPSplitterFrame.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "AnalyzerOperators/Analyzer.h"
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/ImageFileUtilities.h"
#include "SVMFCControls/SVAnalyzerResultDlg.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOGui/SVAdjustToolSizePositionDlg.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

const  LPCTSTR  RegSection = _T( "Settings" );
const  LPCTSTR  RegKeySaveViewPath = _T( "SaveViewFilePath" );
const LPCTSTR  DefaultPath =  _T( "C:\\Images" );  

IMPLEMENT_DYNCREATE(SVImageView, CView)

BEGIN_MESSAGE_MAP(SVImageView, CView)
	//{{AFX_MSG_MAP( SVImageView )
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	ON_WM_NCMOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//! When using Range the IDs must be sequential
	ON_COMMAND_RANGE(ID_ZOOM_SMALLEST, ID_ZOOM_FIT_HEIGHT, OnZoomTypeChanged)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_SMALLEST, ID_ZOOM_FIT_HEIGHT, OnUpdateZoomTypeChanged)
END_MESSAGE_MAP()

static void UpdatePaletteForLut(bool bLut, LPDIRECTDRAWSURFACE7 pSurface)
{
	DDPIXELFORMAT l_ddPixelFormat;
	memset( &l_ddPixelFormat, 0, sizeof( l_ddPixelFormat ) );
	l_ddPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	pSurface->GetPixelFormat(&l_ddPixelFormat);
	size_t l_PixelWidth = l_ddPixelFormat.dwRGBBitCount / 8;
	if (l_PixelWidth <= 1)
	{
		LPDIRECTDRAWPALETTE pPalette;
		HRESULT hr = pSurface->GetPalette(&pPalette);
		if (S_OK == hr)
		{
			if (bLut)
			{
				PALETTEENTRY entry0 = { 0, 0, 255, 0}; 
				PALETTEENTRY entry255 = { 0, 255, 0, 0}; 

				pPalette->SetEntries(0, 0, 1, &entry0);
				pPalette->SetEntries(0, 255, 1, &entry255);
			}
			else
			{
				PALETTEENTRY entry0 = { 0, 0, 0, 0}; 
				PALETTEENTRY entry255 = { 255, 255, 255, 0}; 

				pPalette->SetEntries(0, 0, 1, &entry0);
				pPalette->SetEntries(0, 255, 1, &entry255);
			}
			pPalette->Release();
		}
	}
}

// We operating on the real (unscaled) image here
// function  argument pWidth is in bytes not pixels!
static void CalcLut(BYTE* pFrom, BYTE* pTo, unsigned long pWidth, unsigned long pMask, unsigned long pLowerValue, unsigned long pUpperValue, size_t pixelWidth )
{
	if( 1 < pixelWidth )
	{
		BYTE* l_pSrc = pFrom;
		BYTE* l_pDst = pTo;

		for( size_t l_col = 0; l_col < pWidth / pixelWidth; ++l_col )
		{
			unsigned long* l_pSrcPixel = reinterpret_cast< unsigned long* >( l_pSrc );
			unsigned long* l_pDstPixel = reinterpret_cast< unsigned long* >( l_pDst );

			unsigned long l_Value = *l_pSrcPixel;

			if( 0xFFFFFFFF == ( l_Value | ( ~pMask ) )  )
			{
				*l_pDstPixel = ( l_Value & ( ~pMask ) ) | pUpperValue;
			}
			else if( 0x00000000 == ( l_Value & pMask )  )
			{
				*l_pDstPixel = l_Value | pLowerValue;
			}
			else
			{
				*l_pDstPixel = l_Value;
			}
			l_pSrc += pixelWidth;
			l_pDst += pixelWidth;
		}
	}
}

SVImageView::SVImageView()
: CView()
, m_sourceImageWidth( 0 )
, m_sourceImageHeight( 0 )
, m_sourceBitCount( 0 )
,m_ViewOrImageFilename(SvStl::GlobalPath::Inst().GetSecondObserverPath(_T("Image.bmp")).c_str())
{
	Initialize();
}

void SVImageView::Initialize()
{
	EnableAutomation();

	m_showExtremeLUT = false;

	m_svLocation = SvPb::SVExtentLocationPropertyUnknown;

	m_svMousePickLocation = SvPb::SVExtentLocationPropertyUnknown;

	m_mouseIsOverTool = FALSE;

	m_pDDImageSurface = nullptr;
	m_pDDScaledImageSurface = nullptr;

	m_isPicked = FALSE;

	m_hWindowBackgroundColor = ::CreateSolidBrush( SvDef::cDefaultImageViewBackgroundColor );
}

SVImageView::~SVImageView()
{
	DetachFromImage();

	if( m_hWindowBackgroundColor )
	{
		DeleteObject( m_hWindowBackgroundColor );
	}

	if(nullptr != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon );
		m_hActionIcon = nullptr;
	}
}

void SVImageView::ReleaseImageSurface()
{
	m_sourceImageWidth = 0;
	m_sourceImageHeight = 0;
	m_sourceBitCount = 0;

	if( !( m_OverlayData.empty() ) )
	{
		m_OverlayData.clear();
	}

	if( nullptr != m_pDDImageSurface )
	{
		m_pDDImageSurface->Release();

		m_pDDImageSurface = nullptr;
	}

	if( nullptr != m_pDDScaledImageSurface )
	{
		m_pDDScaledImageSurface->Release();

		m_pDDScaledImageSurface = nullptr;
	}
}

HRESULT SVImageView::GetToolExtents( SVImageExtentClass& rToolExtents )
{
	HRESULT l_hrOk = S_OK;

	if( nullptr != m_pTool )
	{
		rToolExtents = m_pTool->GetImageExtent();
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

void SVImageView::AttachToImage(uint32_t imageId)
{
	if( SvDef::InvalidObjectId != imageId )
	{
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(imageId);
		SvIe::SVImageClass* pImage = SvIe::castObjectToImage(pObject);

		m_ImageIdPair.m_objectId = imageId;
		if( nullptr != pImage )
		{
			m_ImageIdPair.m_imageId = pImage->getObjectId();
			m_imageName = pObject->GetCompleteName();
			//@TODO[MZA][10.10][09.11.2020] if we want have different overlays depending of direct or over group tool input, this here have to be changed.
			GetIPDoc()->RegisterImage( m_ImageIdPair.m_imageId, this );

			CRect l_rect;

			GetImageRect( l_rect );
			SetImageRect( l_rect );
		}
		else
		{
			m_ImageIdPair.m_imageId = SvDef::InvalidObjectId;
		}
	}
	else
	{
		m_ImageIdPair.m_objectId = SvDef::InvalidObjectId;
		m_ImageIdPair.m_imageId = SvDef::InvalidObjectId;
	}


	if(ImageIsEmpty())
	{
		GetIPDoc()->UnregisterImageView( this );

		ReleaseImageSurface();
	}
}

void SVImageView::AttachToImage( LPCTSTR pImageName )
{
	uint32_t l_ImageId = SvDef::InvalidObjectId;
	bool Attach( false );

	std::string l_imageName = pImageName;

	if( l_imageName.empty() )
	{
		l_imageName = m_imageName;
	}

	if( ! l_imageName.empty() )
	{
		SVObjectClass* pObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByDottedName(l_imageName, pObject);

		if( nullptr != pObject)
		{
			l_ImageId = pObject->getObjectId();
			Attach = true;
		}
	}

	if( Attach )
	{
		AttachToImage( l_ImageId );
	}
}

void SVImageView::DetachFromImage()
{
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		GetIPDoc()->UnregisterImageView( this );
	}

	ReleaseImageSurface();
	CPoint point(0, 0);
	SetScrollPosition(point);
	CRect rect(0, 0, 0, 0);
	SetImageRect(rect);

	m_imageName.clear();
	m_ImageIdPair.m_objectId = SvDef::InvalidObjectId;
	m_ImageIdPair.m_imageId = SvDef::InvalidObjectId;
	m_ImageDIB.clear();
	m_OverlayData.clear();
}

void SVImageView::GetImageRect( CRect& rRect )
{
	rRect.top = 0;
	rRect.left = 0;
	rRect.right = 0;
	rRect.bottom = 0;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc && l_pIPDoc->m_bAllowRefresh )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();

		rRect.right = l_BitmapInfo.GetWidth();

		if( rRect.right < 0 )
		{
			rRect.right = 0;
		}

		rRect.bottom = ::labs( l_BitmapInfo.GetHeight() );

		if( rRect.bottom < 0 )
		{
			rRect.bottom = 0;
		}

		rRect.right = static_cast< long >( static_cast< double >( rRect.right * m_ZoomHelper.GetZoom() ) );
		rRect.bottom = static_cast< long >( static_cast< double >( rRect.bottom * m_ZoomHelper.GetZoom() ) );
	}
}

BOOL SVImageView::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
		case ID_CONFIG_ANALYZER:
		{
			if( TheSVObserverApp().OkToEdit() )
			{
				OnRButtonDblClk( 0, m_mousePoint );
			}
			break;
		}
		case ID_ANALYZER_RESULT:
		{
			SVIPDoc* l_psvIPDoc = GetIPDoc();

			if( nullptr != l_psvIPDoc )
			{
				SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
				if( pTool )
				{
					SvDef::SVObjectTypeInfoStruct l_svInfo;
					l_svInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;

					SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(pTool->getFirstObject(l_svInfo));
					if(nullptr != pAnalyzer )
					{
						CPoint l_point;
						l_point.x = m_mousePoint.x;
						l_point.y = m_mousePoint.y;

						TransformFromViewSpace( l_point );

						if( pAnalyzer->IsPtOverResult( l_point ) )
						{
							DisplayAnalyzerResult(pAnalyzer->getAnalyzerResult(), pAnalyzer->GetObjectSubType());
						}
					}
				}
			}
			break;
		}

		case ID_ADJUST_POSITION:
		{
			long l_err = 0;
			if(ImageIsEmpty() ||
				!TheSVObserverApp().OkToEdit() )
			{
				l_err = -1269;
				break;
			}

			CPoint l_mousePoint = m_mousePoint;

			TransformFromViewSpace( l_mousePoint );
			m_isPicked = GetObjectAtPoint( l_mousePoint );	// stores object in m_psvObject
			if( !m_isPicked )
			{
				//--------- This means that the mouse was not over a moveable, sizeable, 
				//--------- object (which means we should not have been able to get here).
				l_err = -1269;
				break;
			}


			if (nullptr != m_pTool)
			{
				std::string DlgName = SvUl::Format(_T("Adjust Tool Size and Position - %s"), m_pTool->GetName());
				SvOg::SVAdjustToolSizePositionDlg dlg(m_pTool->GetInspection()->getObjectId(), m_pTool->getObjectId(), DlgName.c_str(), this);
				dlg.DoModal();
			}
			else
			{
				l_err = -1276;
			}

			m_isPicked = FALSE;
			break;
		}

		case ID_SELECT_DISPLAY_IMAGE:
		{
			SelectDisplayImage();
			break;
		}
		case ID_SAVEVIEW_WITHOVERLAYS:
		{
			//Save view with overlays
			SaveViewOrImageToDisk(true, true);
			break;
		}
		case ID_SAVEVIEW_WITHOUTOVERLAYS:
		{
			//save view without overlays
			SaveViewOrImageToDisk(true, false);
			break;
		}

		case ID_SAVEIMAGE_WITHOUTOVERLAYS:
		{
			//save underlying image
			SaveViewOrImageToDisk(false, false);
			break;
		}

		default:
		{
			break;
		}

	}// end switch (wParam)

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->UpdateWithLastProduct();
	}

	return CView::OnCommand( wParam, lParam );
}

void SVImageView::SaveViewOrImageToDisk(bool ViewOnly, bool showOverlays)
{
	auto pCurrentImage = GetImage();

	if (nullptr == pCurrentImage)
	{
		SvStl::MessageManager NullImageWarning(SvStl::MsgType::Log | SvStl::MsgType::Display );
		NullImageWarning.setMessage(SVMSG_SVO_5018_NULLIMAGE, SvStl::Tid_DisplayedImageIsUnavailable, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_30000_NullImageOnSave);
		return;
	}

	m_ViewOrImageFilename.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
	
	std::string Path = AfxGetApp()->GetProfileString(RegSection, RegKeySaveViewPath, DefaultPath).GetString();
	m_ViewOrImageFilename.SetPathName( Path.c_str() );
	
	bool bResult = m_ViewOrImageFilename.SaveFile(); // Show Save File Dialog

	if(bResult)
	{
		Path = m_ViewOrImageFilename.GetPathName().c_str();
		AfxGetApp()->WriteProfileString( RegSection, RegKeySaveViewPath, Path.c_str() );

		if(ViewOnly)
		{
			CRect SourceRect;
			CRect DestRect;

			/*auto Status = */GetRectInfo( SourceRect, DestRect );

			/*Status =*/ BlitToScaledSurface( SourceRect, DestRect, m_ViewOrImageFilename.GetFullFileName().c_str(), showOverlays);
		}
		else // showOverlays is ignored for underlying images: there was no easy way to mark underlying images with overlays
		{
			pCurrentImage->Save(m_ViewOrImageFilename.GetFullFileName());
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnContextMenu
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageView::OnContextMenu( CWnd* , CPoint point )
{
	CMenu l_menu;
	bool RunOrTestMode = SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );

	m_mousePoint.x = point.x;
	m_mousePoint.y = point.y;

	// Map mouse point to client
	ScreenToClient( &m_mousePoint );

	if( l_menu.LoadMenu( IDR_IMAGE_ADJUST ) )
	{
		if(CMenu* l_pPopup = l_menu.GetSubMenu( 0 ) )
		{
			if(!IsZoomAllowed())
			{
				l_pPopup->DeleteMenu( 0, MF_BYPOSITION );  // delete Zoom
			}

		
			if (nullptr == GetImage())
			{
				l_pPopup->DeleteMenu( ID_SAVEVIEW_WITHOVERLAYS, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_SAVEVIEW_WITHOUTOVERLAYS, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_SAVEIMAGE_WITHOUTOVERLAYS, MF_BYCOMMAND );
			}


			if( !m_mouseIsOverTool || RunOrTestMode )
			{
				l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_ADJUST_POSITION, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_ANALYZER_RESULT, MF_BYCOMMAND );
			}
			else
			{	//RunOrTestMode is false if entry the else
				bool l_resultFound = false;
				SvAo::Analyzer* pAnalyzer = nullptr;
				SVIPDoc *l_psvIPDoc = GetIPDoc();

				if( nullptr != l_psvIPDoc )
				{
					SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID()));
					if( pTool )
					{
						CPoint l_point;
						l_point.x = m_mousePoint.x;
						l_point.y = m_mousePoint.y;

						TransformFromViewSpace( l_point );

						if( GetObjectAtPoint( l_point ) )
						{
							SvDef::SVObjectTypeInfoStruct l_svInfo;
							l_svInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;

							pAnalyzer = dynamic_cast<SvAo::Analyzer*>(pTool->getFirstObject(l_svInfo));
							if (pAnalyzer)
							{
								l_resultFound = pAnalyzer->IsPtOverResult(l_point);
							}
						}
					}
				}

				if( !l_resultFound )
				{
					l_pPopup->DeleteMenu( ID_ANALYZER_RESULT, MF_BYCOMMAND );
				}

				if( !pAnalyzer )
				{
					l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				}

				if( !TheSecurityManager().SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
				{
					l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				}
			}

			l_pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
		}
	}
}

void SVImageView::TransformFromViewSpace( CPoint& point )
{
	SVDrawContext l_svDrawContext( nullptr, m_ZoomHelper.GetZoom() );

	l_svDrawContext.InverseTransform( &point, &point, 1 );
}

void SVImageView::SelectDisplayImage()
{
	if (nullptr != GetIPDoc())
	{
		SVDisplayImageSelect l_svDlg(GetIPDoc()->GetInspectionID(), m_ImageIdPair.m_objectId);

		if( IDOK == l_svDlg.DoModal() )
		{
			if( SvDef::InvalidObjectId !=  l_svDlg.getImageId())
			{
				AttachToImage( l_svDlg.getImageId() );
			}
			else
			{
				DetachFromImage();
			}
		}
	}
}

ImageIdPair SVImageView::GetImageID() const
{
	return m_ImageIdPair;
}

SvIe::SVImageClass* SVImageView::GetImage()
{
	return dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(m_ImageIdPair.m_imageId));
}

HRESULT SVImageView::RecreateImageSurface()
{
	HRESULT hr = DD_FALSE;

	SVBitmapInfo l_BitmapInfo = GetBitmapInfo();

	if( !( l_BitmapInfo.empty() ) )
	{
		hr = SVDirectX::CreateSurface( l_BitmapInfo, &m_pDDImageSurface );

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		if (DD_OK != hr)
		{
			TRACE( _T( "SVImageView::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
		}
#endif
		if (DD_OK == hr)
		{
			SIZE l_Size;

			SVDirectX::GetPrimarySurfaceResolution( l_Size );

			hr = SVDirectX::CreateSurface(l_Size.cx, l_Size.cy, &m_pDDScaledImageSurface);

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			if (DD_OK != hr)
			{
				TRACE( _T( "SVImageView::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
			}
#endif
		}

		CRect l_rect;

		GetImageRect( l_rect );
		SetImageRect( l_rect );
	}
	return hr;
}

void SVImageView::ShowExtremeLUT( bool show /* = true */ )
{
	m_showExtremeLUT = show;
	if( m_pDDImageSurface )
	{
		UpdatePaletteForLut(show, m_pDDImageSurface);
	}
	if( GetSafeHwnd() )
	{
		Invalidate( false );
	}
}

void SVImageView::OnDraw( CDC*  )
{
	if (ImageIsEmpty() || SVSVIMStateClass::CheckState(SV_STATE_CLOSING))
	{
		ReleaseImageSurface();
	}
	else
	{
		HRESULT hr = UpdateSurface();

		if( S_OK == hr)
		{
			/*hr = */DisplaySurface();
		}
		NotifyIPDocDisplayComplete();
	}
}

BOOL SVImageView::Create( LPCTSTR , LPCTSTR , DWORD pointstyle, const RECT& rRect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext )
{
	BOOL Result( TRUE );

	WNDCLASS WindowClass;
	if( !::GetClassInfo( AfxGetInstanceHandle(), _T( "Main Image" ),  &WindowClass) )
	{
		memset( &WindowClass, 0, sizeof(WNDCLASS) );
		WindowClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = ::DefWindowProc; 
		WindowClass.hInstance = AfxGetInstanceHandle();
		WindowClass.hbrBackground = m_hWindowBackgroundColor;
		WindowClass.lpszMenuName = nullptr;
		WindowClass.lpszClassName = _T( "Main Image" );

		Result = AfxRegisterClass(&WindowClass);
	}

	if( Result )
	{
		Result = CWnd::Create( WindowClass.lpszClassName, _T( "Untitled Main Image" ), pointstyle, rRect, pParentWnd, nID, pContext );
	}

	if( Result )
	{
		ReleaseImageSurface();
	}

	return Result;
}

void SVImageView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void SVImageView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	bool Update = true;

	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ObjectIdEnum::EnvironmentImageUpdateId, pObject);
	if (nullptr != pObject)
	{
		double Value;
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		Update = 0.0 < Value ? true : false;
	}

	Update = Update || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

	if( Update )
	{
		CRect l_rect;

		GetImageRect( l_rect );

		if( m_LastRect != l_rect )
		{
			m_LastRect = l_rect;

			SetImageRect( m_LastRect );
		}

		UpdateBufferFromIPDoc();

		CView::OnUpdate( pSender, lHint, pHint );
	}

	if( 0 == GetUpdateRect( nullptr ) )
	{
		NotifyIPDocDisplayComplete();
	}
}

void SVImageView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		SVIPDoc* l_psvIPDoc = GetIPDoc();
		CPoint l_point = point;

		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace( l_point );

		std::string Text = SvUl::Format( _T(" X: %d, Y: %d "), l_point.x, l_point.y );
		SetStatusText( Text.c_str() );

		if (false == ImageIsEmpty() && nullptr != l_psvIPDoc)
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID()));

			if( pTool )
			{
				if( nullptr != pTool->GetObjectAtPoint( SVPoint<double>(l_point) ) )
				{
					l_psvIPDoc->OnEditTool();
				}
			}
		}

		CWnd::OnLButtonDblClk( nFlags, point );
	}
}

void SVImageView::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		CPoint l_point = point;
		SVIPDoc* l_psvIPDoc = GetIPDoc();
		
		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace( l_point );
		
		std::string Text = SvUl::Format( _T(" X: %d, Y: %d "), l_point.x, l_point.y );
		SetStatusText( Text.c_str() );
		
		if( nullptr != l_psvIPDoc && false == ImageIsEmpty())
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(l_psvIPDoc->GetSelectedToolID()));
			if(nullptr !=  pTool)
			{
				// Try to call SetupDialog for first found Analyzer...
				SvDef::SVObjectTypeInfoStruct l_svInfo;
				l_svInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;

				SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*>(pTool->getFirstObject(l_svInfo));
				if(nullptr != pAnalyzer)
				{
					SVSetupDialogManager::Instance().SetupDialog( pAnalyzer->GetClassID(), pAnalyzer->getObjectId(), this );

					l_psvIPDoc->RunOnce();
				}
			}
		}

		CWnd::OnRButtonDblClk( nFlags, point );
	}
}

void SVImageView::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp().OkToEdit() )
	{
		if(false == ImageIsEmpty())
		{
			std::string Text;
			CPoint l_point = point;

			// Compensate for Scaling of Displayed Image
			TransformFromViewSpace( l_point );

			m_isPicked = GetObjectAtPoint( l_point );
			m_svMousePickLocation = m_svLocation;
			if( m_isPicked )
			{
				SVImageExtentClass extent;

				POINT l_tempPoint;

				long l_width = 0;
				long l_height = 0;

				if( S_OK == GetToolExtents( extent ) &&
					S_OK == extent.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, l_tempPoint ) &&
					S_OK == extent.GetExtentProperty( SvPb::SVExtentPropertyWidth, l_width ) &&
					S_OK == extent.GetExtentProperty( SvPb::SVExtentPropertyHeight, l_height ) )
				{
					Text = SvUl::Format( _T(" X: %d, Y: %d    cX: %d, cY: %d "), l_tempPoint.x, l_tempPoint.y, l_width, l_height );
				}
				else
				{
					Text = SvUl::Format( _T(" Col: %d, Row: %d "), point.x, point.y );
				}

				m_lastMouseMovePoint = point;

				SetCapture();
			}
			else
			{
				Text = SvUl::Format( _T(" Col: %d, Row: %d "), point.x, point.y );
			}

			SetStatusText( Text.c_str() );
		}

		CWnd::OnLButtonDown( nFlags, point );
	}
}

void SVImageView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) &&
		TheSVObserverApp().OkToEdit())
	{
		if (m_lastMouseMovePoint == point)
		{
			//nothing to do here if the mouse pointer is not moving
			return;
		}

		CPoint point1 = point;
		CPoint clientPoint = point;
		CPoint screenPoint = point;

		ClientToScreen(&screenPoint);

		CRect rect;
		CRect screenRect;

		GetClientRect(rect);
		GetWindowRect(screenRect);


		// Get Color of Mouse Point ( inside of View )
		HDC hDC = ::GetDC(m_hWnd);
		COLORREF color = ::GetPixel(hDC, point.x, point.y);
		::ReleaseDC(m_hWnd, hDC);

		BYTE redValue = GetRValue(color);
		BYTE greenValue = GetGValue(color);
		BYTE blueValue = GetBValue(color);

		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace(point1);

		// Status Text: Mouse Pos and Color
		std::string Text = SvUl::Format(_T(" Col: %d, Row: %d    RGB: %u/%u/%u "), point1.x, point1.y, redValue, greenValue, blueValue);

		HICON hCursor = nullptr;

		SVImageExtentClass extent;

		SVIPDoc* pIPDoc = GetIPDoc();

		if (m_isPicked)
		{
			if (nullptr != pIPDoc && m_isPicked && S_OK == GetToolExtents(extent))
			{
				CPoint startPoint = m_lastMouseMovePoint;

				SVImageExtentClass tempExtent = extent;

				TransformFromViewSpace(startPoint);

				hCursor = GetObjectCursor(m_svMousePickLocation, point1);
				SVPoint<double> startSVPoint(startPoint);
				if ((SvPb::SVExtentLocationPropertyRotate == m_svMousePickLocation ||
					m_svMousePickLocation == extent.GetLocationPropertyAt(startSVPoint)) &&
					S_OK == tempExtent.Update(m_svMousePickLocation, startSVPoint, SVPoint<double>(point1)))
				{
					bool bUpdate = S_OK == pIPDoc->UpdateExtents(m_pTool, tempExtent);

					if (bUpdate || rect.PtInRect(clientPoint))
					{
						bUpdate = bUpdate || S_OK == pIPDoc->UpdateExtentsToFit(m_pTool, tempExtent);
					}

					if (bUpdate)
					{
						long left = 0;
						long top = 0;
						long width = 0;
						long height = 0;

						extent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointX, left);
						extent.GetExtentProperty(SvPb::SVExtentPropertyPositionPointY, top);
						extent.GetExtentProperty(SvPb::SVExtentPropertyWidth, width);
						extent.GetExtentProperty(SvPb::SVExtentPropertyHeight, height);

						// Status Text: Mouse Pos and Tool Extent
						Text = SvUl::Format(_T(" Col: %d, Row: %d    X: %d, Y: %d    cX: %d, cY: %d "), point1.x, point1.y, left, top, width, height);
					}
				}
			}
			else
			{
				hCursor = GetObjectCursor(point1);
			}
		}
		else
		{
			hCursor = GetObjectCursor(point1);
		}

		//If no icon returned then use standard arrow
		if (nullptr == hCursor)
		{
			m_mouseIsOverTool = FALSE;
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
		SetCursor(hCursor);

		SetStatusText(Text.c_str());

		CWnd::OnMouseMove(nFlags, point);
	}

	m_lastMouseMovePoint = point;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnLButtonUp
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageView::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_isPicked = FALSE;

	m_svMousePickLocation = SvPb::SVExtentLocationPropertyUnknown;

	ReleaseCapture();

	std::string Text = SvUl::Format( _T(" Col: %d, Row: %d "), point.x, point.y );
	SetStatusText( Text.c_str() );

	CWnd::OnLButtonUp( nFlags, point );

	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION ) )
	{
		GetIPDoc()->RunOnce();
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnCaptureChanged
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageView::OnCaptureChanged( CWnd* pWnd )
{
	SetStatusText( nullptr );

	CWnd::OnCaptureChanged( pWnd );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnNcMouseMove
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageView::OnNcMouseMove( UINT hitTest, CPoint point )
{
	SetStatusText( nullptr );

	CWnd::OnNcMouseMove( hitTest, point );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnDestroy
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageView::OnDestroy()
{
	DetachFromImage();

	CView::OnDestroy();
}

#ifdef _DEBUG

void SVImageView::AssertValid() const
{
	CView::AssertValid();
}

void SVImageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

SVIPDoc* SVImageView::GetIPDoc() const
{
	return dynamic_cast< SVIPDoc* >( GetDocument() );
}

bool SVImageView::GetScrollPosition( CPoint& point )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			point = l_psvScroll->GetScrollPosition();
		}
	}

	return l_bOk;
}

bool SVImageView::SetScrollPosition( CPoint& point )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll *l_psvScroll = dynamic_cast< SVImageViewScroll * >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			l_psvScroll->ScrollToPosition( point );
		}
	}

	return l_bOk;
}

bool SVImageView::ImageIsEmpty() const
{
	return SvDef::InvalidObjectId == m_ImageIdPair.m_imageId;
}

bool SVImageView::CalculateZoomFit(ZoomEnum ZoomType)
{
	bool Result{false};

	SVIPDoc* pIPDoc = GetIPDoc();

	if( nullptr != pIPDoc && pIPDoc->m_bAllowRefresh && nullptr != GetSafeHwnd() )
	{
		CSize ImageSize;
		SVBitmapInfo BitmapInfo = GetBitmapInfo();

		ImageSize.cx = labs(BitmapInfo.GetWidth());
		ImageSize.cy = labs(BitmapInfo.GetHeight());

		SVImageViewScroll* psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );
		if(psvScroll)
		{
			CRect rect;
			psvScroll->GetWindowRect(rect);
			CSize viewsize(rect.Width(),rect.Height());
			Result = m_ZoomHelper.CalculateZoomFit(ZoomType, ImageSize, viewsize);
		}
	}

	return Result;
}

void SVImageView::DisplayAnalyzerResult(const SvDef::StringVector& rAnalyzerResults, SvPb::SVObjectSubTypeEnum analyzerType) const
{
	constexpr size_t cPatternResults = 4;

	if(rAnalyzerResults.size() > 0)
	{
		switch(analyzerType)
		{
			case SvPb::SVBlobAnalyzerObjectType:
			{

				SVAnalyzerResultDlg	resultDlg{rAnalyzerResults, IDD_BA_RESULT_DIALOG};
				resultDlg.DoModal();
				break;
			}

			case SvPb::BlobAnalyzer2ObjectType:
			{

				SVAnalyzerResultDlg	resultDlg{ rAnalyzerResults, IDD_BA_RESULT_DIALOG };
				resultDlg.DoModal();
				break;
			}

			case SvPb::SVOCVAnalyzerObjectType:
			{
				
				SVAnalyzerResultDlg resultDlg {rAnalyzerResults, IDD_ANALYZE_RESULT};
				resultDlg.DoModal();
				break;
			}

			case SvPb::SVPatternAnalyzerObjectType:
			{
				SVPatResultDlgClass resultDlg;
				if(rAnalyzerResults.size() >= cPatternResults)
				{
					if(!rAnalyzerResults[0].empty())
					{
						resultDlg.m_strAngle = rAnalyzerResults[0].c_str();
					}
					resultDlg.m_strScore = rAnalyzerResults[1].c_str();
					resultDlg.m_strXPos = rAnalyzerResults[2].c_str();
					resultDlg.m_strYPos = rAnalyzerResults[3].c_str();
					resultDlg.DoModal();
				}
				break;
			}

			default:
				break;
		}
	}
}

double SVImageView::SetZoomValue(double Value, bool bSetZoomSlider )
{
	m_ZoomHelper.ExtendMinMax(Value);
	m_ZoomHelper.SetZoom(Value);

	CRect l_rect;
	GetImageRect( l_rect );
	SetImageRect( l_rect );
	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	if(pFrame && bSetZoomSlider)
	{
		pFrame->SetZoomToolbar(m_ZoomHelper);
	}
	return m_ZoomHelper.GetZoom();
}

bool SVImageView::SetZoom(ZoomEnum ZoomType, unsigned  scaleIndex, bool bSetZoomSlider)
{
	if (ZoomEnum::ZoomValue == ZoomType)
	{
		if( scaleIndex >= m_ZoomHelper.GetScaleCount() )
		{
			return false;
		}
	}
	//For the fit zoom types we need to calculate the zoom factor
	switch(ZoomType)
	{
		case ZoomFitAll:
		case ZoomFitWidth:
		case ZoomFitHeight:
			CalculateZoomFit(ZoomType);
			break;

		default:
			break;
	}

	bool bZoomChanged = m_ZoomHelper.SetZoomType(ZoomType, scaleIndex);

	if (bZoomChanged)
	{
		CRect l_rect;
		GetImageRect( l_rect );
		SetImageRect( l_rect );
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
		if (pFrame && bSetZoomSlider)
		{
			pFrame->SetZoomToolbar(m_ZoomHelper);
		}
	}
	return true;
}

const ZoomHelper& SVImageView::GetZoomHelper() const
{
	return m_ZoomHelper;
}

bool SVImageView::SetImageRect( CRect& rRect )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			l_psvScroll->SetImageSize( rRect.Size() );
		}
	}
	return l_bOk;
}

HRESULT SVImageView::ShouldDraw( const SVExtentMultiLineStruct& rMultiLine )
{
	HRESULT l_hrOk = S_OK;

	uint32_t l_SelectedID = SvDef::InvalidObjectId;

	if( nullptr != GetIPDoc() )
	{
		l_SelectedID = GetIPDoc()->GetSelectedToolID();
	}

	switch( rMultiLine.m_ToolSetDrawFlag )
	{
		case 0:	// "All Tools=0"
			l_hrOk = S_OK;
			return l_hrOk;

		case 1: // "Current Tool"
			if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == rMultiLine.m_ObjectID || l_SelectedID == rMultiLine.m_ToolID ) )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;

			return l_hrOk;

		case 2: // "Warned Tools"
		{
			if( rMultiLine.m_Warned )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;

			return l_hrOk;
		}

		case 3: // "Failed Tools"
		{
			if( rMultiLine.m_Failed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
			
			return l_hrOk;
		}

		case 4: // "Passed Tools"
		{
			if( rMultiLine.m_Passed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;

			return l_hrOk;
		}

		case 5:	// "No Tools"
			l_hrOk = S_FALSE;
			return l_hrOk;

		case 6: // "Tools Controlled"
			break; // Just break...
		
	}
	
	// Check Tool's Draw Flag...
	switch( rMultiLine.m_ToolDrawFlag )
	{
		case 0:	// "Always=0"
			l_hrOk = S_OK;
			break;

		case 1: // "If Current"
			if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == rMultiLine.m_ObjectID || l_SelectedID == rMultiLine.m_ToolID ) )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
			break;

		case 2: // "If Warned"
		{
			if( rMultiLine.m_Warned )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
		}
		break;

		case 3: // "If Failed"
		{
			if( rMultiLine.m_Failed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
		}
		break;

		case 4: // "If Passed"
		{
			if( rMultiLine.m_Passed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
		}
		break;

		case 5:	// "Never"
			l_hrOk = S_FALSE;
			break;
	}

	return l_hrOk;
}

void SVImageView::DrawOverlay( SVDrawContext* PDrawContext, const SVExtentMultiLineStruct& rMultiLine )
{
	BOOL bDrawAllowed = ( ( nullptr != PDrawContext ) && ( S_OK == ShouldDraw( rMultiLine ) ) );

	if( bDrawAllowed )
	{
		HGDIOBJ hPen;
		HGDIOBJ hPenOld;

		COLORREF color = rMultiLine.m_Color;
		POINT titlePoint = static_cast<POINT> (rMultiLine.m_StringPoint);
		std::string Title = rMultiLine.m_csString;

		uint32_t l_SelectedID = SvDef::InvalidObjectId;

		if( nullptr != GetIPDoc() )
		{
			l_SelectedID = GetIPDoc()->GetSelectedToolID();
		}

		// Draw tool title...
		SetTextColor( PDrawContext->DC, color );

		PDrawContext->Transform( &titlePoint, &titlePoint, 1 );

		::TextOut( PDrawContext->DC, titlePoint.x, titlePoint.y, Title.c_str(), static_cast<int> (Title.size()) );

		int l_PenStyle = PS_SOLID;

		// Check for current selection...
		if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == rMultiLine.m_ObjectID || l_SelectedID == rMultiLine.m_ToolID ) )
		{
			l_PenStyle = PS_DOT;
		}

		hPen = CreatePen( l_PenStyle, 1, color );

		hPenOld = SelectObject( PDrawContext->DC, hPen );

		SVDrawObjectList drawObjectList;

		drawObjectList.AddExtentMultiLineData( rMultiLine, l_PenStyle );
		drawObjectList.m_bDrawFigureHatched = rMultiLine.m_bDrawFigureHatched;
		drawObjectList.Draw( PDrawContext );

		SelectObject( PDrawContext->DC, hPenOld );
		DeleteObject( hPen );
	}
}

void SVImageView::UpdateOverlays(HDC hDC, long X, long Y)
{
	// Get drawing device context...
	::SetBkMode(hDC, TRANSPARENT);

	// Select Font...
	HGDIOBJ hFontOld = ::SelectObject(hDC, m_ZoomHelper.GetFont());

	// Use Drawing Context

	SVDrawContext drawContext(hDC, m_ZoomHelper.GetZoom());
	drawContext.ViewPortOffset.x = -X;
	drawContext.ViewPortOffset.y = -Y;

	// Draw tool figures...
	for (SVExtentMultiLineStructVector::iterator l_Iter = m_OverlayData.begin(); l_Iter != m_OverlayData.end(); ++l_Iter)
	{
		DrawOverlay(&drawContext, *l_Iter);
	}

	// Restore Font...
	::SelectObject(hDC, hFontOld);
}

HICON SVImageView::GetObjectCursor( POINT point )
{
	HICON l_hCursor = nullptr;

	if( GetObjectAtPoint( point ) )
	{
		l_hCursor = GetObjectCursor(m_svLocation, point);
	}

	return l_hCursor;
}

HICON SVImageView::GetObjectCursor( SvPb::SVExtentLocationPropertyEnum svLocation, POINT point )
{
	HICON l_hCursor = nullptr;

	LPCTSTR l_cursorId = IDC_ARROW;
	LPCTSTR l_cursorLeftRight = IDC_SIZEWE;
	LPCTSTR l_cursorTopBottom = IDC_SIZENS;
	LPCTSTR l_cursorTopLeftBottomRight = IDC_SIZENWSE;
	LPCTSTR l_cursorTopRightBottomLeft = IDC_SIZENESW;
	LPCTSTR l_cursorMove = IDC_SIZEALL;

	//If action icon is loaded then need to clean it up before next icon
	if(nullptr != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon  );
		m_hActionIcon = nullptr;
	}

	SVImageExtentClass extent;
	//Check the rotation angle to determine icon symbols
	if( S_OK == GetToolExtents( extent ) )
	{
		double l_dRotationAngle = 0.0;
		//Check that this is the Linear tool with rotation
		if(SvPb::SVExtentTranslationProfile == extent.GetTranslation() )
		{
			if( S_OK == extent.GetExtentProperty( SvPb::SVExtentPropertyRotationAngle, l_dRotationAngle) )
			{
				l_dRotationAngle = fmod(l_dRotationAngle, 180.0);
			}
		}
		//Check that this is the Polar Unwrap tool with rotation
		else if(SvPb::SVExtentTranslationPolarUnwrap == extent.GetTranslation() )
		{
			SVPoint<double> center;
			SVPoint<double> mousePosition(point);
			if( S_OK == extent.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, center) )
			{
				//Polar unwrap needs an offset of 90° for the displayed cursor to be correct
				l_dRotationAngle = SVGetRotationAngle(center, mousePosition) + 90.0;
				l_dRotationAngle = fmod(l_dRotationAngle, 180.0);
			}
		}
		//Having icon symbols in 45° steps we check in offsets of 22.5° which icons to use
		if( (22.5 <= l_dRotationAngle) && (67.5 >  l_dRotationAngle) )
		{
			l_cursorLeftRight = IDC_SIZENWSE;
			l_cursorTopBottom = IDC_SIZENESW;
			l_cursorTopLeftBottomRight = IDC_SIZENS;
			l_cursorTopRightBottomLeft = IDC_SIZEWE;
		}
		else if( (67.5 <= l_dRotationAngle) && (112.5 >  l_dRotationAngle) )
		{
			l_cursorLeftRight = IDC_SIZENS;
			l_cursorTopBottom = IDC_SIZEWE;
			l_cursorTopLeftBottomRight = IDC_SIZENESW;
			l_cursorTopRightBottomLeft = IDC_SIZENWSE;
		}
		else if( (112.5 <= l_dRotationAngle) && (157.5 >  l_dRotationAngle) )
		{
			l_cursorLeftRight = IDC_SIZENESW;
			l_cursorTopBottom = IDC_SIZENWSE;
			l_cursorTopLeftBottomRight = IDC_SIZEWE;
			l_cursorTopRightBottomLeft = IDC_SIZENS;
		}
	}

	switch( svLocation )
	{
		case SvPb::SVExtentLocationPropertyRotate:
		{
			l_cursorId = 0;
			m_hActionIcon = static_cast<HICON> (::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ARROW_ROTATE), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
			l_hCursor = m_hActionIcon;
			break;
		}

		case SvPb::SVExtentLocationPropertyTopLeft:
		case SvPb::SVExtentLocationPropertyBottomRight:
		{
			l_cursorId = l_cursorTopLeftBottomRight;

			break;
		}
		case SvPb::SVExtentLocationPropertyTopRight:
		case SvPb::SVExtentLocationPropertyBottomLeft:
		{
			l_cursorId = l_cursorTopRightBottomLeft;

			break;
		}
		case SvPb::SVExtentLocationPropertyLeft:
		case SvPb::SVExtentLocationPropertyRight:
		{
			l_cursorId = l_cursorLeftRight;

			break;
		}
		case SvPb::SVExtentLocationPropertyTop:
		case SvPb::SVExtentLocationPropertyBottom:
		{
			l_cursorId = l_cursorTopBottom;

			break;
		}
		case SvPb::SVExtentLocationPropertyCenter:
		{
			l_cursorId = l_cursorMove;

			break;
		}
		default:
		{
			break;
		}
	}
	if( IDC_ARROW == l_cursorId && SvPb::SVExtentLocationPropertyDisabled != svLocation )
	{
		m_mouseIsOverTool = FALSE;
	}
	else
	{
		m_mouseIsOverTool = TRUE;
	}
	//If no special resource icon call standard windows icon
	if( nullptr == l_hCursor)
	{
		l_hCursor = AfxGetApp()->LoadStandardCursor( l_cursorId );
	}
	return l_hCursor;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getObjectAtPoint
// -----------------------------------------------------------------------------
// .Description : Get Object under Mouse point
////////////////////////////////////////////////////////////////////////////////
BOOL SVImageView::GetObjectAtPoint( POINT point )
{
	BOOL l_bOk = FALSE;

	m_pTool = nullptr;

	m_svLocation = SvPb::SVExtentLocationPropertyUnknown;

	SvTo::SVToolClass* pTool = nullptr;

	if( nullptr != GetIPDoc() )
	{
		pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID()));
	}

	if( nullptr != pTool && pTool->isInputImage( m_ImageIdPair.m_imageId ) )
	{
		SVImageExtentClass extent;
		SVPoint<double> svPoint(point);
		m_pTool = pTool->GetObjectAtPoint(svPoint);

		if( S_OK == GetToolExtents( extent ) )
		{
			m_svLocation = extent.GetLocationPropertyAt(svPoint);
		}
	
		////check if move or sizing is allowed  
		if(false == pTool->isAllowedLocation(m_svLocation))
		{
			m_svLocation = SvPb::SVExtentLocationPropertyDisabled;
		}
	}

	l_bOk = nullptr != m_pTool;

	return l_bOk;
}

void SVImageView::GetParameters(SvOi::IObjectWriter& rWriter)
{
	_variant_t l_svVariant;

	auto* pImage = SVObjectManagerClass::Instance().GetObject(m_ImageIdPair.m_objectId);

	l_svVariant = ( nullptr != pImage );
	rWriter.WriteAttribute(SvXml::CTAG_VIEW_INITIALIZED, l_svVariant);
	l_svVariant.Clear();

	double zoom = m_ZoomHelper.GetZoom();
	l_svVariant = zoom;
	rWriter.WriteAttribute(SvXml::CTAG_IMAGE_ZOOM_FACTOR_EX, l_svVariant);
	l_svVariant.Clear();

	if( nullptr != pImage )
	{
		l_svVariant.SetString( pImage->GetObjectNameToObjectType(SvPb::SVInspectionObjectType).c_str() );
		rWriter.WriteAttribute(SvXml::CTAG_IMAGE_NAME, l_svVariant);
		l_svVariant.Clear();
	}
}

bool SVImageView::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle parent )
{
	bool bZoomExOK = false;

	_variant_t Value;

	bool l_bUseImageView = false;

	bool l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VIEW_INITIALIZED, parent, Value );
	if( l_bOk )
	{
		l_bUseImageView = Value;
		bZoomExOK = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_ZOOM_FACTOR_EX, parent, Value );
		if(bZoomExOK)
		{
			double dZoom = Value;
			if( !SetZoomValue(dZoom, false) )
			{
				SetZoomValue(1.0, false);
			}
		}
	}

	if( l_bOk && !bZoomExOK )
	{
		l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_ZOOM_FACTOR, parent, Value );
		if( l_bOk )
		{
			long l_index = 0;

			l_index = Value;

			if( l_index < 0 )
			{
				l_index++;
			}
			else if( 0 < l_index )
			{
				l_index--;
			}

			if( !SetZoom(ZoomEnum::ZoomValue, m_ZoomHelper.GetScaleCount() / 2 + l_index, false) )
			{
				SetZoom(ZoomEnum::ZoomValue, m_ZoomHelper.GetScaleCount() / 2, false );
			}
		}
	}

	if( l_bOk && l_bUseImageView )
	{
		l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_NAME, parent, Value );
		if( l_bOk )
		{
			_bstr_t l_String( Value.bstrVal );

			if( 0 < l_String.length() )
			{
				AttachToImage( static_cast< LPCTSTR >( l_String ) );
			}
			else
			{
				l_bOk = FALSE;
			}
		}
	}

	UpdateZoomToolbar();
	return l_bOk;
}

bool SVImageView::CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle parent )
{
	_variant_t Value;

	bool l_bUseImageView = false;
	bool bZoomExOK = false;

	bool l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VIEW_INITIALIZED, parent, Value );
	if( l_bOk )
	{
		l_bUseImageView = Value;
		bZoomExOK = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_ZOOM_FACTOR_EX, parent, Value );
		if(bZoomExOK)
		{
			double dZoom = Value;

			if( m_ZoomHelper.GetZoom() != dZoom )
			{
				if(!SetZoomValue(dZoom, false))
				{
					SetZoomValue(1.0, false);
				}
			}
		}
	}

	if( l_bOk && !bZoomExOK )
	{
		l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_ZOOM_FACTOR, parent, Value );
		if( l_bOk )
		{
			long l_index = 0;

			l_index = Value;

			if( l_index < 0 )
			{
				l_index++;
			}
			else if( 0 < l_index )
			{
				l_index--;
			}

			if( m_ZoomHelper.GetScaleIndex() != m_ZoomHelper.GetScaleCount() / 2 + l_index )
			{
				if (!SetZoom(ZoomEnum::ZoomValue, m_ZoomHelper.GetScaleCount() / 2 + l_index, false))
				{
					SetZoom(ZoomEnum::ZoomValue, m_ZoomHelper.GetScaleCount() / 2, false);
				}
			}
		}
	}

	if( l_bOk && l_bUseImageView )
	{
		l_bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_IMAGE_NAME, parent, Value );
		if( l_bOk )
		{
			_bstr_t l_String( Value.bstrVal );

			if( 0 < l_String.length() )
			{
				if( m_imageName != static_cast< LPCTSTR >( l_String ) )
				{
					AttachToImage( static_cast< LPCTSTR >( l_String ) );
				}
			}
			else
			{
				l_bOk = false;
			}
		}
	}

	return l_bOk;
}

BOOL SVImageView::OnEraseBkgnd(CDC*)
{
	BOOL l_bOk = TRUE;

	//calling CView::OnEraseBkgnd causes flicker and will get painted over by image

	return l_bOk;
}

SVBitmapInfo SVImageView::GetBitmapInfo() const
{
	SVBitmapInfo l_Info;

	if( !( m_ImageDIB.empty() ) )
	{
		const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( &( m_ImageDIB[ 0 ] ) );

		if( nullptr != l_pBitmapInfo )
		{
			l_Info.Assign( *l_pBitmapInfo );
		}
	}
	else if(false == ImageIsEmpty())
	{
		SVIPDoc* l_pIPDoc = GetIPDoc();

		if( nullptr != l_pIPDoc )
		{
			l_pIPDoc->GetBitmapInfo( m_ImageIdPair.m_imageId, l_Info );
		}
	}

	return l_Info;
}

const unsigned char* SVImageView::GetBitmapBits() const
{
	SVBitmapInfo l_Info = GetBitmapInfo();

	if( !( m_ImageDIB.empty() ) && !( l_Info.empty() ) )
	{
		return reinterpret_cast<const unsigned char*>(&( m_ImageDIB[ l_Info.GetBitmapInfoSizeInBytes() ] ));
	}

	return nullptr;
}

HRESULT SVImageView::UpdateImageSurfaces( const SVBitmapInfo& rBitmapInfo )
{
	HRESULT status = S_OK;

	if( !( SVDirectX::Instance().empty() ) && !( rBitmapInfo.empty() ) )
	{
		if( m_sourceImageWidth != rBitmapInfo.GetWidth() ||
			m_sourceImageHeight != ::labs( rBitmapInfo.GetHeight() ) ||
			m_sourceBitCount != rBitmapInfo.GetBitCount() ||
			nullptr == m_pDDImageSurface || nullptr == m_pDDScaledImageSurface )
		{
			m_sourceImageWidth = rBitmapInfo.GetWidth();
			m_sourceImageHeight = ::labs( rBitmapInfo.GetHeight() );
			m_sourceBitCount = rBitmapInfo.GetBitCount();

			status = SVDirectX::CreateSurface( rBitmapInfo, &m_pDDImageSurface );
			
			if( DD_OK == status )
			{
				SIZE l_Size;

				SVDirectX::GetPrimarySurfaceResolution( l_Size );

				status = SVDirectX::CreateSurface( l_Size.cx, l_Size.cy, &m_pDDScaledImageSurface );
			}

			CRect l_rect;

			GetImageRect( l_rect );
			SetImageRect( l_rect );
		}
	}
	else
	{
		status = E_FAIL;
	}

	if( DD_OK != status )
	{
		ReleaseImageSurface();

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE( _T( "SVImageView CreateImageSurface was unable to create the image surface.\n" ) );
#endif
	}

	return status;
}

HRESULT SVImageView::CopyBitsToSurface( const CRect& rSourceRect, const SVBitmapInfo& rBitmapInfo, const unsigned char* pBitmapBits )
{
	HRESULT status = S_OK;

	if( !( rBitmapInfo.empty() ) && ( nullptr != pBitmapBits ) && ( nullptr != m_pDDImageSurface ) )
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );
		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );

		status = m_pDDImageSurface->Lock( nullptr, &l_ddSurfaceDesc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr );
		if( DD_OK == status )
		{
			DWORD bitmapBitCount = static_cast<DWORD> (rBitmapInfo.GetBitCount());
			DWORD bitmapWidth = static_cast<DWORD> (rBitmapInfo.GetWidth());
			unsigned long bitmapPitch = static_cast<unsigned long> (rBitmapInfo.GetBitmapImageStrideInBytes());
			unsigned long visiblePitch = ( rSourceRect.Width() * bitmapBitCount / 8 );

			if( ( bitmapWidth == l_ddSurfaceDesc.dwWidth ) && 
				( bitmapBitCount == l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount ) &&
				( visiblePitch <= bitmapPitch ) )
			{
				unsigned long l_Mask = l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;
				unsigned long l_UpperValue = l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask;
				unsigned long l_LowerValue = l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;

				BYTE* l_pTo = static_cast< BYTE* >( l_ddSurfaceDesc.lpSurface );
				BYTE* l_pFrom = const_cast< BYTE* >( pBitmapBits );

				unsigned long l_SurfaceOffset = ( rSourceRect.top * l_ddSurfaceDesc.lPitch ) + ( rSourceRect.left * l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8 );
				unsigned long l_BitmapOffset = ( rSourceRect.top * bitmapPitch ) + ( rSourceRect.left * rBitmapInfo.GetBitCount() / 8 );

				l_pTo += l_SurfaceOffset;
				l_pFrom += l_BitmapOffset;
				size_t l_PixelWidth = l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8;

				for( long l_row = rSourceRect.top; l_row < rSourceRect.bottom; ++l_row )
				{
					if (m_showExtremeLUT && bitmapBitCount > 8)
					{
						CalcLut(l_pFrom, l_pTo, visiblePitch, l_Mask, l_LowerValue, l_UpperValue, l_PixelWidth);
					}
					else
					{
						memcpy( l_pTo, l_pFrom, visiblePitch );
					}

					l_pTo += l_ddSurfaceDesc.lPitch;
					l_pFrom += bitmapPitch;
				}
			}
			else
			{
				status = E_INVALIDARG;
			}

			// preserve the error set if the image widths/pitch are different 
			// (meaning that we didn't copy the image data)
			HRESULT l_Temp = m_pDDImageSurface->Unlock( nullptr );

			if( S_OK == status )
			{
				status = l_Temp;
			}
		}
	}
	else
	{
		status = E_INVALIDARG;
	}

	return status;
}

bool SVImageView::IsZoomAllowed() const
{
	bool allowed  =  !ImageIsEmpty();
	return allowed;
}

void SVImageView::UpdateZoomToolbar()
{
	//only when we have the focus 
	SVImageView* pFocus = dynamic_cast<SVImageView*>(GetFocus());

	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

	if( nullptr != pFrame && pFocus == this )
	{
		if (IsZoomAllowed())
		{
			pFrame->SetZoomToolbar(GetZoomHelper());
		}
	}
}

HRESULT SVImageView::BlitToScaledSurface( CRect& rSourceRect, CRect& rDestRect, LPCTSTR Filepath, bool showOverlays)
{
	HRESULT status = S_OK;

	if( nullptr != m_pDDImageSurface && nullptr != m_pDDScaledImageSurface )
	{
		HDC scaledDC = nullptr;

		CRect l_ScaledRect( 0, 0, rDestRect.Width(), rDestRect.Height() );

		status = m_pDDScaledImageSurface->GetDC( &scaledDC );

		if( DD_OK == status )
		{
			HDC surfaceDC = nullptr;

			status = m_pDDImageSurface->GetDC( &surfaceDC );

			if( DD_OK == status )
			{
				int iStretchMode = ::SetStretchBltMode( scaledDC, STRETCH_DELETESCANS );

				::StretchBlt( 
					scaledDC, 
					l_ScaledRect.left,
					l_ScaledRect.top,
					l_ScaledRect.Width(),
					l_ScaledRect.Height(),
					surfaceDC,
					rSourceRect.left,
					rSourceRect.top,
					rSourceRect.Width(),
					rSourceRect.Height(),
					SRCCOPY );

				::SetStretchBltMode( scaledDC, iStretchMode );

				m_pDDImageSurface->ReleaseDC( surfaceDC );
			}

			if (showOverlays)
			{
				UpdateOverlays( scaledDC, rSourceRect.left, rSourceRect.top );
			}

			if( nullptr != Filepath )
			{
				CreateBmpFileFromHdc(Filepath,scaledDC,l_ScaledRect);
			}

			m_pDDScaledImageSurface->ReleaseDC( scaledDC );
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		if( DD_OK != status)
		{
			TRACE(_T( "SVImageView::UpdateImage - Surface Blit failed %08lx\n" ), status );
		}
#endif
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageView::BlitToPrimarySurface( CRect& rDestRect )
{
	HRESULT status = S_OK;

	if( nullptr != m_pDDScaledImageSurface )
	{
		CRect l_ScaledRect( 0, 0, rDestRect.Width(), rDestRect.Height() );

		SVDirectX::Instance().SetClipperHWnd( 0, m_hWnd );

		status = SVDirectX::Instance().GetBltStatus( DDGBS_CANBLT ); // @Note: a do/while loop around GetBltStatus() was removed in SVO-2746 since it did not seem necessary
		if (S_OK == status)
		{
			status = SVDirectX::Instance().Blt(
				&rDestRect,            // destination rectangle
				m_pDDScaledImageSurface, // source surface
				&l_ScaledRect,           // source rectangle
				DDBLT_WAIT,              // DDBLT_DONOTWAIT| DDBLT_ASYNC, // Flag <<<<<<<<<<<<<<<<<<<<<<
				nullptr);               // Pointer to DDBLTFX Structure
		}
		else
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			SvDef::StringVector msgList;
			std::ostringstream oss;
			oss << "GetBltStatus() Failed!! Result = " << status;
			msgList.push_back(oss.str());
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageView::RecreateLostSurface()
{
	HRESULT status = S_OK;

	if( nullptr != m_pDDImageSurface )
	{
		// preserve the DDERR_SURFACELOST error so the overlays don't get drawn
		if( DDERR_SURFACELOST == m_pDDImageSurface->IsLost() )
		{
			HRESULT l_hrRestore = m_pDDImageSurface->Restore();

			if (DDERR_WRONGMODE == l_hrRestore)
			{
				// Destroy and Recreate surface
				l_hrRestore = RecreateImageSurface();
				
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				if (DD_OK != l_hrRestore)
				{
					TRACE( _T( "SVImageView::UpdateImage - CreateSurface failed\n" ) );
				}
#endif
			}

			// cause a Repaint if Surface was Rrestored/Recreated
			if( DD_OK == l_hrRestore )
			{
				if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
				{
					Invalidate( false );
				}
			}
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageView::GetRectInfo( CRect& rSourceRect, CRect& rDestRect )
{
	HRESULT status = S_OK;

	SVBitmapInfo l_BitmapInfo = GetBitmapInfo();
	CWnd* l_pParent = GetParent(); // The parent of ImageView window is ImageViewScroll.

	if( !( l_BitmapInfo.empty() ) && ( nullptr != l_pParent ) )
	{
		CRect l_parentRect;
		CRect l_imageRect;

		l_pParent->GetClientRect( &l_parentRect );
		int l_xScroll = l_pParent->GetScrollPos( SB_HORZ );
		int l_yScroll = l_pParent->GetScrollPos( SB_VERT );

		GetImageRect( l_imageRect );

		long l_sourceImageHeight = labs( l_BitmapInfo.GetHeight() );
		long l_sourceImageWidth = l_BitmapInfo.GetWidth();
		double l_zoomFactor = 1 / m_ZoomHelper.GetZoom();

		rDestRect.left = l_xScroll; 
		rDestRect.top = l_yScroll;
		rDestRect.right =  std::min( l_parentRect.Width() + l_xScroll, l_imageRect.Width() );
		rDestRect.bottom = std::min( l_parentRect.Height() + l_yScroll, l_imageRect.Height() );

		rSourceRect.right = static_cast< long >( std::min< double >( l_zoomFactor * ( l_xScroll + rDestRect.Width() ), l_sourceImageWidth ) );
		rSourceRect.left = static_cast< long >( l_zoomFactor * l_xScroll );
		rSourceRect.bottom = static_cast< long >( std::min< double >( l_zoomFactor * ( l_yScroll + rDestRect.Height() ), l_sourceImageHeight ) );
		rSourceRect.top = static_cast< long >( l_zoomFactor * l_yScroll );

		ClientToScreen( &rDestRect );
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageView::UpdateBufferFromIPDoc()
{
	HRESULT status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		if( S_FALSE == l_pIPDoc->IsImageDataUpdated( m_ImageIdPair.m_imageId, this ) )
		{
			//@TODO[MZA][10.10][09.11.2020] if we want have different overlays depending of direct or over group tool input, this here have to be changed.
			status = l_pIPDoc->GetImageData(m_ImageIdPair.m_imageId, m_ImageDIB, m_OverlayData );

			if( S_OK == status )
			{
				status = l_pIPDoc->MarkImageDataUpdated(m_ImageIdPair.m_imageId, this );
			}
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

// DisplayImage copies an image to the DirectDraw image surface.
HRESULT SVImageView::UpdateSurface()
{
	HRESULT status = S_OK;

	if( !( SVSVIMStateClass::CheckState( SV_STATE_CLOSING | SV_STATE_CANCELING ) ) )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();
		const unsigned char* l_pBitmapBits = GetBitmapBits();
		CWnd* l_pParent = GetParent(); // The parent of ImageView window is ImageViewScroll.

		if( !( l_BitmapInfo.empty() ) && ( nullptr != l_pBitmapBits ) && ( nullptr != l_pParent ) )
		{
			status = UpdateImageSurfaces( l_BitmapInfo );

			if( S_OK == status )
			{
				CRect l_SourceRect;
				CRect l_DestRect;

				status = GetRectInfo( l_SourceRect, l_DestRect );

				if( S_OK == status )
				{
					status = CopyBitsToSurface( l_SourceRect, l_BitmapInfo, l_pBitmapBits );
				}
			}

			// check for lost surfaces
			if( DDERR_SURFACELOST == status )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE( _T( "SVImageView::UpdateImage - DDERR_SURFACELOST\n" ) );
#endif
				RecreateLostSurface();
			}
		}
		else
		{
			status = E_FAIL;
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

// DisplayImage copies an image to the DirectDraw image surface.
HRESULT SVImageView::DisplaySurface()
{
	HRESULT status = S_OK;

	if( !( SVSVIMStateClass::CheckState( SV_STATE_CLOSING | SV_STATE_CANCELING ) ) )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();
		CWnd* l_pParent = GetParent(); // The parent of ImageView window is ImageViewScroll.

		if( !( l_BitmapInfo.empty() ) && ( nullptr != l_pParent ) )
		{
			CRect l_SourceRect;
			CRect l_DestRect;

			status = GetRectInfo( l_SourceRect, l_DestRect );

			if( S_OK == status )
			{
				status = BlitToScaledSurface( l_SourceRect, l_DestRect );
			}

			if( S_OK == status )
			{
				status = BlitToPrimarySurface( l_DestRect );
			}

			// check for lost surfaces
			if( DDERR_SURFACELOST == status )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE( _T( "SVImageView::UpdateImage - DDERR_SURFACELOST\n" ) );
#endif

				RecreateLostSurface();
			}
		}
		else
		{
			status = E_FAIL;
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageView::NotifyIPDocDisplayComplete()
{
	HRESULT status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		status = l_pIPDoc->MarkImageDataDisplayed( m_ImageIdPair.m_imageId, this );
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

void SVImageView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	SVMainFrame* pFrame =  dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	if(pFrame==nullptr)
	{
		return;
	}

	SVIPSplitterFrame* pSplitterFrame =  dynamic_cast< SVIPSplitterFrame*> (pFrame->GetActiveFrame());

	if( nullptr != pSplitterFrame )
	{
		pSplitterFrame->RefreshAllSplitters();
	}

	if(IsZoomAllowed())
	{
		pFrame->SetZoomToolbar(GetZoomHelper());
	}
}

void SVImageView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	SVIPSplitterFrame* pSplitterFrame = nullptr;
	CMDIFrameWnd* pMDIFrame =  dynamic_cast< CMDIFrameWnd*>(AfxGetApp()->m_pMainWnd);
	if( nullptr != pMDIFrame )
	{
		pSplitterFrame =  dynamic_cast< SVIPSplitterFrame*> (pMDIFrame->GetActiveFrame());
	}
	if( nullptr != pSplitterFrame )
	{
		pSplitterFrame->RefreshAllSplitters();
	}
}

void SVImageView::OnZoomTypeChanged(UINT nId)
{
	if (ID_ZOOM_SMALLEST <= nId && ID_ZOOM_FIT_HEIGHT >= nId)
	{
		ZoomEnum ZoomType = static_cast<ZoomEnum> (nId - ID_ZOOM_SMALLEST);
		SetZoom(ZoomType);
	}
}

void SVImageView::OnUpdateZoomTypeChanged(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsZoomAllowed());
	//For the Zoom Toolbar check once, when ID_ZOOM_NORMAL is checked to set the slider
	if (ID_ZOOM_NORMAL == pCmdUI->m_nID)
	{
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>(AfxGetMainWnd());
		if (nullptr != pFrame)
		{
			pFrame->EnableZoomToolbar(IsZoomAllowed());
		}
	}
}
