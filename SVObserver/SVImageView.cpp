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
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/ImageFileUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVSVIMStateClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVAdjustToolSizePositionDlg.h"
#include "SVAnalyzer.h"
#include "SVDirectX.h"
#include "SVDisplayImageSelect.h"
#include "SVDrawObject.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVImageViewScroll.h"
#include "SVIPDoc.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVObserver.h"
#include "SVSetupDialogManager.h"
#include "SVToolLoadImage.h"
#include "SVMainFrm.h"
#include "RootObject.h"
#include "SVIPChildFrm.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "SVGuiExtentUpdater.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/GlobalPath.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

IMPLEMENT_DYNCREATE( SVImageViewClass, CView )

BEGIN_MESSAGE_MAP( SVImageViewClass, CView )
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
	ON_COMMAND(ID_ZOOM_PLUS, &SVImageViewClass::OnZoomPlus)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_PLUS, &SVImageViewClass::OnUpdateZoomPlus)
	ON_COMMAND(ID_ZOOM_MINUS, &SVImageViewClass::OnZoomMinus)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_MINUS, &SVImageViewClass::OnUpdateZoomMinus)
	ON_COMMAND(ID_ZOOM_ONE, &SVImageViewClass::OnZoomOne)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_ONE, &SVImageViewClass::OnUpdateZoomOne)
	ON_COMMAND(ID_ZOOM_FIT, &SVImageViewClass::OnZoomFit)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT, &SVImageViewClass::OnUpdateZoomFit)
	ON_COMMAND(ID_ZOOM_SLIDER_MOVED, &SVImageViewClass::OnZoomSliderMoved)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

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

	m_psvObject = nullptr;

	m_svLocation = SVExtentLocationPropertyUnknown;

	m_svMousePickLocation = SVExtentLocationPropertyUnknown;

	m_mouseIsOverTool = FALSE;

	m_pDDImageSurface = nullptr;
	m_pDDScaledImageSurface = nullptr;

	m_isPicked = FALSE;

	m_hWindowBackgroundColor = ::CreateSolidBrush( SV_DEFAULT_IMAGE_VIEW_BACKGROUND_COLOR );
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

HRESULT SVImageViewClass::GetToolExtents( SVImageExtentClass& p_svToolExtents )
{
	HRESULT l_hrOk = S_OK;

	if( nullptr != m_psvObject )
	{
		l_hrOk = m_psvObject->GetImageExtent( p_svToolExtents );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

void SVImageViewClass::AttachToImage( const SVGUID& p_rImageId )
{
	if( !( p_rImageId.empty() ) )
	{
		SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( p_rImageId.ToGUID() ) );

		if( nullptr != l_pImage )
		{
			m_ImageId = l_pImage->GetUniqueObjectID();
			m_imageName = l_pImage->GetCompleteObjectName();

			GetIPDoc()->RegisterImage( m_ImageId, this );

			CRect l_rect;

			GetImageRect( l_rect );
			SetImageRect( l_rect );
		}
	}
	else
	{
		m_ImageId.clear();
	}


	if( m_ImageId.empty() )
	{
		GetIPDoc()->UnregisterImageView( this );

		ReleaseImageSurface();
	}
}

void SVImageViewClass::AttachToImage( LPCTSTR p_imageName )
{
	SVGUID l_ImageId;
	BOOL l_attach = FALSE;

	CString l_imageName = p_imageName;

	if( l_imageName.IsEmpty() )
	{
		l_imageName = m_imageName;
	}

	if( ! l_imageName.IsEmpty() )
	{
		SVImageClass* l_psvImage = nullptr;

		l_psvImage = GetImageByName( l_imageName );

		if( nullptr != l_psvImage )
		{
			l_ImageId = l_psvImage->GetUniqueObjectID();
			l_attach = TRUE;
		}

		m_imageName = l_imageName;
	}

	if( l_attach )
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

	m_imageName.Empty();
	m_ImageId.clear();
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
	long l_err = 0;
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
				SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
				if( l_psvTool )
				{
					SVObjectTypeInfoStruct l_svInfo;
					l_svInfo.ObjectType = SVAnalyzerObjectType;

					SVAnalyzerClass* l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
					if( l_psvAnalyzer )
					{
						CPoint l_point;
						l_point.x = m_mousePoint.x;
						l_point.y = m_mousePoint.y;

						TransformFromViewSpace( l_point );

						if( l_psvAnalyzer->IsPtOverResult( l_point ) )
						{
							l_psvAnalyzer->DisplayAnalyzerResult();
						}
					}
				}
			}
			break;
		}

		case ID_ADJUST_POSITION:
		{
			if( m_ImageId.empty() ||
				!TheSVObserverApp.OkToEdit() )
			{
				l_err = -1269;
				break;
			}

			SVIPDoc* l_psvIPDoc = GetIPDoc();
			SVToolClass* l_psvTool = nullptr;
			CPoint l_mousePoint;

			SVObjectInfoStruct l_svInfo;
			SVObjectTypeInfoStruct l_svTypeInfo;

			l_mousePoint = m_mousePoint;

			TransformFromViewSpace( l_mousePoint );
			SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
			SVDrawContext drawContext( nullptr, l_pImage );

			if( nullptr != l_psvIPDoc )
			{
				l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
			}

			m_isPicked = GetObjectAtPoint( l_mousePoint );	// stores object in m_psvObject
			if( !m_isPicked )
			{
				//--------- This means that the mouse was not over a moveable, sizeable, 
				//--------- object (which means we should not have been able to get here).
				l_err = -1269;
				break;
			}

			l_svInfo = m_psvObject->GetObjectInfo();
			l_svTypeInfo = l_svInfo.ObjectTypeInfo;

			//------ Warp tool hands back a SVPolarTransformObjectType. Sub type 1792.
			//------ Window tool, Luminance hands back a SVImageObjectType. Sub type 0.
			CString DlgName; //used for the Adjust Tool Size and Position dialog

			switch( l_svTypeInfo.ObjectType )
			{
				case SVPolarTransformObjectType:
				{
					switch( l_svTypeInfo.SubType )
					{
						case SVImagePolarTransformObjectType: // 1792
						{
							DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());

							SVAdjustToolSizePositionDlg dlg(DlgName, this, m_psvObject );
							dlg.DoModal();
							break;
						}

						default:
						{
							l_err = -1275;
							break;
						}
					}

					break;
				}

				case SVImageObjectType:
				{
					switch( l_svTypeInfo.SubType )
					{
						case SVNotSetSubObjectType:
						{
							if( SV_IS_KIND_OF( l_psvTool, SVLoadImageToolClass ) )
							{
								DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());
								SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
								dlg.DoModal();
							}
							else
							{
								SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
								dlg.DoModal();
							}

							break;
						}
						default:
						{
							l_err = -1265;
							break;
						}
					}
					break;
				}

				case SVAnalyzerObjectType:
				{
					l_err = -1283;
					break;
				}

				case SVToolObjectType:
				{
					DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());
					SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
					dlg.DoModal();
					break;
				}

				default:
				{
					l_err = -1276;
					break;
				}
			}

			m_isPicked = FALSE;

			break;
		}

		case ID_ZOOM_SMALLEST:
		{
			SetZoomIndex(EZOOM_SMALLEST);
			break;
		}

		case ID_ZOOM_SMALL:
		{
			SetZoomIndex(EZOOM_SMALL);
			break;
		}

		case ID_ZOOM_NORMAL:
		{
			SetZoomIndex(EZOOM_NORMAL);
			break;
		}

		case ID_ZOOM_LARGE:
		{
			SetZoomIndex(EZOOM_LARGE);
			break;
		}

		case ID_ZOOM_LARGEST:
		{
			SetZoomIndex(EZOOM_LARGEST);
			break;
		}

		case ID_ZOOM_IN:
		{
			SetZoomIndex(EZOOM_IN);
			break;
		}

		case ID_ZOOM_OUT:
		{
			SetZoomIndex(EZOOM_OUT);
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
			SetZoomIndex(EZOOM_NORMAL);
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
		SvStl::MessageMgrDisplayAndNotify NullImageWarning( SvStl::LogAndDisplay );
		NullImageWarning.setMessage(SVMSG_SVO_5018_NULLIMAGE, SvOi::Tid_DisplayedImageIsUnavailable, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_30000_NullImageOnSave);
		return;
	}

	m_ViewOrImageFilename.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);

	BOOL bResult = m_ViewOrImageFilename.SaveFile(); // Show Save File Dialog
	CString Filepath = m_ViewOrImageFilename.GetFullFileName();

	if(ViewOnly)
	{
		CRect SourceRect;
		CRect DestRect;

		auto Status = GetRectInfo( SourceRect, DestRect );

		Status = BlitToScaledSurface( SourceRect, DestRect, Filepath, showOverlays);
	}
	else // showOverlays is ignored for underlying images: there was no easy way to mark underlying images with overlays
	{
		pCurrentImage->Save(SVString(Filepath));
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
	CMenu* l_pPopup( nullptr );
	bool RunOrTestMode( false );

	RunOrTestMode = SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );

	//Get the current selected tool and check to see if it has extents.  if it does not then remove Adjust Size and Position menu option
	SVToolClass* CurrentTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID() ) );

	m_mousePoint.x = p_point.x;
	m_mousePoint.y = p_point.y;

	// Map mouse point to client
	ScreenToClient( &m_mousePoint );

	if( l_menu.LoadMenu( IDR_IMAGE_ADJUST ) )
	{
		if( l_pPopup = l_menu.GetSubMenu( 0 ) )
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
				BOOL l_resultFound = FALSE;
				SVAnalyzerClass* l_psvAnalyzer = nullptr;
				SVIPDoc *l_psvIPDoc = GetIPDoc();

				if( nullptr != l_psvIPDoc )
				{
					SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
					if( l_psvTool )
					{
						CPoint l_point;
						l_point.x = m_mousePoint.x;
						l_point.y = m_mousePoint.y;

						TransformFromViewSpace( l_point );

						if( GetObjectAtPoint( l_point ) )
						{
							l_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>( m_psvObject );

							if( nullptr == l_psvAnalyzer )
							{
								SVObjectTypeInfoStruct l_svInfo;
								l_svInfo.ObjectType = SVAnalyzerObjectType;

								l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
								if( l_psvAnalyzer )
								{
									l_resultFound = l_psvAnalyzer->IsPtOverResult( l_point );
								}
							}
						}
					}
				}

				if( !l_resultFound || RunOrTestMode )
				{
					l_pPopup->DeleteMenu( ID_ANALYZER_RESULT, MF_BYCOMMAND );
				}

				if( !l_psvAnalyzer || RunOrTestMode )
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
	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
	SVDrawContext l_svDrawContext( nullptr, l_pImage, m_ZoomHelper.GetZoom() );

	l_svDrawContext.InverseTransform( &p_point, &p_point, 1 );
}

void SVImageViewClass::SelectDisplayImage()
{
	SVDisplayImageSelect l_svDlg;
	
	l_svDlg.m_pDoc = GetIPDoc();

	if( nullptr != l_svDlg.m_pDoc )
	{
		SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
		l_svDlg.m_pCurrentImage = l_pImage;

		if( IDOK == l_svDlg.DoModal() )
		{
			if( l_svDlg.m_pCurrentImage )
			{
				AttachToImage( l_svDlg.m_pCurrentImage->GetUniqueObjectID() );
			}
			else
			{
				DetachFromImage();
			}
		}
	}
}

const SVGUID& SVImageViewClass::GetImageID() const
{
	return m_ImageId;
}

SVImageClass* SVImageViewClass::GetImage()
{
	return dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
}

SVImageClass* SVImageViewClass::GetImageByName( CString& p_imageName ) const
{
	SVImageClass* l_pImage = nullptr;
	SVIPDoc* l_pDocument = GetIPDoc();

	if( nullptr != l_pDocument )
	{
		l_pImage = l_pDocument->GetImageByName( p_imageName );
	}
	return l_pImage;
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
	if( m_ImageId.empty() || SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) )
	{
		ReleaseImageSurface();
	}
	else
	{
		HRESULT hr = UpdateSurface();

		if( S_OK == hr)
		{
			hr = DisplaySurface();
		}
		NotifyIPDocDisplayComplete();
	}
}

BOOL SVImageViewClass::Create( LPCTSTR p_className, LPCTSTR p_windowName, DWORD p_style, const RECT& p_rect, CWnd* p_pParentWnd, UINT p_NID, CCreateContext* p_pContext )
{
	LPCTSTR l_className = AfxRegisterWndClass( CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW, 0, m_hWindowBackgroundColor , 0 );

	BOOL l_bOk = CWnd::Create( l_className, _T( "Untitled Main Image" ), p_style, p_rect, p_pParentWnd, p_NID, p_pContext );

	if( l_bOk )
	{
		ReleaseImageSurface();
	}

	return l_bOk;
}

void SVImageViewClass::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void SVImageViewClass::OnUpdate( CView* p_pSender, LPARAM p_lHint, CObject* p_pHint )
{
	bool Update = TRUE;
	RootObject::getRootChildValue( SvOl::FqnEnvironmentImageUpdate, Update );
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

		CString l_text;
		l_text.Format( " X: %d, Y: %d ", l_point.x, l_point.y );
		TheSVObserverApp.SetStatusText( l_text );

		if( !( m_ImageId.empty() ) && nullptr != l_psvIPDoc )
		{
			SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );

			if( l_psvTool )
			{
				if( nullptr != l_psvTool->GetObjectAtPoint( l_point ) )
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
		
		CString l_text;
		l_text.Format( " X: %d, Y: %d ", l_point.x, l_point.y );
		TheSVObserverApp.SetStatusText( l_text );
		
		if( nullptr != l_psvIPDoc && !( m_ImageId.empty() ) )
		{
			SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
			if( l_psvTool )
			{
				// Try to call SetupDialog for first found Analyzer...
				SVObjectTypeInfoStruct l_svInfo;
				l_svInfo.ObjectType = SVAnalyzerObjectType;

				SVAnalyzerClass* l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
				if( l_psvAnalyzer )
				{
					// Set Display Flag so Errors will be displayed on dialog.
					l_psvAnalyzer->GetTool()->msvError.m_bDisplayError = true;
					l_psvAnalyzer->msvError.m_bDisplayError = true;

					SVSetupDialogManager::Instance().SetupDialog( l_psvAnalyzer->GetClassID(), l_psvAnalyzer->GetUniqueObjectID(), this );

					l_psvAnalyzer->msvError.m_bDisplayError = false;
					l_psvAnalyzer->GetTool()->msvError.m_bDisplayError = false;

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
		if( !( m_ImageId.empty() ) )
		{
			CString l_text;
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
					S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_tempPoint ) &&
					S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyWidth, l_width ) &&
					S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_height ) )
				{
					l_text.Format( " X: %d, Y: %d    cX: %d, cY: %d ", l_tempPoint.x, l_tempPoint.y, l_width, l_height );
				}
				else
				{
					l_text.Format( " Col: %d, Row: %d ", p_point.x, p_point.y );
				}

				m_lastMouseMovePoint = p_point;

				SetCapture();
			}
			else
			{
				l_text.Format( " Col: %d, Row: %d ", p_point.x, p_point.y );
			}

			TheSVObserverApp.SetStatusText( l_text );
		}

		CWnd::OnLButtonDown( p_nFlags, p_point );
	}
}

void SVImageViewClass::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if( !SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp.OkToEdit() )
	{
		CString l_text;

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
		l_text.Format( " Col: %d, Row: %d    RGB: %u/%u/%u ", l_point.x, l_point.y, redValue, greenValue, blueValue ); 

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

				if( ( SVExtentLocationPropertyRotate == m_svMousePickLocation ||
					m_svMousePickLocation == l_svExtents.GetLocationPropertyAt( l_startPoint ) ) &&
					S_OK == l_svTempExtents.Update( m_svMousePickLocation, l_startPoint, l_point ) )
				{
					bool l_bUpdate = false;

					l_bUpdate = S_OK == l_pIPDoc->UpdateExtents( m_psvObject, l_svTempExtents );

					if( l_bUpdate || l_rect.PtInRect( l_clientPoint ) )
					{
						l_bUpdate = l_bUpdate || S_OK == l_pIPDoc->UpdateExtentsToFit( m_psvObject, l_svTempExtents );
					}

					if( l_bUpdate )
					{
						long l_left = 0;
						long l_top = 0;
						long l_width = 0;
						long l_height = 0;

						l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, l_left );
						l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, l_top );
						l_svExtents.GetExtentProperty( SVExtentPropertyWidth, l_width );
						l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_height );

						// Status Text: Mouse Pos and Tool Extent
						l_text.Format( " Col: %d, Row: %d    X: %d, Y: %d    cX: %d, cY: %d ", 
							l_point.x, l_point.y, l_left, l_top, l_width, l_height );
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
		
		TheSVObserverApp.SetStatusText( l_text );

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

	m_svMousePickLocation = SVExtentLocationPropertyUnknown;

	ReleaseCapture();

	CString l_text;
	l_text.Format( " Col: %d, Row: %d ", p_point.x, p_point.y );
	TheSVObserverApp.SetStatusText( l_text );

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
	return m_ImageId.empty();
}

bool SVImageViewClass::CalculateZoomFit()
{
	bool ret = false;

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
			BOOL hasHorScroll, hasVertScroll;
			psvScroll->CheckScrollBars(hasHorScroll, hasVertScroll);
			if(hasHorScroll)
			{
				CRect rect;
				CScrollBar* pScrollbar = psvScroll->GetScrollBarCtrl( SB_HORZ );

				if( nullptr != pScrollbar )
				{
					pScrollbar->GetWindowRect(&rect);
					viewsize.cy -= rect.Height();
				}
			}

			if( hasVertScroll )
			{
				CRect rect;
				CScrollBar* pScrollbar = psvScroll->GetScrollBarCtrl( SB_VERT );
				if( nullptr != pScrollbar )
				{
					pScrollbar->GetWindowRect(&rect);
					viewsize.cx -= rect.Width(); 
				}
			}

		ret = m_ZoomHelper.CalculateZoomFit(ImageSize, viewsize);
		}
	}

	return ret;
}

double SVImageViewClass::SetZoom( EZoom zoom, double value, bool bSetZoomSlider )
{
	double res = 1.0;
	switch (zoom)
	{
	case EZoomFit:
		{
			bool suc = CalculateZoomFit();

			if(suc)
			{
				m_ZoomHelper.SetToFit();
			}
		}
		break;
	case EZoomOne:
		m_ZoomHelper.SetToOne();
		break;
	case EZoomMinus:
		m_ZoomHelper.ZoomMinus();
		break;
	case EZoomPlus:
		m_ZoomHelper.ZoomPlus();
		break;
	case EZoomValue:
		m_ZoomHelper.ExtendMinMax(value);
		m_ZoomHelper.SetZoom(value);
		break;
	default:
		return res;
		break;
	}

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

bool SVImageViewClass::SetZoomIndex( EZoomMode eZoom, unsigned  scaleIndex, bool bSetZoomSlider )
{
	if (EZOOM_VALUE == eZoom)
	{
		if( scaleIndex >= m_ZoomHelper.GetScaleCount() )
		{
			return false;
		}
	}

	bool bZoomChanged = m_ZoomHelper.ChangeScaleIndex(eZoom, scaleIndex);

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

const ZoomHelperEx& SVImageViewClass::GetZoomHelper() const
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

	SVGUID l_SelectedID;

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
			if( !( l_SelectedID.empty() ) && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
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
			if( !( l_SelectedID.empty() ) && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
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
		POINT titlePoint = p_rMultiLine.m_StringPoint;
		CString strTitle = p_rMultiLine.m_csString;

		SVGUID l_SelectedID;

		if( nullptr != GetIPDoc() )
		{
			l_SelectedID = GetIPDoc()->GetSelectedToolID();
		}

		// Draw tool title...
		SetTextColor( PDrawContext->DC, color );

		PDrawContext->Transform( &titlePoint, &titlePoint, 1 );

		::TextOut( PDrawContext->DC, titlePoint.x, titlePoint.y, strTitle, strTitle.GetLength() );

		int l_PenStyle = PS_SOLID;

		// Check for current selection...
		if( !( l_SelectedID.empty() ) && ( l_SelectedID == p_rMultiLine.m_ObjectID || l_SelectedID == p_rMultiLine.m_ToolID ) )
		{
			l_PenStyle = PS_DOT;
		}

		hPen = CreatePen( l_PenStyle, 1, color );

		hPenOld = SelectObject( PDrawContext->DC, hPen );

		SVDrawObjectListClass l_DrawClass;

		l_DrawClass.AddExtentMultiLineData( p_rMultiLine, l_PenStyle );
		if( p_rMultiLine.m_ObjectID == SVUserMaskOperatorClassGuid)
		{
			l_DrawClass.m_bDrawFigureHatched = true;
		}

		l_DrawClass.Draw( PDrawContext );

		SelectObject( PDrawContext->DC, hPenOld );
		DeleteObject( hPen );
	}
}

void SVImageViewClass::UpdateOverlays( HDC p_hDC, long p_X, long p_Y )
{
	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );

	if( nullptr != l_pImage )
	{
		// Get drawing device context...
		::SetBkMode( p_hDC, TRANSPARENT );

		// Select Font...
		HGDIOBJ hFontOld = ::SelectObject( p_hDC,m_ZoomHelper.GetFont() );

		// Use Drawing Context

		SVDrawContext drawContext( p_hDC, l_pImage, m_ZoomHelper.GetZoom() );
		drawContext.ViewPortOffset.x = -p_X;
		drawContext.ViewPortOffset.y = -p_Y;

		// Draw tool figures...
		for( SVExtentMultiLineStructCArray::iterator l_Iter = m_OverlayData.begin(); l_Iter != m_OverlayData.end(); ++l_Iter )
		{
			DrawOverlay( &drawContext, *l_Iter );
		}

		// Restore Font...
		::SelectObject( p_hDC, hFontOld );
	}
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

HICON SVImageViewClass::GetObjectCursor( SVExtentLocationPropertyEnum p_svLocation, POINT p_point )
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
		if( SVExtentTranslationProfile == l_svExtents.GetTranslation() )
		{
			if( S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyRotationAngle, l_dRotationAngle) )
			{
				l_dRotationAngle = fmod(l_dRotationAngle, 180.0);
			}
		}
		//Check that this is the Polar Unwrap tool with rotation
		else if( SVExtentTranslationPolarUnwrap == l_svExtents.GetTranslation() )
		{
			SVExtentPointStruct l_svCenter;
			SVExtentPointStruct l_svMouse(p_point);
			if( S_OK == l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_svCenter) )
			{
				//Polar unwrap needs an offset of 90° for the displayed cursor to be correct
				l_dRotationAngle = SVGetRotationAngle(l_svCenter, l_svMouse) + 90.0;
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
		case SVExtentLocationPropertyRotate:
		{
			l_cursorId = 0;
			m_hActionIcon = ::LoadImage(AfxGetResourceHandle(),
	                                   MAKEINTRESOURCE(IDI_ARROW_ROTATE),
	                                   IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);
			l_hCursor = m_hActionIcon;
			break;
		}

		case SVExtentLocationPropertyTopLeft:
		case SVExtentLocationPropertyBottomRight:
		{
			l_cursorId = l_cursorTopLeftBottomRight;

			break;
		}
		case SVExtentLocationPropertyTopRight:
		case SVExtentLocationPropertyBottomLeft:
		{
			l_cursorId = l_cursorTopRightBottomLeft;

			break;
		}
		case SVExtentLocationPropertyLeft:
		case SVExtentLocationPropertyRight:
		{
			l_cursorId = l_cursorLeftRight;

			break;
		}
		case SVExtentLocationPropertyTop:
		case SVExtentLocationPropertyBottom:
		{
			l_cursorId = l_cursorTopBottom;

			break;
		}
		case SVExtentLocationPropertyCenter:
		{
			l_cursorId = l_cursorMove;

			break;
		}
		default:
		{
			break;
		}
	}
	if( IDC_ARROW == l_cursorId && SVExtentLocationPropertyDisabled != p_svLocation )
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

	m_psvObject = nullptr;

	m_svLocation = SVExtentLocationPropertyUnknown;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
	SVToolClass* l_psvTool = nullptr;

	if( nullptr != GetIPDoc() )
	{
		l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID() ) );
	}

	if( nullptr != l_psvTool && S_OK == l_psvTool->IsInputImage( l_pImage ) )
	{
		SVImageExtentClass l_svExtents;

		m_psvObject = l_psvTool->GetObjectAtPoint( p_point );

		if( S_OK == GetToolExtents( l_svExtents ) )
		{
			m_svLocation = l_svExtents.GetLocationPropertyAt( p_point );
		}
	
		////check if move or sizing is allowed  
		if(false == l_psvTool->IsAllowedLocation(m_svLocation))
		{
				m_svLocation = SVExtentLocationPropertyDisabled;
		}
	}

	l_bOk = nullptr != m_psvObject;

	return l_bOk;
}

BOOL SVImageViewClass::GetParameters(SVObjectWriter& rWriter)
{
	BOOL l_bOk = TRUE;

	_variant_t l_svVariant;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );

	l_svVariant = ( nullptr != l_pImage );
	rWriter.WriteAttribute(CTAG_VIEW_INITIALIZED, l_svVariant);
	l_svVariant.Clear();

	double zoom = m_ZoomHelper.GetZoom();
	l_svVariant = zoom;
	rWriter.WriteAttribute(CTAG_IMAGE_ZOOM_FACTOR_EX, l_svVariant);
	l_svVariant.Clear();

	if( nullptr != l_pImage )
	{
		l_svVariant = l_pImage->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
		rWriter.WriteAttribute(CTAG_IMAGE_NAME, l_svVariant);
		l_svVariant.Clear();
	}

	return l_bOk;
}

BOOL SVImageViewClass::SetParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent )
{
	BOOL l_bOk = FALSE;
	BOOL bZoomExOK = FALSE;

	_variant_t l_svVariant;

	bool l_bUseImageView = false;

	l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_VIEW_INITIALIZED, p_parent, l_svVariant );
	if( l_bOk )
	{
		l_bUseImageView = l_svVariant;
	}

	if( l_bOk )
	{
		bZoomExOK = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, l_svVariant );
		if(bZoomExOK)
		{
			double dZoom = l_svVariant;
			if( !SetZoom(EZoomValue, dZoom, false) )
			{
				SetZoom( EZoomValue, 1.0, false );
			}
		}
	}

	if( l_bOk && !bZoomExOK )
	{
		l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR, p_parent, l_svVariant );
		if( l_bOk )
		{
			long l_index = 0;

			l_index = l_svVariant;

			if( l_index < 0 )
			{
				l_index++;
			}
			else if( 0 < l_index )
			{
				l_index--;
			}

			if( !SetZoomIndex(EZOOM_VALUE, m_ZoomHelper.GetScaleCount() / 2 + l_index, false) )
			{
				SetZoomIndex(EZOOM_VALUE, m_ZoomHelper.GetScaleCount() / 2, false );
			}
		}
	}

	if( l_bOk && l_bUseImageView )
	{
		l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_NAME, p_parent, l_svVariant );
		if( l_bOk )
		{
			_bstr_t l_String( l_svVariant );

			if( 0 < l_String.length() )
			{
				if( l_bOk )
				{
					AttachToImage( static_cast< LPCTSTR >( l_String ) );
				}
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

BOOL SVImageViewClass::CheckParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent )
{
	BOOL l_bOk = FALSE;

	_variant_t l_svVariant;

	bool l_bUseImageView = false;
	bool bZoomExOK = false;

	l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_VIEW_INITIALIZED, p_parent, l_svVariant );
	if( l_bOk )
	{
		l_bUseImageView = l_svVariant;
	}

	if( l_bOk )
	{
		bZoomExOK = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, l_svVariant );
		if(bZoomExOK)
		{
			double dZoom = l_svVariant;

			if( m_ZoomHelper.GetZoom() != dZoom )
			{
				if( !SetZoom(EZoomValue, dZoom, false) )
				{
					SetZoom( EZoomValue, 1.0, false );
				}
			}
		}
	}

	if( l_bOk && !bZoomExOK )
	{
		l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR, p_parent, l_svVariant );
		if( l_bOk )
		{
			long l_index = 0;

			l_index = l_svVariant;

			if( l_index < 0 )
			{
				l_index++;
			}
			else if( 0 < l_index )
			{
				l_index--;
			}

			if( m_ZoomHelper.GetScaleIndex() != m_ZoomHelper.GetScaleCount() / 2 + l_index, false )
			{
				if( !SetZoomIndex( EZOOM_VALUE, m_ZoomHelper.GetScaleCount() / 2 + l_index, false ) )
				{
					SetZoomIndex( EZOOM_VALUE, m_ZoomHelper.GetScaleCount() / 2, false );
				}
			}
		}
	}

	if( l_bOk && l_bUseImageView )
	{
		l_bOk = SVNavigateTree::GetItem( p_tree, CTAG_IMAGE_NAME, p_parent, l_svVariant );
		if( l_bOk )
		{
			_bstr_t l_String( l_svVariant );

			if( 0 < l_String.length() )
			{
				if( l_bOk )
				{
					if( m_imageName != static_cast< LPCTSTR >( l_String ) )
					{
						AttachToImage( static_cast< LPCTSTR >( l_String ) );
					}
				}
			}
			else
			{
				l_bOk = FALSE;
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
	else if( !( m_ImageId.empty() ) )
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
		return &( m_ImageDIB[ l_Info.GetBitmapInfoSizeInBytes() ] );
	}

	return nullptr;
}

HRESULT SVImageViewClass::UpdateImageSurfaces( const SVBitmapInfo& p_rBitmapInfo )
{
	HRESULT l_Status = S_OK;

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

			l_Status = SVDirectX::CreateSurface( p_rBitmapInfo, &m_pDDImageSurface );
			
			if( DD_OK == l_Status )
			{
				SIZE l_Size;

				SVDirectX::GetPrimarySurfaceResolution( l_Size );

				l_Status = SVDirectX::CreateSurface( l_Size.cx, l_Size.cy, &m_pDDScaledImageSurface );
			}

			CRect l_rect;

			GetImageRect( l_rect );
			SetImageRect( l_rect );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( DD_OK != l_Status )
	{
		ReleaseImageSurface();

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE( _T( "SVImageView CreateImageSurface was unable to create the image surface.\n" ) );
#endif
	}

	return l_Status;
}

HRESULT SVImageViewClass::CopyBitsToSurface( const CRect& p_rSourceRect, const SVBitmapInfo& p_rBitmapInfo, const unsigned char* p_pBitmapBits )
{
	HRESULT l_Status = S_OK;

	if( !( p_rBitmapInfo.empty() ) && ( nullptr != p_pBitmapBits ) && ( nullptr != m_pDDImageSurface ) )
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );
		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );

		l_Status = m_pDDImageSurface->Lock( nullptr, &l_ddSurfaceDesc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr );
		if( DD_OK == l_Status )
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
				l_Status = E_INVALIDARG;
			}

			// preserve the error set if the image widths/pitch are different 
			// (meaning that we didn't copy the image data)
			HRESULT l_Temp = m_pDDImageSurface->Unlock( nullptr );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

void SVImageViewClass::OnZoomPlus()
{
	SetZoom( EZoomPlus );
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
		bool bZoom = IsZoomAllowed();
		pFrame->EnableZoomToolbar(bZoom);

		if( true == bZoom )
		{
			pFrame->SetZoomToolbar(GetZoomHelper());
		}
	}
}

void SVImageViewClass::OnUpdateZoomPlus(CCmdUI *pCmdUI)
{
	bool enable  = IsZoomAllowed();
	pCmdUI->Enable(enable);
}

void SVImageViewClass::OnZoomMinus()
{
	SetZoom( EZoomMinus);
}

void SVImageViewClass::OnUpdateZoomMinus(CCmdUI *pCmdUI)
{
	bool enable  = IsZoomAllowed();
	pCmdUI->Enable(enable);
}

void SVImageViewClass::OnZoomOne()
{
	SetZoom( EZoomOne);
}

void SVImageViewClass::OnUpdateZoomOne(CCmdUI *pCmdUI)
{
	bool enable  = IsZoomAllowed();
	pCmdUI->Enable(enable);
}

void SVImageViewClass::OnZoomFit()
{
	SetZoom( EZoomFit);
}

void SVImageViewClass::OnUpdateZoomFit(CCmdUI *pCmdUI)
{
	bool enable  = IsZoomAllowed();
	pCmdUI->Enable(enable);
}

void SVImageViewClass::OnZoomSliderMoved()
{
	SVMainFrame* pFrame =  dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	double val = 1.0;

	if(pFrame)
	{
		val = pFrame->GetZoomToolbarValue();
	}

	SetZoom( EZoomValue, val );
}

HRESULT SVImageViewClass::BlitToScaledSurface( CRect& p_rSourceRect, CRect& p_rDestRect, CString Filepath, bool showOverlays)
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDDImageSurface && nullptr != m_pDDScaledImageSurface )
	{
		HDC scaledDC = nullptr;

		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

		l_Status = m_pDDScaledImageSurface->GetDC( &scaledDC );

		if( DD_OK == l_Status )
		{
			HDC surfaceDC = nullptr;

			l_Status = m_pDDImageSurface->GetDC( &surfaceDC );

			if( DD_OK == l_Status )
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

			if (Filepath != "")
			{
				CreateBmpFileFromHdc(Filepath,scaledDC,l_ScaledRect);
			}

			m_pDDScaledImageSurface->ReleaseDC( scaledDC );
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		if( DD_OK != l_Status)
		{
			TRACE(_T( "SVImageViewClass::UpdateImage - Surface Blit failed %08lx\n" ), l_Status );
		}
#endif
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageViewClass::BlitToPrimarySurface( CRect& p_rDestRect )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDDScaledImageSurface )
	{
		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

		SVDirectX::Instance().SetClipperHWnd( 0, m_hWnd );

		// @Note: the following loop can result in an infinite cycle
		do
		{
			l_Status = SVDirectX::Instance().GetBltStatus( DDGBS_CANBLT );

			if( S_OK != l_Status )
			{
				Sleep( 0 );
			}
		}
		while( S_OK != l_Status && E_FAIL != l_Status );

		if( S_OK == l_Status )
		{
			l_Status = SVDirectX::Instance().Blt(
					&p_rDestRect,            // destination rectangle
					m_pDDScaledImageSurface, // source surface
					&l_ScaledRect,           // source rectangle
					DDBLT_WAIT,              // DDBLT_DONOTWAIT| DDBLT_ASYNC, // Flag <<<<<<<<<<<<<<<<<<<<<<
					nullptr );                  // Pointer to DDBLTFX Structure
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageViewClass::RecreateLostSurface()
{
	HRESULT l_Status = S_OK;

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
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageViewClass::GetRectInfo( CRect& p_rSourceRect, CRect& p_rDestRect )
{
	HRESULT l_Status = S_OK;

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
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageViewClass::UpdateBufferFromIPDoc()
{
	HRESULT l_Status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		if( S_FALSE == l_pIPDoc->IsImageDataUpdated( m_ImageId, this ) )
		{
			l_Status = l_pIPDoc->GetImageData( m_ImageId, m_ImageDIB, m_OverlayData );

			if( S_OK == l_Status )
			{
				l_Status = l_pIPDoc->MarkImageDataUpdated( m_ImageId, this );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// DisplayImage copies an image to the DirectDraw image surface.
HRESULT SVImageViewClass::UpdateSurface()
{
	HRESULT l_Status = S_OK;

	if( !( SVSVIMStateClass::CheckState( SV_STATE_CLOSING | SV_STATE_CANCELING ) ) )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();
		const unsigned char* l_pBitmapBits = GetBitmapBits();
		CWnd* l_pParent = GetParent(); // The parent of ImageView window is ImageViewScroll.

		if( !( l_BitmapInfo.empty() ) && ( nullptr != l_pBitmapBits ) && ( nullptr != l_pParent ) )
		{
			l_Status = UpdateImageSurfaces( l_BitmapInfo );

			if( S_OK == l_Status )
			{
				CRect l_SourceRect;
				CRect l_DestRect;

				l_Status = GetRectInfo( l_SourceRect, l_DestRect );

				if( S_OK == l_Status )
				{
					l_Status = CopyBitsToSurface( l_SourceRect, l_BitmapInfo, l_pBitmapBits );
				}
			}

			// check for lost surfaces
			if( DDERR_SURFACELOST == l_Status )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );
#endif
				RecreateLostSurface();
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// DisplayImage copies an image to the DirectDraw image surface.
HRESULT SVImageViewClass::DisplaySurface()
{
	HRESULT l_Status = S_OK;

	if( !( SVSVIMStateClass::CheckState( SV_STATE_CLOSING | SV_STATE_CANCELING ) ) )
	{
		SVBitmapInfo l_BitmapInfo = GetBitmapInfo();
		CWnd* l_pParent = GetParent(); // The parent of ImageView window is ImageViewScroll.

		if( !( l_BitmapInfo.empty() ) && ( nullptr != l_pParent ) )
		{
			CRect l_SourceRect;
			CRect l_DestRect;

			l_Status = GetRectInfo( l_SourceRect, l_DestRect );

			if( S_OK == l_Status )
			{
				l_Status = BlitToScaledSurface( l_SourceRect, l_DestRect );
			}

			if( S_OK == l_Status )
			{
				l_Status = BlitToPrimarySurface( l_DestRect );
			}

			// check for lost surfaces
			if( DDERR_SURFACELOST == l_Status )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );
#endif

				RecreateLostSurface();
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageViewClass::NotifyIPDocDisplayComplete()
{
	HRESULT l_Status = S_OK;
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_Status = l_pIPDoc->MarkImageDataDisplayed( m_ImageId, this );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVImageViewClass::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	SVMainFrame* pFrame  = nullptr;
	pFrame =  dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	if(pFrame==nullptr)
	{
		return;
	}

	SVIPSplitterFrame* pSplitterFrame = nullptr;
	pSplitterFrame =  dynamic_cast< SVIPSplitterFrame*> (pFrame->GetActiveFrame());

	if( nullptr != pSplitterFrame )
	{
		pSplitterFrame->RefreshAllSplitters();
	}

	bool bZoom = IsZoomAllowed();
	pFrame->EnableZoomToolbar(bZoom);

	if( bZoom )
	{
		pFrame->SetZoomToolbar(GetZoomHelper());
	}
}

void SVImageViewClass::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	SVIPSplitterFrame* pSplitterFrame = nullptr;
	CMDIFrameWnd* pMDIFrame =  nullptr;
	pMDIFrame =  dynamic_cast< CMDIFrameWnd*>(AfxGetApp()->m_pMainWnd);
	if( nullptr != pMDIFrame )
	{
		pSplitterFrame =  dynamic_cast< SVIPSplitterFrame*> (pMDIFrame->GetActiveFrame());
	}
	if( nullptr != pSplitterFrame )
	{
		pSplitterFrame->RefreshAllSplitters();
	}
}
