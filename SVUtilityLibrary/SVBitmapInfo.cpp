//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBitmapInfo
//* .File Name       : $Workfile:   SVBitmapInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVBitmapInfo.h"

SVBitmapInfo::SVColorTable SVBitmapInfo::GetDefaultColorTable( unsigned short p_BitCount )
{
	SVColorTable l_Colors;

	switch( p_BitCount )
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

unsigned long SVBitmapInfo::GetColorTableSize( unsigned short p_BitCount, unsigned long p_Compression )
{
	unsigned long l_Size = 0;

	switch( p_BitCount )
	{
		case 1:
		{
			l_Size = 1 << p_BitCount;

			break;
		}
		case 2:
		{
			l_Size = 1 << p_BitCount;

			break;
		}
		case 4:
		{
			l_Size = 1 << p_BitCount;

			break;
		}
		case 8:
		{
			l_Size = 1 << p_BitCount;

			break;
		}
		case 16:
		{
			switch( p_Compression )
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
		case 32:
		{
			switch( p_Compression )
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
	}

	return l_Size;
}

unsigned long SVBitmapInfo::GetImageStrideInBytes( long p_Width, unsigned short p_BitCount )
{
	unsigned long l_Stride = 0;
	unsigned long l_Width = ::labs( p_Width );
	unsigned long l_BitCount = p_BitCount;

	if( l_Width != 0 && l_BitCount != 0 )
	{
		l_Stride = l_Width;           //Start with Image Width
		l_Stride *= l_BitCount / 8;   //Multiply width by byte count of pixel
		l_Stride += 3;                //Add 3 bytes to round up to the next 4 byte alignment
		l_Stride = l_Stride & ~3;     //Clear bits 0 and 1 (~3 == 0xFFFFFFFC) to remove extra bytes above 4 byte alignment
	}

	return l_Stride;
}

unsigned long SVBitmapInfo::GetImageSizeInBytes( long p_Width, long p_Height, unsigned short p_BitCount )
{
	unsigned long l_Size = ::labs( p_Height ) * GetImageStrideInBytes( p_Width, p_BitCount );

	return l_Size;
}

SVBitmapInfo::SVBitmapInfo()
:	m_BufferSize( 0 )
, m_pBuffer( NULL )
{
}

SVBitmapInfo::SVBitmapInfo( const SVBitmapInfo& p_rObject )
:	m_BufferSize( 0 )
, m_pBuffer( NULL )
{
	if( p_rObject.m_pBuffer != NULL )
	{
		Assign( *( p_rObject.GetBitmapInfo() ) );
	}
}

SVBitmapInfo::SVBitmapInfo( const BITMAPINFO& p_rObject )
:	m_BufferSize( 0 )
, m_pBuffer( NULL )
{
	Assign( p_rObject );
}

SVBitmapInfo::SVBitmapInfo( long p_Width, long p_Height, unsigned short p_BitCount, const SVColorTable& p_rColorTable )
:	m_BufferSize( 0 )
, m_pBuffer( NULL )
{
	Assign( p_Width, p_Height, p_BitCount, p_rColorTable );
}

SVBitmapInfo::~SVBitmapInfo()
{
	clear();
}

bool SVBitmapInfo::empty() const
{
	return ( m_pBuffer == NULL );
}

void SVBitmapInfo::clear()
{
	if( m_pBuffer != NULL )
	{
		delete[] m_pBuffer;

		m_BufferSize = 0;
		m_pBuffer = NULL;
	}
}

size_t SVBitmapInfo::GetBitmapInfoSizeInBytes() const
{
	size_t l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		size_t l_ColorCount = l_pBitmapInfo->bmiHeader.biClrUsed;

		if( l_ColorCount == 0 )
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

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = GetImageSizeInBytes( l_pBitmapInfo->bmiHeader.biWidth, l_pBitmapInfo->bmiHeader.biHeight, l_pBitmapInfo->bmiHeader.biBitCount );
	}

	return l_Size;
}

size_t SVBitmapInfo::GetBitmapImageStrideInBytes() const
{
	size_t l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = GetImageStrideInBytes( l_pBitmapInfo->bmiHeader.biWidth, l_pBitmapInfo->bmiHeader.biBitCount );
	}

	return l_Size;
}


long SVBitmapInfo::GetWidth() const
{
	long l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biWidth;
	}

	return l_Size;
}

long SVBitmapInfo::GetHeight() const
{
	long l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biHeight;
	}

	return l_Size;
}

unsigned short SVBitmapInfo::GetBitCount() const
{
	unsigned short l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biBitCount;
	}

	return l_Size;
}

unsigned long SVBitmapInfo::GetColorTableSize() const
{
	unsigned long l_Size = 0;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_Size = l_pBitmapInfo->bmiHeader.biClrUsed;
	}

	return l_Size;
}

RGBQUAD* SVBitmapInfo::GetColorTable() const
{
	RGBQUAD* l_pColorTable = NULL;

	if( m_pBuffer != NULL )
	{
		BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

		l_pColorTable = l_pBitmapInfo->bmiColors;
	}

	return l_pColorTable;
}

SVBitmapInfo::SVPaletteTable SVBitmapInfo::GetPaletteTable() const
{
	SVPaletteTable l_PaletteTable;

	if( m_pBuffer != NULL )
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
	if( m_pBuffer != NULL )
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

	if( m_pBuffer != NULL && m_BufferSize < l_Size )
	{
		clear();
	}

	if( m_pBuffer == NULL )
	{
		m_pBuffer = new unsigned char[ l_Size ];
		m_BufferSize = l_Size;
	}

	::memcpy( m_pBuffer, &p_rObject, l_Size );
}

void SVBitmapInfo::Assign( long p_Width, long p_Height, unsigned short p_BitCount, const SVColorTable& p_rColorTable )
{
	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( p_rColorTable.size() * sizeof( RGBQUAD ) );

	if( m_pBuffer != NULL && m_BufferSize < l_Size )
	{
		clear();
	}

	if( m_pBuffer == NULL )
	{
		m_pBuffer = new unsigned char[ l_Size ];
		m_BufferSize = l_Size;
	}

	::memset( m_pBuffer, 0, l_Size );

	BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( m_pBuffer );

	l_pBitmapInfo->bmiHeader.biSize = sizeof( l_pBitmapInfo->bmiHeader ); 
	l_pBitmapInfo->bmiHeader.biWidth = p_Width;
	l_pBitmapInfo->bmiHeader.biHeight = p_Height;
	l_pBitmapInfo->bmiHeader.biPlanes = 1;
	l_pBitmapInfo->bmiHeader.biBitCount = p_BitCount;
	l_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	l_pBitmapInfo->bmiHeader.biSizeImage = GetImageSizeInBytes( p_Width, p_Height, p_BitCount ); 
	l_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	l_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	l_pBitmapInfo->bmiHeader.biClrUsed = static_cast< unsigned long >( p_rColorTable.size() ); 
	l_pBitmapInfo->bmiHeader.biClrImportant = 0;

	for( size_t i = 0; i < l_pBitmapInfo->bmiHeader.biClrUsed; ++i )
	{
		l_pBitmapInfo->bmiColors[ i ] = p_rColorTable[ i ];
	}
}

const SVBitmapInfo& SVBitmapInfo::operator=( const SVBitmapInfo& p_rObject )
{
	if( this != &p_rObject )
	{
		if( p_rObject.m_pBuffer != NULL )
		{
			Assign( *( p_rObject.GetBitmapInfo() ) );
		}
		else
		{
			clear();
		}
	}

	return *this;
}

bool SVBitmapInfo::operator==( const SVBitmapInfo& p_rObject ) const
{
	bool l_Status = true;

	const BITMAPINFO* l_pLeftInfo = GetBitmapInfo();
	const BITMAPINFO* l_pRightInfo = p_rObject.GetBitmapInfo();

	l_Status = l_Status && ( l_pLeftInfo != NULL ) && ( l_pRightInfo != NULL );

	if( l_Status )
	{
		size_t l_ColorCount = l_pLeftInfo->bmiHeader.biClrUsed;

		if( l_ColorCount == 0 )
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

bool SVBitmapInfo::operator==( const BITMAPINFO& p_rObject ) const
{
	bool l_Status = true;

	size_t l_ColorCount = p_rObject.bmiHeader.biClrUsed;

	if( l_ColorCount == 0 )
	{
		l_ColorCount = GetColorTableSize( p_rObject.bmiHeader.biBitCount, p_rObject.bmiHeader.biCompression );
	}

	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

	const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( m_pBuffer );

	l_Status = l_Status && ( l_pBitmapInfo != NULL );

	if( l_Status )
	{
		l_Status = ( ::memcmp( l_pBitmapInfo, &p_rObject, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biWidth == p_rObject.bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( l_pBitmapInfo->bmiHeader.biHeight ) == ::labs( p_rObject.bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biBitCount == p_rObject.bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVBitmapInfo::operator==( const BITMAPINFOHEADER& p_rObject ) const
{
	bool l_Status = true;

	size_t l_Size = sizeof( BITMAPINFOHEADER );

	const BITMAPINFO* l_pBitmapInfo = reinterpret_cast< const BITMAPINFO* >( m_pBuffer );

	l_Status = l_Status && ( l_pBitmapInfo != NULL );

	if( l_Status )
	{
		l_Status = ( ::memcmp( l_pBitmapInfo, &p_rObject, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biWidth == p_rObject.biWidth );
			l_Temp = l_Temp && ( ::labs( l_pBitmapInfo->bmiHeader.biHeight ) == ::labs( p_rObject.biHeight ) );
			l_Temp = l_Temp && ( l_pBitmapInfo->bmiHeader.biBitCount == p_rObject.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVBitmapInfo::operator!=( const SVBitmapInfo& p_rObject ) const
{
	return !( *this == p_rObject );
}

bool SVBitmapInfo::operator!=( const BITMAPINFO& p_rObject ) const
{
	return !( *this == p_rObject );
}

bool SVBitmapInfo::operator!=( const BITMAPINFOHEADER& p_rObject ) const
{
	return !( *this == p_rObject );
}

bool operator==( const BITMAPINFO& p_rLeft, const SVBitmapInfo& p_rRight )
{
	bool l_Status = true;

	size_t l_ColorCount = p_rLeft.bmiHeader.biClrUsed;

	if( l_ColorCount == 0 )
	{
		l_ColorCount = SVBitmapInfo::GetColorTableSize( p_rLeft.bmiHeader.biBitCount, p_rLeft.bmiHeader.biCompression );
	}

	size_t l_Size = sizeof( BITMAPINFOHEADER ) + ( l_ColorCount * sizeof( RGBQUAD ) );

	const BITMAPINFO* l_pRightInfo = p_rRight.GetBitmapInfo();

	l_Status = l_Status && ( l_pRightInfo != NULL );

	if( l_Status )
	{
		l_Status = l_Status && ( ::memcmp( &p_rLeft, l_pRightInfo, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( p_rLeft.bmiHeader.biWidth == l_pRightInfo->bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( p_rLeft.bmiHeader.biHeight ) == ::labs( l_pRightInfo->bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( p_rLeft.bmiHeader.biBitCount == l_pRightInfo->bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool operator==( const BITMAPINFOHEADER& p_rLeft, const SVBitmapInfo& p_rRight )
{
	bool l_Status = true;

	size_t l_Size = sizeof( BITMAPINFOHEADER );

	const BITMAPINFO* l_pRightInfo = p_rRight.GetBitmapInfo();

	l_Status = l_Status && ( l_pRightInfo != NULL );

	if( l_Status )
	{
		l_Status = l_Status && ( ::memcmp( &p_rLeft, l_pRightInfo, l_Size ) == 0 );

		if( !l_Status )
		{
			bool l_Temp = true;

			l_Temp = l_Temp && ( p_rLeft.biWidth == l_pRightInfo->bmiHeader.biWidth );
			l_Temp = l_Temp && ( ::labs( p_rLeft.biHeight ) == ::labs( l_pRightInfo->bmiHeader.biHeight ) );
			l_Temp = l_Temp && ( p_rLeft.biBitCount == l_pRightInfo->bmiHeader.biBitCount );

			l_Status = l_Temp;
		}
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVBitmapInfo.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:20:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Sep 2011 15:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added aditional methods for default bitmap information based on bitmap data elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2011 14:02:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

