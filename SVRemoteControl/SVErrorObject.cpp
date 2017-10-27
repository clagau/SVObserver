//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVErrorObject
//* .File Name       : $Workfile:   SVErrorObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:36:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVErrorObject.h"
#pragma endregion Includes

SVErrorObject::SVErrorObject()
{
}
	
HRESULT SVErrorObject::FinalConstruct()
{
	return S_OK;
}

void SVErrorObject::FinalRelease()
{
}

STDMETHODIMP SVErrorObject::get_Name(BSTR* pVal)
{
	ATL::CComBSTR tmp;
	tmp.Attach(*pVal);
	tmp = m_Name;
	*pVal = tmp.Detach();
	return S_OK;
}


STDMETHODIMP SVErrorObject::put_Name(BSTR newVal)
{
	m_Name = newVal;
	return S_OK;
}

STDMETHODIMP SVErrorObject::get_Status(LONG* pVal)
{
	*pVal = m_Status;
	return S_OK;
}

STDMETHODIMP SVErrorObject::put_Status(LONG newVal)
{
	m_Status = newVal;
	return S_OK;
}
