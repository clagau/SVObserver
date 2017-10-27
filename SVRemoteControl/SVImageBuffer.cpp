//******************************************************************************
//*                     I M P L E M E N T A T I O N F I L E
//* COPYRIGHT (c) 1997 by Seidenader Maschinenbau GmbH, München
//* COPYRIGHT (c) 1998 by SVResearch Gesellschaft für Bildverarbeitung mbH, München
//* COPYRIGHT (c) 1999,2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* HEADER:
//* ============================================================================
//* .Module Name     : SVImageBuffer
//* .File Name       : $Workfile:   SVImageBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Module Type     : Data Type Module
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:35:10  $
//* ----------------------------------------------------------------------------

#include "stdafx.h"
#include "SVImageBuffer.h"

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
SVImageBufferClass::SVImageBufferClass()
{
	m_pBuffer		= NULL;
	m_lBufferSize	= 0;

	m_lOverlaySize	= 0;
	m_ppOverlays	= NULL;
}// end ctor

SVImageBufferClass::~SVImageBufferClass()
{
	delete [] m_pBuffer;
	m_pBuffer		= NULL;
	m_lBufferSize	= 0;

	m_lOverlaySize	= 0;
	m_ppOverlays	= NULL;
}// end dtor


BOOL SVImageBufferClass::CalculateSize( long &lSize ) const
{
	lSize = IMAGEBUFFER_CALCBUFFERSIZE( m_pBuffer );

	return TRUE;
}// end CalculateSize

BOOL SVImageBufferClass::Clear()
{
	delete [] m_pBuffer;
	m_pBuffer		= NULL;
	m_lBufferSize	= 0;

	m_lOverlaySize	= 0;
	m_ppOverlays	= NULL;

	return TRUE;
}// end Clear

/////////////////////////////////////////////////////////////////////
// *************************************************************** //
// Getting Section                                                 //
// *************************************************************** //
/////////////////////////////////////////////////////////////////////

// General and header section
BOOL SVImageBufferClass::GetBufferSize( long &lSize ) const
{
	lSize = m_lBufferSize;

	return TRUE;
}// end GetBufferSize

BOOL SVImageBufferClass::GetBuffer( BYTE *&pBuffer ) const
{
	pBuffer = m_pBuffer;

	return TRUE;
}// end GetBuffer

// Overlay section
BOOL SVImageBufferClass::GetOverlayCount( long &lCount ) const
{
	if( m_pBuffer )
		lCount = IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );
	else
		lCount = 0;

	return TRUE;
}// end GetOverlayCount

BOOL SVImageBufferClass::GetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay ) const
{
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, lIndex );

	oOverlay.oTextColor = IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes );
	oOverlay.oTextPoint = IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes );
	oOverlay.strText    = IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes );
	oOverlay.strText    = oOverlay.strText.substr(0, ( IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) ));

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
BOOL SVImageBufferClass::SetBufferSize( long &lSize )
{
	BYTE *pBuffer;

	pBuffer = new BYTE[lSize];

	if( m_pBuffer )
		memcpy( pBuffer, m_pBuffer, __min( m_lBufferSize, lSize ) );

	delete m_pBuffer;
	
	m_pBuffer = pBuffer;
	m_lBufferSize = lSize;

	IMAGEBUFFER_BUFFERSIZE( m_pBuffer ) = m_lBufferSize;

	return TRUE;
}// end SetBufferSize

BOOL SVImageBufferClass::SetBuffer( BYTE *&pBuffer, long &lSize )
{
	m_pBuffer = pBuffer;
	m_lBufferSize = lSize;

	return TRUE;
}// end SetBuffer

BOOL SVImageBufferClass::SetBufferCopy( BYTE *&pBuffer, long &lSize )
{
	m_pBuffer = new BYTE[lSize];
	memcpy( m_pBuffer, pBuffer, lSize );
	m_lBufferSize = lSize;

	return TRUE;
}// end SetBufferCopy

// Overlay section
BOOL SVImageBufferClass::SetOverlayCount( long &lCount )
{
	IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) = lCount;

	return TRUE;
}// end SetOverlayCount

BOOL SVImageBufferClass::SetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay )
{
	long lSize1;
	long lSize2;
	long l;
	BYTE *pOverlayBytes;
	BYTE *pFigureBytes;

	if( lIndex == 0 )
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, lIndex ) = 4 * IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer );
		
	pOverlayBytes = IMAGEBUFFER_OVERLAYX( m_pBuffer, lIndex );

	IMAGEBUFFER_OVERLAYTEXTCOLOR( pOverlayBytes ) = oOverlay.oTextColor;
	IMAGEBUFFER_OVERLAYTEXTPOINT( pOverlayBytes ) = oOverlay.oTextPoint;
	IMAGEBUFFER_OVERLAYTEXTSIZE( pOverlayBytes ) = oOverlay.strText.length();
	memcpy( IMAGEBUFFER_OVERLAYTEXT( pOverlayBytes ), (LPCTSTR) oOverlay.strText.c_str(), oOverlay.strText.length() );

	IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes )  = oOverlay.lFigureSize;

	lSize1 = 20 + oOverlay.strText.length();
	for( l = 0; l < oOverlay.lFigureSize; l++ )
	{
		if( l == 0 )
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l ) = 4 * IMAGEBUFFER_OVERLAYFIGURESCOUNT( pOverlayBytes );

		pFigureBytes = IMAGEBUFFER_OVERLAYFIGURESX( pOverlayBytes, l );

		IMAGEBUFFER_OVERLAYFIGURECOLOR( pFigureBytes ) = oOverlay.pOverlays[l].oFigureColor;
		IMAGEBUFFER_OVERLAYFIGUREPOINTSCOUNT( pFigureBytes ) = oOverlay.pOverlays[l].lFigureSize;
		memcpy( IMAGEBUFFER_OVERLAYFIGUREPOINTS( pFigureBytes ), oOverlay.pOverlays[l].pFigurePoints, sizeof( POINT ) * oOverlay.pOverlays[l].lFigureSize );

		lSize2 = 8 + ( sizeof( POINT ) * oOverlay.pOverlays[l].lFigureSize );
		lSize1 += lSize2 + 4;

		if( l < oOverlay.lFigureSize - 1 )
			IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, (l + 1) ) = lSize2 + IMAGEBUFFER_OVERLAYFIGURESOFFSETX( pOverlayBytes, l );
	}// end for

	if( lIndex < IMAGEBUFFER_OVERLAYCOUNT( m_pBuffer ) - 1 )
		IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, (lIndex + 1) ) = lSize1 + IMAGEBUFFER_OVERLAYOFFSETX( m_pBuffer, lIndex );

	return TRUE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************

/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteControl\SVImageBuffer.cpp_v  $
 * 
 *    Rev 1.0   14 May 2013 14:35:10   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
