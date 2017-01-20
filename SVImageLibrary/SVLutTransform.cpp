//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutTransform
//* .File Name       : $Workfile:   SVLutTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <typeinfo.h>
#include "SVLutTransform.h"
#include "SVLut.h"
#pragma endregion Includes

SVLutTransformOperationMap SVLutTransform::mMapTypes;

SVLutTransformOperationMap::SVLutTransformOperationMap()
{
	maTypeInfo.SetAtGrow(LutTransformTypeNormal, SVLutTransformTypeInfo(LutTransformTypeNormal, new SVLutTransformOperationNormal, _T("Normal")) );
	maTypeInfo.SetAtGrow(LutTransformTypeInversion, SVLutTransformTypeInfo(LutTransformTypeInversion, new SVLutTransformOperationInvert, _T("Invert")) );
	maTypeInfo.SetAtGrow(LutTransformTypeSign, SVLutTransformTypeInfo(LutTransformTypeSign, new SVLutTransformOperationSign(), _T("Sign")) );
	maTypeInfo.SetAtGrow(LutTransformTypeClip, SVLutTransformTypeInfo(LutTransformTypeClip, new SVLutTransformOperationClip(), _T("Clip")) );
	maTypeInfo.SetAtGrow(LutTransformTypeFreeform, SVLutTransformTypeInfo(LutTransformTypeFreeform, new SVLutTransformOperationFreeform(), _T("Freeform")) );
	maTypeInfo.SetAtGrow(LutTransformTypeTwoKnee, SVLutTransformTypeInfo(LutTransformTypeTwoKnee, new SVLutTransformOperationTwoKnee(), _T("Two Knee")) );
};

SVLutTransformOperationMap::~SVLutTransformOperationMap()
{
	for (int i=0; i<MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].m_pType)
		{
			delete maTypeInfo[i].m_pType;
		}
	}
}

SVLutTransformOperation* SVLutTransformOperationMap::GetType(SVLutTransformOperationEnum e)
{
	if (e < MAX_LUT_TRANSFORM_TYPE && (int)e >= 0)
	{
		return maTypeInfo[e].m_pType;
	}
	return nullptr;
}

SVLutTransformOperationEnum SVLutTransformOperationMap::GetType(const SVLutTransformOperation* pType)
{
	assert( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].m_pType && typeid(*maTypeInfo[i].m_pType) == typeid(*pType))
		{
			return maTypeInfo[i].m_eType;
		}
	}
	return LutTransformTypeUnknown;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(SVLutTransformOperationEnum e)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].m_eType == e)
		{
			return &maTypeInfo.ElementAt(i);
		}
	}
	return nullptr;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const SVLutTransformOperation* pType)
{
	assert( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].m_pType && typeid(*maTypeInfo[i].m_pType) == typeid(*pType))
		{
			return &maTypeInfo.ElementAt(i);
		}
	}
	return nullptr;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const SVString& rType)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].m_Type == rType && !rType.empty())
		{
			return &maTypeInfo.ElementAt(i);
		}
	}
	return nullptr;
}

SVLutTransformOperation::~SVLutTransformOperation()
{
}

SVLutTransform::SVLutTransform()
{
	mpType = nullptr;
	mbOwnType = false;
}

SVLutTransform::SVLutTransform(const SVLutTransformOperation& rType)
{
	mpType = nullptr;
	mbOwnType = false;
	SetOperation( rType );
}

SVLutTransform::SVLutTransform(const SVLutTransform& rhs)
{
	mpType = nullptr;
	mbOwnType = false;
	*this = rhs;
}

SVLutTransform::~SVLutTransform()
{
	Cleanup();
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransform& rRhs)
{
	Cleanup();
	if (rRhs.GetOperation())
	{
		SetOperation(*(rRhs.GetOperation()));
	}
	mParam = rRhs.mParam;
	return *this;
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransformOperation& rRhs)
{
	SetOperation(rRhs);
	return *this;
}

HRESULT SVLutTransform::Transform(SVLutBand& data, const SVLutTransformParameters& param)
{
	if (mpType)
	{
		mParam.clear();
		mParam = param;
		return mpType->Transform(*this, data, param);
	}
	return S_FALSE;
}

bool SVLutTransform::SetOperation(const SVLutTransformOperation& rType)
{
	mParam.clear();
	if (mpType != &rType)
	{
		Cleanup();
		mpType = rType.Clone();
		mbOwnType = true;
	}
	return true;
}

bool SVLutTransform::SetOperation(SVLutTransformOperationEnum eType)
{
	mParam.clear();
	mpType = mMapTypes.GetType(eType);
	mbOwnType = false;
	return nullptr != mpType;
}

const SVLutTransformOperation* SVLutTransform::GetOperation() const
{
	return mpType;
}

void SVLutTransform::GetOperation(SVLutTransformOperationEnum& eType) const
{
	eType = mMapTypes.GetType(mpType);
}

void SVLutTransform::Cleanup()
{
	mParam.clear();
	if (mpType && mbOwnType)
	{
		delete mpType;
		mpType = nullptr;
		mbOwnType = false;
	}
}

bool SVLutTransform::GetParameters(SVLutTransformParameters& rParam) const
{
	rParam = mParam;
	return true;
}

bool SVLutTransform::SetParameters(const SVLutTransformParameters& rParam)
{
	mParam = rParam;
	return true;
}

bool SVLutTransform::operator == (const SVLutTransform& rRhs) const
{
	// equal if operaton and params are equal
	bool bEqual = true;
	if ( nullptr == mpType && nullptr == rRhs.mpType )
	{}
	else if ( nullptr == mpType || nullptr == rRhs.mpType )
	{ 
		bEqual = false; 
	}
	else
	{
		bEqual = (*mpType == *(rRhs.mpType));
		if ( bEqual )
		{
			if ( mParam.size() == rRhs.mParam.size() )
			{
				for ( int i=0; i < mParam.size() && bEqual; i++ )
				{
					bEqual = bEqual && mParam[i] == rRhs.mParam[i];
				}
			}
			else
			{
				bEqual = false;
			}
		}
	}
	return bEqual;
}

bool SVLutTransformOperation::operator == ( const SVLutTransformOperation& rhs ) const
{
	return (typeid(*this) == typeid(rhs)) != FALSE;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = (data.Size() - i - 1) * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	unsigned long i;
	for (i=0; i < data.Size()/2; i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size() + data.Info().MaxValue()/2;
	}
	for (i=data.Size()/2; i < data.Size(); i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size() - data.Info().MaxValue()/2;
	}
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_FALSE;
	assert ( param.size() == 2 );
	if (param.size() == 2)
	{
		long lClipMin = param[0];
		long lClipMax = param[1];
		assert ( lClipMin <= lClipMax );
		for (unsigned long i=0; i < data.Size(); i++)
		{
			long lValue = i * ( data.Info().MaxValue() + 1 ) / data.Size();
			if (lValue <= lClipMin)
				data(i) = lClipMin;
			else if (lValue >= lClipMax)
				data(i) = lClipMax;
			else
				data(i) = lValue;
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	assert( param.size() == 5 );
	if ( param.size() == 5 )
	{
		unsigned long x1 = param[0];
		unsigned long y1 = param[1];
		unsigned long x2 = param[2];
		unsigned long y2 = param[3];
		unsigned long lMaxY = param[4];

		if ( x1 >= 0 && x2 >= 0 && x1 < data.Size() && x2 < data.Size() && x2 >= x1 )
		{
			if (x1 > 0)
			{
				double dSlope = ((double)y1 / (double)x1);
				double dY = 0.0;
				for (unsigned long i=0; i <= x1; i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			if (x2 > 0 && x2 > x1)
			{
				double dSlope = ((double)(y2-y1) / (double)(x2-x1));
				double dY = y1;
				for (unsigned long i=x1+1; i <= x2; i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			if (x2 < data.Size() - 1)
			{
				double dSlope = ((double)(lMaxY-y2) / (double)(data.Size()-1-x2));
				double dY = y2;
				for (unsigned long i=x2+1; i < data.Size(); i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			hr = S_OK;
		}
	}
	return hr;
}

