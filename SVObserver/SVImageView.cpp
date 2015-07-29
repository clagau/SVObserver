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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVSVIMStateClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVAdjustToolSizePositionDlg.h"
#include "SVAnalyzer.h"
#include "SVDirectX.h"
#include "SVDisplayImageSelect.h"
#include "SVDrawObject.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVImageViewScroll.h"
#include "SVIPDoc.h"
#include "SVLineAnalyzer.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
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
		if (hr == S_OK)
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

			if( ( l_Value | ( ~p_Mask ) ) == 0xFFFFFFFF )
			{
				*l_pDstPixel = ( l_Value & ( ~p_Mask ) ) | p_UpperValue;
			}
			else if( ( l_Value & p_Mask ) == 0x00000000 )
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
, m_hActionIcon(NULL)
{
	Initialize();
}

void SVImageViewClass::Initialize()
{
	EnableAutomation();

	m_showExtremeLUT = false;

	m_psvObject = NULL;

	m_svLocation = SVExtentLocationPropertyUnknown;

	m_svMousePickLocation = SVExtentLocationPropertyUnknown;

	m_mouseIsOverTool = FALSE;

	m_pDDImageSurface = NULL;
	m_pDDScaledImageSurface = NULL;

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

	if( (HICON) NULL != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon );
		m_hActionIcon = (HICON) NULL;
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

	if( m_pDDImageSurface != NULL )
	{
		m_pDDImageSurface->Release();

		m_pDDImageSurface = NULL;
	}

	if( m_pDDScaledImageSurface != NULL )
	{
		m_pDDScaledImageSurface->Release();

		m_pDDScaledImageSurface = NULL;
	}
}

HRESULT SVImageViewClass::UpdateToolExtents( SVExtentLocationPropertyEnum p_svLocation, double p_x, double p_y )
{
	HRESULT l_hrOk = S_FALSE;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		SVImageExtentClass l_svExtents;

		l_hrOk = GetToolExtents( l_svExtents );

		if( l_hrOk == S_OK )
		{
			l_hrOk = l_svExtents.UpdateFromOutputSpace( p_svLocation, static_cast< long >( p_x ), static_cast< long >( p_y ) );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = UpdateToolExtents( l_svExtents );
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVImageViewClass::UpdateToolExtents( const SVImageExtentClass& p_rExtents )
{
	HRESULT l_hrOk = S_FALSE;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		l_hrOk = l_pIPDoc->UpdateExtents( m_psvObject, p_rExtents );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVImageViewClass::UpdateToolExtents( SVExtentLocationPropertyEnum p_svLocation, double p_angle )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_svLocation == SVExtentLocationPropertyRotate )
	{
		SVImageExtentClass l_svExtents;

		l_hrOk = GetToolExtents( l_svExtents );

		if( l_hrOk == S_OK )
		{
			l_hrOk = l_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, p_angle );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = UpdateToolExtents( l_svExtents );
		}
	}

	return l_hrOk;
}

HRESULT SVImageViewClass::UpdateToolExtentsToFit( const SVImageExtentClass& p_rExtents )
{
	HRESULT l_hrOk = S_FALSE;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		l_hrOk = l_pIPDoc->UpdateExtentsToFit( m_psvObject, p_rExtents );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVImageViewClass::GetToolExtents( SVImageExtentClass& p_svToolExtents )
{
	HRESULT l_hrOk = S_OK;

	if( m_psvObject != NULL )
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

		if( l_pImage != NULL )
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
		SVImageClass* l_psvImage = NULL;

		l_psvImage = GetImageByName( l_imageName );

		if( l_psvImage != NULL )
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

	if( l_pIPDoc != NULL )
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

	if( l_pIPDoc != NULL && l_pIPDoc->m_bAllowRefresh )
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
			SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID() ) );
			if( l_psvTool )
			{
				const SVObjectTypeInfoStruct& rToolType = l_psvTool->GetObjectInfo().ObjectTypeInfo;

				if ( (rToolType.SubType == SVGageToolObjectType) || (rToolType.SubType == SVToolProfileObjectType) )
				{
					AfxMessageBox("This tool is now obsolete and the analyzer can not be edited.");
					break;
				}
			}

			if( TheSVObserverApp.OkToEdit() )
			{
				OnRButtonDblClk( 0, m_mousePoint );
			}
			break;
		}
		case ID_ANALYZER_RESULT:
		{
			SVIPDoc* l_psvIPDoc = GetIPDoc();

			if( l_psvIPDoc != NULL )
			{
				SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
				if( l_psvTool )
				{
					SVObjectTypeInfoStruct l_svInfo;
					l_svInfo.ObjectType = SVAnalyzerObjectType;

					SVAnalyzerClass* l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
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
			SVToolClass* l_psvTool = NULL;
			CPoint l_mousePoint;

			SVObjectInfoStruct l_svInfo;
			SVObjectTypeInfoStruct l_svTypeInfo;

			l_mousePoint = m_mousePoint;

			TransformFromViewSpace( l_mousePoint );
			SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
			SVDrawContext drawContext( NULL, l_pImage );

			if( l_psvIPDoc != NULL )
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
			//------ Gage tool (edge counting analyzer) hands back a SVROIObjectType. Sub type 1280.
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
					if( SV_IS_KIND_OF( m_psvObject, SVLineAnalyzerClass ) )
					{
						DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());
						SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
						dlg.DoModal();
					}
					else
					{
						l_err = -1283;
					}
					break;
				}

				case SVToolObjectType:
				{
					switch( l_svTypeInfo.SubType )
					{
						case SVToolProfileObjectType:
						{
							DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());
							SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
							dlg.DoModal();
							break;
						}
						default:
						{
							DlgName.Format("Adjust Tool Size and Position - %s",l_psvTool->GetName());
							SVAdjustToolSizePositionDlg dlg( DlgName, this, m_psvObject );
							dlg.DoModal();
							break;
						}
					}
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

		default:
		{
			SetZoomIndex(EZOOM_NORMAL);
			break;
		}
	}// end switch (wParam)

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != nullptr )
	{
		l_pIPDoc->UpdateWithLastProduct();
	}

	return CView::OnCommand( p_wParam, p_lParam );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnContextMenu
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :dd.mm.yyyy 			First Implementation
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

			if( !m_mouseIsOverTool || RunOrTestMode )
			{
				l_pPopup->DeleteMenu( ID_CONFIG_ANALYZER, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_ADJUST_POSITION, MF_BYCOMMAND );
				l_pPopup->DeleteMenu( ID_ANALYZER_RESULT, MF_BYCOMMAND );
			}
			else
			{
				BOOL l_resultFound = FALSE;
				SVAnalyzerClass* l_psvAnalyzer = NULL;
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

								l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
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

void SVImageViewClass::TransformToViewSpace( CPoint& p_point )
{
	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
	SVDrawContext l_svDrawContext( NULL, l_pImage, m_ZoomHelper.GetZoom() );

	l_svDrawContext.Transform( &p_point, &p_point, 1 );
}

void SVImageViewClass::TransformFromViewSpace( CPoint& p_point )
{
	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
	SVDrawContext l_svDrawContext( NULL, l_pImage, m_ZoomHelper.GetZoom() );

	l_svDrawContext.InverseTransform( &p_point, &p_point, 1 );
}

void SVImageViewClass::SelectDisplayImage()
{
	SVDisplayImageSelect l_svDlg;
	
	l_svDlg.m_pDoc = GetIPDoc();

	if( l_svDlg.m_pDoc != NULL )
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
	SVImageClass* l_pImage = NULL;
	SVIPDoc* l_pDocument = GetIPDoc();

	if( l_pDocument != NULL )
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

		if (hr != DD_OK)
		{
			TRACE( _T( "SVImageViewClass::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
		}
		if (hr == DD_OK)
		{
			SIZE l_Size;

			SVDirectX::GetPrimarySurfaceResolution( l_Size );

			hr = SVDirectX::CreateSurface(l_Size.cx, l_Size.cy, &m_pDDScaledImageSurface);

			if (hr != DD_OK)
			{
				TRACE( _T( "SVImageViewClass::RecreateImageSurface - CreateSurface failed (%08lx)\n" ), hr );
			}
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
		//TRACE( _T( "SVImageView::OnDraw %s\n" ), m_imageName );

		HRESULT hr = S_OK;

		hr = UpdateSurface();

		if( hr == S_OK )
		{
			//::Sleep( 0 );
			//m_ThreadWait.Sleep();

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
	//TRACE( _T( "SVImageView::OnUpdate %s\n" ), m_imageName );

	bool Update = TRUE;
	RootObject::getRootChildValue( ::EnvironmentImageUpdate, Update );
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

	if( GetUpdateRect( NULL ) == 0 )
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

		if( !( m_ImageId.empty() ) && l_psvIPDoc != NULL )
		{
			SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );

			if( l_psvTool )
			{
				if( l_psvTool->GetObjectAtPoint( l_point ) != NULL )
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
		
		if( l_psvIPDoc != NULL && !( m_ImageId.empty() ) )
		{
			SVToolClass* l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( l_psvIPDoc->GetSelectedToolID() ) );
			if( l_psvTool )
			{
				SVTaskObjectClass* l_pObject = l_psvTool->GetObjectAtPoint( l_point );
				
				// Checking for LineROI...
				if( SV_IS_KIND_OF( l_pObject, SVLineAnalyzerClass ) )
				{
					SVSetupDialogManager::Instance().SetupDialog( l_pObject->GetClassID(), l_pObject->GetUniqueObjectID(), this );
				}
				else
				{
					// Try to call SetupDialog for first found Analyzer...
					SVObjectTypeInfoStruct l_svInfo;
					l_svInfo.ObjectType = SVAnalyzerObjectType;

					SVAnalyzerClass* l_psvAnalyzer = reinterpret_cast<SVAnalyzerClass*>(SVSendMessage( l_psvTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&l_svInfo)) );
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

				if( GetToolExtents( l_svExtents ) == S_OK &&
					l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_tempPoint ) == S_OK &&
					l_svExtents.GetExtentProperty( SVExtentPropertyWidth, l_width ) == S_OK &&
					l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_height ) == S_OK )
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

		/*TRACE( "CLIENT -- RECT L=%d-T=%d-R=%d-B=%d -- POINT X=%d-Y=%d\n",
			l_rect.left, l_rect.top, l_rect.right, l_rect.bottom,
			l_clientPoint.x, l_clientPoint.y );

		TRACE( "SCREEN -- RECT L=%d-T=%d-R=%d-B=%d -- POINT X=%d-Y=%d\n",
			l_screenRect.left, l_screenRect.top, l_screenRect.right, l_screenRect.bottom,
		l_screenPoint.x, l_screenPoint.y );*/

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

		HICON l_hCursor = NULL;

		SVImageExtentClass l_svExtents;

		SVIPDoc* l_pIPDoc = GetIPDoc();

		if (m_isPicked)
		{
			if( l_pIPDoc != NULL && m_isPicked && GetToolExtents( l_svExtents ) == S_OK )
			{
				CPoint l_startPoint = m_lastMouseMovePoint;

				SVImageExtentClass l_svTempExtents = l_svExtents;

				TransformFromViewSpace( l_startPoint );

				l_hCursor = GetObjectCursor( m_svMousePickLocation, l_point );

				if( ( m_svMousePickLocation == SVExtentLocationPropertyRotate ||
					m_svMousePickLocation == l_svExtents.GetLocationPropertyAt( l_startPoint ) ) &&
					l_svTempExtents.Update( m_svMousePickLocation, l_startPoint, l_point ) == S_OK )
				{
					bool l_bUpdate = false;

					l_bUpdate = l_pIPDoc->UpdateExtents( m_psvObject, l_svTempExtents ) == S_OK;

					if( l_bUpdate || l_rect.PtInRect( l_clientPoint ) )
					{
						l_bUpdate = l_bUpdate || l_pIPDoc->UpdateExtentsToFit( m_psvObject, l_svTempExtents ) == S_OK;
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
		if(NULL == l_hCursor)
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
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//  :24.05.1998 RO			Supplements
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
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//  :24.05.1998 RO			Supplements
////////////////////////////////////////////////////////////////////////////////
void SVImageViewClass::OnCaptureChanged( CWnd* p_pWnd )
{
	TheSVObserverApp.SetStatusText( NULL );

	CWnd::OnCaptureChanged( p_pWnd );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnNcMouseMove
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//  :24.05.1998 RO			Supplements
////////////////////////////////////////////////////////////////////////////////
void SVImageViewClass::OnNcMouseMove( UINT p_hitTest, CPoint p_point )
{
	TheSVObserverApp.SetStatusText( NULL );

	CWnd::OnNcMouseMove( p_hitTest, p_point );
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : OnDestroy
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1998 RO			First Implementation
//	:16.02.2000 RO			Added Freeze Panel Support.
//	:20.02.2000 RO			Revised to use CloseImageWindow().
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

	if( GetSafeHwnd() != NULL )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = l_psvScroll != NULL;

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

	if( GetSafeHwnd() != NULL )
	{
		SVImageViewScroll *l_psvScroll = dynamic_cast< SVImageViewScroll * >( GetParent() );

		l_bOk = l_psvScroll != NULL;

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

	if( pIPDoc != NULL && pIPDoc->m_bAllowRefresh && GetSafeHwnd() != NULL )
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

				if(pScrollbar != nullptr)
				{
					pScrollbar->GetWindowRect(&rect);
					viewsize.cy -= rect.Height();
				}
			}

			if(hasVertScroll)
			{
				CRect rect;
				CScrollBar* pScrollbar = psvScroll->GetScrollBarCtrl( SB_VERT );
				if(pScrollbar != nullptr)
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
	if(eZoom == EZOOM_VALUE)
	{
		if( scaleIndex >= m_ZoomHelper.GetScaleCount() )
		{
			return false;
		}
	}

	bool bZoomChanged = m_ZoomHelper.ChangeScaleIndex(eZoom, scaleIndex);

	if(bZoomChanged)
	{
		CRect l_rect;
		GetImageRect( l_rect );
		SetImageRect( l_rect );
		SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
		if(pFrame && bSetZoomSlider)
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

	if( GetSafeHwnd() != NULL )
	{
		SVImageViewScroll* l_psvScroll = dynamic_cast< SVImageViewScroll* >( GetParent() );

		l_bOk = l_psvScroll != NULL;

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

	if( GetIPDoc() != NULL )
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
	BOOL bDrawAllowed = ( ( PDrawContext != NULL ) && ( ShouldDraw( p_rMultiLine ) == S_OK ) );

	if( bDrawAllowed )
	{
		HGDIOBJ hPen;
		HGDIOBJ hPenOld;

		COLORREF color = p_rMultiLine.m_Color;
		POINT titlePoint = p_rMultiLine.m_StringPoint;
		CString strTitle = p_rMultiLine.m_csString;

		SVGUID l_SelectedID;

		if( GetIPDoc() != NULL )
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

	if( l_pImage != NULL )
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
	HICON l_hCursor = NULL;

	if( GetObjectAtPoint( p_point ) )
	{
		l_hCursor = GetObjectCursor(m_svLocation, p_point);
	}

	return l_hCursor;
}

HICON SVImageViewClass::GetObjectCursor( SVExtentLocationPropertyEnum p_svLocation, POINT p_point )
{
	HICON l_hCursor = NULL;

	LPCTSTR l_cursorId = IDC_ARROW;
	LPCTSTR l_cursorLeftRight = IDC_SIZEWE;
	LPCTSTR l_cursorTopBottom = IDC_SIZENS;
	LPCTSTR l_cursorTopLeftBottomRight = IDC_SIZENWSE;
	LPCTSTR l_cursorTopRightBottomLeft = IDC_SIZENESW;
	LPCTSTR l_cursorMove = IDC_SIZEALL;

	//If action icon is loaded then need to clean it up before next icon
	if( (HICON) NULL != m_hActionIcon)
	{
		::DestroyIcon( m_hActionIcon  );
		m_hActionIcon = (HICON) NULL;
	}

	SVImageExtentClass l_svExtents;
	//Check the rotation angle to determine icon symbols
	if( GetToolExtents( l_svExtents ) == S_OK )
	{
		double l_dRotationAngle = 0.0;
		//Check that this is the Linear tool with rotation
		if( l_svExtents.GetTranslation() == SVExtentTranslationProfile )
		{
			if( l_svExtents.GetExtentProperty( SVExtentPropertyRotationAngle, l_dRotationAngle) == S_OK)
			{
				l_dRotationAngle = fmod(l_dRotationAngle, 180.0);
			}
		}
		//Check that this is the Polar Unwrap tool with rotation
		else if( l_svExtents.GetTranslation() == SVExtentTranslationPolarUnwrap )
		{
			SVExtentPointStruct l_svCenter;
			SVExtentPointStruct l_svMouse(p_point);
			if( l_svExtents.GetExtentProperty( SVExtentPropertyPositionPoint, l_svCenter) == S_OK)
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
	if( l_cursorId == IDC_ARROW && p_svLocation != SVExtentLocationPropertyDisabled)
	{
		m_mouseIsOverTool = FALSE;
	}
	else
	{
		m_mouseIsOverTool = TRUE;
	}
	//If no special resource icon call standard windows icon
	if( NULL == l_hCursor)
	{
		l_hCursor = AfxGetApp()->LoadStandardCursor( l_cursorId );
	}
	
	return l_hCursor;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getObjectAtPoint
// -----------------------------------------------------------------------------
// .Description : Get Object under Mouse point
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :22.02.2000 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVImageViewClass::GetObjectAtPoint( POINT p_point )
{
	BOOL l_bOk = FALSE;

	m_psvObject = NULL;

	m_svLocation = SVExtentLocationPropertyUnknown;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );
	SVToolClass* l_psvTool = NULL;

	if( GetIPDoc() != NULL )
	{
		l_psvTool = dynamic_cast< SVToolClass* >( SVObjectManagerClass::Instance().GetObject( GetIPDoc()->GetSelectedToolID() ) );
	}

	//if( l_psvTool != NULL && l_psvTool->ShouldDraw() == S_OK && l_psvTool->IsInputImage( l_pImage ) == S_OK )
	if( l_psvTool != NULL && l_psvTool->IsInputImage( l_pImage ) == S_OK )
	{
		SVImageExtentClass l_svExtents;

		m_psvObject = l_psvTool->GetObjectAtPoint( p_point );

		if( GetToolExtents( l_svExtents ) == S_OK )
		{
			m_svLocation = l_svExtents.GetLocationPropertyAt( p_point );
		}
	
		////check if move or sizing is allowed  
		if(false == l_psvTool->IsAllowedLocation(m_svLocation))
		{
				m_svLocation = SVExtentLocationPropertyDisabled;
		}
		
		
	}

	l_bOk = m_psvObject != NULL;

	return l_bOk;
}

BOOL SVImageViewClass::GetParameters(SVObjectWriter& rWriter)
{
	BOOL l_bOk = TRUE;

	_variant_t l_svVariant;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ImageId.ToGUID() ) );

	l_svVariant = ( l_pImage != NULL );
	rWriter.WriteAttribute(CTAG_VIEW_INITIALIZED, l_svVariant);
	l_svVariant.Clear();

	double zoom = m_ZoomHelper.GetZoom();
	l_svVariant = zoom;
	rWriter.WriteAttribute(CTAG_IMAGE_ZOOM_FACTOR_EX, l_svVariant);
	l_svVariant.Clear();

	if( l_pImage != NULL )
	{
		l_svVariant = l_pImage->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
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

	l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_VIEW_INITIALIZED, p_parent, l_svVariant );
	if( l_bOk )
	{
		l_bUseImageView = l_svVariant;
	}

	if( l_bOk )
	{
		bZoomExOK = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, l_svVariant );
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
		l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR, p_parent, l_svVariant );
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
		l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_NAME, p_parent, l_svVariant );
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

	l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_VIEW_INITIALIZED, p_parent, l_svVariant );
	if( l_bOk )
	{
		l_bUseImageView = l_svVariant;
	}

	if( l_bOk )
	{
		bZoomExOK = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR_EX, p_parent, l_svVariant );
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
		l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_ZOOM_FACTOR, p_parent, l_svVariant );
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
		l_bOk = SVNavigateTreeClass::GetItem( p_tree, CTAG_IMAGE_NAME, p_parent, l_svVariant );
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

	//l_bOk = CView::OnEraseBkgnd( p_pDC );  //causes flicker and will get painted over by image

	return l_bOk;
}

SVTaskObjectClass* SVImageViewClass::GetSelectedObject()
{
	return m_psvObject;
}

SVBitmapInfo SVImageViewClass::GetBitmapInfo() const
{
	SVBitmapInfo l_Info;

	if( !( m_ImageDIB.empty() ) )
	{
		const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( &( m_ImageDIB[ 0 ] ) );

		if( l_pBitmapInfo != NULL )
		{
			l_Info.Assign( *l_pBitmapInfo );
		}
	}
	else if( !( m_ImageId.empty() ) )
	{
		SVIPDoc* l_pIPDoc = GetIPDoc();

		if( l_pIPDoc != NULL )
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

	return NULL;
}

HRESULT SVImageViewClass::UpdateImageSurfaces( const SVBitmapInfo& p_rBitmapInfo )
{
	HRESULT l_Status = S_OK;

	if( !( SVDirectX::Instance().empty() ) && !( p_rBitmapInfo.empty() ) )
	{
		if( m_sourceImageWidth != p_rBitmapInfo.GetWidth() ||
			m_sourceImageHeight != ::labs( p_rBitmapInfo.GetHeight() ) ||
			m_sourceBitCount != p_rBitmapInfo.GetBitCount() ||
			m_pDDImageSurface == NULL || m_pDDScaledImageSurface == NULL )
		{
			m_sourceImageWidth = p_rBitmapInfo.GetWidth();
			m_sourceImageHeight = ::labs( p_rBitmapInfo.GetHeight() );
			m_sourceBitCount = p_rBitmapInfo.GetBitCount();

			l_Status = SVDirectX::CreateSurface( p_rBitmapInfo, &m_pDDImageSurface );
			
			if( l_Status == DD_OK )
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

	if( l_Status != DD_OK )
	{
		ReleaseImageSurface();

		TRACE( _T( "SVImageView CreateImageSurface was unable to create the image surface.\n" ) );
	}

	return l_Status;
}

HRESULT SVImageViewClass::CopyBitsToSurface( const CRect& p_rSourceRect, const SVBitmapInfo& p_rBitmapInfo, const unsigned char* p_pBitmapBits )
{
	HRESULT l_Status = S_OK;

	if( !( p_rBitmapInfo.empty() ) && ( p_pBitmapBits != NULL ) && ( m_pDDImageSurface != NULL ) )
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );
		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );

		l_Status = m_pDDImageSurface->Lock( NULL, &l_ddSurfaceDesc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL );
		if( l_Status == DD_OK )
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

					//Sleep( 0 );
					//m_ThreadWait.Sleep();
				}
			}
			else
			{
				l_Status = E_INVALIDARG;
			}

			// preserve the error set if the image widths/pitch are different 
			// (meaning that we didn't copy the image data)
			HRESULT l_Temp = m_pDDImageSurface->Unlock( NULL );

			if( l_Status == S_OK )
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

/* Obsolete - Replaced due to performance reasons
HRESULT SVImageViewClass::UpdateScaledSurfaceWithExtremeLUT()
{
	HRESULT l_Status = S_OK;

	if( m_showExtremeLUT )
	{
		if( m_pDDScaledImageSurface != NULL )
		{
			DDSURFACEDESC2 l_ddSurfaceDesc;
			memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );
			l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );

			l_Status = m_pDDScaledImageSurface->Lock( NULL, &l_ddSurfaceDesc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL );
			if( l_Status == DD_OK )
			{
				size_t l_PixelWidth = l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8;

				if( 1 < l_PixelWidth )
				{
					unsigned long l_Mask = l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask | l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;
					unsigned long l_UpperValue = l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask;
					unsigned long l_LowerValue = l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask;

					BYTE* l_pBits = static_cast< BYTE* >( l_ddSurfaceDesc.lpSurface );

					for( size_t l_row = 0; l_row < l_ddSurfaceDesc.dwHeight; ++l_row )
					{
						for( size_t l_col = 0; l_col < l_ddSurfaceDesc.dwWidth; ++l_col )
						{
							BYTE* l_pBitsPixel = l_pBits + ( l_col * l_PixelWidth );
							unsigned long* l_pPixel = reinterpret_cast< unsigned long* >( l_pBitsPixel );

							if( ( ( *l_pPixel ) | ( ~l_Mask ) ) == 0xFFFFFFFF )
							{
								( *l_pPixel ) = ( ( *l_pPixel ) & ( ~l_Mask ) ) | l_UpperValue;
							}
							else if( ( ( *l_pPixel ) & ( l_Mask ) ) == 0x00000000 )
							{
								( *l_pPixel ) = ( *l_pPixel ) | l_LowerValue;
							}
						}

						l_pBits += l_ddSurfaceDesc.lPitch;

						//::Sleep( 0 );
						//m_ThreadWait.Sleep();
					}
				}
				else
				{
					l_Status = E_FAIL;
				}

				// preserve the error set if the image widths/pitch are different 
				// (meaning that we didn't copy the image data)
				HRESULT l_Temp = m_pDDScaledImageSurface->Unlock( NULL );

				if( l_Status == S_OK )
				{
					l_Status = l_Temp;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}
*/

HRESULT SVImageViewClass::BlitToScaledSurface( CRect& p_rSourceRect, CRect& p_rDestRect )
{
	HRESULT l_Status = S_OK;

	if( m_pDDImageSurface != NULL && m_pDDScaledImageSurface != NULL )
	{
		HDC scaledDC = NULL;

		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

		l_Status = m_pDDScaledImageSurface->GetDC( &scaledDC );

		if( l_Status == DD_OK )
		{
			HDC surfaceDC = NULL;

			l_Status = m_pDDImageSurface->GetDC( &surfaceDC );

			if( l_Status == DD_OK )
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

				//Sleep( 0 );
				//m_ThreadWait.Sleep();
			}

			UpdateOverlays( scaledDC, p_rSourceRect.left, p_rSourceRect.top );

			m_pDDScaledImageSurface->ReleaseDC( scaledDC );
		}

		if( l_Status == DD_OK )
		{
//			l_Status = UpdateScaledSurfaceWithExtremeLUT();
		}
		else
		{
			TRACE(_T( "SVImageViewClass::UpdateImage - Surface Blit failed %08lx\n" ), l_Status );
		}
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

	if( m_pDDScaledImageSurface != NULL )
	{
		CRect l_ScaledRect( 0, 0, p_rDestRect.Width(), p_rDestRect.Height() );

		SVDirectX::Instance().SetClipperHWnd( 0, m_hWnd );

		do
		{
			l_Status = SVDirectX::Instance().GetBltStatus( DDGBS_CANBLT );

			if( l_Status != S_OK )
			{
				Sleep( 0 );
				//m_ThreadWait.Sleep();
			}
		}
		while( l_Status != S_OK && l_Status != E_FAIL );

		if( l_Status == S_OK )
		{
			//Sleep( 0 );
			//m_ThreadWait.Sleep();

			l_Status = SVDirectX::Instance().Blt(
					&p_rDestRect,            // destination rectangle
					m_pDDScaledImageSurface, // source surface
					&l_ScaledRect,           // source rectangle
					DDBLT_WAIT,              // DDBLT_DONOTWAIT| DDBLT_ASYNC, // Flag <<<<<<<<<<<<<<<<<<<<<<
					NULL );                  // Pointer to DDBLTFX Structure
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

	if( m_pDDImageSurface != NULL )
	{
		// preserve the DDERR_SURFACELOST error so the overlays don't get drawn
		if( m_pDDImageSurface->IsLost() == DDERR_SURFACELOST )
		{
			HRESULT l_hrRestore = m_pDDImageSurface->Restore();

			if (l_hrRestore == DDERR_WRONGMODE)
			{
				// Destroy and Recreate surface
				l_hrRestore = RecreateImageSurface();
				
				if (l_hrRestore != DD_OK)
				{
					TRACE( _T( "SVImageViewClass::UpdateImage - CreateSurface failed\n" ) );
				}
			}

			// cause a Repaint if Surface was Rrestored/Recreated
			if( l_hrRestore == DD_OK )
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

	if( !( l_BitmapInfo.empty() ) && ( l_pParent != NULL ) )
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
		p_rDestRect.right =  min( l_parentRect.Width() + l_xScroll, l_imageRect.Width() );
		p_rDestRect.bottom = min( l_parentRect.Height() + l_yScroll, l_imageRect.Height() );

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

	if( l_pIPDoc != NULL )
	{
		if( l_pIPDoc->IsImageDataUpdated( m_ImageId, this ) == S_FALSE )
		{
			l_Status = l_pIPDoc->GetImageData( m_ImageId, m_ImageDIB, m_OverlayData );

			if( l_Status == S_OK )
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

		if( !( l_BitmapInfo.empty() ) && ( l_pBitmapBits != NULL ) && ( l_pParent != NULL ) )
		{
			l_Status = UpdateImageSurfaces( l_BitmapInfo );

			if( l_Status == S_OK )
			{
				CRect l_SourceRect;
				CRect l_DestRect;

				l_Status = GetRectInfo( l_SourceRect, l_DestRect );

				if( l_Status == S_OK )
				{
					l_Status = CopyBitsToSurface( l_SourceRect, l_BitmapInfo, l_pBitmapBits );
				}
			}

			// check for lost surfaces
			if( l_Status == DDERR_SURFACELOST )
			{
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );

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

		if( !( l_BitmapInfo.empty() ) && ( l_pParent != NULL ) )
		{
			CRect l_SourceRect;
			CRect l_DestRect;

			l_Status = GetRectInfo( l_SourceRect, l_DestRect );

			if( l_Status == S_OK )
			{
				l_Status = BlitToScaledSurface( l_SourceRect, l_DestRect );
			}

			if( l_Status == S_OK )
			{
				l_Status = BlitToPrimarySurface( l_DestRect );
			}

			// check for lost surfaces
			if( l_Status == DDERR_SURFACELOST )
			{
				TRACE( _T( "SVImageViewClass::UpdateImage - DDERR_SURFACELOST\n" ) );

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

	if( l_pIPDoc != NULL )
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

	if(pSplitterFrame != nullptr)
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
	if(pMDIFrame!= nullptr)
	{
		pSplitterFrame =  dynamic_cast< SVIPSplitterFrame*> (pMDIFrame->GetActiveFrame());
	}
	if(pSplitterFrame != nullptr)
	{
		pSplitterFrame->RefreshAllSplitters();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageView.cpp_v  $
 * 
 *    Rev 1.12   09 Dec 2014 09:20:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Operator Move
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Sep 2014 12:12:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Updated OnContextMenu to avoid adding Zoom options to context menu when ImageView is empty.
 * Added parameter bSetZoomSlider to SetZoom and SetZoomIndex methods.
 * Upated methods GetParameters, SetParameters, and CheckParameters to use the new tag CTAG_IMAGE_ZOOM_FACTOR_EX.
 * Added method UpdateZoomToolbar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Aug 2014 15:55:14   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   new commandentries for zoom commands
 * Createfont moved to Zoomhelper
 * scale Factor moved to Zoomhelper
 * new functions Calculate zoomfit, SetZoom, SetZoomIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   17 Jul 2014 19:11:02   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jun 2014 09:42:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Checks the Image display update flag to determine if the view is to be updated
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   26 May 2014 10:51:14   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   changed calls of SVAdjustToolSizePositionDlg-constructor 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 May 2014 12:45:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2014 10:48:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Jan 2014 11:43:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  882
 * SCR Title:  Fix Mask - Zoom bug (e109)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified DrawOverlay function to set the hatch flag for Mask figures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Oct 2013 13:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 2013 16:17:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added m_hActionIcon.
 *   Changed GetObjectCursor( POINT ) to return HICON.
 *   Consolidated duplicate code in GetObjectCursor( POINT ) and GetObjectCursor( SVExtentLocationPropertyEnum ).
 *   Changed GetObjectCursor( SVExtentLocationPropertyEnum ) to take POINT parameter to use rotation angle to determine icon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Sep 2013 14:49:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  864
 * SCR Title:  Fix Bug where Adjust Light Reference causes a crash in test mode.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified ShowExtremeLUT( to avoid a call to UpdatePaletteForLut when ImageSurface is null.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:56:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.140   25 Mar 2013 13:35:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnCommand to check for gage/profile and put up a message saying that they are invalid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.139   01 Mar 2013 10:19:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  816
 * SCR Title:  Color Camera Light Reference Crash
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CalcLut to use the width in pixels not bytes for the columns for loop.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.138   11 Oct 2012 10:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.137   28 Sep 2012 14:49:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.136   25 Sep 2012 15:19:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.135   18 Sep 2012 18:21:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.134   04 Sep 2012 15:27:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.133   09 Aug 2012 16:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed performance issues when update image view when showing extreme LUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.132   07 Aug 2012 16:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with overlay offset based on scroll position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.131   07 Aug 2012 16:11:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to reduce flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.130   27 Jul 2012 08:55:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.129   25 Jul 2012 14:10:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.128   18 Jul 2012 13:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.127   16 Jul 2012 08:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names in overlay structures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.126   09 Jul 2012 14:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.125   02 Jul 2012 16:58:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.124   19 Jun 2012 12:50:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.123   13 Oct 2011 08:14:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Unregistation problem when changing images in image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.122   07 Oct 2011 14:16:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Image View and Display Object based on name changes for methods to make them more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.121   06 Oct 2011 09:03:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with display update problems when the system is going offline or is offline.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.120   06 Oct 2011 09:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with display update problems when the system is going offline or is offline.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.119   04 Oct 2011 18:53:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated handshaking between IPDoc and ImageView to fix freezing of the ImageView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.118   03 Oct 2011 12:52:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated image view class to update the parent window with new extents when image changes size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.117   03 Oct 2011 10:04:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the handshaking between the display object thread and the image view draw functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.116   16 Sep 2011 16:00:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.115   14 Jul 2011 08:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.114   05 Jul 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.113   24 May 2011 15:27:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image class to use Owner Object Id insted of pointer and updated Image View with additional checks to improve performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.112   29 Apr 2011 08:06:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.111   13 Apr 2011 15:19:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.110   30 Mar 2011 15:10:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.109   30 Mar 2011 08:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with Adjust Tool position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.108   16 Mar 2011 13:35:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.107   25 Feb 2011 12:11:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.106   10 Feb 2011 14:55:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.105   27 Jan 2011 11:20:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.104   13 Dec 2010 11:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.103   08 Dec 2010 12:51:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.102   05 Nov 2010 14:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.101   05 Nov 2010 11:02:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.100   04 Nov 2010 13:39:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.99   01 Jun 2010 10:43:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.98   03 Mar 2010 15:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  687
 * SCR Title:  Large Images not displayed correctly in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed zoom problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.97   12 Feb 2010 12:55:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  687
 * SCR Title:  Large Images not displayed correctly in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Additional DirectDraw Surface for Stretch Blitting.
 * Added ReleaseSurface method.
 * Revised Blitting to stretch blit to offscreen surface rather than Primarry surface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.96   16 Dec 2009 11:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.95   14 Oct 2009 17:01:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated branch code into main line code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.94   21 Sep 2009 13:23:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added RecreateSurface method.
 * Removed OnDisplayChange handler.
 * Removed member variable m_surfaceLost.
 * Revised CreateBuffer method to call SVDirectX::GetPrimarySurfacePixelDepth.
 * Revised UpdateImage to check for valid primary surface and to recreate surface when needed..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.93   30 Jul 2009 11:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.92   11 Feb 2009 15:17:40   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Changed  UpdateImages method to retrieve width and height of the source image from the buffer (quick fix for the crash, doesn't address the root problem!)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.91   10 Feb 2009 10:42:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised UpdateImage method to return status code.
 * Revised UpdateImage method to wait for surface locking and blit completion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.90   04 Feb 2009 14:58:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Modifications to Refresh, OnPaint, and UpdateImage to reduce processor usage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.89   29 Jan 2009 16:28:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  646
 * SCR Title:  Archive Tool Buffer until Offline Mode Problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the UpdateImage method to not perform the update if the view is closing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.88   04 Dec 2008 13:38:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to use SVDirectX instead of SVObserverApp for DirectDraw variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.87   14 Nov 2008 11:08:22   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   For 
 * SVImageViewClass::SetParameters ()
 * SVImageViewClass::CheckParameters ()
 * 
 * Replace delete of character string from SVOVariantClass, with SVOString::StaticDestroy ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.86   16 Oct 2008 15:36:54   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed UpdateImage to restore a lost surface
 * Removed OnTimer (empty function)
 * General formatting clean up
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.85   12 Sep 2008 11:49:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to use DirectDraw to display images.
 * Created OnDisplayChange function to respond to changes to display settings.
 * Removed code to force MIL to update display after a zoom change.
 * Changed OnPaint to only call UpdateImage when using DirectX.
 * Added a call that sets the HWND for the DirectDraw clipper before Blt is called.
 * Changed UpdateImage function to create new surface when necessary.
 * Created CreateImageSurface to use member variables m_svImageInfo and m_imageRect.
 * Rearranged and removed unnecessary includes
 * Removed sections of code used to display images using MIL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.84   18 Jun 2008 16:33:46   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to force MIL to update display after a zoom change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.83   30 May 2008 13:28:20   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed all DirectX changes since 5.00 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   29 May 2008 16:13:44   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed OnPaint to only call UpdateImage when using DirectX.
 * Changed UpdateImage function to create new surface when necessary.
 * Changed CreateImageSurface to use member variables m_svImageInfo and m_imageRect.
 * Added a call to SVIPDoc->RefreshAllViews when a new surface is created.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   20 May 2008 12:16:32   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed memcpy to copy only the visible portion of the image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   20 May 2008 08:23:40   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added CreateDisplayImage function.
 * Updated DirectX display code to address issues with result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   15 May 2008 15:08:58   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Restored Matrox display code from 5.00 beta 2
 * Rearranged and removed unnecessary includes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   30 Apr 2008 11:09:42   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added a call that sets the HWND for the DirectDraw clipper before Blt is called.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   14 Apr 2008 10:11:16   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed sections of code used to display images using MIL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76   10 Apr 2008 07:44:08   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Updated to use DirectDraw to display images.
 * * Created OnDisplayChange function to handle changes to display settings.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.75   09 Apr 2008 08:36:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   30 Oct 2007 15:25:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to manage GDI resource.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   24 Jul 2007 13:31:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   21 Jun 2007 13:05:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71.1.2   14 Jul 2009 13:40:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71.1.1   07 Apr 2008 08:19:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71.1.0   30 Oct 2007 09:49:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed GDI leak (HBRUSH)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   16 Jan 2006 17:05:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   02 Nov 2005 09:07:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   19 Oct 2005 14:10:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved RunOnce command to always execute after reset object command instead of only after a valid reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   06 Oct 2005 15:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  515
 * SCR Title:  Fix Bug when trying to edit tools with invalid extents
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnButtonDblClk to set and reset msvError display flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   07 Sep 2005 13:51:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   In method OnLButtonUp it will check to make sure that it is not in Run, Test or Regression mode before doing the RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   02 Sep 2005 12:26:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image view to ignore whether the mouse is over the correct location when adjusting rotation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   31 Aug 2005 08:38:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   24 Aug 2005 09:39:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update logic to make all Create, Update and Destroy calls to the display buffers use the Message Pump.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   22 Aug 2005 11:16:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed name of function SVIsAllowed to SVIsDisplayable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   22 Aug 2005 10:14:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   10 Aug 2005 12:31:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   02 Aug 2005 14:45:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Security Bug in OnCommand
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   02 Aug 2005 13:52:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Functions OkToEdit and OkToEditMove to make menus more consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   18 Jul 2005 12:19:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed bFreeze variable from SVImageView class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   28 Jun 2005 17:11:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added back in CTAG_IMAGE_NAME in GetParameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   27 Jun 2005 09:37:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the freeze image, freeze methods and freeze variables from image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   21 Jun 2005 13:07:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial changes to add SVSecurity
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   18 Feb 2005 11:48:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  382
 * SCR Title:  Add locking mechanism to internal image objects to eliminate access problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added safe copy functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   17 Feb 2005 14:28:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 3.52   16 Feb 2005 14:33:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   12 Jul 2004 10:26:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed from BOOL to HRESULT in BuildImageWindow, BuildImageWindowFromImageName, and CloseImageWindow
 * made SetDisplayImage protected
 * added #include "SVErrorException.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   03 Feb 2004 17:35:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   02 Jan 2004 09:51:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  387
 * SCR Title:  Change display logic to delay updating while the inspection has a backlog
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug so that the PaintDC is set in to transparent mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   24 Dec 2003 14:33:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  387
 * SCR Title:  Change display logic to delay updating while the inspection has a backlog
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed the display to not update tool figures while the display is off and to use the PaintDC instead of the window's DC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   15 Dec 2003 12:44:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed ImageView to set document modified flag when a tool is moved.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   08 Dec 2003 15:18:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made another change to try to correct scrolling/zooming display problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   05 Dec 2003 11:19:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid image scroll view display issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   26 Nov 2003 10:53:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed image panel problems when zooming a scrolled panel.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   10 Nov 2003 12:23:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   27 Aug 2003 08:49:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with scrolling after editing a configuration or selecting a display image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   11 Jul 2003 08:14:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated ShowExtremeLUT method to use the Display system handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   08 Jul 2003 09:28:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   07 May 2003 13:16:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   22 Apr 2003 10:25:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   16 Apr 2003 17:53:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  347
 * SCR Title:  RGB Color system has out of sync displays and improper results
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed color stuff so that the image view panes are all in sync.  Added new class so that color banded images will be displayed at source image indexes instead of result image indexes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   20 Mar 2003 10:26:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  282
 * SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made sure that an analyzer variable was initialized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   14 Mar 2003 17:42:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  282
 * SCR Title:  Dr.W if Blob Analyzer is closed using the System Menu Close Icon.
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed some small existing bugs dealing with: Range always reporting IsValid =  FALSE, Configure Analyzer available on context menu even when there isn't one, and setting ranges on Blob Analyzer results greater than 800,000,000,000.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   06 Mar 2003 13:17:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   04 Mar 2003 11:46:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new Adjust Position Dialog for SVLoadImageToolClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   21 Feb 2003 12:37:06   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  324
 * SCR Title:  Program crashes and Dr. Watson log directs error to CCmdTarget destructor.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Refresh method to fix casting problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   05 Feb 2003 17:29:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   30 Jan 2003 10:47:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated FreezePanel, SetupWindow, DrawImageOnOverlay, ShowExtremeLUT, GetDisplayImageHandle, Refresh, SelectDisplayImage, GetViewSize, CloseImageWindow, BuildImageWindowFromImageName, BuildImageWindow  and constructor methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   10 Jan 2003 14:02:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   19 Nov 2002 13:27:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed references to System to SVImageProcessingClass 
 * 
 *   Changed string literals to constants defined in SVConfigurationTags.h
 *    Call SVImageProcessingClass method instead of MIL method
 *          Refresh
 *           Put check in for SVMainImageInfo object
 *           Removed COM support
 *           Added GetParameters and SetParameters functions
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   17 Oct 2001 15:09:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  234
 * SCR Title:  RGB Main Image not displayed when a configuration is reloaded
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated GetImageByName method in SVImageViewClass to look check for RGB Main Image as well as the tool set images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   04 Oct 2001 15:39:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   16 May 2001 11:28:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  201
 * SCR Title:  Move Operator, right clicking tool should not access Configure Analyzer
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added securtiy check for "Configure Analyzer" in 
 * OnContextMenu
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   10 May 2001 15:42:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * OnContextMenu
 * Refresh
 * RefreshToolBuffers
 * RefreshToolFigures
 * FreezePanel
 * OnLButtonDblClk
 * OnRButtonDblClk
 * OnLButtonDown
 * OnMouseMove
 * OnPaint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   06 Sep 2000 16:30:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   14 Jun 2000 16:28:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  160
 * SCR Title:  Image Display loses image
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Serialize to get the current name of the main image 
 * when storing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   09 Jun 2000 10:39:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  155
 * SCR Title:  Context Menu available in Run Mode
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnContextMenu to only allow menu if not Run or test mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   07 May 2000 23:29:50   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  137
 * SCR Title:  Remove the need to explicitly reference specific version numbers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   26 Apr 2000 19:08:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added viewPickPoint to keep mouse pick point in View Space.
 * Added include for SVAdjustImageLinePosition.h
 * Revised OnCommand to set viewPickPoint to zeroes.
 * Revised OnCommand to display new adjustment dialog for the Angular Profile tool.
 * Revised OnMouseMove to calculate deltas in view space as well as transformed space.
 * Revised OnMouseMove to send both pickpoints for the 
 * following messages:
 * SVM_MOVE,SVM_R_SIZE,SVM_B_SIZE,SVM_L_SIZE,
 * SVM_T_SIZE,SVM_RB_SIZE,SVM_LB_SIZE,
 * SVM_LT_SIZE,SVM_RT_SIZE
 * REvised getObjectAtPoint to set viewPickPoint.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   19 Apr 2000 16:36:32   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  128
 * SCR Title:  Suport for 3.11 versioning
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVImageViewClass::Serialize () was modified to support program version number 3.11 beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   14 Apr 2000 10:57:22   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  120
 * SCR Title:  Pattern Analyzer
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Modified functions 'onMouseMove' 'onContextMenu' and 'onCommand'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   14 Mar 2000 14:42:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised version number to 3.10 release (0x00030AFF)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   14 Mar 2000 12:45:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed small zoom factor from -8 to -4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   13 Mar 2000 16:25:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnContextMenu to map Mouse point from Screen to client.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   09 Mar 2000 08:52:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   07 Mar 2000 14:07:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  101
 * SCR Title:  Versioning 3.10
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Version number to 3.1 Beta 3.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   07 Mar 2000 11:28:52   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  114
 * SCR Title:  Integrate Single Pixel Move Context Menu.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed 
 *   SVImageViewClass::OnContextMenu () to add "Adjust Position" to the popup context menu.
 *   SVImageViewClass::OnCommand () to act on the "Adjust Position" entry on the popup context menu.
 * 
 * Added
 *   SVImageViewClass::UnNormalize () to convert from "real" coordinates to view coordinates.
 *   SVImageViewClass::SetPickType () to give access to protected member.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   03 Mar 2000 13:51:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised handling of SVSendMessage for
 * SVM_GET_ABSOLUTE_EXTENT and SVM_GET_EXTENT
 * to adjust for changed parameters.
 * 
 * Revised to use SVImageInfoClass methods to get image extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   Mar 01 2000 12:26:26   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to handle sizeable and positionable calls for objects which have no standard extent to return.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   26 Feb 2000 20:02:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnMouseMove to cehck return value from 
 * SVSendMessage for the SVM_GET_CONTEXT_EXTENT
 * message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   25 Feb 2000 16:27:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the following methods in SVImageView:
 * OnLButtonDblClk
 * OnRButtonDblClk
 * OnLButtonDown
 * OnMouseMove
 * getObjectCursor
 * getObjectAtPoint
 * for contextual drawing/selection/moving/sizing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   23 Feb 2000 14:52:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified the OnContextMenu so that it would not allow zoom or image selection in run mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Feb 2000 15:10:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added getObjectCursor and getObjectAtPoint methods.
 * Moved code from onMouseMove to getObjectCursor.
 * Moved code from onLButtonDown to getObjectAtPoint.
 * Revised getObjectCursor and getObjectAtPoint to use relative extents.
 * Revised OnLButtonDblClk to check if view initialized.
 * Revised OnRButtonDblClk to check if view initialized.
 * Revised OnLButtonDown to check if view initialized.
 * Revised OnMouseMove to check if view initialized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   Feb 21 2000 10:32:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new Freeze on reject capbility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   16 Feb 2000 16:49:44   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   08 Feb 2000 11:02:02   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 *         1. Modified the constructor/destructor to create an array of fonts
 * of different sizes to be used for scaled text;
 * 
 *         2. Added the variable mImageZoomFactor to the class (contains the
 * current scale factor from -16 to 16);
 * 
 *         3. Changed BuildImageWindow to reflect the new window ownership
 * hierarchy (the SVImageViewClass window is now owned by an SVImageViewScroll
 * window) and call a function to set the scroll bar limits;
 * 
 *         4. Changed Refresh() to call MdispZoom() to set the display scale;
 * 
 *         5. Changed GetDC function to select an appropriately scaled font;
 * 
 *         6. Added a function called GetViewSize which returns the size of the
 * view (scaled appropriately) to the caller;
 * 
 *         7. Added functions OnContextMenu and OnCommand to handle the new pop
 * menu;
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 13:32:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  65
 * SCR Title:  Remove obsolute class SVToolSetViewClass
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed references to SVToolSetViewClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:48:20   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Nov 17 1999 15:04:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO changes for image arithmetic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Oct 26 1999 16:58:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Change HourGlass wait cursor to arrow cursor when no tool has been selected in the image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Oct 13 1999 15:06:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Roberts bug fixes for Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Oct 04 1999 09:40:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Check merge and re-checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Oct 04 1999 09:20:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Put some checking inside to block drawing while loading.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Sep 1999 08:17:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to reactivate Refresh Tool Buffer on Overlay
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Aug 1999 16:36:12   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   BugFix in OnRButtonDblClk().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Aug 1999 16:31:42   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Fixed OnRButtonDblClk() to call SetupDialog().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/