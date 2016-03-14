//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDirectX
//* .File Name       : $Workfile:   SVDirectX.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVDirectX.h"

SVDirectX::~SVDirectX()
{
	LocalClear();

	if( m_pDDObject != NULL )
	{
		m_pDDObject->Release();
		m_pDDObject = NULL;
	}
}

SVDirectX& SVDirectX::Instance()
{
	static SVDirectX l_svDirectX;

	return l_svDirectX;
}

bool SVDirectX::empty() const
{
	bool l_Empty = true;

	l_Empty = l_Empty && ( m_pDDDisplaySurface == NULL );
	l_Empty = l_Empty && ( m_pDDClipper == NULL );

	return l_Empty;
}

void SVDirectX::clear()
{
	LocalClear();
}

// InitializeObject must be called before m_pDDObject or m_pDDDisplaySurface can be used.
HRESULT SVDirectX::Initialize()
{
	HRESULT l_Retval = S_FALSE;

	// Call the destructor to make sure all the member variable pointers are released.
	this->~SVDirectX();

	// Create the DirectDraw Object.
	l_Retval = DirectDrawCreateEx( reinterpret_cast< LPGUID >( NULL ),
		reinterpret_cast< LPVOID* >( &m_pDDObject ),
		IID_IDirectDraw7,
		NULL );

	if( l_Retval == DD_OK )
	{
		l_Retval = m_pDDObject->SetCooperativeLevel( NULL, DDSCL_NORMAL | DDSCL_FPUPRESERVE );
		if( l_Retval != DD_OK )
		{
			TRACE( _T( "SVDirectX::InitializeObject was unable to set DirectDraw cooperative level." ) );
		}
		else if( m_pDDObject != NULL )
		{
			if( m_pDDDisplaySurface == NULL )
			{
				l_Retval = CreatePrimarySurface();
			}
		}
	}
	else
	{
		TRACE( _T( "SVDirectX::InitializeObject was unable to create the DirectDraw object." ) );

		if( m_pDDObject != NULL )
		{
			m_pDDObject->Release();
			m_pDDObject = NULL;
		}
	}

	return l_Retval;
}

HRESULT SVDirectX::RestoreAllSurfaces()
{
	HRESULT l_Status = S_OK;

	if( m_pDDObject != NULL )
	{
		l_Status = m_pDDObject->RestoreAllSurfaces();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDirectX::SetClipperHWnd( DWORD p_Flags, HWND p_Handle )
{
	HRESULT l_Status = S_OK;

	if( m_pDDClipper != NULL )
	{
		l_Status = m_pDDClipper->SetHWnd( p_Flags, p_Handle );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDirectX::Blt( RECT* p_pDestRect, IDirectDrawSurface7* p_pDDSurface, RECT* p_pSourceRect, DWORD p_Flags, DDBLTFX* p_pDDBltFx )
{
	HRESULT l_Status = S_OK;

	if( m_pDDDisplaySurface != NULL )
	{
		l_Status = m_pDDDisplaySurface->Blt( p_pDestRect, p_pDDSurface, p_pSourceRect, p_Flags, p_pDDBltFx );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDirectX::GetBltStatus( DWORD p_Flags )
{
	HRESULT l_Status = S_OK;

	if( m_pDDDisplaySurface != NULL )
	{
		l_Status = m_pDDDisplaySurface->GetBltStatus( p_Flags );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDirectX::IsLost()
{
	HRESULT l_Status = S_OK;

	if( m_pDDDisplaySurface != NULL )
	{
		l_Status = m_pDDDisplaySurface->IsLost();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVDirectX::SVDirectX()
: m_pDDObject( NULL )
, m_pDDClipper( NULL )
, m_pDDDisplaySurface( NULL )
{
}

HRESULT SVDirectX::CreatePrimarySurface()
{
	HRESULT l_Retval = E_FAIL;

	SVDirectX& rDirectX = SVDirectX::Instance();
	if (rDirectX.m_pDDDisplaySurface != NULL)
	{
		rDirectX.m_pDDDisplaySurface->SetClipper( NULL );
	}

	if (rDirectX.m_pDDClipper != NULL)
	{
		rDirectX.m_pDDClipper->Release();
		rDirectX.m_pDDClipper = NULL;
	}
	if (rDirectX.m_pDDDisplaySurface != NULL)
	{
		rDirectX.m_pDDDisplaySurface->Release();
		rDirectX.m_pDDDisplaySurface = NULL;
	}

	if (rDirectX.m_pDDObject != NULL)
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );

		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );
		l_ddSurfaceDesc.dwFlags = DDSD_CAPS;
		l_ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		l_Retval = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, &rDirectX.m_pDDDisplaySurface, NULL );
		if( l_Retval != DD_OK )
		{
			TRACE( _T( "SVDirectX::InitializeSurface was unable to create the display surface." ) );
		}
		else if( rDirectX.m_pDDClipper == NULL && rDirectX.m_pDDDisplaySurface != NULL )
		{
			if( DD_OK == rDirectX.m_pDDObject->CreateClipper( 0, &rDirectX.m_pDDClipper, NULL ) )
			{
				if( DD_OK != rDirectX.m_pDDDisplaySurface->SetClipper( rDirectX.m_pDDClipper ) )
				{
					TRACE( _T( "SVDirectX::InitializeSurface was unable to set the clipper." ) );
				}
			}
			else
			{
				TRACE( _T( "SVDirectX::InitializeSurface was unable to create the clipper." ) );
			}
		}
	}
	return l_Retval;
}

HRESULT SVDirectX::CreateSurface(long width, long height, IDirectDrawSurface7** ppDDSurface )
{
	HRESULT l_hr = E_FAIL;
	SVDirectX& rDirectX = SVDirectX::Instance();

	if( ( rDirectX.m_pDDObject != NULL ) && ( rDirectX.m_pDDDisplaySurface != NULL ) )
	{
		DDPIXELFORMAT l_PixelFormat;
		ZeroMemory( &l_PixelFormat, sizeof( l_PixelFormat ) );
		l_PixelFormat.dwSize = sizeof( l_PixelFormat );
		l_hr = rDirectX.m_pDDDisplaySurface->GetPixelFormat( &l_PixelFormat );
		if( l_hr == DD_OK )
		{
			DDSURFACEDESC2 l_ddSurfaceDesc;
			memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );

			l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );
			l_ddSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			l_ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			l_ddSurfaceDesc.dwWidth = width;
			l_ddSurfaceDesc.dwHeight = height;
			l_ddSurfaceDesc.ddpfPixelFormat = l_PixelFormat;

			//BRW - Release the surface if it already exists or CreateSurface will fail.
			if( ppDDSurface != NULL && *ppDDSurface != NULL )
			{
				(*ppDDSurface)->Release();
				*ppDDSurface = NULL;
			}

			l_hr = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, ppDDSurface, NULL );
			if (l_hr != DD_OK)
			{
				TRACE( _T( "SVDirectX::CreateSurface failed (%08lx)\n" ), l_hr );
			}
		}
	}
	return l_hr;
}

HRESULT SVDirectX::CreateSurface(const SVBitmapInfo& p_rBitmapInfo, IDirectDrawSurface7** ppDDSurface)
{
	HRESULT l_hr = S_OK;

	SVDirectX& rDirectX = SVDirectX::Instance();

	if( ( ppDDSurface != NULL ) && ( rDirectX.m_pDDObject != NULL ) && ( rDirectX.m_pDDDisplaySurface != NULL ) && !( p_rBitmapInfo.empty() ) )
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;

		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );

		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );
		l_ddSurfaceDesc.ddpfPixelFormat.dwSize = sizeof( l_ddSurfaceDesc.ddpfPixelFormat );

		l_ddSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		l_ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		l_ddSurfaceDesc.dwWidth = p_rBitmapInfo.GetWidth();
		l_ddSurfaceDesc.dwHeight = ::labs( p_rBitmapInfo.GetHeight() );

		l_ddSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;
		l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = p_rBitmapInfo.GetBitCount();

		switch( l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount )
		{
		case 1:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwFlags = l_ddSurfaceDesc.ddpfPixelFormat.dwFlags | DDPF_PALETTEINDEXED1;

				break;
			}
		case 2:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwFlags = l_ddSurfaceDesc.ddpfPixelFormat.dwFlags | DDPF_PALETTEINDEXED2;

				break;
			}
		case 4:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwFlags = l_ddSurfaceDesc.ddpfPixelFormat.dwFlags | DDPF_PALETTEINDEXED4;

				break;
			}
		case 8:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwFlags = l_ddSurfaceDesc.ddpfPixelFormat.dwFlags | DDPF_PALETTEINDEXED8;

				break;
			}
		case 16:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x0000F800;
				l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x000007E0;
				l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x0000001F;
				break;
			}
		case 24:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
				l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
				l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
				break;
			}
		case 32:
			{
				l_ddSurfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
				l_ddSurfaceDesc.ddpfPixelFormat.dwRBitMask        = 0x00FF0000;
				l_ddSurfaceDesc.ddpfPixelFormat.dwGBitMask        = 0x0000FF00;
				l_ddSurfaceDesc.ddpfPixelFormat.dwBBitMask        = 0x000000FF;
				break;
			}
		}

		//BRW - Release the surface if it already exists or CreateSurface will fail.
		if( ( *ppDDSurface ) != NULL )
		{
			( *ppDDSurface )->Release();
			( *ppDDSurface ) = NULL;
		}

		l_hr = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, ppDDSurface, NULL );
		if (l_hr != DD_OK)
		{
			TRACE( _T( "SVDirectX::CreateSurface failed (%08lx)\n" ), l_hr );
		}

		if (l_hr == DD_OK)
		{
			DWORD l_Flags = 0;

			switch( l_ddSurfaceDesc.ddpfPixelFormat.dwRGBBitCount )
			{
			case 1:
				{
					l_Flags = DDPCAPS_1BIT;

					break;
				}
			case 2:
				{
					l_Flags = DDPCAPS_2BIT;

					break;
				}
			case 4:
				{
					l_Flags = DDPCAPS_4BIT;

					break;
				}
			case 8:
				{
					l_Flags = DDPCAPS_ALLOW256 | DDPCAPS_8BIT;

					break;
				}
			}

			if( l_Flags != 0 && ( *ppDDSurface ) != NULL )
			{
				IDirectDrawPalette* l_pPalette = NULL;
				SVBitmapInfo::SVPaletteTable l_PaletteTable = p_rBitmapInfo.GetPaletteTable();

				l_hr = rDirectX.m_pDDObject->CreatePalette( l_Flags, &( l_PaletteTable[ 0 ] ), &l_pPalette, NULL );

				if( l_pPalette != NULL )
				{
					l_hr = ( *ppDDSurface )->SetPalette( l_pPalette );

					l_pPalette->Release();

					l_pPalette = NULL;
				}
			}
		}
	}
	else
	{
		l_hr = E_FAIL;
	}

	return l_hr;
}

HRESULT SVDirectX::GetPrimarySurfaceResolution(SIZE& rSize)
{
	HRESULT hr = E_FAIL;
	SVDirectX& rDirectX = SVDirectX::Instance();
	if (rDirectX.m_pDDDisplaySurface)
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );

		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );
		l_ddSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;

		hr = rDirectX.m_pDDDisplaySurface->GetSurfaceDesc(&l_ddSurfaceDesc);
		if (hr == DD_OK)
		{
			rSize.cx = l_ddSurfaceDesc.dwWidth;
			rSize.cy = l_ddSurfaceDesc.dwHeight;
		}
		else
		{
			TRACE( _T( "SVDirectX::GetPrimarySurfaceResolution failed (%08lx)\n" ), hr );
		}
	}
	return hr;
}

HRESULT SVDirectX::GetPrimarySurfacePixelDepth(int& rPixelDepth)
{
	HRESULT hr = E_FAIL;
	SVDirectX& rDirectX = SVDirectX::Instance();

	if (rDirectX.m_pDDDisplaySurface)
	{
		DDPIXELFORMAT l_pPixelFormat;
		ZeroMemory( &l_pPixelFormat, sizeof( l_pPixelFormat ) );
		l_pPixelFormat.dwSize = sizeof( l_pPixelFormat );
		hr = rDirectX.m_pDDDisplaySurface->GetPixelFormat( &l_pPixelFormat );
		if (hr == DD_OK)
		{
			rPixelDepth = l_pPixelFormat.dwRGBBitCount / 8;
		}
		else
		{
			TRACE( _T( "SVDirectX::GetPrimarySurfacePixelDepth failed (%08lx)\n" ), hr );
		}
	}
	return hr;
}

HRESULT SVDirectX::TestCooperativeLevel()
{
	HRESULT hr = E_POINTER;
	SVDirectX& rDirectX = SVDirectX::Instance();
	if (rDirectX.m_pDDObject != NULL)
	{
		hr = rDirectX.m_pDDObject->TestCooperativeLevel();
	}
	return hr;
}

void SVDirectX::LocalClear()
{
	if( m_pDDDisplaySurface != NULL )
	{
		m_pDDDisplaySurface->SetClipper( NULL );
		m_pDDDisplaySurface->Release();
		m_pDDDisplaySurface = NULL;
	}

	if( m_pDDClipper != NULL )
	{
		m_pDDClipper->Release();
		m_pDDClipper = NULL;
	}
}

