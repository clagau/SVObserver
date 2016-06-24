//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "BasicValueObject.h"
#include "SVInspectionProcess.h"
#include "TextDefinesSvO.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
LinkedValue::LinkedValue() : 
 m_pLinkedObject( nullptr )
{
}

LinkedValue::~LinkedValue()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT LinkedValue::GetValueAt( int Bucket, int Index, VARIANT& rValue ) const
{
	HRESULT Result( ValidateIndexes(Bucket,Index) );

	if( S_OK ==  Result )
	{
		if(  SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (m_pLinkedObject) )
		{
			Result = pValueObject->GetValue( Bucket, Index, rValue );
		}
		else if( BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (m_pLinkedObject) )
		{
			_variant_t Value;
			Result = pBasicValueObject->getValue( Value );
			::VariantCopy( &rValue, &Value );
		}
		else
		{
			Result = SVVariantValueObjectClass::GetValueAt( Bucket, Index, rValue );
		}
		if( S_OK == Result )
		{
			if (S_OK != ::VariantChangeType( &rValue, &rValue, VARIANT_ALPHABOOL, GetDefaultType() ) )
			{
				//empty value if variant can not be converted in the right type
				::VariantClear( &rValue );
			}
		}
	}

	return Result;
}

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, CString& rValue ) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if( S_OK == Result && (Value.vt & VT_ARRAY) != VT_ARRAY )
	{
		VARTYPE oldType = Value.vt;
		//Convert the variant to a valid string
		Result = ::VariantChangeTypeEx( &Value.GetVARIANT(), &Value.GetVARIANT(), SvOl::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);
		if ( S_OK == Result )
		{
			rValue = Value.bstrVal;
			if( VT_BOOL == oldType )
			{
				rValue.MakeUpper();
			}
		}
		else
		{
			rValue.Empty();
		}
	}

	return Result;
}

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, BOOL& rValue ) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if ( S_OK == Result )
	{
		switch ( Value.vt )
		{
		case VT_BSTR:
			{
				SVString Temp( _bstr_t( Value.bstrVal) );
				if ( 0 == SvUl_SF::CompareNoCase( Temp, SVString( SvO::cTrue) ) )
				{
					rValue = true;

				}
				if ( 0 == SvUl_SF::CompareNoCase( Temp, SVString( SvO::cFalse) ) )
				{
					rValue = false;
				}
				else if ( Temp == _T("1") )
				{
					rValue = true;
				}
				else if ( Temp == _T("0") )
				{
					rValue = false;
				}
				else
				{
					rValue = false;
				}
			}
			break;

		default:
			{
				Result = ::VariantChangeType( &Value, &Value, 0, VT_BOOL );
				if ( S_OK == Result )
				{
					rValue = Value.boolVal;
				}
				else
				{
					if ( VT_BOOL == DefaultValue().vt )
					{
						rValue = DefaultValue().boolVal;
					}
					else
					{
						rValue = false;
					}
				}
			}
		}
	}

	return Result;
}


HRESULT LinkedValue::GetValueAt( int Bucket, int Index, double& rValue ) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if( S_OK == Result && (Value.vt & VT_ARRAY) != VT_ARRAY )
	{
		Result = ::VariantChangeType( &Value, &Value, 0, VT_R8 );
		if ( S_OK == Result )
		{
			rValue = Value.dblVal;
		}
		else
		{
			rValue = 0.0;
		}
	}

	return Result;
}

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, long& rValue ) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if( S_OK == Result && (Value.vt & VT_ARRAY) != VT_ARRAY )
	{
		Result = ::VariantChangeType( &Value, &Value, 0, VT_I4 );
		if ( S_OK == Result )
		{
			rValue = Value.lVal;
		}
		else
		{
			rValue = 0;
		}
	}

	return Result;
}

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, DWORD& rValue ) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if( S_OK == Result && (Value.vt & VT_ARRAY) != VT_ARRAY )
	{
		Result = ::VariantChangeType( &Value, &Value, 0, VT_UI4 );
		if ( S_OK == Result )
		{
			rValue = Value.ulVal;
		}
		else
		{
			rValue = 0;
		}
	}

	return Result;
}

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, BYTE& rValue) const
{
	HRESULT Result( S_OK );
	_variant_t Value;

	Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );

	if( S_OK == Result && (Value.vt & VT_ARRAY) != VT_ARRAY )
	{
		Result = ::VariantChangeType( &Value, &Value, 0, VT_UI1);
		if ( S_OK == Result )
		{
			rValue = Value.bVal;
		}
		else
		{
			rValue = 0;
		}
	}

	return Result;
}

HRESULT LinkedValue::SetValueAt( int Bucket, int Index, CString Value )
{
	HRESULT Result( S_OK );
	CString ToolSetName;
	SVString ObjectName;

	ToolSetName.LoadString( IDS_CLASSNAME_SVTOOLSET );

	//If the tool set name is at the start then add the inspection name at the beginning
	if( 0 == Value.Find( ToolSetName ) )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> ( GetAncestor( SVInspectionObjectType ) );
		if( nullptr != pInspection )
		{
			ObjectName = pInspection->GetName();
			ObjectName += _T(".");
			ObjectName += Value;
		}
	}
	else
	{
		ObjectName = Value;
	}

	SVObjectClass* pNewLinkedObject( nullptr );
	SVObjectManagerClass::Instance().GetObjectByDottedName( ObjectName, pNewLinkedObject );

	// disconnect existing connection if any existing
	DisconnectInput();

	if ( nullptr != pNewLinkedObject )
	{
		m_pLinkedObject = pNewLinkedObject;
		m_LinkedUid = m_pLinkedObject->GetUniqueObjectID();
		//This must use the base class otherwise causes recursive call to SetValueAt
		Result = SVVariantValueObjectClass::SetValueAt( Bucket, Index, m_LinkedUid.ToString().c_str() );
		if (S_OK == Result)
		{
			DWORD_PTR messageReturn = ConnectInput();
			ASSERT( SVMR_SUCCESS == messageReturn );
		}
	}
	else
	{
		SetType( GetDefaultType() );
		Result = SetValueKeepType( Bucket, Value );
	}
	return Result;
}

void LinkedValue::DisconnectObject( const SVObjectClass* const pObject )
{
	if ( pObject == m_pLinkedObject )
	{
		DisconnectInput();
	}
}

void LinkedValue::UpdateLinkedName()
{
	if( nullptr != m_pLinkedObject && SV_GUID_NULL != m_LinkedUid )
	{
		m_LinkedName.SetValue( 0, m_pLinkedObject->GetCompleteObjectNameToObjectType() );
		m_LinkedName.ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
	}
	else
	{
		CString Value;
		m_LinkedName.GetDefaultValue( Value );
		m_LinkedName.SetValue( 0, Value );
		m_LinkedName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT LinkedValue::UpdateConnection()
{
	HRESULT Result = S_OK;
	SVObjectClass* pLinkedObject( nullptr );
	bool ConvertDottedName( false );
	CString Value;

	//We now need to check the no linked value so call the base class method
	SVVariantValueObjectClass::GetValueAt( m_iLastSetIndex, 0,  Value );

	SVGUID LinkedUid( _bstr_t(Value).GetBSTR() );

	//If valid GUID then should be able to get the linked value from the object manager
	if( SV_GUID_NULL != LinkedUid )
	{
		SVObjectManagerClass::Instance().GetObjectByIdentifier( LinkedUid, pLinkedObject );
		if ( nullptr == pLinkedObject )
		{
			Result = SvOi::Err_10014_LinkedValueConnectInput_InvalidUid;
		}
	}
	else
	{
		CString ToolSetName;
		SVString ObjectName;

		ToolSetName.LoadString( IDS_CLASSNAME_SVTOOLSET );

		//Default name
		ObjectName = Value;
		//If the tool set name is at the start then add the inspection name at the beginning
		if( 0 == Value.Find( ToolSetName ) )
		{
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> ( GetAncestor( SVInspectionObjectType ) );
			if( nullptr != pInspection )
			{
				ObjectName = pInspection->GetName();
				ObjectName += _T(".");
				ObjectName += Value;
			}
		}
		if( S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName( ObjectName, pLinkedObject ) )
		{
			ConvertDottedName = true;
		}
	}
	if ( nullptr != pLinkedObject )	// input is another VO
	{
		//disconnect old object and connect new object if old and new different.
		if( m_pLinkedObject != pLinkedObject )
		{
			//First disconnect and then set the new Linked object
			DisconnectInput();
			m_pLinkedObject = pLinkedObject;
			m_LinkedUid = m_pLinkedObject->GetUniqueObjectID();
			//Convert old dotted name format to Unique GUID
			if( ConvertDottedName )
			{
				SVVariantValueObjectClass::SetValueAt( 0, 0, m_LinkedUid.ToString().c_str() );
			}
			DWORD_PTR messageReturn = ConnectInput();
			ASSERT( SVMR_SUCCESS == messageReturn );
			if( SVMR_SUCCESS != messageReturn )
			{
				Result = SvOi::Err_10015_LinkedValueConnectInput_ConnectFailed;
			}
		}
	}
	else	// plain data
	{
		if( S_OK == Result )
		{
			DisconnectInput();

			_variant_t value;
			SVVariantValueObjectClass::GetValue( value );
			if ( GetDefaultType() != value.vt)
			{
				Result = SvOi::Err_10016_LinkedValueConnectInput_InvalidValue;
			}
		}
	}
	UpdateLinkedName();

	return Result;
}

void LinkedValue::DisconnectInput()
{
	if( nullptr != m_pLinkedObject )
	{
		SVInObjectInfoStruct InputConnectionInfo;
		InputConnectionInfo.SetInputObject( m_pLinkedObject );
		InputConnectionInfo.SetObject( this );
		::SVSendMessage( m_pLinkedObject, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR> (&InputConnectionInfo), 0 );
		m_LinkedUid = SV_GUID_NULL;
		m_pLinkedObject = nullptr;
	}
}

DWORD_PTR LinkedValue::ConnectInput()
{
	SVInObjectInfoStruct InputConnectionInfo;
	InputConnectionInfo.SetInputObject( m_pLinkedObject );
	InputConnectionInfo.SetObject( this );
	return ::SVSendMessage( m_pLinkedObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR> (&InputConnectionInfo), 0 );
}

DWORD_PTR LinkedValue::processMessage( DWORD MessageID, DWORD_PTR MessageValue, DWORD_PTR MessageContext )
{
	DWORD_PTR Result = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD PureMessageID = MessageID & SVM_PURE_MESSAGE;
	switch( PureMessageID )
	{
	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			SVInObjectInfoStruct* pInInfo = reinterpret_cast <SVInObjectInfoStruct*> (MessageValue);
			DisconnectObject( pInInfo->GetInputObjectInfo().PObject );
			Result = SVMR_SUCCESS;
			break;
		}
	case SVMSGID_OBJECT_RENAMED:
		{
			//Get the current name of the object
			UpdateLinkedName();
			Result = SVMR_SUCCESS;
			break;
		}
	}

	Result = SVVariantValueObjectClass::processMessage( MessageID, MessageValue, MessageContext ) | Result;
	return Result;
}

HRESULT LinkedValue::ResetObject()
{
	HRESULT Result = SVVariantValueObjectClass::ResetObject();
	if( S_OK == Result )
	{
		Result = UpdateConnection();
	}
	m_isObjectValid = (S_OK == Result);

	return Result;
}

#pragma endregion Private Methods
