// ******************************************************************************
// * COPYRIGHT (c) 2013 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParam
// * .File Name       : $Workfile:   SVDeviceParam.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:01:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"

SVDeviceParam::SVDeviceParam()
	: m_eParam(DeviceParamInvalid)
	, m_eDataType(DeviceDataTypeUnknown)
	, m_bSupports(true) 
{
	SetOrder(INT_MAX);
	SetDetailLevel(0);
}

SVDeviceParam::SVDeviceParam(SVDeviceParamEnum typeEnum)
	: m_eParam(typeEnum)
	, m_eDataType(DeviceDataTypeUnknown)
	, m_bSupports(true) 
{
	SetOrder(INT_MAX);
	SetDetailLevel(0);
}

SVDeviceParam::SVDeviceParam(const SVDeviceParam& rRhs)
{
	m_eParam = rRhs.Type();
	m_eDataType = rRhs.DataType();
	m_bSupports = rRhs.DeviceSupports();

	SetOrder( rRhs.GetOrder() );
	SetDetailLevel( rRhs.DetailLevel() );
	SetName( rRhs.Name() );
	SetVisualName( rRhs.VisualName() );
	SetDescription( rRhs.Description() );
}

SVDeviceParam::~SVDeviceParam()
{
}

/*static*/std::string SVDeviceParam::GetParameterName(SVDeviceParamEnum e)
{
	std::string Result;
	TheDeviceParamFactory::InstanceType::const_iterator iter = TheDeviceParamFactory::Instance().PrimaryMap().find(e);
	if (iter != TheDeviceParamFactory::Instance().PrimaryMap().end())
	{
		Result = iter->second;
	}
	return Result;
}

SVDeviceParam* SVDeviceParam::Create(SVDeviceParamEnum eType)
{
	SVDeviceParam* pParam = nullptr;

	try
	{
		pParam = TheDeviceParamFactory::Instance().New(eType);
	}
	catch (std::runtime_error&)
	{
		ASSERT(FALSE);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		TRACE(_T("SVDeviceParam::Create unable to create type %d\n"), (long)eType);
#endif
		throw;
	}

	ASSERT(pParam);
	return pParam;
}

HRESULT SVDeviceParam::SetName(const std::string& rName)
{
	m_strName = rName;
	return S_OK;
}

HRESULT SVDeviceParam::SetVisualName( const std::string& rName)
{
	m_strVisualName = rName;
	return S_OK;
}

HRESULT SVDeviceParam::SetDetailLevel( long lDetailLevel )
{
	m_lDetailLevel = lDetailLevel;
	return S_OK;
}

HRESULT SVDeviceParam::SetDescription(const std::string& rDescription)
{
	m_strDescription = rDescription;
	return S_OK;
}

HRESULT SVDeviceParam::SetType( SVDeviceParamEnum e )
{
	if ( m_eParam == DeviceDataTypeUnknown )
	{
		m_eParam = e;
		return S_OK;
	}
	return S_FALSE;
}

HRESULT SVDeviceParam::GetValue( VARIANT& rv ) const
{
	return S_FALSE;	// derived class needs to implement
}

HRESULT SVDeviceParam::SetValue( const VARIANT& rv )
{
	return S_FALSE;	// derived class needs to implement
}
