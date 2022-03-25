//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductItems
//* .File Name       : $Workfile:   SVProductItems.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:10:24  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVProductItems.h"
#include "SVValueObjectList.h"
#include "SVValueObject.h"
#pragma endregion Includes

HRESULT SVProductItems::FinalConstruct()
{
	m_values.CoCreateInstance(__uuidof(SVValueObjectList));
	m_images.CoCreateInstance(__uuidof(SVImageObjectList));
	m_errors.CoCreateInstance(__uuidof(SVErrorObjectList));

	return S_OK;
}

void SVProductItems::FinalRelease()
{
	m_values.Release();
	m_images.Release();
	m_errors.Release();
}

HRESULT SVProductItems::AddValue(ISVValueObject* pValue)
{
	HRESULT hr = S_OK;
	CComPtr<ISVValueObject> valueObj(pValue);
	ISVValueObject* pObject = NULL;
	valueObj.QueryInterface(&pObject);
	if (pObject != NULL)
	{
		m_values.p->Add(pObject);
		pObject->Release(); // call Release cause IUnknown::QueryInterface calls Addref
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT SVProductItems::AddImage(ISVImageObject* pValue)
{
	HRESULT hr = S_OK;
	CComPtr<ISVImageObject> valueObj(pValue);
	ISVImageObject* pObject = NULL;
	valueObj.QueryInterface(&pObject);
	if (pObject != NULL)
	{
		m_images.p->Add(pObject);
		pObject->Release(); // call Release cause IUnknown::QueryInterface calls Addref
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT SVProductItems::AddError(ISVErrorObject* pValue)
{
	HRESULT hr = S_OK;
	CComPtr<ISVErrorObject> valueObj(pValue);
	ISVErrorObject* pObject = NULL;
	valueObj.QueryInterface(&pObject);
	if (pObject != NULL)
	{
		m_errors.p->Add(pObject);
		pObject->Release(); // call Release cause IUnknown::QueryInterface calls Addref
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

STDMETHODIMP SVProductItems::get_Values(ISVValueObjectList ** pVal)
{
	if (m_values.p)
	{
		ISVValueObjectList* pList = m_values.p;
		pList->AddRef();
		*pVal = pList;
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}

STDMETHODIMP SVProductItems::put_Values(ISVValueObjectList * newVal)
{
	if (m_values.p != newVal)
	{
		m_values = newVal;
	}
	return S_OK;
}

STDMETHODIMP SVProductItems::get_Images(ISVImageObjectList** pVal)
{
	if (m_images.p)
	{
		ISVImageObjectList* pList = m_images.p;
		pList->AddRef();
		*pVal = pList;
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}


STDMETHODIMP SVProductItems::put_Images(ISVImageObjectList* newVal)
{
	if (m_images.p != newVal)
	{
		m_images = newVal;
	}
	return S_OK;
}

STDMETHODIMP SVProductItems::get_Errors(ISVErrorObjectList ** pVal)
{
	if (m_images.p)
	{
		ISVErrorObjectList* pList = m_errors.p;
		pList->AddRef();
		*pVal = pList;
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}

STDMETHODIMP SVProductItems::put_Errors(ISVErrorObjectList * newVal)
{
	if (m_errors.p != newVal)
	{
		m_errors = newVal;
	}
	return S_OK;
}
