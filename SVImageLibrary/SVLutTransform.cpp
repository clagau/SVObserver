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


#include "stdafx.h"
#include "SVLutTransform.h"
#include "SVLut.h"
#include <typeinfo.h>

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
		if (maTypeInfo[i].pType)
			delete maTypeInfo[i].pType;
	}
}


SVLutTransformOperation* SVLutTransformOperationMap::GetType(SVLutTransformOperationEnum e)
{
	if (e < MAX_LUT_TRANSFORM_TYPE && (int)e >= 0)
		return maTypeInfo[e].pType;
	else
		return NULL;
}

SVLutTransformOperationEnum SVLutTransformOperationMap::GetType(const SVLutTransformOperation* pType)
{
	ASSERT( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].pType && typeid(*maTypeInfo[i].pType) == typeid(*pType))
			return maTypeInfo[i].eType;
	}
	return LutTransformTypeUnknown;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(SVLutTransformOperationEnum e)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].eType == e)
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const SVLutTransformOperation* pType)
{
	ASSERT( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].pType && typeid(*maTypeInfo[i].pType) == typeid(*pType))
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const CString& sType)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].sType == sType && !sType.IsEmpty())
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}






SVLutTransformOperation::~SVLutTransformOperation()
{
}



SVLutTransform::SVLutTransform()
{
	mpType = NULL;
	mbOwnType = false;
}

SVLutTransform::SVLutTransform(const SVLutTransformOperation& rType)
{
	mpType = NULL;
	mbOwnType = false;
	SetOperation( rType );
}

SVLutTransform::SVLutTransform(const SVLutTransform& rhs)
{
	mpType = NULL;
	mbOwnType = false;
	*this = rhs;
}

SVLutTransform::~SVLutTransform()
{
	Cleanup();
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransform& rhs)
{
	Cleanup();
	if (rhs.GetOperation())
		SetOperation(*(rhs.GetOperation()));
	mParam.Copy(rhs.mParam);
	return *this;
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransformOperation& rhs)
{
	SetOperation(rhs);
	return *this;
}

HRESULT SVLutTransform::Transform(SVLutBand& data, const SVLutTransformParameters& param)
{
	if (mpType)
	{
		mParam.RemoveAll();
		mParam.Copy(param);
		return mpType->Transform(*this, data, param);
	}
	else
		return S_FALSE;
}

bool SVLutTransform::SetOperation(const SVLutTransformOperation& rType)
{
	mParam.RemoveAll();
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
	mParam.RemoveAll();
	mpType = mMapTypes.GetType(eType);
	mbOwnType = false;
	return mpType != NULL;
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
	mParam.RemoveAll();
	if (mpType && mbOwnType)
	{
		delete mpType;
		mpType = NULL;
		mbOwnType = false;
	}
}

bool SVLutTransform::GetParameters(SVLutTransformParameters& rParam) const
{
	rParam.Copy(mParam);
	return true;
}

bool SVLutTransform::SetParameters(const SVLutTransformParameters& rParam)
{
	mParam.Copy(rParam);
	return true;
}

bool SVLutTransform::operator == (const SVLutTransform& rhs) const
{
	// equal if operaton and params are equal
	bool bEqual = true;
	if ( mpType == NULL && rhs.mpType == NULL )
		{}
	else if ( mpType == NULL || rhs.mpType == NULL )
		{ bEqual = false; }
	else
	{
		bEqual = (*mpType == *(rhs.mpType));
		if ( bEqual )
		{
			if ( mParam.GetSize() == rhs.mParam.GetSize() )
			{
				for ( int i=0; i < mParam.GetSize() && bEqual; i++ )
				{
					bEqual = bEqual && mParam[i] == rhs.mParam[i];
				}
			}
			else
				bEqual = false;
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
	ASSERT ( param.GetSize() == 2 );
	if (param.GetSize() == 2)
	{
		long lClipMin = param[0];
		long lClipMax = param[1];
		ASSERT ( lClipMin <= lClipMax );
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
	ASSERT( param.GetSize() == 5 );
	if ( param.GetSize() == 5 )
	{
		unsigned long x1 = param[0];
		unsigned long y1 = param[1];
		unsigned long x2 = param[2];
		unsigned long y2 = param[3];
		unsigned long lMaxY = param[4];

		//ASSERT ( x1 >= 0 && x2 >= 0 && x1 < data.Size() && x2 < data.Size() && x2 >= x1 );
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

