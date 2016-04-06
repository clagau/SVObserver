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
#pragma region Includes
#include "stdafx.h"
#include "SVDirectX.h"
#pragma endregion Includes

SVDirectX::~SVDirectX()
{
	LocalClear();

	if( nullptr != m_pDDObject )
	{
		m_pDDObject->Release();
		m_pDDObject = nullptr;
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

	l_Empty = l_Empty && ( nullptr == m_pDDDisplaySurface );
	l_Empty = l_Empty && ( nullptr == m_pDDClipper );

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
	l_Retval = DirectDrawCreateEx( reinterpret_cast< LPGUID >( 0 ),
		reinterpret_cast< LPVOID* >( &m_pDDObject ),
		IID_IDirectDraw7,
		nullptr );

	if( DD_OK == l_Retval )
	{
		l_Retval = m_pDDObject->SetCooperativeLevel( nullptr, DDSCL_NORMAL | DDSCL_FPUPRESERVE );
		if( DD_OK != l_Retval )
		{
			TRACE( _T( "SVDirectX::InitializeObject was unable to set DirectDraw cooperative level." ) );
		}
		else if( nullptr != m_pDDObject )
		{
			if( nullptr == m_pDDDisplaySurface )
			{
				l_Retval = CreatePrimarySurface();
			}
		}
	}
	else
	{
		TRACE( _T( "SVDirectX::InitializeObject was unable to create the DirectDraw object." ) );

		if( nullptr != m_pDDObject )
		{
			m_pDDObject->Release();
			m_pDDObject = nullptr;
		}
	}

	return l_Retval;
}

HRESULT SVDirectX::RestoreAllSurfaces()
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDDObject )
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

	if( nullptr != m_pDDClipper )
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

	if( nullptr != m_pDDDisplaySurface )
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

	if( nullptr != m_pDDDisplaySurface )
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

	if( nullptr != m_pDDDisplaySurface )
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
: m_pDDObject( nullptr )
, m_pDDClipper( nullptr )
, m_pDDDisplaySurface( nullptr )
{
}

HRESULT SVDirectX::CreatePrimarySurface()
{
	HRESULT l_Retval = E_FAIL;

	SVDirectX& rDirectX = SVDirectX::Instance();
	if (nullptr != rDirectX.m_pDDDisplaySurface)
	{
		rDirectX.m_pDDDisplaySurface->SetClipper( nullptr );
	}

	if (nullptr != rDirectX.m_pDDClipper )
	{
		rDirectX.m_pDDClipper->Release();
		rDirectX.m_pDDClipper = nullptr;
	}
	if (nullptr != rDirectX.m_pDDDisplaySurface)
	{
		rDirectX.m_pDDDisplaySurface->Release();
		rDirectX.m_pDDDisplaySurface = nullptr;
	}

	if (nullptr != rDirectX.m_pDDObject)
	{
		DDSURFACEDESC2 l_ddSurfaceDesc;
		memset( &l_ddSurfaceDesc, 0, sizeof( l_ddSurfaceDesc ) );

		l_ddSurfaceDesc.dwSize = sizeof( l_ddSurfaceDesc );
		l_ddSurfaceDesc.dwFlags = DDSD_CAPS;
		l_ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		l_Retval = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, &rDirectX.m_pDDDisplaySurface, nullptr );
		if( DD_OK != l_Retval )
		{
			TRACE( _T( "SVDirectX::InitializeSurface was unable to create the display surface." ) );
		}
		else if( nullptr == rDirectX.m_pDDClipper && nullptr != rDirectX.m_pDDDisplaySurface )
		{
			if( DD_OK == rDirectX.m_pDDObject->CreateClipper( 0, &rDirectX.m_pDDClipper, nullptr ) )
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

	if( ( nullptr != rDirectX.m_pDDObject ) && ( nullptr != rDirectX.m_pDDDisplaySurface ) )
	{
		DDPIXELFORMAT l_PixelFormat;
		ZeroMemory( &l_PixelFormat, sizeof( l_PixelFormat ) );
		l_PixelFormat.dwSize = sizeof( l_PixelFormat );
		l_hr = rDirectX.m_pDDDisplaySurface->GetPixelFormat( &l_PixelFormat );
		if( DD_OK == l_hr )
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
			if( nullptr != ppDDSurface && nullptr != *ppDDSurface)
			{
				(*ppDDSurface)->Release();
				*ppDDSurface = nullptr;
			}

			l_hr = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, ppDDSurface, NULL );
			if (DD_OK != l_hr)
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

	if( ( nullptr != ppDDSurface ) && ( nullptr != rDirectX.m_pDDObject ) && ( nullptr != rDirectX.m_pDDDisplaySurface ) && !( p_rBitmapInfo.empty() ) )
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
		if( nullptr != ( *ppDDSurface ) )
		{
			( *ppDDSurface )->Release();
			( *ppDDSurface ) = nullptr;
		}

		l_hr = rDirectX.m_pDDObject->CreateSurface( &l_ddSurfaceDesc, ppDDSurface, nullptr );
		if (DD_OK != l_hr )
		{
			TRACE( _T( "SVDirectX::CreateSurface failed (%08lx)\n" ), l_hr );
		}

		if (DD_OK == l_hr)
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

			if( l_Flags != 0 && nullptr != ( *ppDDSurface ) )
			{
				IDirectDrawPalette* l_pPalette = nullptr;
				SVBitmapInfo::SVPaletteTable l_PaletteTable = p_rBitmapInfo.GetPaletteTable();

				l_hr = rDirectX.m_pDDObject->CreatePalette( l_Flags, &( l_PaletteTable[ 0 ] ), &l_pPalette, nullptr );

				if( nullptr != l_pPalette )
				{
					l_hr = ( *ppDDSurface )->SetPalette( l_pPalette );

					l_pPalette->Release();

					l_pPalette = nullptr;
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
		if (DD_OK == hr)
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
		if (DD_OK == hr)
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
	if (nullptr != rDirectX.m_pDDObject)
	{
		hr = rDirectX.m_pDDObject->TestCooperativeLevel();
	}
	return hr;
}

void SVDirectX::LocalClear()
{
	if( nullptr!= m_pDDDisplaySurface )
	{
		m_pDDDisplaySurface->SetClipper( nullptr );
		m_pDDDisplaySurface->Release();
		m_pDDDisplaySurface = nullptr;
	}

	if( nullptr != m_pDDClipper )
	{
		m_pDDClipper->Release();
		m_pDDClipper = nullptr;
	}
}

