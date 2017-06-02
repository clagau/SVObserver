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
#include "SVStatusLibrary/ErrorNumbers.h"
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
 m_CircularReference(false)
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
		if (nullptr != m_LinkedObjectRef.getValueObject())
		{
			Result = m_LinkedObjectRef.getValue(rValue, Bucket);
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
	if ( nullptr != pObjectInInfo && pObjectInInfo->GetInputObjectInfo().m_pObject == m_LinkedObjectRef.getObject())
	{
		DisconnectInput();
	}
	return __super::DisconnectObjectInput(pObjectInInfo);
}

void LinkedValue::UpdateLinkedName()
{
	if( nullptr != m_LinkedObjectRef.getObject() )
	{
		m_LinkedName.SetValue( m_LinkedObjectRef.GetCompleteOneBasedObjectName() );
		m_LinkedName.SetObjectAttributesAllowed( SvOi::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
	}
	else
	{
		SVString Value = m_LinkedName.GetDefaultValue();
		m_LinkedName.SetValue( Value );
		m_LinkedName.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods
_variant_t LinkedValue::ConvertString2Type( const SVString& rValue ) const
{
	_variant_t Result;

	SVObjectReference LinkedObjectRef = ConvertStringInObject(rValue);

	if (nullptr != LinkedObjectRef.getObject())
	{
		if( CheckLinkedObject( LinkedObjectRef.getObject()) )
		{
			//Change the value directly as the default value may have a different variant type
			Result = _variant_t(LinkedObjectRef.GetGuidAndIndexOneBased().c_str());
		}
		else
		{
			//! This means the linked object is invalid
			SVStringVector msgList;
			msgList.push_back( GetName() );
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			Exception.Throw();
		}
	}
	else
	{
		_variant_t vtTemp;
		vtTemp.SetString( rValue.c_str() );

		if( GetDefaultValue().vt != VT_EMPTY )
		{
			if ( S_OK != ::VariantChangeType(&vtTemp, &vtTemp, 0, GetDefaultValue().vt))
			{
				SVStringVector msgList;
				msgList.push_back(GetName());
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				Exception.Throw();
			}
			Result = vtTemp;
		}
		else
		{
			Result = SVVariantValueObjectClass::ConvertString2Type(rValue);
		}
	}
	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool LinkedValue::UpdateConnection(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	bool ConvertDottedName( false );
	_variant_t Value;

	//! Here we need the non linked value (SVGUID as string or constant value)
	__super::GetValue( Value );

	SVString guidAndIndexString = SvUl_SF::createSVString(Value);
	SVObjectReference LinkedObjectRef(guidAndIndexString);
	
	//If valid GUID then should be able to get the linked value from the object manager
	if( nullptr != LinkedObjectRef.getObject() )
	{
		Result = CheckLinkedObject(LinkedObjectRef.getObject(), pErrorMessages );
		if( !Result )
		{
			LinkedObjectRef = SVObjectReference();
		}
	}
	else
	{	
		//Check if current value is a GUID, but not exist anymore. In this case it is probably an deleted object. Set this value to invalid.
		SVString::size_type Pos = guidAndIndexString.find_first_of(_T("["));
		SVString guidString = guidAndIndexString.substr(0, Pos);
		if (SV_GUID_NULL != SVGUID(_bstr_t(guidString.c_str())))
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}
		else
		{
			//this part is only for backward compatibility, because in older version the name was saved and not the GUID.
			SVString ToolSetName;
			SVString ObjectName;

			ToolSetName = SvUl_SF::LoadSVString(IDS_CLASSNAME_SVTOOLSET);

			//Default name
			ObjectName = SvUl_SF::createSVString(Value);
			//If the tool set name is at the start then add the inspection name at the beginning
			if (0 == ObjectName.find(ToolSetName))
			{
				SvOi::IObjectClass* pInspection = GetAncestorInterface(SVInspectionObjectType);
				if (nullptr != pInspection)
				{
					ObjectName = pInspection->GetName();
					ObjectName += _T(".");
					ObjectName += SvUl_SF::createSVString(Value);
				}
			}
			if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, LinkedObjectRef) && nullptr != LinkedObjectRef.getObject())
			{
				ConvertDottedName = true;
			}
		}		
	}
	if ( nullptr != LinkedObjectRef.getObject())	// input is another VO
	{
		//disconnect old object and connect new object if old and new different.
		if( LinkedObjectRef.getObject() != m_LinkedObjectRef.getObject() )
		{
			//First disconnect and then set the new Linked object
			DisconnectInput();
			m_LinkedObjectRef = LinkedObjectRef;
			
			//Convert old dotted name format to Unique GUID
			if( ConvertDottedName )
			{
				SVVariantValueObjectClass::setValue( m_LinkedObjectRef.GetCompleteOneBasedObjectName() );
			}
			bool messageReturn = ConnectInput();
			assert( messageReturn );
			if( !messageReturn )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConnectInputFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			m_LinkedObjectRef = LinkedObjectRef;
		}
	}
	else	// plain data
	{
		if( Result )
		{
			DisconnectInput();

			if ( GetDefaultType() != Value.vt)
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongType, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	if (Result)
	{
		UpdateLinkedName();
	}

	return Result;
}

void LinkedValue::DisconnectInput()
{
	if( nullptr != m_LinkedObjectRef.getObject())
	{
		SVInObjectInfoStruct InputConnectionInfo;
		InputConnectionInfo.SetInputObject(m_LinkedObjectRef.getObject());
		InputConnectionInfo.SetObject( this );
		m_LinkedObjectRef.getObject()->DisconnectObjectInput(&InputConnectionInfo);
		m_LinkedObjectRef = SVObjectReference();
	}
}

bool LinkedValue::ConnectInput()
{
	bool Result = false;
	SVInObjectInfoStruct InputConnectionInfo;
	InputConnectionInfo.SetInputObject(m_LinkedObjectRef.getObject());
	InputConnectionInfo.SetObject( this );
	if (nullptr != m_LinkedObjectRef.getObject())
	{
		Result = m_LinkedObjectRef.getObject()->ConnectObjectInput(&InputConnectionInfo);
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

SVObjectReference LinkedValue::ConvertStringInObject( const SVString& rValue ) const
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

	SVObjectReference objectRef;
	SVObjectManagerClass::Instance().GetObjectByDottedName( ObjectName, objectRef);
	return objectRef;
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidLinkedObjectOrSame, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongInspection, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

#pragma endregion Private Methods
