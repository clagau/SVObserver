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

#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary\SVSafeArray.h"
#include "CameraLibrary\SVBoolValueDeviceParam.h"
#include "CameraLibrary\SVLongValueDeviceParam.h"
#include "CameraLibrary\SVStringValueDeviceParam.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
BasicValueObject::BasicValueObject( LPCTSTR ObjectName,  SVObjectClass* pOwner, bool Node, SvPb::SVObjectSubTypeEnum ObjectSubType )
: SVObjectClass(ObjectName)
	, m_Created(false)
	, m_Node(Node)

{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVBasicValueObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType =  ObjectSubType;
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

HRESULT BasicValueObject::getValue(double& rValue, int Index /*= -1*/) const
{
	return getValue<double>(rValue);
}

HRESULT BasicValueObject::setValue(const _variant_t& rValue, int Index /*= -1*/ )
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
			if( VT_BSTR == TempValue.vt )
			{
				std::string Temp = SvUl::createStdString( TempValue );
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

HRESULT BasicValueObject::setValue( const std::string& rValue, int Index /*= -1*/ )
{
	HRESULT Result( S_FALSE );

	Lock();
	m_Value.Clear();
	m_Value.SetString( rValue.c_str() );
	Unlock();

	RefreshOwner( SVObjectClass::PostRefresh );
	return Result;
}

HRESULT BasicValueObject::getValue(_variant_t& rValue, int Index /*= -1*/) const
{
	RefreshOwner( SVObjectClass::PreRefresh );

	rValue.Clear();
	rValue = m_Value;
	return S_OK;
}

HRESULT BasicValueObject::getValue(std::string& rValue, int Index /*= -1*/) const
{
	HRESULT	Result = S_OK;

	RefreshOwner( SVObjectClass::PreRefresh );

	rValue.clear();
	switch(m_Value.vt)
	{
	case VT_BSTR:
		{
			rValue = SvUl::createStdString(m_Value.bstrVal);
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
			rValue = SvUl::Format( _T("%d"), m_Value.lVal );
		}
		break;
	case VT_I8:
		{
			rValue = SvUl::Format( _T("%d"), m_Value.llVal );
		}
		break;
	case VT_R4:
		{
			rValue = SvUl::Format( _T("%f"), m_Value.fltVal );
		}
		break;
	case VT_R8:
		{
			rValue = SvUl::Format( _T("%f"), m_Value.dblVal );
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

long BasicValueObject::GetByteSize(bool useResultSize) const
{
	long result(0L);

	//Note: BasicValueObject cannot be an array so memory size only for one element
	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed())
	{
		switch (GetType())
		{
		case VT_BOOL:
			result = sizeof(VARIANT::boolVal);
			break;
		case VT_I4:
			result = sizeof(VARIANT::lVal);
			break;
		case VT_I8:
			result = sizeof(VARIANT::llVal);
			break;
		case VT_INT:
			result = sizeof(VARIANT::intVal);
			break;
		case VT_R4:
			result = sizeof(VARIANT::fltVal);
			break;
		case VT_R8:
			result = sizeof(VARIANT::dblVal);
			break;
		case VT_BSTR:
			{
				std::string tempString = SvUl::createStdString(m_Value.bstrVal);
				//Add place for the ending \0 of the string
				result = static_cast<long> (tempString.size() + 1);
				break;
			}
		default:
			break;
		}
	}
	return result;
}

long BasicValueObject::CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const
{
	long result {0L};

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed() && -1 != m_memOffset)
	{
		result = GetByteSize(false);
		if (result <= MemByteSize)
		{
			BYTE* pMemoryLocation = pMemoryBlock + m_memOffset;
			//Note: BasicValueObject cannot be an array so copy only one element
			const void* pValue(nullptr);

			switch (m_Value.vt)
			{
			case VT_BOOL:
				pValue = &m_Value.boolVal;
				break;
			case VT_I4:
				pValue = &m_Value.lVal;
				break;
			case VT_I8:
				pValue = &m_Value.llVal;
				break;
			case VT_INT:
				pValue = &m_Value.intVal;
				break;
			case VT_R4:
				pValue = &m_Value.fltVal;
				break;
			case VT_R8:
				pValue = &m_Value.dblVal;
				break;
			case VT_BSTR:
				{
					std::string tempString = SvUl::createStdString(m_Value.bstrVal);
					pValue = nullptr;
					//Copy also the ending \0 of the string
					memcpy(pMemoryBlock, tempString.c_str(), tempString.size() + 1);
					break;
				}
			default:
				break;
			}
			//This is for all types except VT_BSTR
			if (nullptr != pValue)
			{
				memcpy(pMemoryLocation, pValue, result);
			}
		}
		else
		{
			result = -1L;
		}
	}
	else
	{
		result = 0L;
	}


	return result;
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

std::string BasicValueObject::getTypeName() const
{
	std::string Result = _T("Invalid");

	switch (m_Value.vt)
	{
	case VT_BSTR:
		{
			Result = _T("Text");
		}
		break;
	case VT_BOOL:
	case VT_INT:		//BOOL implicit conversion to VT_INT
		{
			Result = _T("Bool");
		}
		break;
	case VT_I4:
		{
			Result = _T("Integer32");
		}
		break;
	case VT_I8:
		{
			Result = _T("Integer64");
		}
		break;
	case VT_R4:
	case VT_R8:
		{
			Result = _T("Decimal");
		}
		break;
	default:
		break;
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

DWORD BasicValueObject::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}
#pragma endregion Public Methods


#pragma region Private Methods
void BasicValueObject::Create( SVObjectClass* pOwner )
{
	m_Value.Clear();
	if( !m_Node )
	{
		SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	}
	if( nullptr != pOwner )
	{
		SVObjectLevelCreateStruct CreateStruct;

		CreateStruct.OwnerObjectInfo.SetObject(pOwner);
		CreateObject( CreateStruct );

	}
	::InitializeCriticalSection( &m_CriticalSection );
	m_Created = true;
}

void BasicValueObject::Destroy()
{
	if( m_Created )
	{
		m_Value.Clear();
		::DeleteCriticalSection( &m_CriticalSection );
		m_Created = false;
	}
}

void BasicValueObject::Lock()
{
	::EnterCriticalSection( &m_CriticalSection );
}

void BasicValueObject::Unlock()
{
	::LeaveCriticalSection( &m_CriticalSection );
}

HRESULT BasicValueObject::RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type )
{
	HRESULT Result( S_FALSE );
	
	SVObjectClass* pOwner = GetParent();

	if( nullptr != pOwner )
	{
		Result = pOwner->RefreshObject( pSender, Type );
	}

	return Result;
}

bool BasicValueObject::RefreshOwner( RefreshObjectType Type ) const
{
	bool Result( false );

	SVObjectClass* pOwner = GetParent();

	if( nullptr != pOwner )
	{
		Result = S_OK == pOwner->RefreshObject( this, Type ) ? true : false;
	}
	
	return Result;
}

HRESULT BasicValueObject::ConvertArrayToVariant( _variant_t& rValue ) const
{
	HRESULT Status = S_OK;
	SvUl::SVSAFEARRAY SafeArray( rValue );

	//BasicValueObject can only have one value
	if ( 1 == SafeArray.size() )
	{
		Status = SafeArray.GetElement( 0, rValue );
		//From SVRC only of type BSTR should be received
		if(VT_BSTR == rValue.vt)
		{
			std::string StringValue;
			StringValue = SvUl::createStdString(rValue.bstrVal);
			rValue.Clear();
			rValue.vt = m_Value.vt;
			switch(m_Value.vt)
			{
			case VT_BSTR:
				rValue.bstrVal = bstr_t(StringValue.c_str());
				break;
			case VT_BOOL:
				if( 0 == SvUl::CompareNoCase( StringValue, _T("True") ))
				{
					rValue.boolVal = TRUE;
				}
				else if( 0 == SvUl::CompareNoCase( StringValue, _T("False") ) )
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

} //namespace SvVol
