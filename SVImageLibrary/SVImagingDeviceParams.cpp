// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImagingDeviceParams
// * .File Name       : $Workfile:   SVImagingDeviceParams.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:51:30  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImagingDeviceParams.h"
#pragma endregion Includes

namespace	// create unnamed namespace for the variables to live in
{
	REGISTER_DEVICE_PARAM( DeviceParamLightReference, DeviceParamLightReference_String, SVLightReferenceDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamLut,            DeviceParamLut_String,            SVLutDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraFormats,  DeviceParamCameraFormats_String,  SVCameraFormatsDeviceParam );
}// end unnamed namespace

SVLutDeviceParam::SVLutDeviceParam()
	: SVDeviceParam(DeviceParamLut)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLutDeviceParam::SVLutDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLutDeviceParam::~SVLutDeviceParam()
{
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam()
	: SVDeviceParam(DeviceParamLightReference)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::~SVLightReferenceDeviceParam()
{
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam()
	: SVStringValueDeviceParam(DeviceParamCameraFormats)
{
	m_eDataType = DeviceDataTypeString;
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum)
: SVStringValueDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeString;
}

SVCameraFormatsDeviceParam::~SVCameraFormatsDeviceParam()
{
}

SVLutDeviceParam::SVLutDeviceParam(const SVLutDeviceParam& rhs)
: SVDeviceParam(rhs)
, lut(rhs.lut)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam(const SVLightReferenceDeviceParam& rhs)
: SVDeviceParam(rhs)
, lr(rhs.lr)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs) 
: SVStringValueDeviceParam(rhs)
, options(rhs.options)
{
	m_eDataType = DeviceDataTypeString;
}

SVClonable* SVLutDeviceParam::CloneImpl() const
{
	return new SVLutDeviceParam(*this);
}

SVClonable* SVLightReferenceDeviceParam::CloneImpl() const
{
	return new SVLightReferenceDeviceParam( *this );
}

SVClonable* SVCameraFormatsDeviceParam::CloneImpl() const
{
	return new SVCameraFormatsDeviceParam(*this);
}

SVLutDeviceParam& SVLutDeviceParam::operator=(const SVLutDeviceParam& rhs)
{
	if (this != &rhs)
	{
		lut = rhs.lut;
	}
	return *this;
}

SVLightReferenceDeviceParam& SVLightReferenceDeviceParam::operator=(const SVLightReferenceDeviceParam& rhs)
{
	if (this != &rhs)
	{
		lr = rhs.lr;
	}
	return *this;
}

const SVCameraFormatsDeviceParam& SVCameraFormatsDeviceParam::operator=(const SVCameraFormatsDeviceParam& rhs)
{
	if (this != &rhs)
	{
		options = rhs.options;
		strValue = rhs.strValue;
	}
	return *this;
}

HRESULT SVCameraFormatsDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVCameraFormatsDeviceParam* pParam = dynamic_cast<const SVCameraFormatsDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		OptionsType::iterator iterUser;
		OptionsType::iterator iterCameraFile;
		for (iterUser = options.begin(); iterUser != options.end(); ++iterUser)
		{
			SVCameraFormat& rcf = iterUser->second;
			iterCameraFile = const_cast<SVCameraFormatsDeviceParam *>(pParam)->options.find(iterUser->first);
			if (iterCameraFile != pParam->options.end())
			{
				SVCameraFormat& rcfCameraFile = iterCameraFile->second;
				rcf.AssignConstantValues(rcfCameraFile);
			}
		}
		hr = S_OK;
	}
	return hr;
}

bool SVCameraFormatsDeviceParam::SupportsColor() const
{
	bool Result = false;

	SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
	for (iter = options.begin(); !Result && iter != options.end(); ++iter)
	{
		Result = iter->second.m_bColor ? true : false;
	}
	return Result;
}

bool SVCameraFormatsDeviceParam::SupportsMono() const
{
	bool Result = false;

	SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
	for (iter = options.begin(); !Result && iter != options.end(); ++iter)
	{
		Result = iter->second.m_bColor ? false : true;
	}
	return Result;
}
#if defined(_DEBUG)

SVLut& Lut(SVDeviceParamWrapper& w) 
{
	const SVLutDeviceParam* p = w.DerivedValue(p); 
	if (nullptr  == p) 
	{
		w = SVLutDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	assert(p); 
	return const_cast<SVLutDeviceParam*>(p)->lut;
}

SVLightReference& LR(SVDeviceParamWrapper& w) 
{
	const SVLightReferenceDeviceParam* p = w.DerivedValue(p); 
	if (nullptr == p) 
	{
		w = SVLightReferenceDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	assert(p); 
	return const_cast<SVLightReferenceDeviceParam*>(p)->lr;
}

#endif	// #if defined(_DEBUG)

