//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputMatchItem
//* .File Name       : $Workfile:   SVInputMatchItem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:38:26  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInputMatchItem.h"
#pragma endregion Includes

SVInputMatchItem::SVInputMatchItem()
: m_MatchString( NULL ), m_TxnIdentifier( 0 )
{
}

HRESULT SVInputMatchItem::FinalConstruct()
{
	return S_OK;
}

void SVInputMatchItem::FinalRelease()
{
	m_TxnIdentifier = 0;

	m_MatchString.Empty();
}

STDMETHODIMP SVInputMatchItem::get_MatchString(BSTR* pVal)
{
	HRESULT l_Status = S_OK;

	if( pVal != NULL )
	{
		if( *pVal != NULL )
		{
			::SysFreeString( *pVal );

			*pVal = NULL;
		}

		if( m_MatchString.Length() )
		{
			*pVal = m_MatchString.Copy();
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVInputMatchItem::put_MatchString(BSTR Val)
{
	HRESULT l_Status = S_OK;

	m_MatchString = Val;

	return l_Status;
}

STDMETHODIMP SVInputMatchItem::get_TxnIdentifier(LONG* pVal)
{
	HRESULT l_Status = S_OK;

	if( pVal != NULL )
	{
		*pVal = m_TxnIdentifier;
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVInputMatchItem::put_TxnIdentifier(LONG Val)
{
	HRESULT l_Status = S_OK;

	m_TxnIdentifier = Val;

	return l_Status;
}
