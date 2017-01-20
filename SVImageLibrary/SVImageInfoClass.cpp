//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageInfoClass.h"

#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVImageInfoClass::SVImageInfoClass()
{
	Initialize();
}

SVImageInfoClass::SVImageInfoClass( const SVImageInfoClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVImageInfoClass::SVImageInfoClass( const BITMAPINFOHEADER& p_rBitmapHeader )
{
	*this = p_rBitmapHeader;
}

SVImageInfoClass::~SVImageInfoClass()
{
	Initialize();
}

const SVImageInfoClass &SVImageInfoClass::operator=( const SVImageInfoClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		m_OwnerImageID = p_rsvValue.m_OwnerImageID;
		m_OwnerObjectID = p_rsvValue.m_OwnerObjectID;
		m_svExtents = p_rsvValue.m_svExtents;
		m_svProperties = p_rsvValue.m_svProperties;
		m_isDibBuffer = p_rsvValue.m_isDibBuffer;

		m_svExtents.UpdateData();
	}
	return ( *this );
}


// And how 
const SVImageInfoClass &SVImageInfoClass::operator=( const BITMAPINFOHEADER& p_rBitmapHeader )
{
	HRESULT l_hr = 0;
	Initialize();

	int l_iFormat = SVImageFormatUnknown;
	if( p_rBitmapHeader.biBitCount == 8 )
	{
		l_iFormat = SVImageFormatMono8;
	}
	else
	if( p_rBitmapHeader.biBitCount == 24 )
	{
		l_iFormat = SVImageFormatRGB888;
	}
	else
	if( p_rBitmapHeader.biBitCount == 32 )
	{
		l_iFormat = SVImageFormatRGB8888;
	}
	SetImageProperty( SVImagePropertyFormat, l_iFormat );

	SetImageProperty( SVImagePropertyPixelDepth, p_rBitmapHeader.biBitCount );
	SetImageProperty( SVImagePropertyBandNumber, 1 );
	SetImageProperty( SVImagePropertyBandLink, 0 );

	SetExtentProperty( SVExtentPropertyHeight, abs(p_rBitmapHeader.biHeight) );
	SetExtentProperty( SVExtentPropertyWidth, p_rBitmapHeader.biWidth );

	if( p_rBitmapHeader.biBitCount == 24 )
	{
		SetImageProperty( SVImagePropertyPixelDepth, 8 );
		SetImageProperty( SVImagePropertyBandNumber, 3 );
	}

	return *this;
}

HRESULT SVImageInfoClass::Initialize()
{
	HRESULT l_hrOk = S_OK;
	m_isDibBuffer = true;

	m_OwnerImageID.clear();

	m_OwnerObjectID.clear();

	if ( S_OK != m_svExtents.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svProperties.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetTranslation( SVExtentTranslationShift ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyPositionPoint, 10.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyRectangle, 100.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyWidthScaleFactor,  1.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyHeightScaleFactor,  1.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, 0.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffset, 0.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SVImagePropertyPixelDepth, 8 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SVImagePropertyBandNumber, 1 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SVImagePropertyBandLink, 0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SVImagePropertyResetOwner, false ) )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVImagePropertiesClass SVImageInfoClass::GetImageProperties() const
{
	return m_svProperties;
}

HRESULT SVImageInfoClass::SetImageProperties( SVImagePropertiesClass p_svImageProperties )
{
	m_svProperties = p_svImageProperties;

	return S_OK;
}

SVImageExtentClass SVImageInfoClass::GetExtents() const
{
	return m_svExtents;
}

HRESULT SVImageInfoClass::SetExtents( SVImageExtentClass p_svExtents )
{
	m_svExtents = p_svExtents;

	HRESULT l_hrOk = m_svExtents.UpdateData();

	return l_hrOk;
}

SVExtentTranslationEnum SVImageInfoClass::GetTranslation()
{
	return m_svExtents.GetTranslation();
}

HRESULT SVImageInfoClass::SetTranslation( SVExtentTranslationEnum p_eTranslation )
{
	HRESULT l_hrOk = m_svExtents.SetTranslation( p_eTranslation );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageProperty( SVImagePropertyEnum p_eProperty, int &p_riValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.GetImageProperty( p_eProperty, p_riValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageProperty( SVImagePropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.GetImageProperty( p_eProperty, p_rlValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetImageProperty( SVImagePropertyEnum p_eProperty, long p_lValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.SetImageProperty( p_eProperty, p_lValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	double l_dValue = 0.0;

	l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( S_OK == l_hrOk )
	{
		p_rlValue = static_cast<long>(l_dValue);
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_rdValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_roValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_rsvValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.SetExtentProperty( p_eProperty, p_dValue );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.SetExtentProperty( p_eProperty, p_svValue );

	if ( S_OK == l_hrOk )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent )
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oNewRect;
	RECT l_oParentRect;

	SVExtentPointStruct l_svPositionPoint;

	if( S_OK == GetOutputRectangle( l_oParentRect ) &&
		S_OK == p_svInExtent.GetRectangle( l_oNewRect ) &&
		S_OK == p_svInExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPositionPoint ) )
	{
		long l_lWidth = l_oNewRect.right - l_oNewRect.left;
		long l_lHeight = l_oNewRect.bottom - l_oNewRect.top;

		if( l_oParentRect.right - l_oParentRect.left < l_lWidth )
		{
			l_lWidth = l_oParentRect.right - l_oParentRect.left;
		}

		if( l_oParentRect.bottom - l_oParentRect.top < l_lHeight )
		{
			l_lHeight = l_oParentRect.bottom - l_oParentRect.top;
		}

		if( l_oNewRect.left < 0 || ( l_oParentRect.right - l_oParentRect.left < l_oNewRect.right - l_oNewRect.left ) )
		{
			l_svPositionPoint.m_dPositionX = 0.0;
		}
		else if( l_oParentRect.right < l_oNewRect.right )
		{
			l_svPositionPoint.m_dPositionX -= l_oNewRect.right - l_oParentRect.right;
		}

		if( l_oNewRect.top < 0  || ( l_oParentRect.bottom - l_oParentRect.top < l_oNewRect.bottom - l_oNewRect.top ) )
		{
			l_svPositionPoint.m_dPositionY = 0.0;
		}
		else if( l_oParentRect.bottom < l_oNewRect.bottom )
		{
			l_svPositionPoint.m_dPositionY -= l_oNewRect.bottom - l_oParentRect.bottom;
		}

		l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPositionPoint );

		if( S_OK == l_hrOk )
		{
			l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyWidth, l_lWidth );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyHeight, l_lHeight );
		}

		if( S_OK == l_hrOk )
		{
			p_rsvOutExtent = p_svInExtent;
		}
	}
	
	return l_hrOk;
}

HRESULT SVImageInfoClass::GetOutputRectangle( RECT &p_roRect ) const
{
	return m_svExtents.GetOutputRectangle( p_roRect );
}

HRESULT SVImageInfoClass::GetFigure( SVExtentFigureStruct &p_rsvFigure )
{
	return m_svExtents.GetFigure( p_rsvFigure );
}

HRESULT SVImageInfoClass::GetOwnerImage( SVObjectClass*& p_rpObject ) const
{
	HRESULT l_Status = S_OK;

	p_rpObject = SVObjectManagerClass::Instance().GetObject( m_OwnerImageID );

	if( nullptr == p_rpObject )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

const SVGUID& SVImageInfoClass::GetOwnerImageID() const
{
	return m_OwnerImageID;
}

void SVImageInfoClass::SetOwnerImage( const GUID& p_rObjectID )
{
	m_OwnerImageID = p_rObjectID;
}

SVObjectClass* SVImageInfoClass::GetOwner() const
{
	SVObjectClass* l_pObject = nullptr;

	l_pObject = SVObjectManagerClass::Instance().GetObject( m_OwnerObjectID );

	return l_pObject;
}

const SVGUID& SVImageInfoClass::GetOwnerID() const
{
	return m_OwnerObjectID;
}

void SVImageInfoClass::SetOwner( const GUID& p_rObjectID )
{
	m_OwnerObjectID = p_rObjectID;
}

HRESULT SVImageInfoClass::ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const
{
	return m_svExtents.ValidateAgainstOutputSpace( p_rsvValue );
}

bool SVImageInfoClass::IsEqualExcludePosition( const SVImageInfoClass &p_rsvValue ) const
{
	bool l_bOk = true;
	
	l_bOk = l_bOk && m_OwnerImageID == p_rsvValue.m_OwnerImageID;
	l_bOk = l_bOk && m_OwnerObjectID == p_rsvValue.m_OwnerObjectID;
	l_bOk = l_bOk && m_svExtents.IsEqualExcludePosition( p_rsvValue.m_svExtents );
	l_bOk = l_bOk && m_svProperties == p_rsvValue.m_svProperties;

	return l_bOk;
}

bool SVImageInfoClass::IsPositionEqual( const SVImageInfoClass &p_rsvValue ) const
{
	return m_svExtents.IsPositionEqual( p_rsvValue.m_svExtents );
}

bool SVImageInfoClass::operator==( const SVImageInfoClass &p_rsvValue ) const
{
	bool l_bOk = true;
	
	l_bOk = l_bOk && m_OwnerImageID == p_rsvValue.m_OwnerImageID;
	l_bOk = l_bOk && m_OwnerObjectID == p_rsvValue.m_OwnerObjectID;
	l_bOk = l_bOk && m_svExtents == p_rsvValue.m_svExtents;
	l_bOk = l_bOk && m_svProperties == p_rsvValue.m_svProperties;

	return l_bOk;
}

bool SVImageInfoClass::operator!=( const SVImageInfoClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

long SVImageInfoClass::GetBufferSize()
{
	long lDepth = 1;
	RECT rect;
	m_svExtents.GetOutputRectangle(rect);
	long lHeight = abs(rect.top - rect.bottom);
	long lWidth = abs(rect.right - rect.left);
	long lFormat;
	m_svProperties.GetImageProperty( SVImagePropertyFormat, lFormat );

	switch ( lFormat )
	{
		case SVImageFormatMono8:  // Mono
		{
			break;
		}
		case SVImageFormatRGB888:   // RGB
		{
			lDepth = 3;
			break;
		}
		case SVImageFormatRGB8888:  // RGB
		{
			lDepth = 4;
			break;
		}
	}
	return lHeight * lWidth * lDepth;
}

BITMAPINFOHEADER SVImageInfoClass::GetBitmapInfoHeader() const
{
	HRESULT l_hr = S_OK;
	BITMAPINFOHEADER l_info;
	int l_iPixelDepth=0;
	long l_iWidth = 0;
	long l_iHeight = 0;
	int l_iFormat = 0;
	int l_iBands = 0;

	l_hr = GetImageProperty( SVImagePropertyPixelDepth, l_iPixelDepth );
	l_hr = m_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_iWidth );
	l_hr = m_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_iHeight );
	l_hr = GetImageProperty( SVImagePropertyFormat, l_iFormat );
	l_hr = GetImageProperty( SVImagePropertyBandNumber, l_iBands );


	l_info.biWidth = l_iWidth;
	l_info.biHeight = l_iHeight;
	l_info.biBitCount = l_iPixelDepth;

	WORD wClrBits = 8;
	if ( l_iFormat == SVImageFormatRGB888 )
		wClrBits = 24;
	else if ( l_iFormat == SVImageFormatRGB8888 )
		wClrBits = 32;

	l_info.biSize = sizeof(BITMAPINFOHEADER); 
	l_info.biPlanes = 1;
	l_info.biBitCount = wClrBits; 
	if (wClrBits < 24)
	{
		l_info.biClrUsed = (1<<wClrBits); 
	}
	
	// If the bitmap is not compressed, set the BI_RGB flag. 
	l_info.biCompression = BI_RGB;
	
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed.
	l_info.biSizeImage = ((abs(l_info.biWidth) * wClrBits +31) & ~31) /8 * abs(l_info.biHeight); 

	// 0 indicates that all colors are important.
	l_info.biClrImportant = 0;
	
	return l_info;
}

SVImageInfoClass::operator BITMAPINFOHEADER () const
{
	return GetBitmapInfoHeader();
}


