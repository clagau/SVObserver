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
#include "SVPatResultDlgClass.h"
#include "SVSetupDialogManager.h"
#include "SVMainFrm.h"
#include "SVIPChildFrm.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
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

IMPLEMENT_DYNCREATE(SVImageViewClass, CView)

BEGIN_MESSAGE_MAP(SVImageViewClass, CView)
	//{{AFX_MSG_MAP( SVImageViewClass )
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
// function  argument p_Width is in bytes not pixels!
static void CalcLut(BYTE* p_pFrom, BYTE* p_pTo, unsigned long p_Width, unsigned long p_Mask, unsigned long p_LowerValue, unsigned long p_UpperValue, size_t p_PixelWidth )
{
	if( 1 < p_PixelWidth )
	{
		BYTE* l_pSrc = p_pFrom;
		BYTE* l_pDst = p_pTo;

		for( size_t l_col = 0; l_col < p_Width / p_PixelWidth; ++l_col )
		{
			unsigned long* l_pSrcPixel = reinterpret_cast< unsigned long* >( l_pSrc );
			unsigned long* l_pDstPixel = reinterpret_cast< unsigned long* >( l_pDst );

			unsigned long l_Value = *l_pSrcPixel;

			if( 0xFFFFFFFF == ( l_Value | ( ~p_Mask ) )  )
			{
				*l_pDstPixel = ( l_Value & ( ~p_Mask ) ) | p_UpperValue;
			}
			else if( 0x00000000 == ( l_Value & p_Mask )  )
			{
				*l_pDstPixel = l_Value | p_LowerValue;
			}
			else
			{
				*l_pDstPixel = l_Value;
			}
			l_pSrc += p_PixelWidth;
			l_pDst += p_PixelWidth;
		}
	}
}

SVImageViewClass::SVImageViewClass()
: CView()
, m_sourceImageWidth( 0 )
, m_sourceImageHeight( 0 )
, m_sourceBitCount( 0 )
, m_hActionIcon(nullptr)
,m_ViewOrImageFilename(SvStl::GlobalPath::Inst().GetSecondObserverPath(_T("Image.bmp")).c_str())
{
	Initialize();
}

void SVImageViewClass::Initialize()
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

SVImageViewClass::~SVImageViewClass()
{
	DetachFromImage();

	if( m_hWindowBackgroundColor )
	{
		DeleteObject( m_hWindowBackgroundColor );
	}

	if( (HICON) nullptr != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon );
		m_hActionIcon = (HICON) nullptr;
	}
}

void SVImageViewClass::ReleaseImageSurface()
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

HRESULT SVImageViewClass::GetToolExtents( SVImageExtentClass& rToolExtents )
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

void SVImageViewClass::AttachToImage(uint32_t imageId)
{
	if( SvDef::InvalidObjectId != imageId )
	{
		SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(imageId));

		if( nullptr != pImage )
		{
			m_ImageId = pImage->getObjectId();
			m_imageName = pImage->GetCompleteName();

			GetIPDoc()->RegisterImage( m_ImageId, this );

			CRect l_rect;

			GetImageRect( l_rect );
			SetImageRect( l_rect );
		}
	}
	else
	{
		m_ImageId = imageId;
	}


	if(SvDef::InvalidObjectId == m_ImageId)
	{
		GetIPDoc()->UnregisterImageView( this );

		ReleaseImageSurface();
	}
}

void SVImageViewClass::AttachToImage( LPCTSTR p_imageName )
{
	uint32_t l_ImageId = SvDef::InvalidObjectId;
	bool Attach( false );

	std::string l_imageName = p_imageName;

	if( l_imageName.empty() )
	{
		l_imageName = m_imageName;
	}

	if( ! l_imageName.empty() )
	{
		SvIe::SVImageClass* pImage = GetImageByName( l_imageName.c_str() );

		if( nullptr != pImage )
		{
			l_ImageId = pImage->getObjectId();
			Attach = true;
		}

		m_imageName = l_imageName;
	}

	if( Attach )
	{
		AttachToImage( l_ImageId );
	}
}

void SVImageViewClass::DetachFromImage()
{
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		GetIPDoc()->UnregisterImageView( this );
	}

	ReleaseImageSurface();

	SetScrollPosition( CPoint( 0, 0 ) );

	SetImageRect( CRect( 0, 0, 0, 0 ) );

	m_imageName.clear();
	m_ImageId = SvDef::InvalidObjectId;
	m_ImageDIB.clear();
	m_OverlayData.clear();
}

void SVImageViewClass::GetImageRect( CRect& p_rect )
{
	p_rect.top = 0;
	p_rect.left = 0;
	p_rect.right = 0;
	p_rect.bottom = 0;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc && l_pIPDoc->m_bAllowRefresh )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();

		p_rect.right = l_BitmapInfo.GetWidth();

		if( p_rect.right < 0 )
		{
			p_rect.right = 0;
		}

		p_rect.bottom = ::labs( l_BitmapInfo.GetHeight() );

		if( p_rect.bottom < 0 )
		{
			p_rect.bottom = 0;
		}

		p_rect.right = static_cast< long >( static_cast< double >( p_rect.right * m_ZoomHelper.GetZoom() ) );
		p_rect.bottom = static_cast< long >( static_cast< double >( p_rect.bottom * m_ZoomHelper.GetZoom() ) );
	}
}

BOOL SVImageViewClass::OnCommand( WPARAM p_wParam, LPARAM p_lParam )
{
	long l_index = 0;

	switch( p_wParam )
	{
		case ID_CONFIG_ANALYZER:
		{
			if( TheSVObserverApp.OkToEdit() )
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

					SvAo::SVAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass*>(pTool->getFirstObject(l_svInfo));
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
			if(SvDef::InvalidObjectId == m_ImageId ||
				!TheSVObserverApp.OkToEdit() )
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

	return CView::OnCommand( p_wParam, p_lParam );
}

void SVImageViewClass::SaveViewOrImageToDisk(bool ViewOnly, bool showOverlays)
{
	auto pCurrentImage = GetImage();

	if (nullptr == pCurrentImage)
	{
		SvStl::MessageMgrStd NullImageWarning(SvStl::MsgType::Log | SvStl::MsgType::Display );
		NullImageWarning.setMessage(SVMSG_SVO_5018_NULLIMAGE, SvStl::Tid_DisplayedImageIsUnavailable, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_30000_NullImageOnSave);
		return;
	}

	m_ViewOrImageFilename.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
	
	std::string Path = AfxGetApp()->GetProfileString( RegSection, RegKeySaveViewPath, DefaultPath );
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
void SVImageViewClass::OnContextMenu( CWnd* p_pWnd, CPoint p_point )
{
	CMenu l_menu;
	bool RunOrTestMode( false );

	RunOrTestMode = SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );

	m_mousePoint.x = p_point.x;
	m_mousePoint.y = p_point.y;

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
			{
				bool l_resultFound = false;
				SvAo::SVAnalyzerClass* pAnalyzer = nullptr;
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

							pAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass*>(pTool->getFirstObject(l_svInfo));
							if (pAnalyzer)
							{
								l_resultFound = pAnalyzer->IsPtOverResult(l_point);
							}
						}
					}
				}

				if( !l_resultFound || RunOrTestMode )
				{
					l_pPopup->DeleteMenu( ID_ANALYZER_RESULT, MF_BYCOMMAND );
				}

				if( !pAnalyzer || RunOrTestMode )
				{
					l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				}

				if( !TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) || RunOrTestMode )
				{
					l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				}
			}

			l_pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, p_point.x, p_point.y, this );
		}
	}
}

void SVImageViewClass::TransformFromViewSpace( CPoint& p_point )
{
	SVDrawContext l_svDrawContext( nullptr, m_ZoomHelper.GetZoom() );

	l_svDrawContext.InverseTransform( &p_point, &p_point, 1 );
}

void SVImageViewClass::SelectDisplayImage()
{
	SVDisplayImageSelect l_svDlg;
	
	l_svDlg.m_pDoc = GetIPDoc();

	if( nullptr != l_svDlg.m_pDoc )
	{
		SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(m_ImageId));
		l_svDlg.m_pCurrentImage = pImage;

		if( IDOK == l_svDlg.DoModal() )
		{
			if( l_svDlg.m_pCurrentImage )
			{
				AttachToImage( l_svDlg.m_pCurrentImage->getObjectId() );
			}
			else
			{
				DetachFromImage();
			}
		}
	}
}

uint32_t SVImageViewClass::GetImageID() const
{
	return m_ImageId;
}

SvIe::SVImageClass* SVImageViewClass::GetImage()
{
	return dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(m_ImageId));
}

SvIe::SVImageClass* SVImageViewClass::GetImageByName( LPCTSTR ImageName ) const
{
	SvIe::SVImageClass* pImage = nullptr;
	SVIPDoc* l_pDocument = GetIPDoc();

	if( nullptr != l_pDocument )
	{
		pImage = l_pDocument->GetImageByName(ImageName);
	}
	return pImage;
}

HRESULT SVImageViewClass::RecreateImageSurface()
{
	HRESULT hr = DD_FALSE;

	SVBitmapInfo l_BitmapInfo = GetBitmapInfo();

	if( !( l_BitmapInfo.empty() ) )
	{
		hr = SVDirectX::CreateSurface( l_BitmapInfo, &m_pDDImageSurface );

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		if (DD_OK != hr)
		{
			TRACE( _T( "SVImageViewClass::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
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
				TRACE( _T( "SVImageViewClass::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
			}
#endif
		}

		CRect l_rect;

		GetImageRect( l_rect );
		SetImageRect( l_rect );
	}
	return hr;
}

void SVImageViewClass::ShowExtremeLUT( bool p_show /* = true */ )
{
	m_showExtremeLUT = p_show;
	if( m_pDDImageSurface )
	{
		UpdatePaletteForLut(p_show, m_pDDImageSurface);
	}
	if( GetSafeHwnd() )
	{
		Invalidate( false );
	}
}

void SVImageViewClass::OnDraw( CDC* p_pDC )
{
	if (SvDef::InvalidObjectId == m_ImageId || SVSVIMStateClass::CheckState(SV_STATE_CLOSING))
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

BOOL SVImageViewClass::Create( LPCTSTR p_className, LPCTSTR p_windowName, DWORD p_style, const RECT& p_rect, CWnd* p_pParentWnd, UINT p_NID, CCreateContext* p_pContext )
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
		Result = CWnd::Create( WindowClass.lpszClassName, _T( "Untitled Main Image" ), p_style, p_rect, p_pParentWnd, p_NID, p_pContext );
	}

	if( Result )
	{
		ReleaseImageSurface();
	}

	return Result;
}

void SVImageViewClass::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void SVImageViewClass::OnUpdate( CView* p_pSender, LPARAM p_lHint, CObject* p_pHint )
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

		CView::OnUpdate( p_pSender, p_lHint, p_pHint );
	}

	if( 0 == GetUpdateRect( nullptr ) )
	{
		NotifyIPDocDisplayComplete();
	}
}

void SVImageViewClass::OnLButtonDblClk( UINT p_nFlags, CPoint p_point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		SVIPDoc* l_psvIPDoc = GetIPDoc();
		CPoint l_point = p_point;

		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace( l_point );

		std::string Text = SvUl::Format( _T(" X: %d, Y: %d "), l_point.x, l_point.y );
		TheSVObserverApp.SetStatusText( Text.c_str() );

		if (SvDef::InvalidObjectId != m_ImageId && nullptr != l_psvIPDoc)
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

		CWnd::OnLButtonDblClk( p_nFlags, p_point );
	}
}

void SVImageViewClass::OnRButtonDblClk( UINT p_nFlags, CPoint p_point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
	{
		CPoint l_point = p_point;
		SVIPDoc* l_psvIPDoc = GetIPDoc();
		
		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace( l_point );
		
		std::string Text = SvUl::Format( _T(" X: %d, Y: %d "), l_point.x, l_point.y );
		TheSVObserverApp.SetStatusText( Text.c_str() );
		
		if( nullptr != l_psvIPDoc && SvDef::InvalidObjectId != m_ImageId )
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(l_psvIPDoc->GetSelectedToolID()));
			if(nullptr !=  pTool)
			{
				// Try to call SetupDialog for first found Analyzer...
				SvDef::SVObjectTypeInfoStruct l_svInfo;
				l_svInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;

				SvAo::SVAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass*>(pTool->getFirstObject(l_svInfo));
				if(nullptr != pAnalyzer)
				{
					SVSetupDialogManager::Instance().SetupDialog( pAnalyzer->GetClassID(), pAnalyzer->getObjectId(), this );

					l_psvIPDoc->RunOnce();
				}
			}
		}

		CWnd::OnRButtonDblClk( p_nFlags, p_point );
	}
}

void SVImageViewClass::OnLButtonDown( UINT p_nFlags, CPoint p_point )
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp.OkToEdit() )
	{
		if(SvDef::InvalidObjectId != m_ImageId)
		{
			std::string Text;
			CPoint l_point = p_point;

			// Compensate for Scaling of Displayed Image
			TransformFromViewSpace( l_point );

			m_isPicked = GetObjectAtPoint( l_point );
			m_svMousePickLocation = m_svLocation;
			if( m_isPicked )
			{
				SVImageExtentClass l_svExtents;

				POINT l_tempPoint;

				long l_width = 0;
				long l_height = 0;

				if( S_OK == GetToolExtents( l_svExtents ) &&
					S_OK == l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, l_tempPoint ) &&
					S_OK == l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyWidth, l_width ) &&
					S_OK == l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyHeight, l_height ) )
				{
					Text = SvUl::Format( _T(" X: %d, Y: %d    cX: %d, cY: %d "), l_tempPoint.x, l_tempPoint.y, l_width, l_height );
				}
				else
				{
					Text = SvUl::Format( _T(" Col: %d, Row: %d "), p_point.x, p_point.y );
				}

				m_lastMouseMovePoint = p_point;

				SetCapture();
			}
			else
			{
				Text = SvUl::Format( _T(" Col: %d, Row: %d "), p_point.x, p_point.y );
			}

			TheSVObserverApp.SetStatusText( Text.c_str() );
		}

		CWnd::OnLButtonDown( p_nFlags, p_point );
	}
}

void SVImageViewClass::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp.OkToEdit() )
	{
		if (m_lastMouseMovePoint == point)
		{
			//avoid calling this function when mous is not moving
			return;
		}
		
		CPoint l_point = point;
		CPoint l_clientPoint = point;
		CPoint l_screenPoint = point;

		ClientToScreen( &l_screenPoint );

		CRect l_rect;
		CRect l_screenRect;

		GetClientRect( l_rect );
		GetWindowRect( l_screenRect );


		// Get Color of Mouse Point ( inside of View )
		HDC hDC = ::GetDC( m_hWnd );
		COLORREF color  = ::GetPixel( hDC, point.x, point.y );
		::ReleaseDC( m_hWnd, hDC );

		BYTE redValue   = GetRValue( color );
		BYTE greenValue = GetGValue( color );
		BYTE blueValue  = GetBValue( color );

		// Compensate for Scaling of Displayed Image
		TransformFromViewSpace( l_point );

		// Status Text: Mouse Pos and Color
		std::string Text = SvUl::Format( _T(" Col: %d, Row: %d    RGB: %u/%u/%u "), l_point.x, l_point.y, redValue, greenValue, blueValue ); 

		HICON l_hCursor = nullptr;

		SVImageExtentClass l_svExtents;

		SVIPDoc* l_pIPDoc = GetIPDoc();

		if (m_isPicked)
		{
			if( nullptr != l_pIPDoc && m_isPicked && S_OK == GetToolExtents( l_svExtents ) )
			{
				CPoint l_startPoint = m_lastMouseMovePoint;

				SVImageExtentClass l_svTempExtents = l_svExtents;

				TransformFromViewSpace( l_startPoint );

				l_hCursor = GetObjectCursor( m_svMousePickLocation, l_point );
				SVPoint<double> startPoint(l_startPoint);
				if( (SvPb::SVExtentLocationPropertyRotate == m_svMousePickLocation ||
					m_svMousePickLocation == l_svExtents.GetLocationPropertyAt(startPoint) ) &&
					S_OK == l_svTempExtents.Update( m_svMousePickLocation, startPoint, SVPoint<double>(l_point) ) )
				{
					bool l_bUpdate = S_OK == l_pIPDoc->UpdateExtents( m_pTool, l_svTempExtents );

					if( l_bUpdate || l_rect.PtInRect( l_clientPoint ) )
					{
						l_bUpdate = l_bUpdate || S_OK == l_pIPDoc->UpdateExtentsToFit( m_pTool, l_svTempExtents );
					}

					if( l_bUpdate )
					{
						long l_left = 0;
						long l_top = 0;
						long l_width = 0;
						long l_height = 0;

						l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPointX, l_left );
						l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPointY, l_top );
						l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyWidth, l_width );
						l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyHeight, l_height );

						// Status Text: Mouse Pos and Tool Extent
						Text = SvUl::Format( _T(" Col: %d, Row: %d    X: %d, Y: %d    cX: %d, cY: %d "), l_point.x, l_point.y, l_left, l_top, l_width, l_height );
					}
				}
			}
			else
			{
				l_hCursor = GetObjectCursor( l_point );
			}
		}
		else
		{
			l_hCursor = GetObjectCursor( l_point );
		}

		//If no icon returned then use standard arrow
		if (nullptr == l_hCursor)
		{
			m_mouseIsOverTool = FALSE;
			l_hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		}
		SetCursor( l_hCursor );
		
		TheSVObserverApp.SetStatusText( Text.c_str() );

		CWnd::OnMouseMove( nFlags, point );
	}

	m_lastMouseMovePoint = point;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnLButtonUp
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageViewClass::OnLButtonUp( UINT p_nFlags, CPoint p_point )
{
	m_isPicked = FALSE;

	m_svMousePickLocation = SvPb::SVExtentLocationPropertyUnknown;

	ReleaseCapture();

	std::string Text = SvUl::Format( _T(" Col: %d, Row: %d "), p_point.x, p_point.y );
	TheSVObserverApp.SetStatusText( Text.c_str() );

	CWnd::OnLButtonUp( p_nFlags, p_point );

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
void SVImageViewClass::OnCaptureChanged( CWnd* p_pWnd )
{
	TheSVObserverApp.SetStatusText( nullptr );

	CWnd::OnCaptureChanged( p_pWnd );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnNcMouseMove
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageViewClass::OnNcMouseMove( UINT p_hitTest, CPoint p_point )
{
	TheSVObserverApp.SetStatusText( nullptr );

	CWnd::OnNcMouseMove( p_hitTest, p_point );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnDestroy
// -----------------------------------------------------------------------------
// .Description : ...
////////////////////////////////////////////////////////////////////////////////
void SVImageViewClass::OnDestroy()
{
	DetachFromImage();

	CView::OnDestroy();
}

#ifdef _DEBUG

void SVImageViewClass::AssertValid() const
{
	CView::AssertValid();
}

void SVImageViewClass::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

SVIPDoc* SVImageViewClass::GetIPDoc() const
{
	return dynamic_cast< SVIPDoc* >( GetDocument() );
}

bool SVImageViewClass::GetScrollPosition( CPoint& p_point )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			p_point = l_psvScroll->GetScrollPosition();
		}
	}

	return l_bOk;
}

bool SVImageViewClass::SetScrollPosition( CPoint& p_point )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll *l_psvScroll = dynamic_cast< SVImageViewScroll * >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			l_psvScroll->ScrollToPosition( p_point );
		}
	}

	return l_bOk;
}

bool SVImageViewClass::ImageIsEmpty() const
{
	return SvDef::InvalidObjectId == m_ImageId;
}

bool SVImageViewClass::CalculateZoomFit(ZoomEnum ZoomType)
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

void SVImageViewClass::DisplayAnalyzerResult(const SvDef::StringVector& rAnalyzerResults, SvPb::SVObjectSubTypeEnum analyzerType) const
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

double SVImageViewClass::SetZoomValue(double Value, bool bSetZoomSlider )
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

bool SVImageViewClass::SetZoom(ZoomEnum ZoomType, unsigned  scaleIndex, bool bSetZoomSlider)
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

const ZoomHelper& SVImageViewClass::GetZoomHelper() const
{
	return m_ZoomHelper;
}

bool SVImageViewClass::SetImageRect( CRect& p_rect )
{
	bool l_bOk = false;

	if( nullptr != GetSafeHwnd() )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = nullptr != l_psvScroll;

		if( l_bOk )
		{
			l_psvScroll->SetImageSize( p_rect.Size() );
		}
	}
	return l_bOk;
}

HRESULT SVImageViewClass::ShouldDraw( const SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_hrOk = S_OK;

	uint32_t l_SelectedID = SvDef::InvalidObjectId;

	if( nullptr != GetIPDoc() )
	{
		l_SelectedID = GetIPDoc()->GetSelectedToolID();
	}

	switch( p_rMultiLine.m_ToolSetDrawFlag )
	{
		case 0:	// "All Tools=0"
			l_hrOk = S_OK;
			return l_hrOk;

		case 1: // "Current Tool"
			if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;

			return l_hrOk;

		case 2: // "Warned Tools"
		{
			if( p_rMultiLine.m_Warned )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;

			return l_hrOk;
		}

		case 3: // "Failed Tools"
		{
			if( p_rMultiLine.m_Failed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
			
			return l_hrOk;
		}

		case 4: // "Passed Tools"
		{
			if( p_rMultiLine.m_Passed )
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
	switch( p_rMultiLine.m_ToolDrawFlag )
	{
		case 0:	// "Always=0"
			l_hrOk = S_OK;
			break;

		case 1: // "If Current"
			if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
			break;

		case 2: // "If Warned"
		{
			if( p_rMultiLine.m_Warned )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
		}
		break;

		case 3: // "If Failed"
		{
			if( p_rMultiLine.m_Failed )
				l_hrOk = S_OK;
			else
				l_hrOk = S_FALSE;
		}
		break;

		case 4: // "If Passed"
		{
			if( p_rMultiLine.m_Passed )
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

void SVImageViewClass::DrawOverlay( SVDrawContext* PDrawContext, const SVExtentMultiLineStruct& p_rMultiLine )
{
	BOOL bDrawAllowed = ( ( nullptr != PDrawContext ) && ( S_OK == ShouldDraw( p_rMultiLine ) ) );

	if( bDrawAllowed )
	{
		HGDIOBJ hPen;
		HGDIOBJ hPenOld;

		COLORREF color = p_rMultiLine.m_Color;
		POINT titlePoint = static_cast<POINT> (p_rMultiLine.m_StringPoint);
		std::string Title = p_rMultiLine.m_csString;

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
		if(SvDef::InvalidObjectId != l_SelectedID && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
		{
			l_PenStyle = PS_DOT;
		}

		hPen = CreatePen( l_PenStyle, 1, color );

		hPenOld = SelectObject( PDrawContext->DC, hPen );

		SVDrawObjectListClass drawObjectList;

		drawObjectList.AddExtentMultiLineData( p_rMultiLine, l_PenStyle );
		drawObjectList.m_bDrawFigureHatched = p_rMultiLine.m_bDrawFigureHatched;
		drawObjectList.Draw( PDrawContext );

		SelectObject( PDrawContext->DC, hPenOld );
		DeleteObject( hPen );
	}
}

void SVImageViewClass::UpdateOverlays(HDC hDC, long p_X, long p_Y)
{
	// Get drawing device context...
	::SetBkMode(hDC, TRANSPARENT);

	// Select Font...
	HGDIOBJ hFontOld = ::SelectObject(hDC, m_ZoomHelper.GetFont());

	// Use Drawing Context

	SVDrawContext drawContext(hDC, m_ZoomHelper.GetZoom());
	drawContext.ViewPortOffset.x = -p_X;
	drawContext.ViewPortOffset.y = -p_Y;

	// Draw tool figures...
	for (SVExtentMultiLineStructVector::iterator l_Iter = m_OverlayData.begin(); l_Iter != m_OverlayData.end(); ++l_Iter)
	{
		DrawOverlay(&drawContext, *l_Iter);
	}

	// Restore Font...
	::SelectObject(hDC, hFontOld);
}

HICON SVImageViewClass::GetObjectCursor( POINT p_point )
{
	HICON l_hCursor = nullptr;

	if( GetObjectAtPoint( p_point ) )
	{
		l_hCursor = GetObjectCursor(m_svLocation, p_point);
	}

	return l_hCursor;
}

HICON SVImageViewClass::GetObjectCursor( SvPb::SVExtentLocationPropertyEnum p_svLocation, POINT p_point )
{
	HICON l_hCursor = nullptr;

	LPCTSTR l_cursorId = IDC_ARROW;
	LPCTSTR l_cursorLeftRight = IDC_SIZEWE;
	LPCTSTR l_cursorTopBottom = IDC_SIZENS;
	LPCTSTR l_cursorTopLeftBottomRight = IDC_SIZENWSE;
	LPCTSTR l_cursorTopRightBottomLeft = IDC_SIZENESW;
	LPCTSTR l_cursorMove = IDC_SIZEALL;

	//If action icon is loaded then need to clean it up before next icon
	if( (HICON) nullptr != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon  );
		m_hActionIcon = (HICON) nullptr;
	}

	SVImageExtentClass l_svExtents;
	//Check the rotation angle to determine icon symbols
	if( S_OK == GetToolExtents( l_svExtents ) )
	{
		double l_dRotationAngle = 0.0;
		//Check that this is the Linear tool with rotation
		if(SvPb::SVExtentTranslationProfile == l_svExtents.GetTranslation() )
		{
			if( S_OK == l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyRotationAngle, l_dRotationAngle) )
			{
				l_dRotationAngle = fmod(l_dRotationAngle, 180.0);
			}
		}
		//Check that this is the Polar Unwrap tool with rotation
		else if(SvPb::SVExtentTranslationPolarUnwrap == l_svExtents.GetTranslation() )
		{
			SVPoint<double> center;
			SVPoint<double> mousePosition(p_point);
			if( S_OK == l_svExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, center) )
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

	switch( p_svLocation )
	{
		case SvPb::SVExtentLocationPropertyRotate:
		{
			l_cursorId = 0;
			m_hActionIcon = ::LoadImage(AfxGetResourceHandle(),
	                                   MAKEINTRESOURCE(IDI_ARROW_ROTATE),
	                                   IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);
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
	if( IDC_ARROW == l_cursorId && SvPb::SVExtentLocationPropertyDisabled != p_svLocation )
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
BOOL SVImageViewClass::GetObjectAtPoint( POINT p_point )
{
	BOOL l_bOk = FALSE;

	m_pTool = nullptr;

	m_svLocation = SvPb::SVExtentLocationPropertyUnknown;

	SvTo::SVToolClass* pTool = nullptr;

	if( nullptr != GetIPDoc() )
	{
		pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID()));
	}

	if( nullptr != pTool && pTool->isInputImage( m_ImageId ) )
	{
		SVImageExtentClass l_svExtents;
		SVPoint<double> point(p_point);
		m_pTool = pTool->GetObjectAtPoint(point);

		if( S_OK == GetToolExtents( l_svExtents ) )
		{
			m_svLocation = l_svExtents.GetLocationPropertyAt(point);
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

void SVImageViewClass::GetParameters(SvOi::IObjectWriter& rWriter)
{
	_variant_t l_svVariant;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(m_ImageId));

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

bool SVImageViewClass::SetParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent )
{
	bool bZoomExOK = false;

	_variant_t Value;

	bool l_bUseImageView = false;

	bool l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_VIEW_INITIALIZED, p_parent, Value );
	if( l_bOk )
	{
		l_bUseImageView = Value;
		bZoomExOK = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, Value );
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
		l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_ZOOM_FACTOR, p_parent, Value );
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
		l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_NAME, p_parent, Value );
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

bool SVImageViewClass::CheckParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent )
{
	_variant_t Value;

	bool l_bUseImageView = false;
	bool bZoomExOK = false;

	bool l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_VIEW_INITIALIZED, p_parent, Value );
	if( l_bOk )
	{
		l_bUseImageView = Value;
		bZoomExOK = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, Value );
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
		l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_ZOOM_FACTOR, p_parent, Value );
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
		l_bOk = SvXml::SVNavigateTree::GetItem( p_tree, SvXml::CTAG_IMAGE_NAME, p_parent, Value );
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

BOOL SVImageViewClass::OnEraseBkgnd( CDC* p_pDC )
{
	BOOL l_bOk = TRUE;

	//calling CView::OnEraseBkgnd causes flicker and will get painted over by image

	return l_bOk;
}

SVBitmapInfo SVImageViewClass::GetBitmapInfo() const
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
	else if(SvDef::InvalidObjectId != m_ImageId)
	{
		SVIPDoc* l_pIPDoc = GetIPDoc();

		if( nullptr != l_pIPDoc )
		{
			l_pIPDoc->GetBitmapInfo( m_ImageId, l_Info );
		}
	}

	return l_Info;
}

const unsigned char* SVImageViewClass::GetBitmapBits() const
{
	SVBitmapInfo l_Info = GetBitmapInfo();

	if( !( m_ImageDIB.empty() ) && !( l_Info.empty() ) )
	{
		return reinterpret_cast<const unsigned char*>(&( m_ImageDIB[ l_Info.GetBitmapInfoSizeInBytes() ] ));
	}

	return nullptr;
}

HRESULT SVImageViewClass::UpdateImageSurfaces( const SVBitmapInfo& p_rBitmapInfo )
{
	HRESULT status = S_OK;

	if( !( SVDirectX::Instance().empty() ) && !( p_rBitmapInfo.empty() ) )
	{
		if( m_sourceImageWidth != p_rBitmapInfo.GetWidth() ||
			m_sourceImageHeight != ::labs( p_rBitmapInfo.GetHeight() ) ||
			m_sourceBitCount != p_rBitmapInfo.GetBitCount() ||
			nullptr == m_pDDImageSurface || nullptr == m_pDDScaledImageSurface )
		{
			m_sourceImageWidth = p_rBitmapInfo.GetWidth();
			m_sourceImageHeight = ::labs( p_rBitmapInfo.GetHeight() );
			m_sourceBitCount = p_rBitmapInfo.GetBitCount();

			status = SVDirectX::CreateSurface( p_rBitmapInfo, &m_pDDImageSurface );
			
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

HRESULT SVImageViewClass::CopyBitsToSurface( const CRect& p_rSourceRect, const SVBitmapInfo& p_rBitmapInfo, const unsigned char* p_pBitmapBits )
{
	HRESULT status = S_OK;

	if( !( p_rBitmapInfo.empty() ) && ( nullptr != p_pBitmapBits ) && ( nullptr != m_pDDImageSurface ) )
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );
		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );

		status = m_pDDImageSurface->Lock( nullptr, &l_ddSurfaceDesc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr );
		if( DD_OK == status )
		{
			unsigned short l_BitmapBitCount = p_rBitmapInfo.GetBitCount();
			unsigned long l_BitmapWidth = p_rBitmapInfo.GetWidth();
			unsigned long l_BitmapPitch = static_cast<unsigned long>(p_rBitmapInfo.GetBitmapImageStrideInBytes());
			unsigned long l_VisiblePitch = ( p_rSourceRect.Width() * l_BitmapBitCount / 8 );

			if( ( l_BitmapWidth == l_ddSurfaceDesc.dwWidth ) && 
				( l_BitmapBitCount == l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount ) &&
				( l_VisiblePitch <= l_BitmapPitch ) )
			{
				unsigned long l_Mask = l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;
				unsigned long l_UpperValue = l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask;
				unsigned long l_LowerValue = l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;

				BYTE* l_pTo = static_cast< BYTE* >( l_ddSurfaceDesc.lpSurface );
				BYTE* l_pFrom = const_cast< BYTE* >( p_pBitmapBits );

				unsigned long l_SurfaceOffset = ( p_rSourceRect.top * l_ddSurfaceDesc.lPitch ) + ( p_rSourceRect.left * l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8 );
				unsigned long l_BitmapOffset = ( p_rSourceRect.top * l_BitmapPitch ) + ( p_rSourceRect.left * p_rBitmapInfo.GetBitCount() / 8 );

				l_pTo += l_SurfaceOffset;
				l_pFrom += l_BitmapOffset;
				size_t l_PixelWidth = l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8;

				for( long l_row = p_rSourceRect.top; l_row < p_rSourceRect.bottom; ++l_row )
				{
					if (m_showExtremeLUT && l_BitmapBitCount > 8)
					{
						CalcLut(l_pFrom, l_pTo, l_VisiblePitch, l_Mask, l_LowerValue, l_UpperValue, l_PixelWidth);
					}
					else
					{
						memcpy( l_pTo, l_pFrom, l_VisiblePitch );
					}

					l_pTo += l_ddSurfaceDesc.lPitch;
					l_pFrom += l_BitmapPitch;
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

bool SVImageViewClass::IsZoomAllowed() const
{
	bool allowed  =  !ImageIsEmpty();
	return allowed;
}

void SVImageViewClass::UpdateZoomToolbar()
{
	//only when we have the focus 
	SVImageViewClass* pFocus = dynamic_cast<SVImageViewClass*>(GetFocus());

	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

	if( nullptr != pFrame && pFocus == this )
	{
		if (IsZoomAllowed())
		{
			pFrame->SetZoomToolbar(GetZoomHelper());
		}
	}
}

HRESULT SVImageViewClass::BlitToScaledSurface( CRect& p_rSourceRect, CRect& p_rDestRect, LPCTSTR Filepath, bool showOverlays)
{
	HRESULT status = S_OK;

	if( nullptr != m_pDDImageSurface && nullptr != m_pDDScaledImageSurface )
	{
		HDC scaledDC = nullptr;

		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

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
					p_rSourceRect.left,
					p_rSourceRect.top,
					p_rSourceRect.Width(),
					p_rSourceRect.Height(),
					SRCCOPY );

				::SetStretchBltMode( scaledDC, iStretchMode );

				m_pDDImageSurface->ReleaseDC( surfaceDC );
			}

			if (showOverlays)
			{
				UpdateOverlays( scaledDC, p_rSourceRect.left, p_rSourceRect.top );
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
			TRACE(_T( "SVImageViewClass::UpdateImage - Surface Blit failed %08lx\n" ), status );
		}
#endif
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageViewClass::BlitToPrimarySurface( CRect& p_rDestRect )
{
	HRESULT status = S_OK;

	if( nullptr != m_pDDScaledImageSurface )
	{
		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

		SVDirectX::Instance().SetClipperHWnd( 0, m_hWnd );

		status = SVDirectX::Instance().GetBltStatus( DDGBS_CANBLT ); // @Note: a do/while loop around GetBltStatus() was removed in SVO-2746 since it did not seem necessary
		if (S_OK == status)
		{
			status = SVDirectX::Instance().Blt(
				&p_rDestRect,            // destination rectangle
				m_pDDScaledImageSurface, // source surface
				&l_ScaledRect,           // source rectangle
				DDBLT_WAIT,              // DDBLT_DONOTWAIT| DDBLT_ASYNC, // Flag <<<<<<<<<<<<<<<<<<<<<<
				nullptr);               // Pointer to DDBLTFX Structure
		}
		else
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
			SvDef::StringVector msgList;
			std::ostringstream oss;
			oss << "GetBltStatus() Failed!! Result = " << status;
			msgList.push_back(oss.str());
			Msg.setMessage(SVMSG_SVO_0_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageViewClass::RecreateLostSurface()
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
					TRACE( _T( "SVImageViewClass::UpdateImage - CreateSurface failed\n" ) );
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

HRESULT SVImageViewClass::GetRectInfo( CRect& p_rSourceRect, CRect& p_rDestRect )
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

		p_rDestRect.left = l_xScroll; 
		p_rDestRect.top = l_yScroll;
		p_rDestRect.right =  std::min( l_parentRect.Width() + l_xScroll, l_imageRect.Width() );
		p_rDestRect.bottom = std::min( l_parentRect.Height() + l_yScroll, l_imageRect.Height() );

		p_rSourceRect.right = static_cast< long >( std::min< double >( l_zoomFactor * ( l_xScroll + p_rDestRect.Width() ), l_sourceImageWidth ) );
		p_rSourceRect.left = static_cast< long >( l_zoomFactor * l_xScroll );
		p_rSourceRect.bottom = static_cast< long >( std::min< double >( l_zoomFactor * ( l_yScroll + p_rDestRect.Height() ), l_sourceImageHeight ) );
		p_rSourceRect.top = static_cast< long >( l_zoomFactor * l_yScroll );

		ClientToScreen( &p_rDestRect );
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

HRESULT SVImageViewClass::UpdateBufferFromIPDoc()
{
	HRESULT status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		if( S_FALSE == l_pIPDoc->IsImageDataUpdated( m_ImageId, this ) )
		{
			status = l_pIPDoc->GetImageData( m_ImageId, m_ImageDIB, m_OverlayData );

			if( S_OK == status )
			{
				status = l_pIPDoc->MarkImageDataUpdated( m_ImageId, this );
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
HRESULT SVImageViewClass::UpdateSurface()
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
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );
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
HRESULT SVImageViewClass::DisplaySurface()
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
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );
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

HRESULT SVImageViewClass::NotifyIPDocDisplayComplete()
{
	HRESULT status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		status = l_pIPDoc->MarkImageDataDisplayed( m_ImageId, this );
	}
	else
	{
		status = E_FAIL;
	}

	return status;
}

void SVImageViewClass::OnSetFocus(CWnd* pOldWnd)
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

void SVImageViewClass::OnKillFocus(CWnd* pNewWnd)
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

void SVImageViewClass::OnZoomTypeChanged(UINT nId)
{
	if (ID_ZOOM_SMALLEST <= nId && ID_ZOOM_FIT_HEIGHT >= nId)
	{
		ZoomEnum ZoomType = static_cast<ZoomEnum> (nId - ID_ZOOM_SMALLEST);
		SetZoom(ZoomType);
	}
}

void SVImageViewClass::OnUpdateZoomTypeChanged(CCmdUI* pCmdUI)
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
