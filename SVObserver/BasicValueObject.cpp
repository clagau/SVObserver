//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Apr 2014 12:48:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObject.h"

#include "SVUtilityLibrary\SVSAFEARRAY.h"
#include "SVOMFCLibrary\SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary\SVLongValueDeviceParam.h"
#include "SVOMFCLibrary\SVStringValueDeviceParam.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObserver;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor
BasicValueObject::BasicValueObject( LPCSTR ObjectName )
: SVObjectClass(ObjectName),
	m_Created(FALSE)
{
	Create();
}

BasicValueObject::~BasicValueObject()
{
	Destroy();
}
#pragma endregion Constructor

#pragma region Public Methods
bool BasicValueObject::operator==(const BasicValueObject& rRhs) const
{
	bool Result = false;

	if(m_Value == rRhs.m_Value)
	{
		Result = true;
	}
	return Result;
}

HRESULT BasicValueObject::setValue(const _variant_t& rValue )
{
	HRESULT			Status;
	_variant_t		Temp;

	//SVRC sends data as array
	if( rValue.vt & VT_ARRAY )
	{
		SVSAFEARRAY SafeArray( rValue );

		//BasicValueObject can only have one value
		if ( 1 == SafeArray.size() )
		{
			Status = SafeArray.GetElement( 0, Temp );
			//From SVRC only of type BSTR should be received
			if(VT_BSTR == Temp.vt)
			{
				SVString StringValue;
				StringValue = Temp.bstrVal;
				Temp.Clear();
				Temp.vt = m_Value.vt;
				switch(m_Value.vt)
				{
				case VT_BSTR:
					Temp.bstrVal = StringValue.ToBSTR();
					break;
				case VT_I4:
					Temp.lVal = atol(StringValue.c_str());
					break;
				case VT_R8:
					Temp.dblVal = atof(StringValue.c_str());
					break;
				default:
					break;
				}
			}
			Status = E_INVALIDARG;
		}
		else
		{
			Status = E_INVALIDARG;
		}
	}
	else
	{
		Temp = rValue;
	}

	Lock();
	m_Value = Temp;
	Unlock();
	return S_OK;
}

HRESULT BasicValueObject::setValue(const LPCTSTR Value )
{
	_variant_t VariantValue;
	VariantValue.vt = VT_BSTR;
	_bstr_t Temp;
	Temp = Value;
	VariantValue.bstrVal = Temp.Detach();
	setValue(VariantValue);
	return S_OK;
}

HRESULT BasicValueObject::setValue(const BOOL Value )
{
	_variant_t VariantValue;
	VariantValue.vt = VT_BOOL;
	VariantValue.boolVal = Value;
	setValue(VariantValue);
	return S_OK;
}

HRESULT BasicValueObject::setValue(const long Value )
{
	_variant_t VariantValue;
	VariantValue.vt = VT_I4;
	VariantValue.lVal = Value;
	setValue(VariantValue);
	return S_OK;
}

HRESULT BasicValueObject::setValue(const __int64 Value )
{
	_variant_t VariantValue;
	VariantValue.vt = VT_I8;
	VariantValue.llVal = Value;
	setValue(VariantValue);
	return S_OK;
}

HRESULT BasicValueObject::setValue(const double Value )
{
	_variant_t VariantValue;
	VariantValue.vt = VT_R8;
	VariantValue.dblVal = Value;
	setValue(VariantValue);
	return S_OK;
}

HRESULT BasicValueObject::getValue( _variant_t& rValue ) const
{
	RefreshOwner();

	rValue = m_Value;
	return S_OK;
}

HRESULT BasicValueObject::getValue( CString& rValue ) const
{
	HRESULT	Result = S_OK;

	RefreshOwner();

	switch(m_Value.vt)
	{
	case VT_BSTR:
		{
			rValue = m_Value.bstrVal;
		}
		break;
	case VT_I4:
		{
			rValue.Format( _T("%d"), m_Value.lVal );
		}
		break;
	case VT_I8:
		{
			rValue.Format( _T("%d"), m_Value.llVal );
		}
		break;
	case VT_R4:
		{
			rValue.Format( _T("%f"), m_Value.dblVal );
		}
		break;
	default:
		{
			Result = S_FALSE;
		}
		break;
	}

	return Result;
}

HRESULT BasicValueObject::getValue( BOOL& rValue ) const
{
	HRESULT	Result = S_FALSE;

	RefreshOwner();

	if(VT_BOOL == m_Value.vt)
	{
		rValue = m_Value.boolVal;
		Result = S_OK;
	}
	return Result;
}

HRESULT BasicValueObject::getValue( long& rValue ) const
{
	HRESULT	Result = S_FALSE;

	RefreshOwner();

	if(VT_I4 == m_Value.vt)
	{
		rValue = m_Value.lVal;
		Result = S_OK;
	}
	return Result;
}

HRESULT BasicValueObject::getValue( __int64& rValue ) const
{
	HRESULT	Result = S_FALSE;

	RefreshOwner();

	if(VT_I8 == m_Value.vt)
	{
		rValue = m_Value.llVal;
		Result = S_OK;
	}
	return Result;
}

HRESULT BasicValueObject::getValue( double& rValue ) const
{
	HRESULT	Result = S_FALSE;

	RefreshOwner();

	if(VT_R4 == m_Value.vt)
	{
		rValue = m_Value.dblVal;
		Result = S_OK;
	}
	return Result;
}

HRESULT BasicValueObject::updateDeviceParameter(SVDeviceParam* pDeviceParam)
{
	HRESULT Status = S_OK;
	
	if(nullptr == pDeviceParam)
	{
		Status = S_FALSE;
		return Status;
	}
	switch(pDeviceParam->DataType())
	{
	case DeviceDataTypeBool:
		{
			SVBoolValueDeviceParam* pBoolDeviceParam = nullptr;
			pBoolDeviceParam = dynamic_cast< SVBoolValueDeviceParam* > (pDeviceParam);
			if(nullptr != pBoolDeviceParam)
			{
				if(VT_BOOL == m_Value.vt)
				{
					Status = pBoolDeviceParam->SetValue(m_Value.GetVARIANT());
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

	case DeviceDataTypeLong:
		{
			SVLongValueDeviceParam* pLongDeviceParam = nullptr;
			pLongDeviceParam = dynamic_cast< SVLongValueDeviceParam* > (pDeviceParam);
			if(nullptr != pLongDeviceParam)
			{
				if(VT_I4 == m_Value.vt)
				{
					if((m_Value.lVal >= pLongDeviceParam->GetScaledMin()) && (m_Value.lVal <= pLongDeviceParam->GetScaledMax()))
					{
						pLongDeviceParam->SetScaledValue(m_Value.lVal);
					}
					else
					{
						//When failed then need to set the value to the same DeviceParam value
						m_Value.lVal = pLongDeviceParam->GetScaledValue();
						Status = S_FALSE;
					}
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

	case DeviceDataTypeString:
		{
			SVStringValueDeviceParam* pStringDeviceParam = nullptr;
			pStringDeviceParam = dynamic_cast< SVStringValueDeviceParam* > (pDeviceParam);
			if(nullptr != pStringDeviceParam)
			{
				if(VT_BSTR == m_Value.vt)
				{
					Status = pStringDeviceParam->SetValue(m_Value.GetVARIANT());
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

	default:
		break;
	}
	return Status;
}
#pragma endregion Public Methods

#pragma region Private Methods
BOOL BasicValueObject::Create()
{
	m_Value.Clear();
	SetObjectDepth(1);
	ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
	::InitializeCriticalSection( &m_CriticalSection );
	m_Created = TRUE;
	return true;
}

BOOL BasicValueObject::Destroy()
{
	if( m_Created )
	{
		m_Value.Clear();
		SetObjectDepth(-1);
		::DeleteCriticalSection( &m_CriticalSection );
		m_Created = FALSE;
	}
	return true;
}

BOOL BasicValueObject::Lock()
{
	::EnterCriticalSection( &m_CriticalSection );

	return TRUE;
}

BOOL BasicValueObject::Unlock()
{
	::LeaveCriticalSection( &m_CriticalSection );

	return TRUE;
}

BOOL BasicValueObject::RefreshOwner() const
{
	BOOL Result = FALSE;
	
	SVObjectClass* pObject = SVObjectClass::GetOwner();

	if(( NULL != pObject ) && ( S_OK == pObject->RefreshObject() ) )
	{
		Result = TRUE;
	}

	return Result;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObject.cpp_v  $
 * 
 *    Rev 1.1   02 Apr 2014 12:48:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed text "Result" which was a merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Mar 2014 15:10:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/