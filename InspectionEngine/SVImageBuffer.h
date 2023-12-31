//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes

namespace SvIe
{

struct SVOverlayFigureStruct
{
	SVOverlayFigureStruct()
	{
		lFigureSize = 0;
		pFigurePoints = nullptr;
		oFigureColor = 0;
	}

	explicit SVOverlayFigureStruct(const SVExtentLineStruct& p_ExtentLineStruct)
	{
		lFigureSize = 0;
		pFigurePoints = nullptr;
		*this = p_ExtentLineStruct;
	}

	const SVOverlayFigureStruct& operator = (const SVExtentLineStruct& p_ExtentLineStruct)
	{
		Clear();

		oFigureColor = p_ExtentLineStruct.m_dwColor;

		lFigureSize = static_cast<long> (p_ExtentLineStruct.m_PointVector.size());

		pFigurePoints = new POINT[lFigureSize];
		for ( int i = 0; i < lFigureSize; i++ )
		{
			pFigurePoints[i] = static_cast<POINT> (p_ExtentLineStruct.m_PointVector[i]);
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

		if( nullptr != pFigurePoints )
		{
			delete [] pFigurePoints;
			pFigurePoints = nullptr;
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
		pOverlays = nullptr;
		m_TextColor = 0;
	}
	
	explicit SVOverlayStruct(const SVExtentMultiLineStruct& p_MultiLineStruct)
	{
		lFigureSize = 0;
		pOverlays = nullptr;
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

		if( nullptr != pOverlays )
		{
			delete [] pOverlays;
			pOverlays = nullptr;
		}
	}

	const SVOverlayStruct& operator = ( const SVExtentMultiLineStruct& p_rStruct )
	{
		Clear();

		m_TextColor = p_rStruct.m_Color;
		m_TextPoint = p_rStruct.m_StringPoint.operator POINT();
		m_Text = p_rStruct.m_csString;
		
		lFigureSize = static_cast<long> (p_rStruct.m_svLineArray.size());

		pOverlays = new SVOverlayFigureStruct[lFigureSize];

		for ( int i = 0; i < lFigureSize; i++ )
		{
			pOverlays[i] = p_rStruct.m_svLineArray[i];
			pOverlays[i].oFigureColor = p_rStruct.m_Color;
		}

		return *this;
	}

	COLORREF m_TextColor;
	POINT m_TextPoint;
	std::string m_Text;

	long lFigureSize;
	SVOverlayFigureStruct *pOverlays;
};// end SVOverlayStruct

class SVImageOverlayClass
{
public:
	// Construction section
	SVImageOverlayClass();
	SVImageOverlayClass( const SVImageOverlayClass& rRhs );
	explicit SVImageOverlayClass(const SVExtentMultiLineStructVector& rMultiLineArrayStruct);
	~SVImageOverlayClass();

	const SVImageOverlayClass& operator = ( const SVImageOverlayClass& rRhs );
	const SVImageOverlayClass& operator = ( const SVExtentMultiLineStructVector& rMultiLineArrayStruct );

	// General and header section
	long GetBufferSize() const { return m_lBufferSize; };
	BYTE* GetBuffer() { return m_pBuffer; };

	// Overlay section
	long GetOverlayCount() const;
	void GetOverlayFigureAt( long Index, SVOverlayStruct& rOverlay );

	// General and header section
	void SetBufferSize( long Size, bool ClearBuffer = true );

	// Overlay section
	void SetOverlayCount( long Count );
	void SetOverlayFigureAt( long Index, SVOverlayStruct& rOverlay );

	long CalculateSize();

	void Clear();

private:
	void Init();

	long m_lBufferSize;
	BYTE *m_pBuffer;
};

} //namespace SvIe
