//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBuffer
//* .File Name       : $Workfile:   SVImageBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVImageLibrary/SVExtentPointStruct.h"
#pragma endregion Includes

struct SVOverlayFigureStruct
{
	SVOverlayFigureStruct()
	{
		lFigureSize = 0;
		pFigurePoints = NULL;
		oFigureColor = 0;
	}

	SVOverlayFigureStruct(const SVExtentLineStruct& p_ExtentLineStruct)
	{
		lFigureSize = 0;
		pFigurePoints = NULL;
		*this = p_ExtentLineStruct;
	}

	const SVOverlayFigureStruct& operator = (const SVExtentLineStruct& p_ExtentLineStruct)
	{
		Clear();

		oFigureColor = p_ExtentLineStruct.m_dwColor;

		lFigureSize = static_cast< long >( p_ExtentLineStruct.m_svPointArray.GetSize() );

		pFigurePoints = new POINT[lFigureSize];
		for ( int i = 0; i < lFigureSize; i++ )
		{
			SVExtentPointStruct svExtentPtStruct = p_ExtentLineStruct.m_svPointArray.GetAt(i);
			pFigurePoints[i] = svExtentPtStruct.operator POINT();
		}
		
		return *this;
	}

	~SVOverlayFigureStruct()
	{
		Clear();
	}

	void Clear()
	{
		lFigureSize = 0;

		if( pFigurePoints != NULL )
		{
			delete [] pFigurePoints;
			pFigurePoints = NULL;
		}
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
		oTextColor = 0;
	}
	
	SVOverlayStruct(const SVExtentMultiLineStruct& p_MultiLineStruct)
	{
		lFigureSize = 0;
		pOverlays = NULL;
		*this = p_MultiLineStruct;
	}

	~SVOverlayStruct()
	{
		Clear();
	}

	void Clear()
	{
		for( int i = 0; i < lFigureSize; i++ )
		{
			pOverlays[i].Clear();
		}

		lFigureSize = 0;

		if( pOverlays != NULL )
		{
			delete [] pOverlays;
			pOverlays = NULL;
		}
	}

	const SVOverlayStruct& operator = ( const SVExtentMultiLineStruct& p_rStruct )
	{
		Clear();

		oTextColor = p_rStruct.m_Color;
		oTextPoint = p_rStruct.m_StringPoint.operator POINT();
		strText = p_rStruct.m_csString;
		
		lFigureSize = static_cast< long >( p_rStruct.m_svLineArray.GetSize() );

		pOverlays = new SVOverlayFigureStruct[lFigureSize];

		for ( int i = 0; i < lFigureSize; i++ )
		{
			pOverlays[i] = p_rStruct.m_svLineArray.GetAt(i);
			pOverlays[i].oFigureColor = p_rStruct.m_Color;
		}

		return *this;
	}

	COLORREF oTextColor;
	POINT oTextPoint;
	CString strText;

	long lFigureSize;
	SVOverlayFigureStruct *pOverlays;
};// end SVOverlayStruct

class SVImageOverlayClass
{
public:
	// Construction section
	SVImageOverlayClass();
	SVImageOverlayClass( const SVImageOverlayClass& rhs );
	SVImageOverlayClass(const SVExtentMultiLineStructCArray& p_MultiLineArrayStruct);
	~SVImageOverlayClass();

	const SVImageOverlayClass& operator = ( const SVImageOverlayClass& rhs );
	const SVImageOverlayClass& operator = ( const SVExtentMultiLineStructCArray& p_rMultiLineArrayStruct );

	// General and header section
	long GetBufferSize() const;
	BOOL GetBufferSize( long &lSize ) const;
	BOOL GetBuffer( BYTE *&pBuffer );

	// Overlay section
	BOOL GetOverlayCount( long &lCount ) const;
	BOOL GetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay );

	// General and header section
	BOOL SetBufferSize( long lSize, bool bClearBuffer = true );
	//BOOL SetBuffer( BYTE *&pBuffer, long &lSize );

	// Overlay section
	BOOL SetOverlayCount( long &lCount );
	BOOL SetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay );

	BOOL CalculateSize( long &lSize );

	void Clear();

private:
	void Init();

	long m_lBufferSize;
	BYTE *m_pBuffer;
};

