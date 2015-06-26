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
BasicValueObject::BasicValueObject( LPCSTR ObjectName,  SVObjectClass* pOwner, bool Node, SVObjectTypeEnum ObjectType )
: SVObjectClass(ObjectName)
	, m_Created(false)
	, m_Node(Node)
{
	outObjectInfo.ObjectTypeInfo.ObjectType =  ObjectType;
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
			Lock();
			m_Value = TempValue;
			Unlock();
		}
	}

	RefreshOwner( SVObjectClass::PostRefresh );
	return Result;
}

HRESULT BasicValueObject::setValue(const LPCTSTR Value )
{
	HRESULT Result( S_FALSE );

	_variant_t VariantValue;
	VariantValue.vt = VT_BSTR;
	_bstr_t Temp;
	Temp = Value;
	VariantValue.bstrVal = Temp.Detach();
	Result = setValue(VariantValue);

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
			rValue = m_Value.bstrVal;
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
			rValue.Format( _T("%f"), m_Value.fltVal );
		}
		break;
	case VT_R8:
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
	SVString retString = _T("");
	switch(m_Value.vt)
	{
	case VT_BSTR:
		{
			retString = "Text";
		}
		break;
	case VT_BOOL:
	case VT_INT:		//BOOL implicit conversion to VT_INT
		{
			retString = "Bool";
		}
		break;
	case VT_I4:
		{
			retString = "Integer32";
		}
		break;
	case VT_I8:
		{
			retString = "Integer64";
		}
		break;
	case VT_R4:
	case VT_R8:
		{
			retString = "Decimal";
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
			StringValue = rValue.bstrVal;
			rValue.Clear();
			rValue.vt = m_Value.vt;
			switch(m_Value.vt)
			{
			case VT_BSTR:
				rValue.bstrVal = StringValue.ToBSTR();
				break;
			case VT_BOOL:
				if( 0 == StringValue.CompareNoCase( _T("True") ))
				{
					rValue.boolVal = TRUE;
				}
				else if( 0 == StringValue.CompareNoCase( _T("False") ) )
				{
					rValue.boolVal = FALSE;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObject.cpp_v  $
 * 
 *    Rev 1.6   16 Dec 2014 17:51:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method getTypeName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Sep 2014 15:33:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed getValue methods to be able to handle multiple types of values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2014 17:39:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Jul 2014 16:45:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed that getValue also returns values of type boolean
 * Method changed getValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2014 08:31:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added pOwner and Node to constructor
 * Changed Create method to take pOwner parameter
 * Added method CovertArrayToVariant
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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