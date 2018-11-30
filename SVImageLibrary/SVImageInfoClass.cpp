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

	int l_iFormat = SvDef::SVImageFormatUnknown;
	if( p_rBitmapHeader.biBitCount == 8 )
	{
		l_iFormat = SvDef::SVImageFormatMono8;
	}
	else
	if( p_rBitmapHeader.biBitCount == 24 )
	{
		l_iFormat = SvDef::SVImageFormatRGB888;
	}
	else
	if( p_rBitmapHeader.biBitCount == 32 )
	{
		l_iFormat = SvDef::SVImageFormatRGB8888;
	}
	SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_iFormat );

	SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, p_rBitmapHeader.biBitCount );
	SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );
	SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );

	SetExtentProperty( SvDef::SVExtentPropertyHeight, abs(p_rBitmapHeader.biHeight) );
	SetExtentProperty( SvDef::SVExtentPropertyWidth, p_rBitmapHeader.biWidth );

	if( p_rBitmapHeader.biBitCount == 24 )
	{
		SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 );
		SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3 );
	}

	return *this;
}

HRESULT SVImageInfoClass::Initialize()
{
	HRESULT l_hrOk = S_OK;
	m_isDibBuffer = true;

	m_OwnerImageID.clear();

	m_OwnerObjectID.clear();

	m_svExtents.Initialize();

	if ( S_OK != m_svProperties.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	m_svExtents.SetTranslation(SvDef::SVExtentTranslationShift);

	if ( S_OK != m_svExtents.SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, SVPoint<double>(10.0, 10.0)) )
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvDef::SVExtentPropertyWidth, 100.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvDef::SVExtentPropertyHeight, 100.0))
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SvDef::SVExtentPropertyWidthScaleFactor,  1.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SvDef::SVExtentPropertyHeightScaleFactor,  1.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SvDef::SVExtentPropertyRotationAngle, 0.0 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svExtents.SetExtentProperty( SvDef::SVExtentPropertyTranslationOffset, SVPoint<double>(0.0, 0.0)) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 ) )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 ) )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVImagePropertiesClass SVImageInfoClass::GetImageProperties() const
{
	return m_svProperties;
}

void SVImageInfoClass::SetImageProperties(const SVImagePropertiesClass& rImageProperties )
{
	m_svProperties = rImageProperties;
}

const SVImageExtentClass& SVImageInfoClass::GetExtents() const
{
	return m_svExtents;
}

HRESULT SVImageInfoClass::SetExtents(const SVImageExtentClass& rExtents )
{
	m_svExtents = rExtents;

	HRESULT l_hrOk = m_svExtents.UpdateData();

	return l_hrOk;
}

SvDef::SVExtentTranslationEnum SVImageInfoClass::GetTranslation()
{
	return m_svExtents.GetTranslation();
}

HRESULT SVImageInfoClass::SetTranslation( SvDef::SVExtentTranslationEnum p_eTranslation )
{
	m_svExtents.SetTranslation( p_eTranslation );
	return m_svExtents.UpdateData();
}

HRESULT SVImageInfoClass::GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, int &p_riValue ) const
{
	return m_svProperties.GetImageProperty( p_eProperty, p_riValue );
}

HRESULT SVImageInfoClass::GetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long &p_rlValue ) const
{
	return m_svProperties.GetImageProperty( p_eProperty, p_rlValue );
}

HRESULT SVImageInfoClass::SetImageProperty( SvDef::SVImagePropertyEnum p_eProperty, long p_lValue )
{
	return m_svProperties.SetImageProperty( p_eProperty, p_lValue );
}

HRESULT SVImageInfoClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
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

HRESULT SVImageInfoClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	return m_svExtents.GetExtentProperty( p_eProperty, p_rdValue );
}

HRESULT SVImageInfoClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	return m_svExtents.GetExtentProperty( p_eProperty, p_roValue );
}

HRESULT SVImageInfoClass::GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVPoint<double> &rValue ) const
{
	return m_svExtents.GetExtentProperty( p_eProperty, rValue );
}

HRESULT SVImageInfoClass::SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT result = m_svExtents.SetExtentProperty( p_eProperty, p_dValue );

	if ( S_OK == result )
	{
		result = m_svExtents.UpdateData();
	}

	return result;
}

HRESULT SVImageInfoClass::SetExtentProperty( SvDef::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue )
{
	HRESULT result = m_svExtents.SetExtentProperty( eProperty, rValue );

	if ( S_OK == result )
	{
		result = m_svExtents.UpdateData();
	}

	return result;
}

HRESULT SVImageInfoClass::GetImageExtentsToFit( SVImageExtentClass inExtent, SVImageExtentClass& rOutExtent )
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oNewRect;
	RECT l_oParentRect;

	SVPoint<double> position;

	if( S_OK == GetOutputRectangle( l_oParentRect ) &&
		S_OK == inExtent.GetRectangle( l_oNewRect ) &&
		S_OK == inExtent.GetExtentProperty( SvDef::SVExtentPropertyPositionPoint, position ) )
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
			position.m_x = 0.0;
		}
		else if( l_oParentRect.right < l_oNewRect.right )
		{
			position.m_x -= l_oNewRect.right - l_oParentRect.right;
		}

		if( l_oNewRect.top < 0  || ( l_oParentRect.bottom - l_oParentRect.top < l_oNewRect.bottom - l_oNewRect.top ) )
		{
			position.m_y = 0.0;
		}
		else if( l_oParentRect.bottom < l_oNewRect.bottom )
		{
			position.m_y -= l_oNewRect.bottom - l_oParentRect.bottom;
		}

		l_hrOk = inExtent.SetExtentProperty( SvDef::SVExtentPropertyPositionPoint, position );

		if( S_OK == l_hrOk )
		{
			l_hrOk = inExtent.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_lWidth );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = inExtent.SetExtentProperty( SvDef::SVExtentPropertyHeight, l_lHeight );
		}

		if( S_OK == l_hrOk )
		{
			rOutExtent = inExtent;
		}
	}
	
	return l_hrOk;
}

HRESULT SVImageInfoClass::GetOutputRectangle( RECT &p_roRect ) const
{
	return m_svExtents.GetOutputRectangle( p_roRect );
}

const SVExtentFigureStruct& SVImageInfoClass::GetFigure() const
{
	return m_svExtents.GetFigure();
}

HRESULT SVImageInfoClass::GetOwnerImage( SVObjectClass*& rpObject ) const
{
	HRESULT result = S_OK;

	rpObject = SVObjectManagerClass::Instance().GetObject( m_OwnerImageID );

	if( nullptr == rpObject )
	{
		result = E_FAIL;
	}

	return result;
}

void SVImageInfoClass::SetOwnerImage( const GUID& p_rObjectID )
{
	m_OwnerImageID = p_rObjectID;
}

SVObjectClass* SVImageInfoClass::GetOwner() const
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( m_OwnerObjectID );

	return pObject;
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
	m_svProperties.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, lFormat );

	switch ( lFormat )
	{
		case SvDef::SVImageFormatMono8:  // Mono
		{
			break;
		}
		case SvDef::SVImageFormatRGB888:   // RGB
		{
			lDepth = 3;
			break;
		}
		case SvDef::SVImageFormatRGB8888:  // RGB
		{
			lDepth = 4;
			break;
		}
	}
	return lHeight * lWidth * lDepth;
}

BITMAPINFOHEADER SVImageInfoClass::GetBitmapInfoHeader() const
{
	BITMAPINFOHEADER l_info;
	int pixelDepth{0};
	long width{0L};
	long height{0L};
	int format{0};
	int bands{0};

	GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, pixelDepth );
	m_svExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, width );
	m_svExtents.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, height );
	GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, format );
	GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, bands );


	l_info.biWidth = width;
	l_info.biHeight = height;
	l_info.biBitCount = pixelDepth;

	WORD wClrBits = 8;
	if ( format == SvDef::SVImageFormatRGB888 )
	{
		wClrBits = 24;
	}
	else if ( format == SvDef::SVImageFormatRGB8888 )
	{
		wClrBits = 32;
	}

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


