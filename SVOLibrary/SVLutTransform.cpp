//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	m_TypeInfoMap[LutTransformTypeNormal] = SVLutTransformTypeInfo(LutTransformTypeNormal, new SVLutTransformOperationNormal, _T("Normal"));
	m_TypeInfoMap[LutTransformTypeInversion] = SVLutTransformTypeInfo(LutTransformTypeInversion, new SVLutTransformOperationInvert, _T("Invert"));
	m_TypeInfoMap[LutTransformTypeSign] = SVLutTransformTypeInfo(LutTransformTypeSign, new SVLutTransformOperationSign(), _T("Sign"));
	m_TypeInfoMap[LutTransformTypeClip] = SVLutTransformTypeInfo(LutTransformTypeClip, new SVLutTransformOperationClip(), _T("Clip"));
	m_TypeInfoMap[LutTransformTypeFreeform] = SVLutTransformTypeInfo(LutTransformTypeFreeform, new SVLutTransformOperationFreeform(), _T("Freeform"));
	m_TypeInfoMap[LutTransformTypeTwoKnee] = SVLutTransformTypeInfo(LutTransformTypeTwoKnee, new SVLutTransformOperationTwoKnee(), _T("Two Knee"));
};

SVLutTransformOperationMap::~SVLutTransformOperationMap()
{
	for (auto& rEntry : m_TypeInfoMap)
	{
		delete rEntry.second.m_pType;
	}
}

SVLutTransformOperation* SVLutTransformOperationMap::GetType(SVLutTransformOperationEnum e)
{
	return (m_TypeInfoMap.end() != m_TypeInfoMap.find(e)) ? m_TypeInfoMap[e].m_pType : nullptr;
}

SVLutTransformOperationEnum SVLutTransformOperationMap::GetType(const SVLutTransformOperation* pType)
{
	assert( pType );
	auto iter = std::find_if(m_TypeInfoMap.begin(), m_TypeInfoMap.end(), [&pType](const auto rEntry)
	{
		return nullptr != pType && nullptr != rEntry.second.m_pType && typeid(*rEntry.second.m_pType) == typeid(*pType);
	});

	return m_TypeInfoMap.end() != iter ? iter->second.m_eType : SVLutTransformOperationEnum::LutTransformTypeUnknown;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(SVLutTransformOperationEnum e)
{
	return (m_TypeInfoMap.end() != m_TypeInfoMap.find(e)) ? &m_TypeInfoMap[e] : nullptr;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const SVLutTransformOperation* pType)
{
	assert( pType );
	auto iter = std::find_if(m_TypeInfoMap.begin(), m_TypeInfoMap.end(), [&pType](const auto rEntry)
	{
		return nullptr != pType && nullptr != rEntry.second.m_pType && typeid(*rEntry.second.m_pType) == typeid(*pType);
	});

	return m_TypeInfoMap.end() != iter ? &iter->second : nullptr;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const std::string& rType)
{
	auto iter = std::find_if(m_TypeInfoMap.begin(), m_TypeInfoMap.end(), [&rType](const auto rEntry)
	{
		return rType == rEntry.second.m_Type && false == rType.empty();
	});

	return m_TypeInfoMap.end() != iter ? &iter->second : nullptr;
}

SVLutTransformOperation::~SVLutTransformOperation()
{
}

SVLutTransform::SVLutTransform(const SVLutTransformOperation& rType)
{
	SetOperation( rType );
}

SVLutTransform::SVLutTransform(const SVLutTransform& rhs)
{
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

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationNormal& , SVLutBand& data, const SVLutTransformParameters& )
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationInvert& , SVLutBand& data, const SVLutTransformParameters& )
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = (data.Size() - i - 1) * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationSign& , SVLutBand& data, const SVLutTransformParameters& )
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

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationClip& , SVLutBand& data, const SVLutTransformParameters& param)
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

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationFreeform&, SVLutBand& , const SVLutTransformParameters& )
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationTwoKnee&, SVLutBand& data, const SVLutTransformParameters& param)
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

		if ( x1 < data.Size() && x2 < data.Size() && x2 >= x1 )
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

