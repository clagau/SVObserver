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
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
LinkedValue::LinkedValue() : 
 m_pLinkedObject( nullptr )
,m_CircularReference( false )
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

	//! If this flag is still set then this value is trying to reference itself!
	if( m_CircularReference )
	{
		Result = SVMSG_SVO_105_CIRCULAR_REFERENCE;
	}


	if( S_OK ==  Result )
	{
		//! When getting the value from an indirect value make sure it is not referencing this object
		m_CircularReference = true;
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
		m_CircularReference = false;
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

HRESULT LinkedValue::GetValueAt( int Bucket, int Index, SVString& rValue ) const
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
			rValue = SvUl_SF::createSVString( Value.bstrVal );
			if( VT_BOOL == oldType )
			{
				SvUl_SF::MakeUpper( rValue );
			}
		}
		else
		{
			rValue.clear();
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
				if ( 0 == SvUl_SF::CompareNoCase( Temp, SVString( SvOi::cTrue) ) )
				{
					rValue = true;

				}
				if ( 0 == SvUl_SF::CompareNoCase( Temp, SVString( SvOi::cFalse) ) )
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

HRESULT LinkedValue::SetValueAt( int Bucket, int Index, const SVString& rValue )
{
	HRESULT Result( S_OK );
	SVObjectClass* pNewLinkedObject = ConvertStringInObject( SVString( rValue ) );

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
			bool messageReturn = ConnectInput();
			assert( messageReturn );
			//To check that the linked value has no circular reference we do a GetValue
			_variant_t Value;
			Result = GetValueAt( Bucket, Index, Value.GetVARIANT() );
		}
	}
	else
	{
		SetType( GetDefaultType() );
		Result = SetValueKeepType( Bucket, rValue.c_str() );
	}

	UpdateLinkedName();
	return Result;
}

bool LinkedValue::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if ( nullptr != pObjectInInfo && pObjectInInfo->GetInputObjectInfo().PObject == m_pLinkedObject )
	{
		DisconnectInput();
	}
	return __super::DisconnectObjectInput(pObjectInInfo);
}

void LinkedValue::UpdateLinkedName()
{
	if( nullptr != m_pLinkedObject && SV_GUID_NULL != m_LinkedUid )
	{
		m_LinkedName.SetValue( 0, SVString(m_pLinkedObject->GetCompleteObjectNameToObjectType()) );
		m_LinkedName.ObjectAttributesAllowedRef() = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
	}
	else
	{
		SVString Value;
		m_LinkedName.GetDefaultValue( Value );
		m_LinkedName.SetValue( 0, Value );
		m_LinkedName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
void LinkedValue::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	SVObjectClass* pLinkedObject = ConvertStringInObject(rValue);

	if ( nullptr != pLinkedObject )
	{
		if( CheckLinkedObject( pLinkedObject ) )
		{
			//This must use the base class otherwise causes recursive call to ValidateValue
			SVVariantValueObjectClass::ValidateValue( iBucket, iIndex, pLinkedObject->GetUniqueObjectID().ToString().c_str() );
		}
		else
		{
			pLinkedObject = nullptr;
			//! This means the linked object is invalid
			SVStringVector msgList;
			msgList.push_back( GetName() );
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			Exception.Throw();
		}
	}
	else
	{
		_variant_t vtTemp;
		vtTemp = rValue.c_str();

		if( DefaultValue().vt != VT_EMPTY )
		{
			HRESULT Result = ::VariantChangeType( &vtTemp, &vtTemp, 0, DefaultValue().vt );
			if ( S_OK != Result) //object index out of range will not throw
			{
				SVStringVector msgList;
				msgList.push_back(GetName());
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				Exception.Throw();
			}
		}

		base::ValidateValue( iBucket, iIndex, rValue );
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	SVObjectClass* pLinkedObject( nullptr );
	bool ConvertDottedName( false );
	SVString Value;

	//We now need to check the no linked value so call the base class method
	SVVariantValueObjectClass::GetValueAt( m_iLastSetIndex, 0,  Value );

	SVGUID LinkedUid( _bstr_t( Value.c_str() ) );

	//If valid GUID then should be able to get the linked value from the object manager
	if( SV_GUID_NULL != LinkedUid  )
	{
		SVObjectManagerClass::Instance().GetObjectByIdentifier( LinkedUid, pLinkedObject );
		
		Result = CheckLinkedObject( pLinkedObject, pErrorMessages );
		if( !Result )
		{
			pLinkedObject = nullptr;
		}
	}
	else
	{
		SVString ToolSetName;
		SVString ObjectName;

		ToolSetName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTOOLSET );

		//Default name
		ObjectName = Value;
		//If the tool set name is at the start then add the inspection name at the beginning
		if( 0 == Value.find( ToolSetName ) )
		{
			SvOi::IObjectClass* pInspection = GetAncestorInterface( SVInspectionObjectType );
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
			bool messageReturn = ConnectInput();
			assert( messageReturn );
			if( !messageReturn )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	else	// plain data
	{
		if( Result )
		{
			DisconnectInput();

			_variant_t value;
			SVVariantValueObjectClass::GetValue( value );
			if ( GetDefaultType() != value.vt)
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
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
		m_pLinkedObject->DisconnectObjectInput(&InputConnectionInfo);
		m_LinkedUid = SV_GUID_NULL;
		m_pLinkedObject = nullptr;
	}
}

bool LinkedValue::ConnectInput()
{
	bool Result = false;
	SVInObjectInfoStruct InputConnectionInfo;
	InputConnectionInfo.SetInputObject( m_pLinkedObject );
	InputConnectionInfo.SetObject( this );
	if (nullptr != m_pLinkedObject)
	{
		Result = m_pLinkedObject->ConnectObjectInput(&InputConnectionInfo);
	}
	return Result;
}

bool LinkedValue::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVVariantValueObjectClass::ResetObject(pErrorMessages);
	Result = Result && UpdateConnection(pErrorMessages);
	m_isObjectValid = Result;
	return Result;
}

SVObjectClass* LinkedValue::ConvertStringInObject( const SVString& rValue ) const
{
	SVString ToolSetName;
	SVString ObjectName;

	ToolSetName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTOOLSET );

	//If the tool set name is at the start then add the inspection name at the beginning
	if( 0 == rValue.find( ToolSetName.c_str() ) )
	{
		const SvOi::IObjectClass* pInspection = GetAncestorInterface( SVInspectionObjectType );
		if( nullptr != pInspection )
		{
			ObjectName = pInspection->GetName();
			ObjectName += _T(".");
			ObjectName += rValue;
		}
	}
	else
	{
		ObjectName = rValue;
	}

	SVObjectClass* pNewLinkedObject( nullptr );
	SVObjectManagerClass::Instance().GetObjectByDottedName( ObjectName, pNewLinkedObject );
	return pNewLinkedObject;
}

bool LinkedValue::CheckLinkedObject( const SVObjectClass* const pLinkedObject, SvStl::MessageContainerVector *pErrorMessages ) const
{
	bool Result( true );

	//! Check if a valid object and make sure it does not link to itself
	if ( nullptr == pLinkedObject || this == pLinkedObject )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_InvalidLinkedObjectOrSame, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	else
	{
		//! This is important when copying tools that the value of another inspection is not used due to the GUID being valid
		//! That is why check that the linked value of an object is in the same inspection
		const IObjectClass* pLinkedObjectInspection =  pLinkedObject->GetAncestorInterface( SVInspectionObjectType );
		bool isSameInpection = GetAncestorInterface( SVInspectionObjectType ) == pLinkedObjectInspection;
		//! If linked object has no inspection (e.g. Global Constants) then we don't need to check that the inspections are the same
		if( nullptr != pLinkedObjectInspection && !isSameInpection )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_WrongInspection, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

#pragma endregion Private Methods
