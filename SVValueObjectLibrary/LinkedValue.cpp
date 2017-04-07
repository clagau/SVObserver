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
 m_pLinkedObject(nullptr)
,m_pLinkedValueObject(nullptr)
,m_CircularReference(false)
,m_getNonLinkedValue(false)
{
	m_LinkedName.setStatic( true );
}

LinkedValue::~LinkedValue()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT LinkedValue::GetValue( _variant_t& rValue,  int Bucket /*= -1*/, int Index /*= -1*/ ) const
{
	//If get non linked value call base class method
	if( m_getNonLinkedValue )
	{
		return __super::GetValue( rValue, Bucket, Index );
	}

	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;
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
		if( nullptr != m_pLinkedValueObject )
		{
			Result = m_pLinkedValueObject->getValue( rValue, Bucket, Index );
		}
		else
		{
			Result = __super::GetValue( rValue, Bucket, Index );
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

bool LinkedValue::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if ( nullptr != pObjectInInfo && pObjectInInfo->GetInputObjectInfo().m_pObject == m_pLinkedObject )
	{
		DisconnectInput();
	}
	return __super::DisconnectObjectInput(pObjectInInfo);
}

void LinkedValue::UpdateLinkedName()
{
	if( nullptr != m_pLinkedObject && SV_GUID_NULL != m_LinkedUid )
	{
		m_LinkedName.SetValue( SVString(m_pLinkedObject->GetCompleteObjectNameToObjectType()) );
		m_LinkedName.SetObjectAttributesAllowed( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
	}
	else
	{
		SVString Value = m_LinkedName.GetDefaultValue();
		m_LinkedName.SetValue( Value );
		m_LinkedName.SetObjectAttributesAllowed( SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
_variant_t LinkedValue::ConvertString2Type( const SVString& rValue ) const
{
	_variant_t Result;

	SVObjectClass* pLinkedObject = ConvertStringInObject(rValue);

	if ( nullptr != pLinkedObject )
	{
		if( CheckLinkedObject( pLinkedObject ) )
		{
			//This must use the base class otherwise causes recursive call to ValidateValue
			Result = SVVariantValueObjectClass::ConvertString2Type( pLinkedObject->GetUniqueObjectID().ToString() );
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
		vtTemp.SetString( rValue.c_str() );

		if( GetDefaultValue().vt != VT_EMPTY )
		{
			HRESULT Result = ::VariantChangeType( &vtTemp, &vtTemp, 0, GetDefaultValue().vt );
			if ( S_OK != Result) //object index out of range will not throw
			{
				SVStringVector msgList;
				msgList.push_back(GetName());
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				Exception.Throw();
			}
		}

		Result = SVVariantValueObjectClass::ConvertString2Type( rValue );
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	SVObjectClass* pLinkedObject( nullptr );
	bool ConvertDottedName( false );
	SVString Value;

	m_getNonLinkedValue = true;
	getValue( Value );
	m_getNonLinkedValue = false;

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
			m_pLinkedValueObject = dynamic_cast<SvOi::IValueObject*> (m_pLinkedObject);
			m_LinkedUid = m_pLinkedObject->GetUniqueObjectID();
			//Convert old dotted name format to Unique GUID
			if( ConvertDottedName )
			{
				SVVariantValueObjectClass::setValue( m_LinkedUid.ToString() );
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

			_variant_t VariantValue;
			m_getNonLinkedValue = true;
			GetValue( VariantValue );
			m_getNonLinkedValue = false;
			if ( GetDefaultType() != VariantValue.vt)
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
		m_pLinkedValueObject = nullptr;
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
