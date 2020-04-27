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

SVImageInfoClass::SVImageInfoClass(const SVImageInfoClass &p_rsvValue)
{
	Initialize();

	*this = p_rsvValue;
}

SVImageInfoClass::SVImageInfoClass(const BITMAPINFOHEADER& p_rBitmapHeader)
{
	*this = p_rBitmapHeader;
}

SVImageInfoClass::~SVImageInfoClass()
{
	Initialize();
}

SVImageInfoClass &SVImageInfoClass::operator=(const SVImageInfoClass &p_rsvValue)
{
	if (this != &p_rsvValue)
	{
		m_OwnerImageID = p_rsvValue.m_OwnerImageID;
		m_OwnerObjectID = p_rsvValue.m_OwnerObjectID;
		m_svExtents = p_rsvValue.m_svExtents;
		m_svProperties = p_rsvValue.m_svProperties;
		m_isDibBuffer = p_rsvValue.m_isDibBuffer;

		m_svExtents.UpdateData();
	}
	return (*this);
}


// And how 
const SVImageInfoClass &SVImageInfoClass::operator=(const BITMAPINFOHEADER& p_rBitmapHeader)
{
	Initialize();

	int l_iFormat = SvDef::SVImageFormatUnknown;
	if (p_rBitmapHeader.biBitCount == 8)
	{
		l_iFormat = SvDef::SVImageFormatMono8;
	}
	else
		if (p_rBitmapHeader.biBitCount == 24)
		{
			l_iFormat = SvDef::SVImageFormatBGR888;
		}
		else
			if (p_rBitmapHeader.biBitCount == 32)
			{
				l_iFormat = SvDef::SVImageFormatBGR888X;
			}
	SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_iFormat);

	SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, p_rBitmapHeader.biBitCount);
	SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);
	SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0);

	SetExtentProperty(SvPb::SVExtentPropertyHeight, abs(p_rBitmapHeader.biHeight));
	SetExtentProperty(SvPb::SVExtentPropertyWidth, p_rBitmapHeader.biWidth);

	if (p_rBitmapHeader.biBitCount == 24)
	{
		SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8);
		SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3);
	}

	return *this;
}

HRESULT SVImageInfoClass::Initialize()
{
	HRESULT l_hrOk = S_OK;
	m_isDibBuffer = false;

	m_OwnerImageID = SvDef::InvalidObjectId;
	m_OwnerObjectID = SvDef::InvalidObjectId;

	m_svExtents.Initialize();

	if (S_OK != m_svProperties.Initialize())
	{
		l_hrOk = S_FALSE;
	}

	m_svExtents.SetTranslation(SvPb::SVExtentTranslationShift);

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(10.0, 10.0)))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyWidth, 100.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyHeight, 100.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyWidthScaleFactor, 1.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, 1.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyRotationAngle, 0.0))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != m_svExtents.SetExtentProperty(SvPb::SVExtentPropertyTranslationOffset, SVPoint<double>(0.0, 0.0)))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatMono8))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1))
	{
		l_hrOk = S_FALSE;
	}

	if (S_OK != SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0))
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVImagePropertiesClass SVImageInfoClass::GetImageProperties() const
{
	return m_svProperties;
}

void SVImageInfoClass::SetImageProperties(const SVImagePropertiesClass& rImageProperties)
{
	m_svProperties = rImageProperties;
}

const SVImageExtentClass& SVImageInfoClass::GetExtents() const
{
	return m_svExtents;
}

HRESULT SVImageInfoClass::SetExtents(const SVImageExtentClass& rExtents)
{
	m_svExtents = rExtents;

	HRESULT l_hrOk = m_svExtents.UpdateData();

	return l_hrOk;
}

SvPb::SVExtentTranslationEnum SVImageInfoClass::GetTranslation()
{
	return m_svExtents.GetTranslation();
}

HRESULT SVImageInfoClass::SetTranslation(SvPb::SVExtentTranslationEnum p_eTranslation)
{
	m_svExtents.SetTranslation(p_eTranslation);
	return m_svExtents.UpdateData();
}

HRESULT SVImageInfoClass::GetImageProperty(SvDef::SVImagePropertyEnum p_eProperty, int &p_riValue) const
{
	return m_svProperties.GetImageProperty(p_eProperty, p_riValue);
}

HRESULT SVImageInfoClass::GetImageProperty(SvDef::SVImagePropertyEnum p_eProperty, long &p_rlValue) const
{
	return m_svProperties.GetImageProperty(p_eProperty, p_rlValue);
}

HRESULT SVImageInfoClass::SetImageProperty(SvDef::SVImagePropertyEnum p_eProperty, long p_lValue)
{
	return m_svProperties.SetImageProperty(p_eProperty, p_lValue);
}

HRESULT SVImageInfoClass::GetExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, long &p_rlValue) const
{
	double l_dValue = 0.0;
	HRESULT l_hrOk = GetExtentProperty(p_eProperty, l_dValue);

	if (S_OK == l_hrOk)
	{
		p_rlValue = static_cast<long>(l_dValue);
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, double &p_rdValue) const
{
	return m_svExtents.GetExtentProperty(p_eProperty, p_rdValue);
}

HRESULT SVImageInfoClass::GetExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, POINT &p_roValue) const
{
	return m_svExtents.GetExtentProperty(p_eProperty, p_roValue);
}

HRESULT SVImageInfoClass::GetExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, SVPoint<double> &rValue) const
{
	return m_svExtents.GetExtentProperty(p_eProperty, rValue);
}

HRESULT SVImageInfoClass::SetExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, double p_dValue)
{
	HRESULT result = m_svExtents.SetExtentProperty(p_eProperty, p_dValue);

	if (S_OK == result)
	{
		result = m_svExtents.UpdateData();
	}

	return result;
}

HRESULT SVImageInfoClass::SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue)
{
	HRESULT result = m_svExtents.SetExtentProperty(eProperty, rValue);

	if (S_OK == result)
	{
		result = m_svExtents.UpdateData();
	}

	return result;
}

HRESULT SVImageInfoClass::GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass& rOutExtent)
{
	HRESULT hrOk {S_FALSE};
	RECT newRect;
	RECT parentRect;
	SVPoint<double> position;
	long Width {0}, Height {0};
	if (S_OK != GetOutputRectangle(parentRect) ||
		S_OK != inExtent.GetRectangle(newRect) ||
		S_OK != inExtent.GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position)
		)
	{
		return S_FALSE;
	}
	if (inExtent.hasFigure() &&
		(inExtent.GetTranslation() == SvPb::SVExtentTranslationProfile))
	{

		///make sure  the center of the LinearTool with rotation is within the Outputrectangle 
		///figure.msvCenter is not calculated yet

		hrOk = S_OK;
		SVExtentFigureStruct Figure = inExtent.GetFigure();
		Figure.m_svCenter = (Figure.m_svTopLeft + Figure.m_svBottomRight) / 2.0;

		///Currently the size of the ROI for a LinearTool is not changed. 
		///If you want to change the size you should insert the appropriate code here. 


		if (Figure.m_svCenter.m_x < 0)
		{
			position.m_x -= Figure.m_svCenter.m_x;
		}
		if (Figure.m_svCenter.m_x > parentRect.right)
		{
			position.m_x -= (Figure.m_svCenter.m_x - parentRect.right);
		}
		if (Figure.m_svCenter.m_y < 0)
		{
			position.m_y -= Figure.m_svCenter.m_y;
		}
		if (Figure.m_svCenter.m_y > parentRect.bottom)
		{
			position.m_y -= (Figure.m_svCenter.m_y - parentRect.bottom);
		}

	}
	else
	{

		hrOk = S_OK;
		Width = newRect.right - newRect.left;
		Height = newRect.bottom - newRect.top;

		Width = min(Width, parentRect.right - parentRect.left);
		Height = min(Height, parentRect.bottom - parentRect.top);

		if (newRect.left < 0 || (parentRect.right - parentRect.left < newRect.right - newRect.left))
		{
			position.m_x = 0.0;
		}
		else if (parentRect.right < newRect.right)
		{
			position.m_x -= newRect.right - parentRect.right;
		}

		if ((newRect.top) < 0 || (parentRect.bottom - parentRect.top < newRect.bottom - newRect.top))
		{
			position.m_y = 0;

		}
		else if (parentRect.bottom < newRect.bottom)
		{
			position.m_y -= newRect.bottom - parentRect.bottom;
		}
	}
	if (S_OK == hrOk)
	{

		hrOk = inExtent.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, position);

		if (S_OK == hrOk && Width > 0)
		{
			hrOk = inExtent.SetExtentProperty(SvPb::SVExtentPropertyWidth, Width);
		}

		if (S_OK == hrOk && Height > 0)
		{
			hrOk = inExtent.SetExtentProperty(SvPb::SVExtentPropertyHeight, Height);
		}

		if (S_OK == hrOk)
		{
			rOutExtent = inExtent;
		}
	}

	return hrOk;
}

HRESULT SVImageInfoClass::GetOutputRectangle(RECT &p_roRect) const
{
	return m_svExtents.GetOutputRectangle(p_roRect);
}

const SVExtentFigureStruct& SVImageInfoClass::GetFigure() const
{
	return m_svExtents.GetFigure();
}

HRESULT SVImageInfoClass::GetOwnerImage(SVObjectClass*& rpObject) const
{
	HRESULT result = S_OK;

	rpObject = SVObjectManagerClass::Instance().GetObject(m_OwnerImageID);

	if (nullptr == rpObject)
	{
		result = E_FAIL;
	}

	return result;
}

void SVImageInfoClass::SetOwnerImage(uint32_t objectID)
{
	m_OwnerImageID = objectID;
}

SVObjectClass* SVImageInfoClass::GetOwner() const
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(m_OwnerObjectID);

	return pObject;
}

uint32_t SVImageInfoClass::GetOwnerID() const
{
	return m_OwnerObjectID;
}

void SVImageInfoClass::SetOwner(uint32_t objectID)
{
	m_OwnerObjectID = objectID;
}

HRESULT SVImageInfoClass::ValidateAgainstOutputSpace(const SVImageExtentClass &p_rsvValue) const
{
	return m_svExtents.ValidateAgainstOutputSpace(p_rsvValue);
}

bool SVImageInfoClass::operator==(const SVImageInfoClass &p_rsvValue) const
{
	bool l_bOk = true;

	l_bOk = l_bOk && m_OwnerImageID == p_rsvValue.m_OwnerImageID;
	l_bOk = l_bOk && m_OwnerObjectID == p_rsvValue.m_OwnerObjectID;
	l_bOk = l_bOk && m_svExtents == p_rsvValue.m_svExtents;
	l_bOk = l_bOk && m_svProperties == p_rsvValue.m_svProperties;

	return l_bOk;
}

bool SVImageInfoClass::operator!=(const SVImageInfoClass &p_rsvValue) const
{
	return !(*this == p_rsvValue);
}

long SVImageInfoClass::GetBufferSize()
{
	long lDepth = 1;
	RECT rect;
	m_svExtents.GetOutputRectangle(rect);
	long lHeight = abs(rect.top - rect.bottom);
	long lWidth = abs(rect.right - rect.left);
	long lFormat;
	m_svProperties.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, lFormat);

	switch (lFormat)
	{
		case SvDef::SVImageFormatMono8:  // Mono
		{
			break;
		}
		case SvDef::SVImageFormatBGR888:   // RGB
		{
			lDepth = 3;
			break;
		}
		case SvDef::SVImageFormatBGR888X:  // RGB
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
	int pixelDepth {0};
	long width {0L};
	long height {0L};
	int format {0};
	int bands {0};

	GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, pixelDepth);
	m_svExtents.GetExtentProperty(SvPb::SVExtentPropertyOutputWidth, width);
	m_svExtents.GetExtentProperty(SvPb::SVExtentPropertyOutputHeight, height);
	GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, format);
	GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, bands);


	l_info.biWidth = width;
	l_info.biHeight = height;
	l_info.biBitCount = pixelDepth;

	WORD wClrBits = 8;
	if (format == SvDef::SVImageFormatBGR888)
	{
		wClrBits = 24;
	}
	else if (format == SvDef::SVImageFormatBGR888X)
	{
		wClrBits = 32;
	}

	l_info.biSize = sizeof(BITMAPINFOHEADER);
	l_info.biPlanes = 1;
	l_info.biBitCount = wClrBits;
	if (wClrBits < 24)
	{
		l_info.biClrUsed = (1 << wClrBits);
	}

	// If the bitmap is not compressed, set the BI_RGB flag. 
	l_info.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed.
	l_info.biSizeImage = ((abs(l_info.biWidth) * wClrBits + 31) & ~31) / 8 * abs(l_info.biHeight);

	// 0 indicates that all colors are important.
	l_info.biClrImportant = 0;

	return l_info;
}

SVImageInfoClass::operator BITMAPINFOHEADER () const
{
	return GetBitmapInfoHeader();
}


