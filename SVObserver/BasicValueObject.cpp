//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 17:51:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObject.h"

#include "TextDefinesSvO.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary\SVSAFEARRAY.h"
#include "SVOMFCLibrary\SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary\SVLongValueDeviceParam.h"
#include "SVOMFCLibrary\SVStringValueDeviceParam.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
BasicValueObject::BasicValueObject( LPCTSTR ObjectName,  SVObjectClass* pOwner, bool Node, SVObjectTypeEnum ObjectType )
: SVObjectClass(ObjectName)
	, m_Created(false)
	, m_Node(Node)
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType =  ObjectType;
	Create( pOwner );
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
	HRESULT	Result = S_OK;

	_variant_t	TempValue( rValue );

	//SVRC sends data as array
	if( VT_ARRAY == (TempValue.vt & VT_ARRAY) )
	{
		Result = ConvertArrayToVariant( TempValue );
	}

	//If variant is of type VT_EMPTY then this is only a node
	if( VT_EMPTY == rValue.vt )
	{
		m_Node = true;
	}
	else
	{
		if ( S_OK == Result )
		{
			//If it is VT_BSTR then we need to copy the text otherwise we get just a pointer to the string
			if( TempValue.vt == VT_BSTR )
			{
				SVString Temp = SvUl_SF::createSVString( TempValue );
				Lock();
				m_Value.Clear();
				m_Value.vt = VT_BSTR;
				m_Value.bstrVal = _bstr_t(Temp.c_str()).copy();
				Unlock();
			}
			else
			{
				Lock();
				m_Value = TempValue;
				Unlock();
			}
		}
	}

	RefreshOwner( SVObjectClass::PostRefresh );
	return Result;
}

HRESULT BasicValueObject::setValue(const LPCTSTR Value )
{
	HRESULT Result( S_FALSE );

	_bstr_t Temp;
	Temp = Value;
	Lock();
	m_Value.Clear();
	m_Value.vt = VT_BSTR;
	m_Value.bstrVal = Temp.Detach();
	Unlock();

	RefreshOwner( SVObjectClass::PostRefresh );
	return Result;
}

HRESULT BasicValueObject::getValue( _variant_t& rValue ) const
{
	RefreshOwner( SVObjectClass::PreRefresh );

	rValue.Clear();
	rValue = m_Value;
	return S_OK;
}

HRESULT BasicValueObject::getValue( SVString& rValue ) const
{
	HRESULT	Result = S_OK;

	RefreshOwner( SVObjectClass::PreRefresh );

	rValue.clear();
	switch(m_Value.vt)
	{
	case VT_BSTR:
		{
			rValue = SvUl_SF::createSVString(m_Value.bstrVal);
		}
		break;
	case VT_BOOL:
		{
			rValue =  m_Value.boolVal ? _T( "True") : _T( "False" );
		}
		break;
	//The old BOOL converts to VT_IINT so this is also of type boolean
	case VT_INT:
		{
			rValue =  m_Value.intVal == 1 ? _T( "True") : _T( "False" );
		}
		break;
	case VT_I4:
		{
			rValue = SvUl_SF::Format( _T("%d"), m_Value.lVal );
		}
		break;
	case VT_I8:
		{
			rValue = SvUl_SF::Format( _T("%d"), m_Value.llVal );
		}
		break;
	case VT_R4:
		{
			rValue = SvUl_SF::Format( _T("%f"), m_Value.fltVal );
		}
		break;
	case VT_R8:
		{
			rValue = SvUl_SF::Format( _T("%f"), m_Value.dblVal );
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
	HRESULT Result = S_OK;

	RefreshOwner( SVObjectClass::PreRefresh );

	switch(m_Value.vt)
	{
	case VT_BOOL:
	case VT_INT:
	case VT_I4:
	case VT_I8:
		rValue = static_cast<BOOL> (m_Value);
		break;
	case VT_R4:
	case VT_R8:
	case VT_BSTR:
	default:
		Result = S_FALSE;
		break;
	}

	return Result;
}

HRESULT BasicValueObject::getValue( bool& rValue ) const
{
	HRESULT Result = S_OK;

	BOOL Value(false);
	//Use the BOOL version and then convert to type bool
	Result = getValue( Value );
	rValue = Value ? true : false;

	return Result;
}

SVString BasicValueObject::getTypeName() const
{
	SVString retString = SvO::Invalid;
	switch(m_Value.vt)
	{
	case VT_BSTR:
		{
			retString = _T("Text");
		}
		break;
	case VT_BOOL:
	case VT_INT:		//BOOL implicit conversion to VT_INT
		{
			retString = _T("Bool");
		}
		break;
	case VT_I4:
		{
			retString = _T("Integer32");
		}
		break;
	case VT_I8:
		{
			retString = _T("Integer64");
		}
		break;
	case VT_R4:
	case VT_R8:
		{
			retString = _T("Decimal");
		}
		break;
	default:
		break;
	}
	return retString;
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
BOOL BasicValueObject::Create( SVObjectClass* pOwner )
{
	m_Value.Clear();
	SetObjectDepth( 1 );
	if( !m_Node )
	{
		ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
	}
	if( nullptr != pOwner )
	{
		SVObjectLevelCreateStruct CreateStruct;

		CreateStruct.OwnerObjectInfo.SetObject(pOwner);
		CreateObject( &CreateStruct );

	}
	::InitializeCriticalSection( &m_CriticalSection );
	m_Created = true;
	return true;
}

BOOL BasicValueObject::Destroy()
{
	if( m_Created )
	{
		m_Value.Clear();
		SetObjectDepth( -1 );
		::DeleteCriticalSection( &m_CriticalSection );
		m_Created = false;
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

HRESULT BasicValueObject::RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type )
{
	HRESULT Result( S_FALSE );
	
	SVObjectClass* pOwner = GetOwner();

	if( nullptr != pOwner )
	{
		Result = pOwner->RefreshObject( pSender, Type );
	}

	return Result;
}

bool BasicValueObject::RefreshOwner( RefreshObjectType Type ) const
{
	bool Result( false );

	SVObjectClass* pOwner = GetOwner();

	if( nullptr != pOwner )
	{
		Result = S_OK == pOwner->RefreshObject( this, Type ) ? true : false;
	}
	
	return Result;
}

HRESULT BasicValueObject::ConvertArrayToVariant( _variant_t& rValue ) const
{
	HRESULT Status = S_OK;
	SVSAFEARRAY SafeArray( rValue );

	//BasicValueObject can only have one value
	if ( 1 == SafeArray.size() )
	{
		Status = SafeArray.GetElement( 0, rValue );
		//From SVRC only of type BSTR should be received
		if(VT_BSTR == rValue.vt)
		{
			SVString StringValue;
			StringValue = SvUl_SF::createSVString(rValue.bstrVal);
			rValue.Clear();
			rValue.vt = m_Value.vt;
			switch(m_Value.vt)
			{
			case VT_BSTR:
				rValue.bstrVal = bstr_t(StringValue.c_str());
				break;
			case VT_BOOL:
				if( 0 == SvUl_SF::CompareNoCase( StringValue, _T("True") ))
				{
					rValue.boolVal = TRUE;
				}
				else if( 0 == SvUl_SF::CompareNoCase( StringValue, _T("False") ) )
				{
					rValue.boolVal = FALSE;
				}
				else if( 0 == StringValue.compare( _T("0") ) )
				{
					rValue.boolVal = FALSE;
				}
				else if( 0 == StringValue.compare( _T("1") ) )
				{
					rValue.boolVal = TRUE;
				}
				else
				{
					Status = E_INVALIDARG;
				}
				break;
			case VT_I4:
				rValue.lVal = atol(StringValue.c_str());
				break;
			case VT_I8:
				rValue.llVal = atol(StringValue.c_str());
				break;
			case VT_R8:
				rValue.dblVal = atof(StringValue.c_str());
				break;
			default:
				Status = E_INVALIDARG;
				break;
			}
		}
		else
		{
			Status = E_INVALIDARG;
		}
	}
	else
	{
		Status = E_INVALIDARG;
	}

	return Status;
}
#pragma endregion Private Methods

