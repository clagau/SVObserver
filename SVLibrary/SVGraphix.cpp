//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGraphixDrawObjectClass
//* .File Name       : $Workfile:   SVGraphix.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   03 Jun 2014 13:48:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVGraphix.h"
#include "SVUtilityLibrary/StringHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void Serialize(SVGraphixDrawObjectClass* pObject, CArchive& rArchive)
{
	if(nullptr != pObject)
	{
		if (rArchive.IsStoring())
		{
			rArchive << pObject->GetDrawingColor()
				<< pObject->getPenStyle()
				<< BOOL(pObject->getBrushStyle()); //Must be BOOL for backward compatibility

			//Need to use CArray for backward compatibility
			CArray< POINT, POINT& > pointArray;
			for(auto& rPoint : pObject->getPointVector())
			{
				pointArray.Add(rPoint);
			}
			pointArray.Serialize(rArchive);
		}
		else
		{
			COLORREF color;
			int penStyle{0};
			//This must be BOOL for backward compatibility reading configurations using CArchive
			BOOL bSolidBrush{false};

			rArchive >> color
				>> penStyle
				>> bSolidBrush;

			//Need to use CArray for backward compatibility
			CArray< POINT, POINT& > pointArray;
			pointArray.Serialize(rArchive);
			pObject->getPointVector().clear();
			for(int i=0; i < pointArray.GetSize(); i++)
			{
				pObject->AddPoint(pointArray[i]);
			}

			// Force object to use serialized
			// color, penStyle and bSolidBrush
			// instead of default settings...
			pObject->SetPenStyle(penStyle);
			pObject->SetBrushStyle(bSolidBrush ? true : false);
			pObject->SetDrawingColor(color);
		}
	}
}

std::string SVConvertToHexString(DWORD size, char* buff)
{
	// put len in string first
	std::string result = SvUl::Format(_T("0x%08x"), size);

	for (DWORD i = 0; i < size; i++)
	{
		result += SvUl::Format(_T(",0x%02x"), buff[i]);
	}

	return result;
}

std::string SVConvertFromHexString(const std::string& rHexString)
{
	std::string result;

	size_t rightPos = rHexString.find(',');
	if(std::string::npos != rightPos)
	{
		long convertedCount{0L};
		std::string Text = SvUl::Left(rHexString, rightPos);
		long byteCount = strtoul(Text.c_str(), nullptr ,16);
		result.resize(byteCount);
		size_t leftPos = rightPos + 1;
		rightPos = rHexString.find(',', leftPos);
		Text = rHexString.substr(leftPos, rightPos - leftPos);
		while(convertedCount < byteCount)
		{
			char byte = static_cast<char> (strtoul(Text.c_str(), nullptr, 16));
			result[convertedCount] = byte;
			convertedCount++;

			leftPos = rightPos + 1;
			rightPos = rHexString.find(',', leftPos);
			Text = rHexString.substr(leftPos, rightPos - leftPos);
		}
	}

	return result;
}



SVGraphixDrawObjectClass::~SVGraphixDrawObjectClass()
{
	if (nullptr != m_hPen)
	{
		::DeleteObject(m_hPen);
		m_hPen = nullptr;
	}

	if (nullptr != m_hBrush)
	{
		::DeleteObject(m_hBrush);
		m_hBrush = nullptr;
	}
}

void SVGraphixDrawObjectClass::SetPenStyle(int NewStyle)
{
	if (nullptr != m_hPen)
	{
		::DeleteObject(m_hPen);
		m_hPen = nullptr;
	}

	m_penStyle = NewStyle;
	if (m_needPen)
	{
		m_hPen = ::CreatePen(m_penStyle, 1, m_color);
	}
}

void SVGraphixDrawObjectClass::SetBrushStyle(bool bSolidBrush)
{
	if (nullptr != m_hBrush)
	{
		::DeleteObject(m_hBrush);
		m_hBrush = nullptr;
	}

	m_isSolidBrush = bSolidBrush;
	if (m_needBrush)
	{
		m_hBrush = m_isSolidBrush ? ::CreateSolidBrush(m_color) : ::CreateHatchBrush(HS_DIAGCROSS, m_color);
	}
}

void SVGraphixDrawObjectClass::SetDrawingColor(COLORREF Color)
{
	m_color = Color;
	SetPenStyle(m_penStyle);
	SetBrushStyle(m_isSolidBrush);
}

bool SVGraphixDrawObjectClass::AddPoint(const POINT& rNewPoint)
{
	if (m_pointVector.size() <= m_maxPointCount)
	{
		m_pointVector.emplace_back(rNewPoint);
		return true;
	}
	return false;
}

bool SVGraphixDrawObjectClass::ReplacePoint(POINT& rReplacePoint)
{
	if (m_pointVector.size() > 0)
	{
		POINT& lastPoint = m_pointVector[m_pointVector.size() - 1];
		m_pointVector[m_pointVector.size() - 1] = rReplacePoint;
		rReplacePoint = lastPoint;
		return true;
	}
	return false;
}

SVGraphixClass::SVGraphixClass()
{
    // Default raster operation code: overwrite
    m_dwROP = SRCCOPY;

    // Default overlay mode: stretch
    m_overlayMode = SVStretchMode;

    // Default background color: white
    m_backGroundColor = RGB( 255, 255, 255 );

    // Default size of Graphix canvas: 100 x 100
    m_rect.left = 0;
    m_rect.top  = 0;
    m_rect.right  = 50;
    m_rect.bottom = 50;

	// Default scale factors...
	m_scaleX = 1.0;	// No scaling
	m_scaleY = 1.0;	// No scaling
}

SVGraphixClass::~SVGraphixClass()
{
	FlushDrawObjects();
}

std::string SVGraphixClass::store(bool convertToHex /*= false*/)
{
	std::string result;

	CMemFile storage;
	CArchive archive(&storage, CArchive::store);
	// Serialize draw objects...
	__int32 size = static_cast<__int32>(m_drawObjectVector.size());
	archive << size;
	for (const auto pDrawObject : m_drawObjectVector)
	{
		archive << pDrawObject->GetObjectType();

		::Serialize(pDrawObject, archive);
	}

	// Serialize graphix data...
	archive << m_dwROP << m_overlayMode << m_backGroundColor;

	archive << m_rect.left << m_rect.top << m_rect.right << m_rect.bottom;

	// flush the archive
	archive.Flush();

	// get the size
	DWORD dataSize = static_cast<unsigned long>(storage.GetLength());

	// position to the beginning
	storage.SeekToBegin();

	// create a buffer
	char* pBuff = new char[dataSize];

	// get the data
	storage.Read(pBuff, dataSize);

	if(convertToHex)
	{
		result = ::SVConvertToHexString(dataSize, pBuff);
	}
	else
	{
		result.assign(pBuff, dataSize);
	}


	delete [] pBuff;

	return result;

}

void SVGraphixClass::load(const std::string& rData, bool convertFromHex /*= false*/)
{
	FlushDrawObjects();

	if(rData.size() > 0)
	{
		std::string convertedData = convertFromHex ? SVConvertFromHexString(rData) : rData;

		CMemFile storage;
		// write the data to the memory file
		storage.Write(&convertedData[0], static_cast<UINT> (convertedData.size()));
		storage.Flush();
		storage.SeekToBegin();

		// Create a CArchive to load from
		CArchive archive(&storage, CArchive::load);

		__int32 size = 0;
		__int32 objectType = 0;
		SVGraphixDrawObjectClass* pNewObject = nullptr;
		// Serialize draw objects...
		archive >> size;
		for (__int32 i = 0; i < size; ++i)
		{
			archive >> objectType;
			// Create and insert draw object...
			pNewObject = GetNewDrawObject(static_cast<SVGraphixDrawObjectEnum> (objectType));
			if (nullptr != pNewObject)
			{
				::Serialize(pNewObject, archive);
			}
			else
			{
				SVGraphixDrawObjectClass dummyObject;
				::Serialize(&dummyObject, archive);
			}
		}

		// Serialize graphix data...
		int overlayMode;
		archive >> m_dwROP >> overlayMode >> m_backGroundColor;

		m_overlayMode = static_cast<SVGraphixOverlayModeEnum> (overlayMode);

		archive >> m_rect.left >> m_rect.top >> m_rect.right >> m_rect.bottom;
	}
}

void SVGraphixClass::FlushDrawObjects()
{
	for(auto pDrawObject : m_drawObjectVector)
	{
		delete pDrawObject;
		pDrawObject = nullptr;
	}

	m_drawObjectVector.clear();
}

void SVGraphixClass::InsertDrawObject(SVGraphixDrawObjectClass* pNewDrawObject)
{
	if (nullptr != pNewDrawObject)
	{
		m_drawObjectVector.emplace_back(pNewDrawObject);
	}
}

bool SVGraphixClass::CheckPoint(const POINT& rPoint, int ScaleX /*= 1*/, int ScaleY /*= 1*/)
{
	POINT pointLT, pointRB;
	pointLT.x = m_rect.left;
	pointLT.y = m_rect.top;
	pointRB.x = m_rect.right;
	pointRB.y = m_rect.bottom;

	// Regard overlay mode...
	ScalePoint(pointLT, TRUE);
	ScalePoint(pointRB, TRUE);

	// Regard extern scale factors...
	if (ScaleX)
	{
		pointLT.x = (ScaleX < 0) ? (pointLT.x / (-ScaleX)) : (pointLT.x * ScaleX);
		pointRB.x = (ScaleX < 0) ? (pointRB.x / (-ScaleX)) : (pointRB.x * ScaleX);
	}

	if (ScaleY)
	{
		pointLT.y = (ScaleY < 0) ? (pointLT.y / (-ScaleY)) : (pointLT.y * ScaleY);
		pointRB.y = (ScaleY < 0) ? (pointRB.y / (-ScaleY)) : (pointRB.y * ScaleY);
	}

	return(rPoint.x >= pointLT.x && rPoint.y >= pointLT.y  && rPoint.x < pointRB.x && rPoint.y < pointRB.y);
}

void SVGraphixClass::ScalePoint(POINT& rPoint, bool bDown /*= true*/)
{
	if (m_scaleX != 0.0)
	{
		rPoint.x = (bDown) ? ((long)(((double)rPoint.x) / m_scaleX)) : ((long)(((double)rPoint.x) * m_scaleX));
	}
	if (m_scaleY != 0.0)
	{
		rPoint.y = (bDown) ? ((long)(((double)rPoint.y) / m_scaleY)) : ((long)(((double)rPoint.y) * m_scaleY));
	}
}

void SVGraphixClass::Draw( HDC hDC, const RECT& rRect )
{
    int graphixWidth  = m_rect.right - m_rect.left;
    int graphixHeight = m_rect.bottom - m_rect.top;
    int destWidth  = rRect.right - rRect.left;
    int destHeight = rRect.bottom - rRect.top;
            
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBRUSH hBackgroundBrush = ::CreateSolidBrush( m_backGroundColor );
	HBITMAP hBM = ::CreateCompatibleBitmap( hDC, graphixWidth, graphixHeight );

	if(nullptr != hMemDC && nullptr != hBackgroundBrush && nullptr != hBM)
	{
		// Select background brush for memDC...
		HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject( hMemDC, hBackgroundBrush ));

		// Select bitmap for memDC...
		HBITMAP hOldBM = static_cast<HBITMAP> (::SelectObject( hMemDC, hBM ));
        
		// Set memDC background color...
		::PatBlt( hMemDC, m_rect.left, m_rect.top, graphixWidth, graphixHeight, PATCOPY );

		// Deselect background brush for memDC...
		::SelectObject( hMemDC, hOldBrush );

		// Draw graphix...
		//...
		for (auto pDrawObject : m_drawObjectVector)
		{
			if(nullptr != pDrawObject)
			{
				pDrawObject->Draw(hMemDC);
			}
		}


		// Select background brush for destiny DC...
		hOldBrush = ( HBRUSH ) ::SelectObject( hDC, hBackgroundBrush );

		if( m_overlayMode != SVStretchMode )
		{
			// Set destiny DC background color...
			::PatBlt( hDC, rRect.left, rRect.top, destWidth, destHeight, PATCOPY );
		}

		// Set stretch mode...
		::SetStretchBltMode( hDC, COLORONCOLOR );

		switch( m_overlayMode )
		{
			case SVStretchMode: 
			{
				::StretchBlt( hDC, rRect.left, rRect.top, destWidth, destHeight, 
								hMemDC, m_rect.left, m_rect.top, graphixWidth, graphixHeight, m_dwROP );

				m_scaleX = ( double ) graphixWidth  / ( double ) destWidth;
				m_scaleY = ( double ) graphixHeight / ( double ) destHeight;
			}
			break;

			case SVClipMode: 
			{
				::BitBlt( hDC, rRect.left, rRect.top, graphixWidth, graphixHeight,
							hMemDC, m_rect.left, m_rect.top, m_dwROP );

				m_scaleX = 1.0;
				m_scaleY = 1.0;
			}
			break;

			case SVClipCenteredMode: 
			{
				int destLeft = rRect.left + ( destWidth - graphixWidth ) / 2;
				int destTop  = rRect.top + ( destHeight - graphixHeight ) / 2;
				::BitBlt( hDC, destLeft, destTop, graphixWidth, graphixHeight,
							hMemDC, m_rect.left, m_rect.top, m_dwROP );

				m_scaleX = 1.0;
				m_scaleY = 1.0;
			}
			break;
		}

		::SelectObject( hDC, hOldBrush );

		// Deselect bitmap for memDC...
		::SelectObject( hMemDC, hOldBM );
	}

	if (nullptr != hBM)
	{
        ::DeleteObject(hBM);
	}
    if(nullptr != hBackgroundBrush)
	{
		::DeleteObject( hBackgroundBrush );
	}
    if( hMemDC )
	{
		::DeleteDC( hMemDC );
	}
}

SVGraphixDrawObjectClass* SVGraphixClass::GetNewDrawObject( SVGraphixDrawObjectEnum NewObject, bool bInsert )
{
	SVGraphixDrawObjectClass* pDrawObject = nullptr;

	switch( NewObject )
	{
		case SVDrawObjectPoint: 
			pDrawObject = new SVGraphixPointDrawObjectClass;
			break;

		case SVDrawObjectFilledRect: 
			pDrawObject = new SVGraphixFilledRectDrawObjectClass;
			break;
			
		case SVDrawObjectRect:
			pDrawObject = new SVGraphixRectDrawObjectClass;
			break;

		case SVDrawObjectFilledEllipse:
			pDrawObject = new SVGraphixFilledEllipseDrawObjectClass;
			break;

		case SVDrawObjectEllipse:
			pDrawObject = new SVGraphixEllipseDrawObjectClass;
			break;

		case SVDrawObjectPolygon:
			pDrawObject = new SVGraphixPolygonDrawObjectClass;
			break;

		case SVDrawObjectPolyline:
			pDrawObject = new SVGraphixPolylineDrawObjectClass;
			break;

		case SVDrawObjectFill:
			pDrawObject = new SVGraphixFillDrawObjectClass;
			break;

	}

	assert(pDrawObject);

	if( bInsert ) 
	{
		InsertDrawObject( pDrawObject );
	}

	return pDrawObject;
}

HGLOBAL SVGraphixClass::GetGraphixData() 
{
	std::string data = store();

	size_t memFileSize = data.size();
	HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, memFileSize );
	BYTE* pMem = ( BYTE* ) ::GlobalLock( hMem );
	if( nullptr != pMem && memFileSize > 0)
	{
		memcpy( pMem, &data[0], memFileSize );
		::GlobalUnlock( pMem );
		return hMem;
	}
	if(pMem) ::GlobalUnlock( pMem );
	if(hMem) ::GlobalFree( hMem );
	return nullptr;
}

bool SVGraphixClass::SetGraphixData( HGLOBAL hGlobalMem ) 
{
	bool result{false};

	SIZE_T memFileSize = ::GlobalSize( hGlobalMem ); 
	char* pMemData = static_cast<char*> (::GlobalLock(hGlobalMem));
	if(nullptr != pMemData && memFileSize > 0)
	{
		std::string data;
		data.assign(pMemData, memFileSize);
		load(data);

		::GlobalUnlock( pMemData );
		result = true;
	}
	::GlobalFree( hGlobalMem );

	return result;
}

