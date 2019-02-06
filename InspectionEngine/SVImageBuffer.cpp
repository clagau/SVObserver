//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBuffer
//* .File Name       : $Workfile:   SVImageBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageBuffer.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMAGEBUFFER_BUFFERSIZE(a)				( * ( (long *) ( ( (BYTE*)a ) + 0 ) ) )
#define IMAGEBUFFER_OVERLAYCOUNT(a)				( * ( (long *) ( ( (BYTE*)a ) + 4 ) ) )
#define IMAGEBUFFER_OVERLAYOFFSETX(a,b)			( * ( (long *) ( ( (BYTE*)a ) + 8 + ( 4 * b ) ) ) )
#define IMAGEBUFFER_OVERLAYX(a,b)				( (BYTE *) ( ( (BYTE*)a ) + 8 + IMAGEBUFFER_OVERLAYOFFSETX(a,b) ) )

#define IMAGEBUFFER_OVERLAYTEXTCOLOR(a)			( * ( (COLORREF *) ( ( (BYTE*)a ) + 0 ) ) )
#define IMAGEBUFFER_OVERLAYTEXTPOINT(a)			( * ( (POINT *) ( ( (BYTE*)a ) + 4 ) ) )
#define IMAGEBUFFER_OVERLAYTEXTSIZE(a)			( * ( (long *) ( ( (BYTE*)a ) + 12 ) ) )
#define IMAGEBUFFER_OVERLAYTEXT(a)				( (char *) ( ( (BYTE*)a ) + 16 ) )
#define IMAGEBUFFER_OVERLAYFIGURESCOUNT(a)		( * ( (long *) ( ( (BYTE*)a ) + 16 + IMAGEBUFFER_OVERLAYTEXTSIZE(a) ) ) )
#define IMAGEBUFFER_OVERLAYFIGURESOFFSETX(a,b)	( * ( (long *) ( ( (BYTE*)a ) + 20 + ( 4 * b ) + IMAGEBUFFER_OVERLAYTEXTSIZE(a) ) ) )
#define IMAGEBUFFER_OVERLAYFIGURESX(a,b)		( (BYTE *) ( ( (BYTE*)a ) + IMAGEBUFFER_OVERLAYFIGURESOFFSETX(a,b) + 20 + IMAGEBUFFER_OVERLAYTEXTSIZE(a) ) )

#define IMAGEBUFFER_OVERLAYFIGURECOLOR(a)		( * ( (COLORREF *) ( ( (BYTE*)a ) + 0 ) ) )
#define IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT(a)	( * ( (long *) ( ( (BYTE*)a ) + 4 ) ) )
#define IMAGEBUFFER_OVERLAYFIGUREPOINTS(a)		( (POINT *) ( ( (BYTE*)a ) + 8  ) )

#define IMAGEBUFFER_LASTOVERLAY(a)				( (BYTE*) ( IMAGEBUFFER_OVERLAYX(a, (IMAGEBUFFER_OVERLAYCOUNT(a) - 1) ) ) )
#define IMAGEBUFFER_LASTOVERLAYFIGURE(a)		( (BYTE*) ( IMAGEBUFFER_OVERLAYFIGURESX( IMAGEBUFFER_LASTOVERLAY(a), (IMAGEBUFFER_OVERLAYFIGURESCOUNT( IMAGEBUFFER_LASTOVERLAY(a) ) - 1) ) ) )
#define IMAGEBUFFER_CALCBUFFERSIZE(a)			( ( (long) ( IMAGEBUFFER_LASTOVERLAYFIGURE(a) - a ) ) + 8 + IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT(IMAGEBUFFER_LASTOVERLAYFIGURE(a) ) * sizeof( POINT ) )

static const long cDefaultCalculatedSize = 8;
#pragma endregion Declarations

// Construction section
SVImageOverlayClass::SVImageOverlayClass()
{
	Init();
}// end ctor

SVImageOverlayClass::SVImageOverlayClass(const SVExtentMultiLineStructVector& rMultiLineArrayStruct)
{
	Init();

	*this = rMultiLineArrayStruct;
}

void SVImageOverlayClass::Init()
{
	m_lBufferSize	= 0;
	m_pBuffer		= nullptr;
}

SVImageOverlayClass::SVImageOverlayClass( const SVImageOverlayClass& rRhs )
{
	Init();
	*this = rRhs;
}

const SVImageOverlayClass& SVImageOverlayClass::operator = ( const SVImageOverlayClass& rRhs )
{
	SetBufferSize( rRhs.GetBufferSize(), false );

	memcpy( m_pBuffer, rRhs.m_pBuffer, GetBufferSize() );

	return *this;
}

const SVImageOverlayClass& SVImageOverlayClass::operator = (const SVExtentMultiLineStructVector& rMultiLineArrayStruct)
{
	Clear();

	long l_Size = 131072;	// WHY THIS MAGIC NUMBER???
	long l_BufferCount = static_cast<long> (rMultiLineArrayStruct.size());
	SetBufferSize(l_Size);
	SetOverlayCount(l_BufferCount);
	// fill in the overlaystruct
	SVExtentMultiLineStruct l_MultiLineStruct;

	for ( long l = 0; l < l_BufferCount; l++ )
	{
		SVOverlayStruct OverlayStruct( rMultiLineArrayStruct[l] );
		SetOverlayFigureAt(l, OverlayStruct);
	}
	return *this;
}

SVImageOverlayClass::~SVImageOverlayClass()
{
	Clear();
}// end dtor

long SVImageOverlayClass::CalculateSize()
{
	long Result( cDefaultCalculatedSize );
	if( IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) )
	{
		Result = IMAGEBUFFER_CALCBUFFERSIZE( m_pBuffer );
	}

	return Result;
}// end CalculateSize

/////////////////////////////////////////////////////////////////////
// *************************************************************** //
// Getting Section                                                 //
// *************************************************************** //
/////////////////////////////////////////////////////////////////////

// Overlay section
long SVImageOverlayClass::GetOverlayCount() const
{
	long Result = IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );

	return Result;
}

void SVImageOverlayClass::GetOverlayFigureAt( long Index, SVOverlayStruct& rOverlay )
{
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, Index );

	rOverlay.m_TextColor = IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes );
	rOverlay.m_TextPoint = IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes );
	rOverlay.m_Text	= IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes );
	rOverlay.m_Text	= SvUl::Left( rOverlay.m_Text,  IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) );

	rOverlay.lFigureSize = IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes );
	rOverlay.pOverlays = nullptr;

	if( rOverlay.lFigureSize )
		rOverlay.pOverlays = new SVOverlayFigureStruct[rOverlay.lFigureSize];

	for( l = 0; l < rOverlay.lFigureSize; l++ )
	{
		pFigureBytes = IMAGEBUFFER_OVERLAYFIGURESX( pOverlayBytes, l );

		rOverlay.pOverlays[l].oFigureColor = IMAGEBUFFER_OVERLAYFIGURECOLOR( pFigureBytes );
		rOverlay.pOverlays[l].lFigureSize = IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT( pFigureBytes );
		rOverlay.pOverlays[l].pFigurePoints = new POINT[rOverlay.pOverlays[l].lFigureSize];

		memcpy( rOverlay.pOverlays[l].pFigurePoints, IMAGEBUFFER_OVERLAYFIGUREPOINTS( pFigureBytes ), sizeof( POINT ) * rOverlay.pOverlays[l].lFigureSize );
	}// end for
}// end GetOverlayFigureAt

/////////////////////////////////////////////////////////////////////
// *************************************************************** //
// Setting Section                                                 //
// *************************************************************** //
/////////////////////////////////////////////////////////////////////

// General and header section
void SVImageOverlayClass::SetBufferSize( long Size, bool ClearBuffer )
{
	if ( Size > 0 )
	{
		if ( Size != m_lBufferSize )
		{
			BYTE *pBuffer;

			pBuffer = new BYTE[Size];
			if ( ClearBuffer )
			{
				memset( pBuffer, 0, Size );
			}

			if( m_pBuffer )
			{
				memcpy( pBuffer, m_pBuffer, __min( m_lBufferSize, Size ) );
			}

			if ( nullptr != m_pBuffer )
			{
				delete [] m_pBuffer;
			}
			
			m_pBuffer = pBuffer;
			m_lBufferSize = Size;
		}

		IMAGEBUFFER_BUFFERSIZE( m_pBuffer ) = m_lBufferSize;
	}
	else	// size 0
	{
		m_pBuffer = nullptr;
		m_lBufferSize = 0;
	}
}// end SetBufferSize

// Overlay section
void SVImageOverlayClass::SetOverlayCount( long Count )
{
	IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) = Count;
}// end SetOverlayCount

void SVImageOverlayClass::SetOverlayFigureAt( long Index, SVOverlayStruct& rOverlay )
{
	long lSize1;
	long lSize2;
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	if( Index == 0 )
	{
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, Index ) = 4 * IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );
	}
		
	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, Index );

	IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes ) = rOverlay.m_TextColor;
	IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes ) = rOverlay.m_TextPoint;
	IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) = static_cast<long> (rOverlay.m_Text.size());
	memcpy( IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes ), rOverlay.m_Text.c_str(), static_cast<long> (rOverlay.m_Text.size()) );

	IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes )  = rOverlay.lFigureSize;

	lSize1 = 20 + static_cast<long> (rOverlay.m_Text.size());
	for( l = 0; l < rOverlay.lFigureSize; l++ )
	{
		if( l == 0 )
		{
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l ) = 4 * IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes );
		}

		pFigureBytes = IMAGEBUFFER_OVERLAYFIGURESX( pOverlayBytes, l );

		IMAGEBUFFER_OVERLAYFIGURECOLOR( pFigureBytes ) = rOverlay.pOverlays[l].oFigureColor;
		IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT( pFigureBytes ) = rOverlay.pOverlays[l].lFigureSize;
		memcpy( IMAGEBUFFER_OVERLAYFIGUREPOINTS( pFigureBytes ), rOverlay.pOverlays[l].pFigurePoints, sizeof( POINT ) * rOverlay.pOverlays[l].lFigureSize );

		lSize2 = 8 + ( sizeof( POINT ) * rOverlay.pOverlays[l].lFigureSize );
		lSize1 += lSize2 + 4;

		if( l < rOverlay.lFigureSize - 1 )
		{
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, (l + 1) ) = lSize2 + IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l );
		}
	}// end for

	if( Index < IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) - 1 )
	{
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, (Index + 1) ) = lSize1 + IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, Index );
	}
}// end SetOverlayFigureAt

void SVImageOverlayClass::Clear()
{
	m_lBufferSize	= 0;
	if( nullptr != m_pBuffer )
	{
		delete [] m_pBuffer;
		m_pBuffer = nullptr;
	}
}

} //namespace SvIe
