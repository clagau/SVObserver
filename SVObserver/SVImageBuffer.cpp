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

#include "stdafx.h"
#include "SVImageBuffer.h"	//SVImageOverlayClass

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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

// Construction section
SVImageOverlayClass::SVImageOverlayClass()
{
	Init();
}// end ctor

SVImageOverlayClass::SVImageOverlayClass(const SVExtentMultiLineStructCArray& p_MultiLineArrayStruct)
{
	Init();

	*this = p_MultiLineArrayStruct;

	//IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) =  = p_MultiLineArrayStruct.GetSize();
	//set buffer size to lSize = 131072 //128K
}

void SVImageOverlayClass::Init()
{
	m_lBufferSize	= 0;
	m_pBuffer		= NULL;

	//m_lOverlaySize	= 0;
	//m_ppOverlays	= NULL;
}

SVImageOverlayClass::SVImageOverlayClass( const SVImageOverlayClass& rhs )
{
	Init();
	*this = rhs;
}

const SVImageOverlayClass& SVImageOverlayClass::operator = ( const SVImageOverlayClass& rhs )
{
	SetBufferSize( rhs.GetBufferSize(), false );

	memcpy( m_pBuffer, rhs.m_pBuffer, GetBufferSize() );

	return *this;
}

const SVImageOverlayClass& SVImageOverlayClass::operator = (const SVExtentMultiLineStructCArray& p_MultiLineArrayStruct)
{
	Clear();

	long l_Size = 131072;	// WHY THIS MAGIC NUMBER???
	long l_BufferCount = p_MultiLineArrayStruct.GetSize();
	SetBufferSize(l_Size);
	SetOverlayCount(l_BufferCount);
	// fill in the overlaystruct
	SVExtentMultiLineStruct l_MultiLineStruct;

	for ( long l = 0; l < l_BufferCount; l++ )
	{
		SVOverlayStruct oOverlayStruct( p_MultiLineArrayStruct.GetAt((int)l) );

		/*
		//fill in struct and add  SetOverlayFigureAt
		l_MultiLineStruct = p_MultiLineArrayStruct.GetAt((int)l);

		oOverlayStruct.oTextColor = l_MultiLineStruct.m_Color;
		oOverlayStruct.oTextPoint = l_MultiLineStruct.m_StringPoint.operator POINT();
		oOverlayStruct.strText = l_MultiLineStruct.m_csString;

		int iNumElements = l_MultiLineStruct.m_svLineArray.GetSize();
		oOverlayStruct.pOverlays = new SVOverlayFigureStruct[iNumElements];
		oOverlayStruct.lFigureSize = iNumElements;
		for ( int i = 0; i < iNumElements; i++ )
		{
			oOverlayStruct.pOverlays[i] = l_MultiLineStruct.m_svLineArray.GetAt(i);
			oOverlayStruct.pOverlays[i].oFigureColor = l_MultiLineStruct.m_Color;
		}
		*/
		SetOverlayFigureAt(l,oOverlayStruct);
	}

	return *this;
}

SVImageOverlayClass::~SVImageOverlayClass()
{
	Clear();
}// end dtor

BOOL SVImageOverlayClass::CalculateSize( long &lSize )
{
	if( IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) )
	{
		lSize = IMAGEBUFFER_CALCBUFFERSIZE( m_pBuffer );
	}// end if
	else
	{
		lSize = 8;
	}// end else

	return TRUE;
}// end CalculateSize

/////////////////////////////////////////////////////////////////////
// *************************************************************** //
// Getting Section                                                 //
// *************************************************************** //
/////////////////////////////////////////////////////////////////////

// General and header section
BOOL SVImageOverlayClass::GetBufferSize( long &lSize ) const
{
	lSize = m_lBufferSize;

	return TRUE;
}// end GetBufferSize

long SVImageOverlayClass::GetBufferSize() const
{
	return m_lBufferSize;
}

BOOL SVImageOverlayClass::GetBuffer( BYTE *&pBuffer )
{
	pBuffer = m_pBuffer;

	return TRUE;
}// end GetBuffer

// Overlay section
BOOL SVImageOverlayClass::GetOverlayCount( long &lCount ) const
{
	lCount = IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );

	return TRUE;
}// end GetOverlayCount

BOOL SVImageOverlayClass::GetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay )
{
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, lIndex );

	oOverlay.oTextColor = IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes );
	oOverlay.oTextPoint = IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes );
	oOverlay.strText	= IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes );
	oOverlay.strText	= oOverlay.strText.Left( IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) );

	oOverlay.lFigureSize = IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes );
	oOverlay.pOverlays = NULL;

	if( oOverlay.lFigureSize )
		oOverlay.pOverlays = new SVOverlayFigureStruct[oOverlay.lFigureSize];

	for( l = 0; l < oOverlay.lFigureSize; l++ )
	{
		pFigureBytes = IMAGEBUFFER_OVERLAYFIGURESX( pOverlayBytes, l );

		oOverlay.pOverlays[l].oFigureColor = IMAGEBUFFER_OVERLAYFIGURECOLOR( pFigureBytes );
		oOverlay.pOverlays[l].lFigureSize = IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT( pFigureBytes );
		oOverlay.pOverlays[l].pFigurePoints = new POINT[oOverlay.pOverlays[l].lFigureSize];

		memcpy( oOverlay.pOverlays[l].pFigurePoints, IMAGEBUFFER_OVERLAYFIGUREPOINTS( pFigureBytes ), sizeof( POINT ) * oOverlay.pOverlays[l].lFigureSize );
	}// end for

	return TRUE;
}// end GetOverlayFigureAt

/////////////////////////////////////////////////////////////////////
// *************************************************************** //
// Setting Section                                                 //
// *************************************************************** //
/////////////////////////////////////////////////////////////////////

// General and header section
BOOL SVImageOverlayClass::SetBufferSize( long lSize, bool bClearBuffer )
{
	if ( lSize > 0 )
	{
		if ( lSize != m_lBufferSize )
		{
			BYTE *pBuffer;

			pBuffer = new BYTE[lSize];
			if ( bClearBuffer )
				memset( pBuffer, 0, lSize );

			if( m_pBuffer )
				memcpy( pBuffer, m_pBuffer, __min( m_lBufferSize, lSize ) );

			if ( m_pBuffer )
				delete [] m_pBuffer;
			
			m_pBuffer = pBuffer;
			m_lBufferSize = lSize;
		}

		IMAGEBUFFER_BUFFERSIZE( m_pBuffer ) = m_lBufferSize;
	}
	else	// size 0
	{
		m_pBuffer = NULL;
		m_lBufferSize = 0;
	}

	return TRUE;
}// end SetBufferSize

/*
BOOL SVImageOverlayClass::SetBuffer( BYTE *&pBuffer, long &lSize )
{
	m_pBuffer = pBuffer;
	m_lBufferSize = lSize;

	return TRUE;
}// end SetBuffer
*/

// Overlay section
BOOL SVImageOverlayClass::SetOverlayCount( long &lCount )
{
	IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) = lCount;

	return TRUE;
}// end SetOverlayCount

BOOL SVImageOverlayClass::SetOverlayFigureAt( long &lIndex, SVOverlayStruct& roOverlay )
{
	long lSize1;
	long lSize2;
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	if( lIndex == 0 )
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, lIndex ) = 4 * IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );
		
	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, lIndex );

	IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes ) = roOverlay.oTextColor;
	IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes ) = roOverlay.oTextPoint;
	IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) = roOverlay.strText.GetLength();
	memcpy( IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes ), (LPCTSTR) roOverlay.strText, roOverlay.strText.GetLength() );

	IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes )  = roOverlay.lFigureSize;

	lSize1 = 20 + roOverlay.strText.GetLength();
	for( l = 0; l < roOverlay.lFigureSize; l++ )
	{
		if( l == 0 )
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l ) = 4 * IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes );

		pFigureBytes = IMAGEBUFFER_OVERLAYFIGURESX( pOverlayBytes, l );

		IMAGEBUFFER_OVERLAYFIGURECOLOR( pFigureBytes ) = roOverlay.pOverlays[l].oFigureColor;
		IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT( pFigureBytes ) = roOverlay.pOverlays[l].lFigureSize;
		memcpy( IMAGEBUFFER_OVERLAYFIGUREPOINTS( pFigureBytes ), roOverlay.pOverlays[l].pFigurePoints, sizeof( POINT ) * roOverlay.pOverlays[l].lFigureSize );

		lSize2 = 8 + ( sizeof( POINT ) * roOverlay.pOverlays[l].lFigureSize );
		lSize1 += lSize2 + 4;

		if( l < roOverlay.lFigureSize - 1 )
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, (l + 1) ) = lSize2 + IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l );
	}// end for

	if( lIndex < IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) - 1 )
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, (lIndex + 1) ) = lSize1 + IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, lIndex );

	return TRUE;
}// end SetOverlayFigureAt

void SVImageOverlayClass::Clear()
{
	m_lBufferSize	= 0;
	if( m_pBuffer != NULL )
	{
		delete [] m_pBuffer;
		m_pBuffer		= NULL;
	}

	//m_lOverlaySize	= 0;
	//m_ppOverlays = NULL;
}

