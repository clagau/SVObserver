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
//* .File Name       : $Workfile:   SVImageBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Module Type     : Data Type Module
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:35:10  $
//* ----------------------------------------------------------------------------

#pragma once

#pragma region Includes
#include <string>
#pragma endregion Includes

struct SVOverlayFigureStruct
{
	SVOverlayFigureStruct()
	{
		pFigurePoints = NULL;
	}
	~SVOverlayFigureStruct()
	{
		delete [] pFigurePoints;
		pFigurePoints = NULL;
	}
	void Clear()
	{
		delete [] pFigurePoints;
		pFigurePoints = NULL;
	}

	COLORREF oFigureColor;
	long lFigureSize;
	POINT *pFigurePoints;
};// end SVOverlayFigureStruct

struct SVOverlayStruct
{
	SVOverlayStruct()
	{
		lFigureSize = 0;
		pOverlays = NULL;
	}
	~SVOverlayStruct()
	{
		delete [] pOverlays;
		lFigureSize = 0;
		pOverlays = NULL;
	}
	void Clear()
	{
		for( int i = 0; i < lFigureSize; i++ )
			pOverlays[i].Clear();
		delete [] pOverlays;
		lFigureSize = 0;
		pOverlays = NULL;
	}

	COLORREF oTextColor;
	POINT oTextPoint;
	std::wstring strText;

	long lFigureSize;
	SVOverlayFigureStruct *pOverlays;
};// end SVOverlayStruct

class SVImageBufferClass
{
public:
	// General and header section
	BOOL GetBufferSize( long &lSize ) const;
	BOOL GetBuffer( BYTE *&pBuffer ) const;

	// Overlay section
	BOOL GetOverlayCount( long &lCount ) const;
	BOOL GetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay )const;

	// General and header section
	BOOL SetBufferSize( long &lSize );
	BOOL SetBuffer( BYTE *&pBuffer, long &lSize );
	BOOL SetBufferCopy( BYTE *&pBuffer, long &lSize );

	// Overlay section
	BOOL SetOverlayCount( long &lCount );
	BOOL SetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay );

	// Construction section
	SVImageBufferClass();
	virtual ~SVImageBufferClass();

	BOOL CalculateSize( long &lSize ) const;
	BOOL Clear();

private:
	BYTE *m_pBuffer;
	mutable long m_lBufferSize;

	mutable long m_lOverlaySize;
	SVOverlayStruct **m_ppOverlays;
};
