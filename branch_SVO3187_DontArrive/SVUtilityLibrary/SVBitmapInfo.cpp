//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBitmapInfo
//* .File Name       : $Workfile:   SVBitmapInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVBitmapInfo.h"
#pragma endregion Includes

SVBitmapInfo::SVColorTable SVBitmapInfo::GetDefaultColorTable(int bitCount )
{
	SVColorTable l_Colors;

	switch(bitCount)
	{
		case 8:
		{
			l_Colors.resize( 256 );

			for( size_t i = 0; i < 256; ++i )
			{
				unsigned char l_Value = static_cast< unsigned char >( i );

				RGBQUAD l_Color = { l_Value, l_Value, l_Value, 0 };

				l_Colors[ i ] = l_Color;
			}

			break;
		}
	}

	return l_Colors;
}

unsigned long SVBitmapInfo::GetColorTableSize(int bitCount, unsigned long compression )
{
	unsigned long l_Size = 0;

	switch( bitCount )
	{
		case 1:
		case 2:
		case 4:
		case 8:
		{
			l_Size = 1 << bitCount;
			break;
		}
		case 16:
		case 32:
		{
			switch( compression )
			{
				case BI_RGB:
				{
					l_Size = 0;

					break;
				}
				case BI_BITFIELDS:
				{
					l_Size = 3;

					break;
				}
			}

			break;
		}
		case 24:
		{
			l_Size = 0;

			break;
		}
	}

	return l_Size;
}

unsigned long SVBitmapInfo::GetImageStrideInBytes( long width, int bitCount)
{
	unsigned long l_Stride = 0;
	unsigned long l_Width = ::labs(width);
	unsigned long l_BitCount = bitCount;

	if( 0 != l_Width && 0 != l_BitCount )
	{
		l_Stride = l_Width;           //Start with Image Width
		l_Stride *= l_BitCount / 8;   //Multiply width by byte count of pixel
		l_Stride += 3;                //Add 3 bytes to round up to the next 4 byte alignment
		l_Stride = l_Stride & ~3;     //Clear bits 0 and 1 (~3 == 0xFFFFFFFC) to remove extra bytes above 4 byte alignment
	}

	return l_Stride;
}

unsigned long SVBitmapInfo::GetImageSizeInBytes( long width, long height, int bitCount )
{
	unsigned long l_Size = ::labs(height) * GetImageStrideInBytes(width, bitCount);

	return l_Size;
}

SVBitmapInfo::SVBitmapInfo()
:	m_BufferSize( 0 )
, m_pBuffer( nullptr )
{
}

SVBitmapInfo::SVBitmapInfo( const SVBitmapInfo& rObject )
:	m_BufferSize( 0 )
, m_pBuffer( nullptr )
{
	if( nullptr != rObject.m_pBuffer )
	{
		Assign( *( rObject.GetBitmapInfo() ) );
	}
}

SVBitmapInfo::SVBitmapInfo( const BITMAPINFO& rObject )
:	m_BufferSize( 0 )
, m_pBuffer( nullptr )
{
	Assign( rObject );
}

SVBitmapInfo::SVBitmapInfo( long width, long height, int bitCount, const SVColorTable& rColorTable )
:	m_BufferSize( 0 )
, m_pBuffer( nullptr )
{
	Assign(width, height, bitCount, rColorTable);
}

SVBitmapInfo::~SVBitmapInfo()
{
	clear();
}

bool SVBitmapInfo::empty() const
{
	return ( nullptr == m_pBuffer );
}

void SVBitmapInfo::clear()
{
	if( nullptr != m_pBuffer )
	{
		delete[] m_pBuffer;

		m_BufferSize = 0;
		m_pBuffer = nullptr;
	}
}

size_t SVBitmapInfo::GetBitmapInfoSizeInBytes() const
{
	size_t l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		size_t l_ColorCount = l_pBitmapInfo->bmiHeader.biClrUsed;

		if( 0 == l_ColorCount )
		{
			l_ColorCount = GetColorTableSize( l_pBitmapInfo->bmiHeader.biBitCount, l_pBitmapInfo->bmiHeader.biCompression );
		}

		l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );
	}

	return l_Size;
}

size_t SVBitmapInfo::GetBitmapImageSizeInBytes() const
{
	size_t l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = GetImageSizeInBytes( l_pBitmapInfo->bmiHeader.biWidth, l_pBitmapInfo->bmiHeader.biHeight, l_pBitmapInfo->bmiHeader.biBitCount );
	}

	return l_Size;
}

size_t SVBitmapInfo::GetBitmapImageStrideInBytes() const
{
	size_t l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = GetImageStrideInBytes( l_pBitmapInfo->bmiHeader.biWidth, l_pBitmapInfo->bmiHeader.biBitCount );
	}

	return l_Size;
}


long SVBitmapInfo::GetWidth() const
{
	long l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biWidth;
	}

	return l_Size;
}

long SVBitmapInfo::GetHeight() const
{
	long l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biHeight;
	}

	return l_Size;
}

int SVBitmapInfo::GetBitCount() const
{
	int l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = static_cast<int> (l_pBitmapInfo->bmiHeader.biBitCount);
	}

	return l_Size;
}

unsigned long SVBitmapInfo::GetColorTableSize() const
{
	unsigned long l_Size = 0;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biClrUsed;
	}

	return l_Size;
}

// cppcheck-suppress unusedFunction
RGBQUAD* SVBitmapInfo::GetColorTable() const
{
	RGBQUAD* l_pColorTable = nullptr;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_pColorTable = l_pBitmapInfo->bmiColors;
	}

	return l_pColorTable;
}

SVBitmapInfo::SVPaletteTable SVBitmapInfo::GetPaletteTable() const
{
	SVPaletteTable l_PaletteTable;

	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		size_t l_Size = l_pBitmapInfo->bmiHeader.biClrUsed;
		l_PaletteTable.resize( l_Size );

		for( size_t i = 0; i < l_Size; ++i )
		{
			RGBQUAD& l_rColor = l_pBitmapInfo->bmiColors[ i ];
			PALETTEENTRY& l_rPalette = l_PaletteTable[ i ];

			l_rPalette.peBlue  = l_rColor.rgbBlue;
			l_rPalette.peGreen = l_rColor.rgbGreen;
			l_rPalette.peRed   = l_rColor.rgbRed;
			l_rPalette.peFlags = 0;
		}
	}

	return l_PaletteTable;
}

BITMAPINFO* SVBitmapInfo::GetBitmapInfo()
{
	return reinterpret_cast< BITMAPINFO* >( m_pBuffer );
}

const BITMAPINFO* SVBitmapInfo::GetBitmapInfo() const
{
	return reinterpret_cast< BITMAPINFO* >( m_pBuffer );
}

void SVBitmapInfo::FlipHeight()
{
	if( nullptr != m_pBuffer )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_pBitmapInfo->bmiHeader.biHeight = -l_pBitmapInfo->bmiHeader.biHeight;
	}
}

void SVBitmapInfo::Assign( const BITMAPINFO& p_rObject )
{
	size_t l_ColorCount = p_rObject.bmiHeader.biClrUsed;

	if( l_ColorCount == 0 )
	{
		l_ColorCount = GetColorTableSize( p_rObject.bmiHeader.biBitCount, p_rObject.bmiHeader.biCompression );
	}

	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

	if( nullptr != m_pBuffer && m_BufferSize < l_Size )
	{
		clear();
	}

	if( nullptr == m_pBuffer )
	{
		m_pBuffer = new unsigned char[ l_Size ];
		m_BufferSize = l_Size;
	}

	::memcpy( m_pBuffer, &p_rObject, l_Size );
}

void SVBitmapInfo::Assign( long width, long height, int bitCount, const SVColorTable& rColorTable )
{
	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( rColorTable.size() * sizeof( RGBQUAD ) );

	if( nullptr != m_pBuffer && m_BufferSize < l_Size )
	{
		clear();
	}

	if( nullptr == m_pBuffer )
	{
		m_pBuffer = new unsigned char[ l_Size ];
		m_BufferSize = l_Size;
	}

	::memset( m_pBuffer, 0, l_Size );

	BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

	l_pBitmapInfo->bmiHeader.biSize = sizeof( l_pBitmapInfo->bmiHeader ); 
	l_pBitmapInfo->bmiHeader.biWidth = width;
	l_pBitmapInfo->bmiHeader.biHeight = height;
	l_pBitmapInfo->bmiHeader.biPlanes = 1;
	l_pBitmapInfo->bmiHeader.biBitCount = static_cast<WORD> (bitCount);
	l_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	l_pBitmapInfo->bmiHeader.biSizeImage = GetImageSizeInBytes( width, height, bitCount ); 
	l_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	l_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	l_pBitmapInfo->bmiHeader.biClrUsed = static_cast< unsigned long >( rColorTable.size() ); 
	l_pBitmapInfo->bmiHeader.biClrImportant = 0;

	for( size_t i = 0; i < l_pBitmapInfo->bmiHeader.biClrUsed; ++i )
	{
		l_pBitmapInfo->bmiColors[ i ] = rColorTable[ i ];
	}
}

const SVBitmapInfo& SVBitmapInfo::operator=( const SVBitmapInfo& rObject )
{
	if( this != &rObject )
	{
		if( nullptr != rObject.m_pBuffer )
		{
			Assign( *( rObject.GetBitmapInfo() ) );
		}
		else
		{
			clear();
		}
	}

	return *this;
}

bool SVBitmapInfo::operator==( const SVBitmapInfo& rObject ) const
{
	bool l_Status = true;

	const BITMAPINFO* l_pLeftInfo = GetBitmapInfo();
	const BITMAPINFO* l_pRightInfo = rObject.GetBitmapInfo();

	l_Status = l_Status && ( nullptr != l_pLeftInfo ) && ( nullptr != l_pRightInfo );

	if( l_Status )
	{
		size_t l_ColorCount = l_pLeftInfo->bmiHeader.biClrUsed;

		if( 0 == l_ColorCount )
		{
			l_ColorCount = GetColorTableSize( l_pLeftInfo->bmiHeader.biBitCount, l_pLeftInfo->bmiHeader.biCompression );
		}

		size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

		l_Status = ( ::memcmp( l_pLeftInfo, l_pRightInfo, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( l_pLeftInfo->bmiHeader.biWidth == l_pRightInfo->bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( l_pLeftInfo->bmiHeader.biHeight ) == ::labs( l_pRightInfo->bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( l_pLeftInfo->bmiHeader.biBitCount == l_pRightInfo->bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVBitmapInfo::operator==( const BITMAPINFO& rObject ) const
{
	bool l_Status = true;

	size_t l_ColorCount = rObject.bmiHeader.biClrUsed;

	if( 0 == l_ColorCount )
	{
		l_ColorCount = GetColorTableSize( rObject.bmiHeader.biBitCount, rObject.bmiHeader.biCompression );
	}

	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

	const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( m_pBuffer );

	l_Status = l_Status && ( nullptr != l_pBitmapInfo );

	if( l_Status )
	{
		l_Status = ( ::memcmp( l_pBitmapInfo, &rObject, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biWidth == rObject.bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( l_pBitmapInfo->bmiHeader.biHeight ) == ::labs( rObject.bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biBitCount == rObject.bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVBitmapInfo::operator==( const BITMAPINFOHEADER& rObject ) const
{
	bool l_Status = true;

	size_t l_Size = sizeof( BITMAPINFOHEADER );

	const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( m_pBuffer );

	l_Status = l_Status && ( nullptr != l_pBitmapInfo );

	if( l_Status )
	{
		l_Status = ( 0 == ::memcmp( l_pBitmapInfo, &rObject, l_Size ) );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biWidth == rObject.biWidth );
			l_Temp = l_Temp && ( ::labs( l_pBitmapInfo->bmiHeader.biHeight ) == ::labs( rObject.biHeight ) );
			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biBitCount == rObject.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVBitmapInfo::operator!=( const SVBitmapInfo& rObject ) const
{
	return !( *this == rObject );
}

bool SVBitmapInfo::operator!=( const BITMAPINFO& rObject ) const
{
	return !( *this == rObject );
}

bool SVBitmapInfo::operator!=( const BITMAPINFOHEADER& rObject ) const
{
	return !( *this == rObject );
}

bool operator==( const BITMAPINFO& rLhs, const SVBitmapInfo& rRhs )
{
	bool l_Status = true;

	size_t l_ColorCount = rLhs.bmiHeader.biClrUsed;

	if( 0 == l_ColorCount )
	{
		l_ColorCount = SVBitmapInfo::GetColorTableSize( rLhs.bmiHeader.biBitCount, rLhs.bmiHeader.biCompression );
	}

	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

	const BITMAPINFO* l_pRightInfo = rRhs.GetBitmapInfo();

	l_Status = l_Status && ( nullptr != l_pRightInfo );

	if( l_Status )
	{
		l_Status = l_Status && ( ::memcmp( &rLhs, l_pRightInfo, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( rLhs.bmiHeader.biWidth == l_pRightInfo->bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( rLhs.bmiHeader.biHeight ) == ::labs( l_pRightInfo->bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( rLhs.bmiHeader.biBitCount == l_pRightInfo->bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool operator==( const BITMAPINFOHEADER& rLhs, const SVBitmapInfo& rRhs )
{
	bool l_Status = true;

	size_t l_Size = sizeof( BITMAPINFOHEADER );

	const BITMAPINFO* l_pRightInfo = rRhs.GetBitmapInfo();

	l_Status = l_Status && ( nullptr != l_pRightInfo );

	if( l_Status )
	{
		l_Status = l_Status && ( ::memcmp( &rLhs, l_pRightInfo, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( rLhs.biWidth == l_pRightInfo->bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( rLhs.biHeight ) == ::labs( l_pRightInfo->bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( rLhs.biBitCount == l_pRightInfo->bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

