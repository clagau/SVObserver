//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomDeviceParam
//* .File Name       : $Workfile:   SVCustomDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   13 Jun 2014 10:00:06  $
//******************************************************************************
#include "Stdafx.h"
#include "SVCustomDeviceParam.h"
#include "SVBoolValueDeviceParam.h"
#include "SVLongValueDeviceParam.h"
#include "SVStringValueDeviceParam.h"
#include "SVI64ValueDeviceParam.h"
#include "SVNullDeviceParam.h"

// Not particularly useful... (just here to statisfy factory signature)
SVCustomDeviceParam::SVCustomDeviceParam()
: m_holder(SVDeviceParamWrapper(SVNullDeviceParam()))
{
}

SVCustomDeviceParam::SVCustomDeviceParam(const SVCustomDeviceParam& rhs) 
: SVDeviceParam( rhs )
, m_holder(rhs.m_holder)
{
}

SVCustomDeviceParam::~SVCustomDeviceParam()
{
	m_holder.Clear();
}

SVClonable* SVCustomDeviceParam::CloneImpl() const
{
	return new SVCustomDeviceParam(*this);
}

SVCustomDeviceParam& SVCustomDeviceParam::operator=(const SVCustomDeviceParam& rhs)
{
	if (this != &rhs)
	{
		m_holder = rhs.m_holder;
	}
	return *this;
}

HRESULT SVCustomDeviceParam::GetValue(VARIANT& rv) const
{
	return m_holder->GetValue(rv);
}

HRESULT SVCustomDeviceParam::SetValue(const VARIANT& rv)
{
	return m_holder->SetValue(rv);
}

HRESULT SVCustomDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVCustomDeviceParam* pParam = dynamic_cast<const SVCustomDeviceParam*>(pBaseParam))
	{
		m_holder->SetMetadata(pBaseParam);
	}
	return hr;
}

SVDeviceParam* SVCustomDeviceParam::Create(SVDeviceParamEnum typeEnum, const VARIANT& rv)
{
	SVCustomDeviceParam* pParam(nullptr);

	switch (rv.vt)
	{
		case VT_BSTR:
		{
			SVStringValueDeviceParam stringValueParam(typeEnum);
			stringValueParam.SetValue(rv);
			pParam = new SVCustomDeviceParam(&stringValueParam);
		}
		break;
			
		case VT_I4:
		{
			SVLongValueDeviceParam longValueParam(typeEnum);
			longValueParam.SetValue(rv);
			pParam = new SVCustomDeviceParam(&longValueParam);
		}
		break;
			
		case VT_BOOL:
		{
			SVBoolValueDeviceParam boolValueParam(typeEnum);
			boolValueParam.SetValue(rv);
			pParam = new SVCustomDeviceParam(&boolValueParam);
		}
		break;
			
		case VT_I8:
		{
			 SVi64ValueDeviceParam i64ValueParam(typeEnum);
			i64ValueParam.SetValue(rv);
			pParam = new SVCustomDeviceParam(&i64ValueParam);
		}
		break;
	}
	return pParam;
}

SVDeviceParamWrapper& SVCustomDeviceParam::GetHeldParam()
{
	return m_holder;
}

const SVDeviceParamWrapper& SVCustomDeviceParam::GetHeldParam() const
{
	return m_holder;
}

