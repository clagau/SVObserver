// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImagingDeviceParams
// * .File Name       : $Workfile:   SVImagingDeviceParams.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:51:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImagingDeviceParams.h"

namespace	// create unnamed namespace for the variables to live in
{
	//REGISTER_DEVICE_PARAM( DeviceParamCameraFiles,         SVCameraFilesDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamLightReference,      DeviceParamLightReference_String, SVLightReferenceDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamLut,                 DeviceParamLut_String,            SVLutDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraFormats,       DeviceParamCameraFormats_String,  SVCameraFormatsDeviceParam );
}// end unnamed namespace


SVLutDeviceParam::SVLutDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init(DeviceParamLut);
}

SVLutDeviceParam::SVLutDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init(DeviceParamLightReference);
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam()
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
	Init(DeviceParamCameraFormats);
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum)
: SVStringValueDeviceParam(typeEnum)
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
}

/*
SVCameraFilesDeviceParam::SVCameraFilesDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init( DeviceParamCameraFiles );
}
*/

SVLutDeviceParam::SVLutDeviceParam( const SVLutDeviceParam& rhs ) 
: SVDeviceParam( rhs )
, lut( rhs.lut )
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs) 
: SVDeviceParam( rhs )
, lr( rhs.lr )
{
	m_eDataType = DeviceDataTypeComplex;
}

/*
SVCameraFilesDeviceParam::SVCameraFilesDeviceParam( const SVCameraFilesDeviceParam& rhs ) : SVDeviceParam( rhs ), cf( rhs.cf )
{
	m_eDataType = DeviceDataTypeComplex;
}
*/

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs) 
: SVStringValueDeviceParam( rhs )
, options( rhs.options )
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
}

SVClonable* SVLutDeviceParam::CloneImpl() const
{
	return new SVLutDeviceParam( *this );
}

SVClonable* SVLightReferenceDeviceParam::CloneImpl() const
{
	return new SVLightReferenceDeviceParam( *this );
}

SVClonable* SVCameraFormatsDeviceParam::CloneImpl() const
{
	return new SVCameraFormatsDeviceParam( *this );
}

/*
SVClonable* SVCameraFilesDeviceParam::CloneImpl() const
{
	return new SVCameraFilesDeviceParam( *this );
}
*/

SVLutDeviceParam& SVLutDeviceParam::operator=(const SVLutDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lut = rhs.lut;
	}
	return *this;
}

SVLightReferenceDeviceParam& SVLightReferenceDeviceParam::operator=(const SVLightReferenceDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lr = rhs.lr;
	}
	return *this;
}

const SVCameraFormatsDeviceParam& SVCameraFormatsDeviceParam::operator=(const SVCameraFormatsDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		options = rhs.options;
		strValue = rhs.strValue;
	}
	return *this;
}

HRESULT SVCameraFormatsDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if ( const SVCameraFormatsDeviceParam* pParam = dynamic_cast<const SVCameraFormatsDeviceParam*>(pBaseParam) )
	{
		info = pParam->info;
		OptionsType::iterator iterUser;
		OptionsType::iterator iterCameraFile;
		for ( iterUser = options.begin(); iterUser != options.end(); ++iterUser )
		{
			SVCameraFormat& rcf = iterUser->second;
			iterCameraFile = const_cast<SVCameraFormatsDeviceParam *>(pParam)->options.find( iterUser->first );
			if ( iterCameraFile != pParam->options.end() )
			{
				SVCameraFormat& rcfCameraFile = iterCameraFile->second;
				rcf.AssignConstantValues( rcfCameraFile );
			}
		}
		//options = pParam->options;
		hr = S_OK;
	}
	return hr;
}

bool SVCameraFormatsDeviceParam::SupportsColor() const
{
	bool bSupportsColor = false;

	SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
	for ( iter = options.begin(); iter != options.end(); ++iter)
	{
		if ( iter->second.bColor )
		{
			bSupportsColor = true;
			break;
		}
	}// end for ( iter = pParam->options.begin(); iter != pParam->options.end(); iter++)
	return bSupportsColor;
}

#if defined(_DEBUG) || defined(_MINDEBUG)

SVLut& Lut(SVDeviceParamWrapper& w) 
{
	const SVLutDeviceParam* p = w.DerivedValue(p); 
	if (p==NULL) 
	{
		w = SVLutDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVLutDeviceParam*>(p)->lut;
}
SVLightReference& LR(SVDeviceParamWrapper& w) 
{
	const SVLightReferenceDeviceParam* p = w.DerivedValue(p); 
	if (p==NULL) 
	{
		w = SVLightReferenceDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVLightReferenceDeviceParam*>(p)->lr;
}

#endif	// #if defined(_DEBUG) || defined(_MINDEBUG)

